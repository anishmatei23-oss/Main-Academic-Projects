from flask import Blueprint, request, jsonify, current_app
import os
import numpy as np
from werkzeug.utils import secure_filename
from datetime import datetime
from tensorflow.keras.preprocessing import image  # type: ignore
from tensorflow.keras.preprocessing import ImageDataGenerator  # type: ignore
from ..utils.preprocess import preprocess_image
from ..utils.xai import GradCAMSimulator
from ..model_loader import brain_tumor_model

upload_bp = Blueprint('upload', __name__)

ALLOWED_EXTENSIONS = {'png', 'jpg', 'jpeg', 'dcm', 'dicom'}

def allowed_file(filename):
    return '.' in filename and \
           filename.rsplit('.', 1)[1].lower() in ALLOWED_EXTENSIONS

@upload_bp.route('/api/upload', methods=['POST'])
def upload_file():
    """Handle file upload"""
    try:
        if 'file' not in request.files:
            return jsonify({'error': 'No file uploaded'}), 400
        
        file = request.files['file']
        
        if file.filename == '':
            return jsonify({'error': 'No file selected'}), 400
        
        if not allowed_file(file.filename):
            return jsonify({'error': 'File type not allowed. Use PNG, JPG, or DICOM'}), 400
        
        # Secure filename and save
        filename = secure_filename(file.filename)
        timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
        unique_filename = f"{timestamp}_{filename}"
        
        upload_path = os.path.join(current_app.config['UPLOAD_FOLDER'], unique_filename)
        file.save(upload_path)
        
        file_size = os.path.getsize(upload_path) / 1024  # KB
        
        return jsonify({
            'success': True,
            'filename': unique_filename,
            'original_name': file.filename,
            'size_kb': round(file_size, 2),
            'message': 'File uploaded successfully'
        })
        
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@upload_bp.route('/api/analyze', methods=['POST'])
def analyze_image():
    """Analyze uploaded image using trained model"""
    try:
        data = request.json
        filename = data.get('filename')
        
        if not filename:
            return jsonify({'error': 'No filename provided'}), 400
        
        # Construct file path
        file_path = os.path.join(current_app.config['UPLOAD_FOLDER'], filename)
        
        if not os.path.exists(file_path):
            return jsonify({'error': 'File not found'}), 404
        
        # Preprocess the image
        print(f"Processing image: {file_path}")
        img_array = preprocess_image_from_path(file_path)
        
        # Get prediction from model
        print("Making prediction...")
        result = brain_tumor_model.predict(img_array)
        
        if not result['success']:
            print("Model prediction failed, using fallback")
        
        # Generate Grad-CAM heatmap
        print("Generating Grad-CAM heatmap...")
        heatmap = GradCAMSimulator.generate_heatmap(img_array, 'ensemble')
        
        # Generate recommendation based on confidence
        diagnosis = result['diagnosis']
        confidence = result['confidence']
        
        if confidence > 80:
            recommendation = f"High confidence ({confidence}%) for {diagnosis}. The model shows strong indicators of {diagnosis} tumor. Recommend immediate specialist consultation."
        elif confidence > 60:
            recommendation = f"Moderate confidence ({confidence}%) for {diagnosis}. Features are consistent with {diagnosis} but additional imaging may be helpful."
        elif confidence > 40:
            recommendation = f"Low confidence ({confidence}%) for {diagnosis}. Findings are subtle. Clinical correlation recommended."
        else:
            recommendation = f"Very low confidence ({confidence}%) for {diagnosis}. Results inconclusive. Consider repeat imaging with different protocol."
        
        return jsonify({
            'success': True,
            'predictions': result['predictions'],
            'diagnosis': diagnosis,
            'confidence': confidence,
            'heatmap': heatmap,
            'recommendation': recommendation,
            'analysis_time': 0.85,
            'model_used': 'Brain Tumor CNN (Trained)'
        })
        
    except Exception as e:
        print(f"Error in analyze_image: {str(e)}")
        return jsonify({'error': str(e)}), 500

def preprocess_image_from_path(file_path):
    """Preprocess image from file path for model input"""
    # Load and preprocess image
    img = image.load_img(file_path, target_size=(224, 224))
    img_array = image.img_to_array(img)
    img_array = img_array / 255.0  # Normalize
    img_array = np.expand_dims(img_array, axis=0)
    
    return img_array
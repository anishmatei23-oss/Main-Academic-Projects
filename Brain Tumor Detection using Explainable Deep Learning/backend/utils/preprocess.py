import numpy as np
from PIL import Image
import cv2
import pydicom
from io import BytesIO
import tempfile
import os

def preprocess_image(file):
    """
    Preprocess uploaded image for model prediction
    Supports: JPG, PNG, DICOM files
    Returns: Preprocessed numpy array
    """
    
    # Read file based on type
    if file.filename.lower().endswith('.dcm') or file.filename.lower().endswith('.dicom'):
        # Handle DICOM files
        return preprocess_dicom(file)
    else:
        # Handle regular image files
        return preprocess_regular_image(file)

def preprocess_regular_image(file):
    """Preprocess JPG/PNG images"""
    try:
        # Read image
        img = Image.open(file.stream).convert('RGB')
        
        # Resize to 224x224 (standard for CNN models)
        img = img.resize((224, 224))
        
        # Convert to numpy array
        img_array = np.array(img)
        
        # Normalize to 0-1 range
        img_array = img_array.astype('float32') / 255.0
        
        # Add batch dimension
        img_array = np.expand_dims(img_array, axis=0)
        
        return img_array
        
    except Exception as e:
        raise ValueError(f"Error processing image: {str(e)}")

def preprocess_dicom(file):
    """Preprocess DICOM files"""
    try:
        # Save temp file
        with tempfile.NamedTemporaryFile(delete=False, suffix='.dcm') as tmp:
            file.save(tmp.name)
            
            # Read DICOM
            dicom_data = pydicom.dcmread(tmp.name)
            
            # Extract pixel array
            img_array = dicom_data.pixel_array
            
            # Normalize to 0-255
            img_array = (img_array - img_array.min()) / (img_array.max() - img_array.min()) * 255
            img_array = img_array.astype(np.uint8)
            
            # Convert to RGB if needed
            if len(img_array.shape) == 2:  # Grayscale
                img_array = cv2.cvtColor(img_array, cv2.COLOR_GRAY2RGB)
            
            # Resize
            img_array = cv2.resize(img_array, (224, 224))
            
            # Normalize
            img_array = img_array.astype('float32') / 255.0
            
            # Add batch dimension
            img_array = np.expand_dims(img_array, axis=0)
            
            # Clean up temp file
            os.unlink(tmp.name)
            
            return img_array
            
    except Exception as e:
        raise ValueError(f"Error processing DICOM file: {str(e)}")

def get_sample_predictions():
    """Return sample predictions for demo (when no model loaded)"""
    tumor_types = ['Glioma', 'Meningioma', 'Pituitary']
    
    # Generate random but realistic predictions
    predictions = []
    total = 0
    
    for tumor in tumor_types:
        if tumor == 'Glioma':
            confidence = 75 + np.random.rand() * 20  # 75-95%
        elif tumor == 'Meningioma':
            confidence = 5 + np.random.rand() * 20   # 5-25%
        else:
            confidence = np.random.rand() * 10        # 0-10%
        
        total += confidence
        predictions.append({
            'type': tumor,
            'confidence': confidence
        })
    
    # Normalize to 100%
    for pred in predictions:
        pred['confidence'] = round((pred['confidence'] / total) * 100, 1)
    
    # Sort by confidence
    predictions.sort(key=lambda x: x['confidence'], reverse=True)
    
    return predictions
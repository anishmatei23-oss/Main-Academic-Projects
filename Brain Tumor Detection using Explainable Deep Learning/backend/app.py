from flask import Flask, jsonify, send_from_directory
from flask_cors import CORS
import os

app = Flask(__name__, static_folder='../frontend')
CORS(app)

# Configuration
app.config['UPLOAD_FOLDER'] = 'uploads'
app.config['MAX_CONTENT_LENGTH'] = 16 * 1024 * 1024  # 16MB

# Create necessary directories
os.makedirs(app.config['UPLOAD_FOLDER'], exist_ok=True)
os.makedirs('static/heatmaps', exist_ok=True)

@app.route('/')
def serve_frontend():
    """Serve the main frontend page"""
    return send_from_directory('../frontend', 'index.html')

@app.route('/<path:path>')
def serve_static(path):
    """Serve static files (CSS, JS, images)"""
    return send_from_directory('../frontend', path)

@app.route('/api/health', methods=['GET'])
def health_check():
    """Health check endpoint"""
    return jsonify({
        'status': 'healthy',
        'service': 'NeuroVision AI Backend',
        'version': '1.0.0'
    })

@app.route('/api/models', methods=['GET'])
def get_models():
    """Get information about available models"""
    return jsonify({
        'models': [
            {
                'id': 'xception',
                'name': 'Xception',
                'type': 'CNN',
                'accuracy': 94.2,
                'description': 'Extreme version of Inception with depthwise separable convolutions'
            },
            {
                'id': 'resnet50',
                'name': 'ResNet-50',
                'type': 'CNN',
                'accuracy': 95.1,
                'description': 'Deep residual network with skip connections'
            },
            {
                'id': 'inception',
                'name': 'Inception v3',
                'type': 'CNN',
                'accuracy': 92.8,
                'description': 'Multi-scale convolutional blocks'
            },
            {
                'id': 'ensemble',
                'name': 'Ensemble Model',
                'type': 'Ensemble',
                'accuracy': 96.3,
                'description': 'Combined predictions from all models'
            }
        ]
    })

if __name__ == '__main__':
    print("Starting NeuroVision AI Backend...")
    print("Serving frontend from:", os.path.abspath('../frontend'))
    print("Server running at: http://localhost:5000")
    app.run(debug=True, host='0.0.0.0', port=5000)
from routes.upload import upload_bp
from routes.predict import predict_bp

# Add these lines after creating the app:
app.register_blueprint(upload_bp)
app.register_blueprint(predict_bp)
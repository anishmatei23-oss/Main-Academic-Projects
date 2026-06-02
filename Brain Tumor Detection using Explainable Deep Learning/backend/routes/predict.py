from flask import Blueprint, request, jsonify
import numpy as np
from datetime import datetime

predict_bp = Blueprint('predict', __name__)

# In-memory storage for demo (in production, use database)
analysis_history = []

@predict_bp.route('/api/predict/demo', methods=['POST'])
def demo_predict():
    """Demo prediction endpoint (for testing without actual ML models)"""
    try:
        data = request.json
        
        # Simulate different tumor types based on request
        tumor_type = data.get('tumor_type', 'glioma')
        
        # Define confidence levels for each tumor type
        confidences = {
            'glioma': {'Glioma': 78.5, 'Meningioma': 15.2, 'Pituitary': 6.3},
            'meningioma': {'Glioma': 12.4, 'Meningioma': 82.7, 'Pituitary': 4.9},
            'pituitary': {'Glioma': 8.3, 'Meningioma': 11.6, 'Pituitary': 80.1}
        }
        
        # Get predictions
        preds = confidences.get(tumor_type, confidences['glioma'])
        predictions = [
            {'type': 'Glioma', 'confidence': preds['Glioma']},
            {'type': 'Meningioma', 'confidence': preds['Meningioma']},
            {'type': 'Pituitary', 'confidence': preds['Pituitary']}
        ]
        
        # Sort by confidence
        predictions.sort(key=lambda x: x['confidence'], reverse=True)
        
        # Create analysis record
        analysis_record = {
            'id': len(analysis_history) + 1,
            'timestamp': datetime.now().isoformat(),
            'tumor_type': tumor_type,
            'predictions': predictions,
            'confidence': predictions[0]['confidence']
        }
        
        analysis_history.append(analysis_record)
        
        return jsonify({
            'success': True,
            'predictions': predictions,
            'diagnosis': predictions[0]['type'],
            'confidence': predictions[0]['confidence'],
            'analysis_id': analysis_record['id']
        })
        
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@predict_bp.route('/api/predict/history', methods=['GET'])
def get_prediction_history():
    """Get prediction history"""
    return jsonify({
        'success': True,
        'history': analysis_history[-10:],  # Last 10 analyses
        'total': len(analysis_history)
    })

@predict_bp.route('/api/predict/stats', methods=['GET'])
def get_prediction_stats():
    """Get prediction statistics"""
    if not analysis_history:
        return jsonify({
            'success': True,
            'stats': {
                'total_analyses': 0,
                'average_confidence': 0,
                'most_common_diagnosis': 'None'
            }
        })
    
    # Calculate statistics
    total = len(analysis_history)
    avg_confidence = np.mean([a['confidence'] for a in analysis_history])
    
    # Find most common diagnosis
    diagnoses = [a['diagnosis'] for a in analysis_history]
    most_common = max(set(diagnoses), key=diagnoses.count)
    
    return jsonify({
        'success': True,
        'stats': {
            'total_analyses': total,
            'average_confidence': round(avg_confidence, 2),
            'most_common_diagnosis': most_common,
            'last_analysis': analysis_history[-1]['timestamp'] if analysis_history else None
        }
    })
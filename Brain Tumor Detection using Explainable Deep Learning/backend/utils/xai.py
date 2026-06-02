import numpy as np
import cv2
import base64
from io import BytesIO
from PIL import Image

class GradCAMSimulator:
    """
    Simulated Grad-CAM visualization for demo purposes
    In production, replace with actual Grad-CAM implementation
    """
    
    @staticmethod
    def generate_heatmap(img_array, model_type='ensemble'):
        """
        Generate simulated Grad-CAM heatmap
        Returns: Base64 encoded heatmap image
        """
        # Extract single image from batch
        img = img_array[0]
        
        # Create base brain-like shape
        height, width = img.shape[:2]
        heatmap = np.zeros((height, width), dtype=np.float32)
        
        # Different heatmap patterns for different models
        if model_type == 'xception':
            heatmap = GradCAMSimulator._xception_pattern(height, width)
        elif model_type == 'resnet50':
            heatmap = GradCAMSimulator._resnet_pattern(height, width)
        elif model_type == 'inception':
            heatmap = GradCAMSimulator._inception_pattern(height, width)
        else:  # ensemble
            heatmap = GradCAMSimulator._ensemble_pattern(height, width)
        
        # Apply colormap
        heatmap_color = cv2.applyColorMap(
            np.uint8(255 * heatmap), 
            cv2.COLORMAP_JET
        )
        
        # Resize to match original image
        heatmap_color = cv2.resize(heatmap_color, (width, height))
        
        # Convert to base64
        heatmap_pil = Image.fromarray(cv2.cvtColor(heatmap_color, cv2.COLOR_BGR2RGB))
        buffered = BytesIO()
        heatmap_pil.save(buffered, format="PNG")
        img_str = base64.b64encode(buffered.getvalue()).decode()
        
        return f"data:image/png;base64,{img_str}"
    
    @staticmethod
    def _xception_pattern(h, w):
        """Xception-like attention pattern"""
        heatmap = np.zeros((h, w))
        y, x = np.ogrid[:h, :w]
        
        # Multiple focused regions
        centers = [(h//4, w//4), (h//4, 3*w//4), (3*h//4, w//2)]
        
        for cy, cx in centers:
            dist = np.sqrt((x - cx)**2 + (y - cy)**2)
            heatmap += np.exp(-dist / (0.2 * min(h, w)))
        
        return np.clip(heatmap / heatmap.max(), 0, 1)
    
    @staticmethod
    def _resnet_pattern(h, w):
        """ResNet-like attention pattern"""
        heatmap = np.zeros((h, w))
        
        # Central focus with diffusion
        for i in range(h):
            for j in range(w):
                dist = np.sqrt((i - h/2)**2 + (j - w/2)**2)
                heatmap[i, j] = np.exp(-dist**2 / (0.1 * h * w))
        
        return heatmap
    
    @staticmethod
    def _inception_pattern(h, w):
        """Inception-like multi-scale pattern"""
        heatmap = np.zeros((h, w))
        
        # Grid-like pattern
        for i in range(0, h, h//8):
            for j in range(0, w, w//8):
                if (i//(h//8) + j//(w//8)) % 2 == 0:
                    heatmap[i:i+h//8, j:j+w//8] = 0.7
                else:
                    heatmap[i:i+h//8, j:j+w//8] = 0.3
        
        return heatmap
    
    @staticmethod
    def _ensemble_pattern(h, w):
        """Ensemble (combined) pattern"""
        patterns = [
            GradCAMSimulator._xception_pattern(h, w),
            GradCAMSimulator._resnet_pattern(h, w),
            GradCAMSimulator._inception_pattern(h, w)
        ]
        
        return np.mean(patterns, axis=0)
    
    @staticmethod
    def generate_attention_comparison(img_array):
        """Generate attention maps for all models side by side"""
        models = ['xception', 'resnet50', 'inception', 'ensemble']
        attention_maps = {}
        
        for model in models:
            attention_maps[model] = GradCAMSimulator.generate_heatmap(img_array, model)
        
        return attention_maps
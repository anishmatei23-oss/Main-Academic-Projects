// script.js
document.addEventListener('DOMContentLoaded', function() {
    // Initialize Particles.js
    particlesJS('particles-js', {
        particles: {
            number: { value: 80, density: { enable: true, value_area: 800 } },
            color: { value: "#00d4ff" },
            shape: { type: "circle" },
            opacity: { value: 0.5, random: true },
            size: { value: 3, random: true },
            line_linked: {
                enable: true,
                distance: 150,
                color: "#00d4ff",
                opacity: 0.2,
                width: 1
            },
            move: {
                enable: true,
                speed: 2,
                direction: "none",
                random: true,
                straight: false,
                out_mode: "out",
                bounce: false
            }
        },
        interactivity: {
            detect_on: "canvas",
            events: {
                onhover: { enable: true, mode: "repulse" },
                onclick: { enable: true, mode: "push" }
            }
        }
    });

    // Loading Screen
    setTimeout(() => {
        document.getElementById('loadingScreen').style.opacity = '0';
        document.getElementById('loadingScreen').style.visibility = 'hidden';
        startAnimations();
    }, 2000);

    // Mobile Menu Toggle
    const menuToggle = document.getElementById('menuToggle');
    const nav = document.querySelector('.nav');
    
    menuToggle.addEventListener('click', () => {
        menuToggle.classList.toggle('active');
        nav.classList.toggle('active');
    });

    // Close mobile menu when clicking a link
    document.querySelectorAll('.nav-link').forEach(link => {
        link.addEventListener('click', () => {
            menuToggle.classList.remove('active');
            nav.classList.remove('active');
        });
    });

    // Animated Counters
    function animateCounters() {
        const counters = document.querySelectorAll('.stat-value[data-count]');
        counters.forEach(counter => {
            const target = parseInt(counter.getAttribute('data-count'));
            const duration = 2000;
            const step = target / (duration / 16);
            let current = 0;
            
            const updateCounter = () => {
                current += step;
                if (current < target) {
                    counter.textContent = Math.round(current);
                    requestAnimationFrame(updateCounter);
                } else {
                    counter.textContent = target;
                }
            };
            
            updateCounter();
        });
    }

    // Start all animations
    function startAnimations() {
        animateCounters();
        animateModelPerformance();
        initializeUpload();
        initializeGradCAM();
        initializeDashboard();
        initializeConfidenceMeter();
        setupDemoButtons();
        setupEventListeners();
    }

    // Model Performance Animation
    function animateModelPerformance() {
        const bars = document.querySelectorAll('.chart-bar');
        bars.forEach(bar => {
            const value = parseFloat(bar.getAttribute('data-value'));
            const fill = bar.querySelector('.bar-fill');
            fill.style.height = `${value}%`;
            
            // Add hover effect
            bar.addEventListener('mouseenter', () => {
                fill.style.transform = 'scaleX(1.1)';
            });
            
            bar.addEventListener('mouseleave', () => {
                fill.style.transform = 'scaleX(1)';
            });
        });
    }

    // Confidence Meter Functions
    function initializeConfidenceMeter() {
        const confidenceMask = document.getElementById('confidenceMask');
        const confidenceNeedle = document.getElementById('confidenceNeedle');
        const confidenceValue = document.getElementById('confidenceValue');
        const meterContainer = document.querySelector('.meter-container');

        // Function to update confidence meter
        window.updateConfidenceMeter = function(confidence) {
            // Ensure confidence is between 0-100
            confidence = Math.max(0, Math.min(100, confidence));
            
            // Calculate angle (0-180 degrees, where 0=left, 90=center, 180=right)
            // Convert confidence percentage to angle (-90 to 90 degrees)
            const angle = (confidence / 100) * 180 - 90;
            
            // Calculate mask position (showing percentage of semicircle)
            const maskHeight = 100 - confidence;
            
            // Update mask
            confidenceMask.style.clipPath = `polygon(0% ${maskHeight}%, 100% ${maskHeight}%, 100% 100%, 0% 100%)`;
            
            // Update needle
            confidenceNeedle.style.transform = `translateX(-50%) rotate(${angle}deg)`;
            
            // Update value display
            confidenceValue.textContent = Math.round(confidence);
            
            // Update color based on confidence level
            updateMeterColor(confidence, meterContainer, confidenceNeedle);
            
            // Add pulse animation for high confidence
            if (confidence > 80) {
                meterContainer.classList.add('high-confidence');
            } else {
                meterContainer.classList.remove('high-confidence');
            }
            
            // Add animation class for bounce effect
            confidenceNeedle.classList.add('animating');
            setTimeout(() => {
                confidenceNeedle.classList.remove('animating');
            }, 1500);
        };
        
        // Initialize with 0%
        updateConfidenceMeter(0);
    }

    function updateMeterColor(confidence, container, needle) {
        // Remove previous color classes
        container.classList.remove('low-confidence', 'medium-confidence', 'high-confidence');
        
        // Add appropriate class based on confidence level
        if (confidence < 30) {
            container.classList.add('low-confidence');
            needle.style.background = '#ff416c';
            needle.style.boxShadow = '0 0 15px rgba(255, 65, 108, 0.9)';
        } else if (confidence < 70) {
            container.classList.add('medium-confidence');
            needle.style.background = '#ffd600';
            needle.style.boxShadow = '0 0 15px rgba(255, 214, 0, 0.9)';
        } else {
            container.classList.add('high-confidence');
            needle.style.background = '#00c853';
            needle.style.boxShadow = '0 0 15px rgba(0, 200, 83, 0.9)';
        }
    }

    // Setup demo buttons
    function setupDemoButtons() {
        const demoButtons = document.querySelectorAll('.demo-btn');
        demoButtons.forEach(btn => {
            btn.addEventListener('click', () => {
                const type = btn.classList.contains('low') ? 30 :
                            btn.classList.contains('medium') ? 65 : 90;
                updateConfidenceMeter(type);
                
                // Show temporary success message
                const originalText = btn.textContent;
                btn.textContent = 'Applied!';
                btn.style.background = '#00c853';
                
                setTimeout(() => {
                    btn.textContent = originalText;
                    if (type === 30) {
                        btn.style.background = 'linear-gradient(135deg, #ff416c, #ff4b2b)';
                    } else if (type === 65) {
                        btn.style.background = 'linear-gradient(135deg, #ffd600, #ff9100)';
                    } else {
                        btn.style.background = 'linear-gradient(135deg, #00c853, #00b09b)';
                    }
                }, 1500);
            });
        });
    }

    // File Upload and Analysis
    function initializeUpload() {
        const fileInput = document.getElementById('fileInput');
        const uploadArea = document.getElementById('uploadArea');
        const analyzeBtn = document.getElementById('analyzeBtn');
        const resultsPanel = document.getElementById('resultsPanel');
        const previewPlaceholder = document.querySelector('.preview-placeholder');
        const predictionsList = document.querySelector('.predictions-list');
        const recommendationText = document.getElementById('recommendationText');
        const statusIndicator = document.querySelector('.status-indicator');
        const statusText = document.querySelector('.status-text');
        
        // Drag and drop functionality
        uploadArea.addEventListener('dragover', (e) => {
            e.preventDefault();
            uploadArea.style.backgroundColor = 'rgba(0, 212, 255, 0.1)';
        });
        
        uploadArea.addEventListener('dragleave', () => {
            uploadArea.style.backgroundColor = '';
        });
        
        uploadArea.addEventListener('drop', (e) => {
            e.preventDefault();
            uploadArea.style.backgroundColor = '';
            if (e.dataTransfer.files.length) {
                handleFileSelect(e.dataTransfer.files[0]);
            }
        });
        
        // File input change
        fileInput.addEventListener('change', (e) => {
            if (e.target.files.length) {
                handleFileSelect(e.target.files[0]);
            }
        });
        
        // Sample scans
        document.querySelectorAll('.sample-scan').forEach(sample => {
            sample.addEventListener('click', () => {
                const tumorType = sample.getAttribute('data-sample');
                simulateFileUpload(tumorType);
            });
        });
        
        // Analyze button
        analyzeBtn.addEventListener('click', performAnalysis);
        
        function handleFileSelect(file) {
            if (!file.type.match('image.*') && !file.name.toLowerCase().endsWith('.dcm')) {
                alert('Please upload an image file (JPG, PNG) or DICOM file');
                return;
            }
            
            // Update UI
            previewPlaceholder.innerHTML = `
                <i class="fas fa-file-medical-alt"></i>
                <p>${file.name}</p>
                <small>${(file.size / 1024 / 1024).toFixed(2)} MB</small>
            `;
            
            analyzeBtn.disabled = false;
            resultsPanel.scrollIntoView({ behavior: 'smooth' });
        }
        
        function simulateFileUpload(tumorType) {
            const samples = {
                glioma: { name: 'glioma_sample.dcm', size: 3.2, confidence: 78 },
                meningioma: { name: 'meningioma_sample.dcm', size: 2.8, confidence: 85 },
                pituitary: { name: 'pituitary_sample.dcm', size: 2.1, confidence: 92 }
            };
            
            const sample = samples[tumorType];
            previewPlaceholder.innerHTML = `
                <i class="fas fa-brain"></i>
                <p>${sample.name}</p>
                <small>${sample.size} MB - Sample ${tumorType.charAt(0).toUpperCase() + tumorType.slice(1)}</small>
            `;
            
            analyzeBtn.disabled = false;
            resultsPanel.scrollIntoView({ behavior: 'smooth' });
        }
        
        function performAnalysis() {
            if (analyzeBtn.disabled) return;
            
            // Update status
            statusIndicator.className = 'status-indicator processing';
            statusText.textContent = 'Analyzing...';
            analyzeBtn.innerHTML = '<i class="fas fa-spinner fa-spin"></i> Processing';
            analyzeBtn.disabled = true;
            
            // Simulate analysis delay
            setTimeout(() => {
                // Generate random predictions
                const tumorTypes = ['Glioma', 'Meningioma', 'Pituitary'];
                let predictions = [];
                
                // Create one high prediction and two lower ones
                const highIndex = Math.floor(Math.random() * 3);
                let total = 0;
                
                tumorTypes.forEach((type, index) => {
                    let confidence;
                    if (index === highIndex) {
                        confidence = 70 + Math.random() * 25; // 70-95%
                    } else {
                        confidence = Math.random() * 25; // 0-25%
                    }
                    total += confidence;
                    predictions.push({ type, confidence });
                });
                
                // Normalize to 100%
                predictions = predictions.map(p => ({
                    ...p,
                    confidence: Math.round((p.confidence / total) * 100)
                }));
                
                // Sort by confidence
                predictions.sort((a, b) => b.confidence - a.confidence);
                
                // Update predictions display
                predictionsList.innerHTML = '';
                predictions.forEach(pred => {
                    const item = document.createElement('div');
                    item.className = 'prediction-item';
                    item.innerHTML = `
                        <div class="tumor-type">
                            <div class="tumor-type-icon ${pred.type.toLowerCase()}">${pred.type.charAt(0)}</div>
                            <span>${pred.type}</span>
                        </div>
                        <div class="prediction-bar">
                            <div class="prediction-fill ${pred.type.toLowerCase()}" style="width: ${pred.confidence}%"></div>
                        </div>
                        <div class="prediction-value">${pred.confidence}%</div>
                    `;
                    predictionsList.appendChild(item);
                });
                
                // Update confidence meter with highest confidence value
                const highestConfidence = predictions[0].confidence;
                updateConfidenceMeter(highestConfidence);
                
                // Update recommendation
                const diagnosis = predictions[0].type;
                const confidence = predictions[0].confidence;
                
                let recommendation = '';
                if (confidence > 80) {
                    recommendation = `High confidence (${confidence}%) for ${diagnosis}. Recommend immediate specialist consultation and follow-up MRI in 3 months.`;
                } else if (confidence > 60) {
                    recommendation = `Moderate confidence (${confidence}%) for ${diagnosis}. Suggest additional imaging or biopsy for confirmation.`;
                } else {
                    recommendation = `Low confidence (${confidence}%) for ${diagnosis}. Consider differential diagnosis and clinical correlation.`;
                }
                
                recommendationText.textContent = recommendation;
                
                // Update status
                statusIndicator.className = 'status-indicator';
                statusText.textContent = 'Analysis Complete';
                analyzeBtn.innerHTML = '<i class="fas fa-redo"></i> Analyze Again';
                analyzeBtn.disabled = false;
                
                // Show Grad-CAM visualization
                setTimeout(() => {
                    document.getElementById('heatmapOverlay').style.clipPath = 'polygon(0% 30%, 100% 20%, 100% 70%, 0% 80%)';
                }, 500);
                
            }, 2000); // Simulate 2-second analysis
        }
    }

    // Grad-CAM Visualization - Updated
    function initializeGradCAM() {
        const controlBtns = document.querySelectorAll('.control-btn');
        const heatmapOverlay = document.getElementById('heatmapOverlay');
        const focusPoint = document.getElementById('focusPoint');
        const mriDisplay = document.querySelector('.mri-display');
        
        // Control button interactions
        controlBtns.forEach(btn => {
            btn.addEventListener('click', () => {
                controlBtns.forEach(b => b.classList.remove('active'));
                btn.classList.add('active');
                
                // Update heatmap based on selected layer
                const layer = btn.getAttribute('data-layer');
                updateHeatmap(layer);
                
                // Show layer name in focus point
                focusPoint.textContent = layer.toUpperCase();
                focusPoint.style.fontSize = '0.7rem';
                focusPoint.style.display = 'flex';
                focusPoint.style.alignItems = 'center';
                focusPoint.style.justifyContent = 'center';
                focusPoint.style.color = 'white';
                focusPoint.style.fontWeight = 'bold';
            });
        });
        
        // Interactive heatmap
        mriDisplay.addEventListener('mousemove', (e) => {
            const rect = mriDisplay.getBoundingClientRect();
            const x = ((e.clientX - rect.left) / rect.width) * 100;
            const y = ((e.clientY - rect.top) / rect.height) * 100;
            
            focusPoint.style.left = `${x}%`;
            focusPoint.style.top = `${y}%`;
            focusPoint.style.opacity = '1';
            
            // Calculate heatmap intensity based on position
            const centerX = 50;
            const centerY = 50;
            const distance = Math.sqrt(Math.pow(x - centerX, 2) + Math.pow(y - centerY, 2));
            const maxDistance = 70; // Maximum distance from center
            const intensity = 1 - (distance / maxDistance);
            
            // Update heatmap opacity based on mouse position
            heatmapOverlay.style.opacity = 0.5 + intensity * 0.5;
            
            // Create ripple effect
            createHeatmapRipple(x, y, intensity);
        });
        
        mriDisplay.addEventListener('mouseleave', () => {
            focusPoint.style.opacity = '0';
            heatmapOverlay.style.opacity = '0.8';
            
            // Remove all ripple effects
            document.querySelectorAll('.heatmap-ripple').forEach(ripple => {
                ripple.remove();
            });
        });
        
        // Click to highlight area
        mriDisplay.addEventListener('click', (e) => {
            const rect = mriDisplay.getBoundingClientRect();
            const x = ((e.clientX - rect.left) / rect.width) * 100;
            const y = ((e.clientY - rect.top) / rect.height) * 100;
            
            // Create a temporary highlight
            const highlight = document.createElement('div');
            highlight.className = 'heatmap-highlight';
            highlight.style.position = 'absolute';
            highlight.style.left = `${x}%`;
            highlight.style.top = `${y}%`;
            highlight.style.transform = 'translate(-50%, -50%)';
            highlight.style.width = '60px';
            highlight.style.height = '60px';
            highlight.style.background = 'radial-gradient(circle, rgba(0,212,255,0.8) 0%, transparent 70%)';
            highlight.style.borderRadius = '50%';
            highlight.style.pointerEvents = 'none';
            highlight.style.zIndex = '5';
            highlight.style.animation = 'pulse 1s ease-out';
            
            mriDisplay.appendChild(highlight);
            
            // Remove after animation
            setTimeout(() => {
                highlight.remove();
            }, 1000);
        });
        
        function updateHeatmap(layer) {
            // Remove all layer classes
            heatmapOverlay.classList.remove('conv-layer', 'attention-layer', 'ensemble-layer');
            
            // Add appropriate class based on selected layer
            switch(layer) {
                case 'conv':
                    heatmapOverlay.classList.add('conv-layer');
                    break;
                case 'attention':
                    heatmapOverlay.classList.add('attention-layer');
                    break;
                case 'ensemble':
                    heatmapOverlay.classList.add('ensemble-layer');
                    break;
            }
            
            // Update legend title
            document.querySelector('.legend-title').textContent = 
                `${layer.charAt(0).toUpperCase() + layer.slice(1)} Layer Heatmap`;
        }
        
        function createHeatmapRipple(x, y, intensity) {
            // Remove old ripples
            document.querySelectorAll('.heatmap-ripple').forEach(ripple => {
                ripple.remove();
            });
            
            // Create new ripple
            const ripple = document.createElement('div');
            ripple.className = 'heatmap-ripple';
            ripple.style.position = 'absolute';
            ripple.style.left = `${x}%`;
            ripple.style.top = `${y}%`;
            ripple.style.transform = 'translate(-50%, -50%)';
            ripple.style.width = `${30 + intensity * 50}px`;
            ripple.style.height = `${30 + intensity * 50}px`;
            ripple.style.background = `radial-gradient(circle, rgba(255,255,255,${0.2 * intensity}) 0%, transparent 70%)`;
            ripple.style.borderRadius = '50%';
            ripple.style.pointerEvents = 'none';
            ripple.style.zIndex = '4';
            
            mriDisplay.appendChild(ripple);
            
            // Remove after short time
            setTimeout(() => {
                ripple.remove();
            }, 500);
        }
        
        // Initialize with attention layer
        updateHeatmap('attention');
        
        // Add animation to comparison models
        const attentionVisuals = document.querySelectorAll('.attention-visual');
        attentionVisuals.forEach((visual, index) => {
            visual.style.animationDelay = `${index * 0.5}s`;
        });
    }

    // Dashboard Initialization
    function initializeDashboard() {
        // Confusion Matrix
        const confusionModelSelect = document.getElementById('confusionModel');
        const matrixGrid = document.querySelector('.matrix-grid');
        
        function generateConfusionMatrix(model) {
            matrixGrid.innerHTML = '';
            
            // Generate 3x3 matrix
            for (let i = 0; i < 3; i++) {
                for (let j = 0; j < 3; j++) {
                    const cell = document.createElement('div');
                    cell.className = 'matrix-cell';
                    
                    if (i === j) {
                        // Diagonal (correct predictions)
                        cell.className += ' diagonal';
                        cell.textContent = Math.floor(Math.random() * 40 + 60); // 60-100
                    } else {
                        // Off-diagonal (incorrect predictions)
                        cell.className += ' off-diagonal';
                        cell.textContent = Math.floor(Math.random() * 10); // 0-10
                    }
                    
                    matrixGrid.appendChild(cell);
                }
            }
        }
        
        confusionModelSelect.addEventListener('change', (e) => {
            generateConfusionMatrix(e.target.value);
        });
        
        // Initialize matrix
        generateConfusionMatrix('ensemble');
        
        // Performance toggle
        const toggleBtns = document.querySelectorAll('.toggle-btn');
        toggleBtns.forEach(btn => {
            btn.addEventListener('click', () => {
                toggleBtns.forEach(b => b.classList.remove('active'));
                btn.classList.add('active');
                
                // In a real app, this would update the chart data
                console.log(`Switched to ${btn.getAttribute('data-metric')} view`);
            });
        });
    }

    // Event Listeners Setup
    function setupEventListeners() {
        // Back to top button
        const backToTop = document.getElementById('backToTop');
        
        window.addEventListener('scroll', () => {
            if (window.pageYOffset > 300) {
                backToTop.classList.add('visible');
            } else {
                backToTop.classList.remove('visible');
            }
        });
        
        backToTop.addEventListener('click', () => {
            window.scrollTo({ top: 0, behavior: 'smooth' });
        });
        
        // Navigation smooth scroll
        document.querySelectorAll('a[href^="#"]').forEach(anchor => {
            anchor.addEventListener('click', function(e) {
                e.preventDefault();
                
                const targetId = this.getAttribute('href');
                if (targetId === '#') return;
                
                const targetElement = document.querySelector(targetId);
                if (targetElement) {
                    window.scrollTo({
                        top: targetElement.offsetTop - 80,
                        behavior: 'smooth'
                    });
                }
            });
        });
        
        // Update active nav link on scroll
        window.addEventListener('scroll', updateActiveNavLink);
        
        function updateActiveNavLink() {
            const sections = document.querySelectorAll('section[id]');
            const navLinks = document.querySelectorAll('.nav-link');
            
            let current = '';
            sections.forEach(section => {
                const sectionTop = section.offsetTop;
                const sectionHeight = section.clientHeight;
                if (scrollY >= sectionTop - 100) {
                    current = section.getAttribute('id');
                }
            });
            
            navLinks.forEach(link => {
                link.classList.remove('active');
                if (link.getAttribute('href') === `#${current}`) {
                    link.classList.add('active');
                }
            });
        }
        
        // Initialize
        updateActiveNavLink();
    }

    // Add some interactive effects
    document.querySelectorAll('.model-card').forEach(card => {
        card.addEventListener('mouseenter', () => {
            card.style.transform = 'translateY(-10px) scale(1.02)';
        });
        
        card.addEventListener('mouseleave', () => {
            card.style.transform = 'translateY(0) scale(1)';
        });
    });

    // Auto-rotate brain model on hover
    const brainModel = document.querySelector('.brain-model');
    brainModel.addEventListener('mouseenter', () => {
        brainModel.style.animationPlayState = 'paused';
    });
    
    brainModel.addEventListener('mouseleave', () => {
        brainModel.style.animationPlayState = 'running';
    });
});
// Add these to your existing script.js file

const API_BASE_URL = 'http://localhost:5000';

// Initialize API connection
async function initializeAPI() {
    try {
        const response = await fetch(`${API_BASE_URL}/api/health`);
        const data = await response.json();
        console.log('Backend connected:', data);
        return true;
    } catch (error) {
        console.error('Backend connection failed:', error);
        return false;
    }
}

// Update the performAnalysis function to use real API
async function performAnalysis() {
    const fileInput = document.getElementById('fileInput');
    const analyzeBtn = document.getElementById('analyzeBtn');
    const statusIndicator = document.querySelector('.status-indicator');
    const statusText = document.querySelector('.status-text');
    
    if (!fileInput.files.length) {
        alert('Please upload an MRI image first');
        return;
    }
    
    // Update UI
    statusIndicator.className = 'status-indicator processing';
    statusText.textContent = 'Analyzing...';
    analyzeBtn.innerHTML = '<i class="fas fa-spinner fa-spin"></i> Processing';
    analyzeBtn.disabled = true;
    
    try {
        // 1. Upload file
        const formData = new FormData();
        formData.append('file', fileInput.files[0]);
        
        const uploadResponse = await fetch(`${API_BASE_URL}/api/upload`, {
            method: 'POST',
            body: formData
        });
        
        const uploadData = await uploadResponse.json();
        
        if (!uploadData.success) {
            throw new Error(uploadData.error || 'Upload failed');
        }
        
        // 2. Analyze image
        const analyzeResponse = await fetch(`${API_BASE_URL}/api/analyze`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({
                filename: uploadData.filename,
                model: 'ensemble'
            })
        });
        
        const analysisData = await analyzeResponse.json();
        
        if (!analysisData.success) {
            throw new Error(analysisData.error || 'Analysis failed');
        }
        
        // 3. Update UI with real data
        updatePredictionsUI(analysisData.predictions);
        updateConfidenceMeter(analysisData.confidence);
        updateRecommendation(analysisData.recommendation);
        updateGradCAM(analysisData.heatmap);
        
        // 4. Update status
        statusIndicator.className = 'status-indicator';
        statusText.textContent = 'Analysis Complete';
        analyzeBtn.innerHTML = '<i class="fas fa-redo"></i> Analyze Again';
        analyzeBtn.disabled = false;
        
        console.log('Analysis complete:', analysisData);
        
    } catch (error) {
        console.error('Analysis error:', error);
        
        // Fallback to simulated analysis
        simulateAnalysis();
        
        // Update status
        statusIndicator.className = 'status-indicator';
        statusText.textContent = 'Demo Mode (Backend Error)';
        analyzeBtn.innerHTML = '<i class="fas fa-redo"></i> Try Again';
        analyzeBtn.disabled = false;
    }
}

// Update the simulateFileUpload function
async function simulateFileUpload(tumorType) {
    try {
        // Use demo API for sample scans
        const response = await fetch(`${API_BASE_URL}/api/predict/demo`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({
                tumor_type: tumorType
            })
        });
        
        const data = await response.json();
        
        if (data.success) {
            // Update predictions
            updatePredictionsUI(data.predictions);
            updateConfidenceMeter(data.confidence);
            
            // Show success message
            showNotification(`Sample ${tumorType} analysis complete!`, 'success');
        }
    } catch (error) {
        console.error('Demo API error:', error);
        // Fallback to local simulation
        simulateAnalysis(tumorType);
    }
}

// Add notification function
function showNotification(message, type = 'info') {
    const notification = document.createElement('div');
    notification.className = `notification ${type}`;
    notification.innerHTML = `
        <i class="fas fa-${type === 'success' ? 'check-circle' : 'info-circle'}"></i>
        <span>${message}</span>
        <button class="close-btn">&times;</button>
    `;
    
    document.body.appendChild(notification);
    
    // Auto remove after 5 seconds
    setTimeout(() => {
        notification.remove();
    }, 5000);
    
    // Close button
    notification.querySelector('.close-btn').addEventListener('click', () => {
        notification.remove();
    });
}

// Add CSS for notifications
const notificationCSS = `
.notification {
    position: fixed;
    top: 20px;
    right: 20px;
    padding: 15px 20px;
    border-radius: 10px;
    background: white;
    box-shadow: 0 5px 20px rgba(0,0,0,0.2);
    display: flex;
    align-items: center;
    gap: 10px;
    z-index: 10000;
    animation: slideIn 0.3s ease;
    border-left: 4px solid;
}

.notification.success {
    border-left-color: #00c853;
}

.notification.info {
    border-left-color: #2196f3;
}

.notification.error {
    border-left-color: #ff1744;
}

.notification i {
    font-size: 1.2rem;
}

.notification.success i {
    color: #00c853;
}

.notification.info i {
    color: #2196f3;
}

.notification.error i {
    color: #ff1744;
}

.close-btn {
    background: none;
    border: none;
    font-size: 1.5rem;
    cursor: pointer;
    color: #666;
    padding: 0;
    margin-left: 10px;
}

@keyframes slideIn {
    from {
        transform: translateX(100%);
        opacity: 0;
    }
    to {
        transform: translateX(0);
        opacity: 1;
    }
}
`;

// Add notification CSS to page
const style = document.createElement('style');
style.textContent = notificationCSS;
document.head.appendChild(style);

// Initialize on page load
document.addEventListener('DOMContentLoaded', async () => {
    const isConnected = await initializeAPI();
    
    if (isConnected) {
        showNotification('Connected to NeuroVision AI backend', 'success');
    } else {
        showNotification('Running in demo mode (backend not connected)', 'info');
    }
    
    // Update analyze button to use API
    const analyzeBtn = document.getElementById('analyzeBtn');
    if (analyzeBtn) {
        analyzeBtn.removeEventListener('click', performAnalysis); // Remove old listener
        analyzeBtn.addEventListener('click', performAnalysis);    // Add new listener
    }
});
// Add this function to handle analysis completion
function updatePredictionsUI(predictions) {
    const predictionsList = document.querySelector('.predictions-list');
    if (!predictionsList) return;
    
    predictionsList.innerHTML = '';
    
    predictions.forEach(pred => {
        const item = document.createElement('div');
        item.className = 'prediction-item';
        item.innerHTML = `
            <div class="tumor-type">
                <div class="tumor-type-icon ${pred.type.toLowerCase()}">${pred.type.charAt(0)}</div>
                <span>${pred.type}</span>
            </div>
            <div class="prediction-bar">
                <div class="prediction-fill ${pred.type.toLowerCase()}" 
                     style="width: ${pred.confidence}%"></div>
            </div>
            <div class="prediction-value">${pred.confidence}%</div>
        `;
        predictionsList.appendChild(item);
    });
}

function updateRecommendation(text) {
    const recElement = document.getElementById('recommendationText');
    if (recElement) {
        recElement.textContent = text;
    }
}

function updateGradCAM(heatmapData) {
    const heatmapOverlay = document.getElementById('heatmapOverlay');
    if (heatmapOverlay && heatmapData) {
        heatmapOverlay.style.backgroundImage = `url(${heatmapData})`;
        heatmapOverlay.style.opacity = '0.8';
    }
}

// Make sure analyze button is connected
document.addEventListener('DOMContentLoaded', function() {
    const analyzeBtn = document.getElementById('analyzeBtn');
    if (analyzeBtn) {
        // Remove any existing listeners
        analyzeBtn.replaceWith(analyzeBtn.cloneNode(true));
        
        // Get new button reference
        const newAnalyzeBtn = document.getElementById('analyzeBtn');
        
        newAnalyzeBtn.addEventListener('click', async function() {
            await performAnalysis();
        });
    }
});
// Add this function to debug the API response
window.debugAnalysis = async function() {
    console.log("Running debug analysis...");
    
    try {
        // Try to get the last uploaded file
        const fileInput = document.getElementById('fileInput');
        if (!fileInput || !fileInput.files.length) {
            console.log("No file selected");
            return;
        }
        
        const file = fileInput.files[0];
        console.log("File:", file.name);
        
        // Create FormData
        const formData = new FormData();
        formData.append('file', file);
        
        // Upload
        console.log("Uploading...");
        const uploadRes = await fetch('http://localhost:5000/api/upload', {
            method: 'POST',
            body: formData
        });
        
        const uploadData = await uploadRes.json();
        console.log("Upload response:", uploadData);
        
        if (!uploadData.success) {
            console.log("Upload failed:", uploadData);
            return;
        }
        
        // Analyze
        console.log("Analyzing...");
        const analyzeRes = await fetch('http://localhost:5000/api/analyze', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                filename: uploadData.filename,
                model: 'ensemble'
            })
        });
        
        const analyzeData = await analyzeRes.json();
        console.log("📊 Analysis response:", analyzeData);
        
        if (analyzeData.success) {
            console.log("Diagnosis:", analyzeData.diagnosis);
            console.log("Confidence:", analyzeData.confidence);
            console.log("Predictions:", analyzeData.predictions);
        } else {
            console.log("Analysis failed:", analyzeData);
        }
        
    } catch (error) {
        console.log("Error:", error);
    }
}
// Add this at the END of your script.js file
window.debugAnalysis = async function() {
    console.log("Running debug analysis...");
    console.log("This will help us find why analysis is stuck");
    
    try {
        // Check if file input exists
        const fileInput = document.getElementById('fileInput');
        if (!fileInput) {
            console.log("File input element not found");
            return;
        }
        
        if (!fileInput.files.length) {
            console.log("No file selected");
            alert("Please select a file first");
            return;
        }
        
        const file = fileInput.files[0];
        console.log("Selected file:", file.name, "Size:", file.size);
        
        // Test 1: Check if backend is reachable
        console.log("\nTEST 1: Checking backend connection...");
        try {
            const healthRes = await fetch('http://localhost:5000/api/health');
            const healthData = await healthRes.json();
            console.log("Backend health check passed:", healthData);
        } catch (error) {
            console.log("Backend not reachable. Make sure Flask is running on port 5000");
            console.log("Error:", error);
            return;
        }
        
        // Test 2: Upload file
        console.log("\nTEST 2: Uploading file...");
        const formData = new FormData();
        formData.append('file', file);
        
        const uploadRes = await fetch('http://localhost:5000/api/upload', {
            method: 'POST',
            body: formData
        });
        
        console.log("Upload response status:", uploadRes.status);
        const uploadData = await uploadRes.json();
        console.log("Upload response data:", uploadData);
        
        if (!uploadData.success) {
            console.log("Upload failed:", uploadData.error);
            return;
        }
        
        // Test 3: Analyze
        console.log("\nTEST 3: Analyzing image...");
        const analyzeRes = await fetch('http://localhost:5000/api/analyze', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                filename: uploadData.filename,
                model: 'ensemble'
            })
        });
        
        console.log("Analyze response status:", analyzeRes.status);
        const analyzeData = await analyzeRes.json();
        console.log("Analysis response:", analyzeData);
        
        if (analyzeData.success) {
            console.log("\nRESULTS:");
            console.log("Diagnosis:", analyzeData.diagnosis);
            console.log("Confidence:", analyzeData.confidence);
            console.log("Predictions:", analyzeData.predictions);
        } else {
            console.log("Analysis failed:", analyzeData.error);
        }
        
    } catch (error) {
        console.log("Error during debug:", error);
    }
}
from __future__ import annotations
import re
import warnings
import random
import time
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, Iterable, List, Tuple, Optional
import threading

import matplotlib
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
import numpy as np
import pandas as pd
import seaborn as sns
from itertools import product

import tkinter as tk
from tkinter import ttk, scrolledtext, messagebox

from sklearn.exceptions import ConvergenceWarning
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.metrics import accuracy_score, classification_report, confusion_matrix, precision_recall_fscore_support
from sklearn.model_selection import GridSearchCV, train_test_split
from sklearn.pipeline import Pipeline
from sklearn.preprocessing import LabelEncoder
from sklearn.svm import LinearSVC

warnings.filterwarnings("ignore", category=ConvergenceWarning)

# Expanded database with more examples
BASE_RAW_DATA: Dict[str, List[str]] = {
    "sentence": [
        # Greetings
        "hi", "hello", "hey there", "good morning", "good evening", "good afternoon",
        "hiya", "howdy", "greetings", "what's up", "sup", "hey",
        # Goodbyes
        "bye", "see you later", "goodbye", "take care", "farewell", "see ya",
        "catch you later", "until next time", "so long", "adios",
        # Book flight
        "book a flight", "i want to book a ticket", "reserve a seat", "i need a flight",
        "can i book a plane ticket", "schedule a flight", "arrange travel",
        # Cancel ticket
        "cancel my reservation", "i want to cancel my ticket", "void my booking",
        "cancel the flight", "i need to cancel", "refund my ticket",
        # Weather
        "weather today", "what is the weather", "is it going to rain", "weather forecast",
        "how's the weather", "temperature outside", "will it snow",
        # Play music
        "play a song", "play music", "start the playlist", "put on some music",
        "play my favorite song", "turn on music", "play jazz",
        # Control device
        "turn on the light", "switch on the fan", "turn off the light", "turn off the tv",
        "dim the lights", "brighten the room", "power on the heater",
        # Additional intents
        "set an alarm", "wake me up at 7am", "alarm for tomorrow",
        "what time is it", "tell me the time", "current time",
        "remind me to call mom", "set a reminder", "don't forget the meeting",
        "search for restaurants", "find nearby cafes", "where can i eat",
        "translate hello to spanish", "what does bonjour mean", "how do you say thank you",
    ],
    "intent": [
        "greeting", "greeting", "greeting", "greeting", "greeting", "greeting",
        "greeting", "greeting", "greeting", "greeting", "greeting", "greeting",
        "goodbye", "goodbye", "goodbye", "goodbye", "goodbye", "goodbye",
        "goodbye", "goodbye", "goodbye", "goodbye",
        "book_flight", "book_flight", "book_flight", "book_flight",
        "book_flight", "book_flight", "book_flight",
        "cancel_ticket", "cancel_ticket", "cancel_ticket", "cancel_ticket",
        "cancel_ticket", "cancel_ticket",
        "weather", "weather", "weather", "weather", "weather", "weather", "weather",
        "play_music", "play_music", "play_music", "play_music",
        "play_music", "play_music", "play_music",
        "control_device", "control_device", "control_device", "control_device",
        "control_device", "control_device", "control_device",
        "set_alarm", "set_alarm", "set_alarm",
        "get_time", "get_time", "get_time",
        "set_reminder", "set_reminder", "set_reminder",
        "search", "search", "search",
        "translate", "translate", "translate",
    ],
}

# Expanded template configuration with more variations and higher limits
TEMPLATE_CONFIG = {
    "greeting": {
        "parts": [
            ["hi", "hello", "hey", "good morning", "good evening", "good afternoon", "what's up", "howdy", "greetings"],
            ["", "there", "everyone", "team", "folks", "friend", "sir", "ma'am", "guys"],
            ["", "today", "again", "this morning", "how are you", "nice to see you"],
        ],
        "limit": 60,
    },
    "goodbye": {
        "parts": [
            ["bye", "goodbye", "see you", "catch you later", "talk soon", "farewell", "so long", "adios"],
            ["", "everyone", "for now", "later", "tomorrow", "soon"],
            ["", "and take care", "stay safe", "have a good day", "until next time"],
        ],
        "limit": 50,
    },
    "book_flight": {
        "parts": [
            ["book", "reserve", "schedule", "arrange", "get", "purchase", "buy"],
            ["a flight", "my ticket", "a plane ride", "a seat", "travel", "a trip"],
            ["to New York", "to London", "to Tokyo", "to Berlin", "to Sydney", "to Paris", "to Rome", "to Dubai"],
            ["", "for tomorrow", "for next week", "for Friday night", "one way", "round trip"],
        ],
        "limit": 50,
    },
    "cancel_ticket": {
        "parts": [
            ["cancel", "void", "stop", "change", "refund"],
            ["my ticket", "my reservation", "the booking", "my seat", "the flight"],
            ["", "for tonight", "immediately", "that I made", "please"],
        ],
        "limit": 40,
    },
    "weather": {
        "parts": [
            ["what's", "tell me", "i need", "show me", "check"],
            ["the weather", "the forecast", "weather conditions", "temperature"],
            ["today", "for tomorrow", "for this weekend", "outside", "right now"],
            ["", "in New York", "in London", "downtown", "here"],
        ],
        "limit": 50,
    },
    "play_music": {
        "parts": [
            ["play", "start", "queue", "put on", "turn on"],
            ["some music", "the playlist", "my favorite songs", "jazz tunes", "rock music", "classical"],
            ["", "now", "for dinner", "while I cook", "loud", "quiet"],
        ],
        "limit": 45,
    },
    "control_device": {
        "parts": [
            ["turn", "switch", "power", "set"],
            ["on", "off", "up", "down"],
            ["the lights", "the lamp", "the fan", "the heater", "the air purifier", "the tv", "the ac"],
            ["", "in the living room", "in the bedroom", "outdoors", "please"],
        ],
        "limit": 50,
    },
    "set_alarm": {
        "parts": [
            ["set", "create", "schedule", "wake me up"],
            ["an alarm", "a wake up call", "the alarm"],
            ["at 7am", "at 8am", "for 6am", "for tomorrow", "in the morning"],
        ],
        "limit": 35,
    },
    "get_time": {
        "parts": [
            ["what time", "tell me the time", "what's the time", "current time"],
            ["", "is it", "right now", "please"],
        ],
        "limit": 30,
    },
    "set_reminder": {
        "parts": [
            ["remind me", "set a reminder", "don't forget"],
            ["to call mom", "about the meeting", "to buy groceries", "the appointment"],
            ["", "tomorrow", "later", "at 3pm"],
        ],
        "limit": 35,
    },
    "search": {
        "parts": [
            ["search for", "find", "look for", "where can i"],
            ["restaurants", "nearby cafes", "hotels", "gas stations", "pharmacies"],
            ["", "near me", "in the area", "close by"],
        ],
        "limit": 40,
    },
    "translate": {
        "parts": [
            ["translate", "what does", "how do you say"],
            ["hello", "thank you", "goodbye", "please", "yes", "no"],
            ["to spanish", "to french", "to german", "in english", "mean"],
        ],
        "limit": 35,
    },
}


def generate_from_parts(parts: List[List[str]], limit: int, seen: set[str], randomize: bool = True) -> List[str]:
    sentences: List[str] = []
    all_combos = list(product(*parts))
    
    # Randomize the order of combinations for variety
    if randomize:
        random.shuffle(all_combos)
    
    for combo in all_combos:
        sentence = " ".join(part for part in combo if part).strip()
        normalized = sentence.lower()
        if not sentence or normalized in seen:
            continue
        sentences.append(sentence)
        seen.add(normalized)
        if len(sentences) >= limit:
            break
    return sentences


def build_raw_dataset(random_seed: Optional[int] = None) -> Dict[str, List[str]]:
    # Use random seed for different data each time
    if random_seed is None:
        random_seed = int(time.time() * 1000) % 1000000
    random.seed(random_seed)
    np.random.seed(random_seed)
    
    sentences = list(BASE_RAW_DATA["sentence"])
    intents = list(BASE_RAW_DATA["intent"])
    seen = {s.lower() for s in sentences}

    # Shuffle base data order
    combined = list(zip(sentences, intents))
    random.shuffle(combined)
    sentences, intents = zip(*combined)
    sentences, intents = list(sentences), list(intents)

    # Randomly vary template limits and order
    template_items = list(TEMPLATE_CONFIG.items())
    random.shuffle(template_items)
    
    for intent, cfg in template_items:
        # Vary the limit by ±20% for randomness
        base_limit = cfg["limit"]
        variation = int(base_limit * 0.2)
        limit = random.randint(base_limit - variation, base_limit + variation)
        limit = max(20, limit)  # Ensure minimum limit
        
        generated = generate_from_parts(cfg["parts"], limit, seen, randomize=True)
        sentences.extend(generated)
        intents.extend([intent] * len(generated))

    # Shuffle final dataset
    combined = list(zip(sentences, intents))
    random.shuffle(combined)
    sentences, intents = zip(*combined)
    
    return {"sentence": list(sentences), "intent": list(intents)}


def clean_text(text: str) -> str:
    """Lowercase + keep alphabetic characters and single spaces."""
    lowered = text.lower()
    lowered = re.sub(r"[^a-zA-Z ]", " ", lowered)
    normalized = re.sub(r"\s+", " ", lowered).strip()
    return normalized


@dataclass
class IntentModel:
    pipeline: Pipeline
    label_encoder: LabelEncoder
    training_stats: Dict[str, float]
    test_metrics: Dict[str, object]
    df: pd.DataFrame
    X_train: pd.Series
    X_test: pd.Series
    y_train: np.ndarray
    y_test: np.ndarray

    def predict(self, text: str) -> str:
        cleaned = clean_text(text)
        encoded_prediction = self.pipeline.predict([cleaned])[0]
        return self.label_encoder.inverse_transform([encoded_prediction])[0]


def prepare_dataframe(random_seed: Optional[int] = None) -> pd.DataFrame:
    df = pd.DataFrame(build_raw_dataset(random_seed))
    df["clean_sentence"] = df["sentence"].apply(clean_text)
    return df


def encode_labels(intents: Iterable[str]) -> Tuple[np.ndarray, LabelEncoder]:
    encoder = LabelEncoder()
    encoded = encoder.fit_transform(list(intents))
    return encoded, encoder


def train_classifier(
    sentences: Iterable[str], labels: np.ndarray
) -> Tuple[Pipeline, Dict[str, float]]:
    base_pipeline = Pipeline(
        [
            (
                "tfidf",
                TfidfVectorizer(
                    stop_words="english",
                    ngram_range=(1, 2),
                    min_df=1,
                    max_df=0.95,
                ),
            ),
            ("clf", LinearSVC(C=1.0, class_weight="balanced", max_iter=5000)),
        ]
    )

    param_grid = {
        "tfidf__ngram_range": [(1, 1), (1, 2)],
        "tfidf__max_df": [0.85, 0.95, 1.0],
        "clf__C": [0.5, 1.0, 1.5],
    }

    grid = GridSearchCV(
        base_pipeline,
        param_grid,
        cv=3,
        scoring="f1_macro",
        n_jobs=-1,
        verbose=0,
    )
    grid.fit(list(sentences), labels)

    stats = {
        "cv_best_f1": float(grid.best_score_),
    }
    stats.update({f"param_{k}": v for k, v in grid.best_params_.items()})
    return grid.best_estimator_, stats


def evaluate_classifier(
    model: Pipeline, sentences: Iterable[str], labels: np.ndarray, encoder: LabelEncoder
) -> Dict[str, object]:
    y_pred = model.predict(list(sentences))
    accuracy = accuracy_score(labels, y_pred)
    report = classification_report(
        labels,
        y_pred,
        target_names=encoder.classes_,
        zero_division=0,
        output_dict=True,
    )
    cm = confusion_matrix(labels, y_pred)
    precision, recall, f1, _ = precision_recall_fscore_support(labels, y_pred, average=None, zero_division=0)
    
    return {
        "accuracy": accuracy,
        "report": report,
        "confusion_matrix": cm,
        "precision": precision,
        "recall": recall,
        "f1": f1,
        "y_pred": y_pred,
    }


class IntentClassifierGUI:
    # Dark theme colors
    BG_COLOR = "#1e1e1e"
    FG_COLOR = "#d4d4d4"
    ACCENT_COLOR = "#007acc"
    SUCCESS_COLOR = "#4ec9b0"
    WARNING_COLOR = "#ce9178"
    ERROR_COLOR = "#f48771"
    BUTTON_BG = "#2d2d30"
    BUTTON_HOVER = "#3e3e42"
    ENTRY_BG = "#252526"
    FRAME_BG = "#252526"
    
    def __init__(self, root: tk.Tk):
        self.root = root
        self.root.title("Intent Classifier - Interactive Interface")
        self.root.geometry("1400x900")
        self.root.configure(bg=self.BG_COLOR)
        
        self.intent_model: Optional[IntentModel] = None
        self.training_in_progress = False
        self.current_seed = None
        
        self.setup_dark_theme()
        self.setup_ui()
        self.train_model()
    
    def setup_dark_theme(self):
        """Configure dark theme for matplotlib"""
        plt.style.use('dark_background')
        sns.set_style("darkgrid")
        sns.set_palette("husl")
    
    def setup_ui(self):
        # Configure style
        style = ttk.Style()
        style.theme_use('clam')
        
        # Configure dark theme colors for ttk widgets
        style.configure('TFrame', background=self.BG_COLOR)
        style.configure('TLabel', background=self.BG_COLOR, foreground=self.FG_COLOR)
        style.configure('TLabelFrame', background=self.BG_COLOR, foreground=self.FG_COLOR, borderwidth=2)
        style.configure('TLabelFrame.Label', background=self.BG_COLOR, foreground=self.ACCENT_COLOR, font=('Segoe UI', 10, 'bold'))
        style.configure('TButton', background=self.BUTTON_BG, foreground=self.FG_COLOR, borderwidth=1, relief='flat', padding=8)
        style.map('TButton', background=[('active', self.BUTTON_HOVER), ('pressed', self.ACCENT_COLOR)])
        
        main_frame = ttk.Frame(self.root, padding="15")
        main_frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        self.root.columnconfigure(0, weight=1)
        self.root.rowconfigure(0, weight=1)
        main_frame.columnconfigure(1, weight=1)
        main_frame.rowconfigure(0, weight=1)
        
        # Left panel - Input and controls
        left_panel = ttk.Frame(main_frame, width=380)
        left_panel.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S), padx=(0, 15))
        left_panel.grid_propagate(False)
        
        # Title with icon-like styling
        title_frame = tk.Frame(left_panel, bg=self.BG_COLOR)
        title_frame.pack(fill=tk.X, pady=(0, 15))
        
        title_label = tk.Label(title_frame, text="🎯 Intent Classifier", 
                              font=("Segoe UI", 18, "bold"), 
                              bg=self.BG_COLOR, fg=self.ACCENT_COLOR)
        title_label.pack()
        
        subtitle_label = tk.Label(title_frame, text="AI-Powered Text Intent Recognition", 
                                 font=("Segoe UI", 9), 
                                 bg=self.BG_COLOR, fg=self.FG_COLOR)
        subtitle_label.pack(pady=(5, 0))
        
        # Text input section
        input_frame = ttk.LabelFrame(left_panel, text="📝 Enter Text", padding="12")
        input_frame.pack(fill=tk.BOTH, expand=True, pady=(0, 12))
        
        self.text_input = scrolledtext.ScrolledText(
            input_frame, height=6, width=42, wrap=tk.WORD,
            bg=self.ENTRY_BG, fg=self.FG_COLOR, 
            insertbackground=self.ACCENT_COLOR,
            selectbackground=self.ACCENT_COLOR,
            selectforeground="white",
            font=("Segoe UI", 10),
            relief=tk.FLAT, borderwidth=2, highlightthickness=1,
            highlightbackground="#3e3e42", highlightcolor=self.ACCENT_COLOR
        )
        self.text_input.pack(fill=tk.BOTH, expand=True)
        self.text_input.bind("<Return>", lambda e: self.predict_intent())
        self.text_input.bind("<Control-Return>", lambda e: self.predict_intent())
        
        predict_btn = tk.Button(input_frame, text="🔮 Predict Intent", 
                               command=self.predict_intent,
                               bg=self.ACCENT_COLOR, fg="white",
                               font=("Segoe UI", 10, "bold"),
                               relief=tk.FLAT, padx=20, pady=8,
                               cursor="hand2",
                               activebackground="#005a9e",
                               activeforeground="white")
        predict_btn.pack(pady=(12, 0))
        
        # Prediction result
        result_frame = ttk.LabelFrame(left_panel, text="✨ Prediction Result", padding="12")
        result_frame.pack(fill=tk.X, pady=(0, 12))
        
        self.result_label = tk.Label(result_frame, text="⏳ No prediction yet", 
                                     font=("Segoe UI", 11, "bold"),
                                     bg=self.FRAME_BG, fg=self.FG_COLOR,
                                     wraplength=350, justify=tk.LEFT)
        self.result_label.pack(anchor=tk.W)
        
        self.confidence_label = tk.Label(result_frame, text="", 
                                        font=("Segoe UI", 9),
                                        bg=self.FRAME_BG, fg="#888888",
                                        wraplength=350, justify=tk.LEFT)
        self.confidence_label.pack(anchor=tk.W, pady=(5, 0))
        
        # Control buttons with icons
        control_frame = ttk.LabelFrame(left_panel, text="⚙️ Controls", padding="10")
        control_frame.pack(fill=tk.X, pady=(0, 12))
        
        buttons = [
            ("🔄 Retrain Model", self.train_model, self.ACCENT_COLOR),
            ("📊 Confusion Matrix", self.show_confusion_matrix, "#9b59b6"),
            ("📈 Class Distribution", self.show_class_distribution, "#e67e22"),
            ("📉 Accuracy Metrics", self.show_accuracy_metrics, "#27ae60"),
            ("📋 Precision/Recall", self.show_precision_recall, "#3498db"),
            ("ℹ️ Dataset Info", self.show_dataset_info, "#95a5a6"),
        ]
        
        for text, command, color in buttons:
            btn = tk.Button(control_frame, text=text, command=command,
                           bg=self.BUTTON_BG, fg=self.FG_COLOR,
                           font=("Segoe UI", 9),
                           relief=tk.FLAT, padx=10, pady=6,
                           cursor="hand2",
                           activebackground=color,
                           activeforeground="white",
                           anchor=tk.W)
            btn.pack(fill=tk.X, pady=3)
        
        # Status bar with progress indicator
        status_frame = tk.Frame(left_panel, bg=self.BG_COLOR)
        status_frame.pack(fill=tk.X, pady=(10, 0))
        
        self.status_label = tk.Label(status_frame, text="✅ Ready", 
                                     relief=tk.FLAT, anchor=tk.W, padx=10, pady=8,
                                     bg="#2d2d30", fg=self.SUCCESS_COLOR,
                                     font=("Segoe UI", 9))
        self.status_label.pack(fill=tk.X)
        
        # Right panel - Graph display
        graph_frame = ttk.LabelFrame(main_frame, text="📊 Visualizations", padding="12")
        graph_frame.grid(row=0, column=1, sticky=(tk.W, tk.E, tk.N, tk.S))
        graph_frame.columnconfigure(0, weight=1)
        graph_frame.rowconfigure(0, weight=1)
        
        self.figure = Figure(figsize=(9, 7), dpi=100, facecolor='#1e1e1e')
        self.canvas = FigureCanvasTkAgg(self.figure, graph_frame)
        self.canvas.get_tk_widget().configure(bg=self.BG_COLOR)
        self.canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)
        
        # Initial empty plot with dark theme
        ax = self.figure.add_subplot(111, facecolor='#1e1e1e')
        ax.text(0.5, 0.5, "Select a visualization from the controls\n\n📊 Choose any graph option to get started", 
                ha='center', va='center', fontsize=13, 
                color=self.FG_COLOR, transform=ax.transAxes,
                family='Segoe UI')
        ax.axis('off')
        self.canvas.draw()
    
    def update_status(self, message: str, status_type: str = "info"):
        """Update status bar with color coding"""
        colors = {
            "info": self.FG_COLOR,
            "success": self.SUCCESS_COLOR,
            "warning": self.WARNING_COLOR,
            "error": self.ERROR_COLOR,
            "training": self.ACCENT_COLOR
        }
        icons = {
            "info": "ℹ️",
            "success": "✅",
            "warning": "⚠️",
            "error": "❌",
            "training": "🔄"
        }
        self.status_label.config(
            text=f"{icons.get(status_type, 'ℹ️')} {message}",
            fg=colors.get(status_type, self.FG_COLOR)
        )
        self.root.update_idletasks()
    
    def train_model(self):
        if self.training_in_progress:
            messagebox.showwarning("Training", "Model training already in progress!")
            return
        
        def train_thread():
            self.training_in_progress = True
            # Generate new random seed for different data
            self.current_seed = int(time.time() * 1000) % 1000000
            self.update_status(f"Training model with new random data (seed: {self.current_seed})...", "training")
            
            try:
                df = prepare_dataframe(random_seed=self.current_seed)
                labels, label_encoder = encode_labels(df["intent"])

                # Use random state based on seed for train/test split
                split_seed = self.current_seed % 1000
                X_train, X_test, y_train, y_test = train_test_split(
                    df["clean_sentence"],
                    labels,
                    test_size=0.25,
                    random_state=split_seed,
                    stratify=labels,
                )


                model_pipeline, stats = train_classifier(X_train, y_train)
                metrics = evaluate_classifier(model_pipeline, X_test, y_test, label_encoder)
                self.intent_model = IntentModel(
                    model_pipeline, label_encoder, stats, metrics,
                    df, X_train, X_test, y_train, y_test
                )
                
                self.update_status(
                    f"Model trained! Accuracy: {metrics['accuracy']:.3f} | "
                    f"Dataset: {len(df)} samples | Classes: {len(label_encoder.classes_)}",
                    "success"
                )
                messagebox.showinfo("Training Complete", 
                                  f"Model trained successfully with NEW random data!\n\n"
                                  f"📊 Accuracy: {metrics['accuracy']:.3f}\n"
                                  f"📈 Dataset size: {len(df)} samples\n"
                                  f"🏷️ Number of classes: {len(label_encoder.classes_)}\n"
                                  f"🎲 Random seed: {self.current_seed}")
            except Exception as e:
                messagebox.showerror("Training Error", f"Error during training: {str(e)}")
                self.update_status(f"Training failed: {str(e)}", "error")
            finally:
                self.training_in_progress = False
        
        threading.Thread(target=train_thread, daemon=True).start()
    
    def predict_intent(self):
        if not self.intent_model:
            messagebox.showwarning("No Model", "Please train the model first!")
            return
        
        text = self.text_input.get("1.0", tk.END).strip()
        if not text:
            messagebox.showwarning("Empty Input", "Please enter some text!")
            return
        
        try:
            prediction = self.intent_model.predict(text)
            # Format intent name nicely
            formatted_intent = prediction.replace("_", " ").title()
            self.result_label.config(
                text=f"🎯 Predicted Intent: {formatted_intent}",
                fg=self.SUCCESS_COLOR
            )
            display_text = text if len(text) <= 60 else text[:57] + "..."
            self.confidence_label.config(
                text=f"📝 Input: \"{display_text}\"",
                fg="#888888"
            )
        except Exception as e:
            self.result_label.config(
                text=f"❌ Error: {str(e)}",
                fg=self.ERROR_COLOR
            )
    
    def show_confusion_matrix(self):
        if not self.intent_model:
            messagebox.showwarning("No Model", "Please train the model first!")
            return
        
        self.figure.clear()
        ax = self.figure.add_subplot(111, facecolor='#1e1e1e')
        
        cm = self.intent_model.test_metrics["confusion_matrix"]
        class_names = [name.replace("_", " ").title() for name in self.intent_model.label_encoder.classes_]
        
        sns.heatmap(cm, annot=True, fmt="d", cmap="viridis", ax=ax,
                   xticklabels=class_names, yticklabels=class_names,
                   cbar_kws={'label': 'Count'})
        ax.set_xlabel("Predicted", fontsize=12, color=self.FG_COLOR)
        ax.set_ylabel("Actual", fontsize=12, color=self.FG_COLOR)
        ax.set_title("Confusion Matrix", fontsize=15, fontweight="bold", color=self.FG_COLOR, pad=15)
        ax.tick_params(colors=self.FG_COLOR)
        plt.setp(ax.get_xticklabels(), rotation=45, ha="right", color=self.FG_COLOR)
        plt.setp(ax.get_yticklabels(), rotation=0, color=self.FG_COLOR)
        
        self.figure.tight_layout()
        self.canvas.draw()
    
    def show_class_distribution(self):
        if not self.intent_model:
            messagebox.showwarning("No Model", "Please train the model first!")
            return
        
        self.figure.clear()
        ax = self.figure.add_subplot(111, facecolor='#1e1e1e')
        
        intent_counts = self.intent_model.df["intent"].value_counts()
        colors = plt.cm.viridis(np.linspace(0.2, 0.8, len(intent_counts)))
        
        bars = ax.bar(range(len(intent_counts)), intent_counts.values, 
                     color=colors, edgecolor='white', linewidth=1.5, alpha=0.8)
        ax.set_xticks(range(len(intent_counts)))
        formatted_names = [name.replace("_", " ").title() for name in intent_counts.index]
        ax.set_xticklabels(formatted_names, rotation=45, ha="right", color=self.FG_COLOR)
        ax.set_ylabel("Number of Samples", fontsize=12, color=self.FG_COLOR)
        ax.set_title("Class Distribution in Dataset", fontsize=15, fontweight="bold", 
                    color=self.FG_COLOR, pad=15)
        ax.grid(axis='y', alpha=0.2, color=self.FG_COLOR)
        ax.set_facecolor('#1e1e1e')
        ax.tick_params(colors=self.FG_COLOR)
        
        # Add value labels on bars
        for bar in bars:
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2., height,
                   f'{int(height)}', ha='center', va='bottom', 
                   fontsize=10, fontweight='bold', color='white')
        
        self.figure.tight_layout()
        self.canvas.draw()
    
    def show_accuracy_metrics(self):
        if not self.intent_model:
            messagebox.showwarning("No Model", "Please train the model first!")
            return
        
        self.figure.clear()
        ax = self.figure.add_subplot(111, facecolor='#1e1e1e')
        
        accuracy = self.intent_model.test_metrics["accuracy"]
        cv_f1 = self.intent_model.training_stats["cv_best_f1"]
        
        metrics = ["Test Accuracy", "CV F1-Score"]
        values = [accuracy, cv_f1]
        colors = [self.SUCCESS_COLOR, self.ACCENT_COLOR]
        
        bars = ax.bar(metrics, values, color=colors, alpha=0.8, 
                     edgecolor='white', linewidth=2)
        ax.set_ylabel("Score", fontsize=12, color=self.FG_COLOR)
        ax.set_title("Model Performance Metrics", fontsize=15, fontweight="bold", 
                    color=self.FG_COLOR, pad=15)
        ax.set_ylim([0, 1.1])
        ax.grid(axis='y', alpha=0.2, color=self.FG_COLOR)
        ax.set_facecolor('#1e1e1e')
        ax.tick_params(colors=self.FG_COLOR)
        
        # Add value labels
        for bar, val in zip(bars, values):
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2., height + 0.02,
                   f'{val:.3f}', ha='center', va='bottom', 
                   fontsize=12, fontweight="bold", color='white')
        
        self.figure.tight_layout()
        self.canvas.draw()
    
    def show_precision_recall(self):
        if not self.intent_model:
            messagebox.showwarning("No Model", "Please train the model first!")
            return
        
        self.figure.clear()
        ax = self.figure.add_subplot(111, facecolor='#1e1e1e')
        
        precision = self.intent_model.test_metrics["precision"]
        recall = self.intent_model.test_metrics["recall"]
        f1 = self.intent_model.test_metrics["f1"]
        class_names = [name.replace("_", " ").title() for name in self.intent_model.label_encoder.classes_]
        
        x = np.arange(len(class_names))
        width = 0.25
        
        bars1 = ax.bar(x - width, precision, width, label='Precision', 
                      color='#e74c3c', alpha=0.8, edgecolor='white', linewidth=1)
        bars2 = ax.bar(x, recall, width, label='Recall', 
                      color='#3498db', alpha=0.8, edgecolor='white', linewidth=1)
        bars3 = ax.bar(x + width, f1, width, label='F1-Score', 
                      color='#2ecc71', alpha=0.8, edgecolor='white', linewidth=1)
        
        ax.set_ylabel("Score", fontsize=12, color=self.FG_COLOR)
        ax.set_title("Precision, Recall, and F1-Score by Class", fontsize=15, 
                    fontweight="bold", color=self.FG_COLOR, pad=15)
        ax.set_xticks(x)
        ax.set_xticklabels(class_names, rotation=45, ha="right", color=self.FG_COLOR)
        ax.legend(loc='upper right', facecolor='#252526', edgecolor='#3e3e42', 
                 labelcolor=self.FG_COLOR)
        ax.set_ylim([0, 1.1])
        ax.grid(axis='y', alpha=0.2, color=self.FG_COLOR)
        ax.set_facecolor('#1e1e1e')
        ax.tick_params(colors=self.FG_COLOR)
        
        self.figure.tight_layout()
        self.canvas.draw()
    
    def show_dataset_info(self):
        if not self.intent_model:
            messagebox.showwarning("No Model", "Please train the model first!")
            return
        
        self.figure.clear()
        ax = self.figure.add_subplot(111, facecolor='#1e1e1e')
        ax.axis('off')
        
        df = self.intent_model.df
        total_samples = len(df)
        num_classes = len(self.intent_model.label_encoder.classes_)
        train_samples = len(self.intent_model.X_train)
        test_samples = len(self.intent_model.X_test)
        accuracy = self.intent_model.test_metrics["accuracy"]
        
        formatted_classes = [c.replace("_", " ").title() for c in self.intent_model.label_encoder.classes_]
        
        info_text = f"""📊 DATASET INFORMATION

📈 Total Samples: {total_samples}
🏷️  Number of Classes: {num_classes}
🎓 Training Samples: {train_samples}
🧪 Test Samples: {test_samples}
✅ Test Accuracy: {accuracy:.3f}

🎲 Random Seed: {self.current_seed if self.current_seed else 'N/A'}

📋 CLASSES:
{', '.join(formatted_classes)}

⚙️ TRAINING PARAMETERS:
Best CV F1-Score: {self.intent_model.training_stats['cv_best_f1']:.3f}
"""
        
        for key, value in self.intent_model.training_stats.items():
            if key.startswith('param_'):
                param_name = key.replace('param_', '').replace('__', ' → ')
                info_text += f"{param_name}: {value}\n"
        
        ax.text(0.1, 0.5, info_text, fontsize=11, family='Segoe UI',
               verticalalignment='center', transform=ax.transAxes,
               color=self.FG_COLOR)
        ax.set_title("Dataset and Model Information", fontsize=15, 
                    fontweight="bold", pad=20, color=self.FG_COLOR)
        
        self.canvas.draw()


def main() -> None:
    root = tk.Tk()
    app = IntentClassifierGUI(root)
    root.mainloop()


if __name__ == "__main__":
    main()

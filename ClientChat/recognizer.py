import os
import sys
import base64
import cv2
import mediapipe as mp
import pickle
import numpy as np
from tensorflow.keras.models import load_model

MODEL_PATH = os.path.join(os.path.dirname(__file__), 'gesture_model.h5')
ENCODER_PATH = os.path.join(os.path.dirname(__file__), 'label_encoder.p')


try:
    model = load_model(MODEL_PATH)
    label_encoder = pickle.load(open(ENCODER_PATH, 'rb'))['encoder']
    print("[INFO] Model and label encoder loaded successfully.", file=sys.stderr)
except Exception as e:
    print(f"[ERROR] Failed to load model or encoder: {e}", file=sys.stderr)
    sys.exit(1)


mp_hands = mp.solutions.hands
hands = mp_hands.Hands(
    static_image_mode=False,
    max_num_hands=1,  
    min_detection_confidence=0.5,  
    min_tracking_confidence=0.5
)


CONFIDENCE_THRESHOLD = 0.75  
FRAME_RESIZE = (320, 240)  
MIN_STABLE_COUNT = 2  

def decode_image_from_base64(base64_str):
    try:
        img_data = base64.b64decode(base64_str)
        np_arr = np.frombuffer(img_data, np.uint8)
        img = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)
        if img is None:
            raise ValueError("Failed to decode image")
        return img
    except Exception as e:
        print(f"[ERROR] decode_image_from_base64: {e}", file=sys.stderr)
        return None


def extract_landmark_features(hand_landmarks):
    landmarks = hand_landmarks.landmark
    x_coords = [lm.x for lm in landmarks]
    y_coords = [lm.y for lm in landmarks]

    min_x, max_x = min(x_coords), max(x_coords)
    min_y, max_y = min(y_coords), max(y_coords)
    width, height = max_x - min_x, max_y - min_y

    data_aux = []
    for lm in landmarks:
        norm_x = (lm.x - min_x) / width if width > 0 else 0
        norm_y = (lm.y - min_y) / height if height > 0 else 0
        data_aux.extend([norm_x, norm_y])

    return np.asarray(data_aux, dtype=np.float32)

def recognize_gesture(frame):
    frame_resized = cv2.resize(frame, FRAME_RESIZE)
    frame_rgb = cv2.cvtColor(frame_resized, cv2.COLOR_BGR2RGB)
    results = hands.process(frame_rgb)

    if not results.multi_hand_landmarks:
        return "nothing"

    largest_hand = max(
        results.multi_hand_landmarks,
        key=lambda hand: max(lm.x for lm in hand.landmark) - min(lm.x for lm in hand.landmark)
    )

    features = extract_landmark_features(largest_hand)
    features = np.expand_dims(features, axis=0)  # (1, 42)
    prediction = model.predict(features, verbose=0)

    confidence = float(np.max(prediction))
    if confidence < CONFIDENCE_THRESHOLD:
        return "nothing"

    class_index = int(np.argmax(prediction))
    predicted_label = label_encoder.inverse_transform([class_index])[0]
    return predicted_label

stable_gesture = ""
stable_count = 0

try:
    while True:
        line = sys.stdin.readline()
        if not line:
            break

        base64_str = line.strip()
        if not base64_str:
            continue

        frame = decode_image_from_base64(base64_str)
        if frame is None:
            continue

        gesture = recognize_gesture(frame)
        print(f"[DEBUG] Recognized gesture: {gesture}", file=sys.stderr)

        if gesture == stable_gesture:
            stable_count += 1
        else:
            stable_gesture = gesture
            stable_count = 1

        if stable_count >= MIN_STABLE_COUNT and gesture != "nothing":
            sys.stdout.write(gesture + '\n')
            sys.stdout.flush()
            stable_gesture = ""
            stable_count = 0

except Exception as e:
    print(f"[ERROR] Runtime exception: {e}", file=sys.stderr)
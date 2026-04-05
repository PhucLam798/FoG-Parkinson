import socket
import numpy as np
import tensorflow as tf
import joblib
import time

ESP_IP = "192.168.4.1"
PORT = 8888
MODEL_PATH = "mlp_fog_model.h5"
SCALER_PATH = "scaler.pkl"

# Load model
model = tf.keras.models.load_model(MODEL_PATH)
scaler = joblib.load(SCALER_PATH)
print("Model loaded!")

# Kết nối ESP
s = socket.socket()
while True:
    try:
        s.connect((ESP_IP, PORT))
        print("Kết nối ESP thành công!")
        break
    except:
        print("Đang chờ ESP...")
        time.sleep(1)
    
buffer = b""

while True:
    try:
        data = s.recv(4096)
        if not data:
            raise Exception("Mất kết nối...")

        buffer += data

        while b"\n" in buffer:
            line, buffer = buffer.split(b"\n", 1)
            txt = line.decode("utf-8", errors="ignore").strip()
            if txt.count(",") != 5:
                continue

            values = np.array(txt.split(","), dtype=np.float32).reshape(1, -1)
            scaled = scaler.transform(values)
            pred = model.predict(scaled, verbose=0)
            label = np.argmax(pred[0])
            if label == 2:  # FoG
                print(label)
                s.sendall(b"FOG\n")
                print("FOG DETECTED")
                time.sleep(4) 
                print("LOAD MODEL")
                model, scaler = load_model_and_scaler()  
            else:
                print(label)

    except Exception as e:
        print("Lỗi kết nối, đang thử lại...")
        s.close()
        s = socket.socket()
        buffer = b""
        time.sleep(2)
        while True:
            try:
                s.connect((ESP_IP, PORT))
                print("Tái kết nối thành công!")
                break
            except:
                time.sleep(1)
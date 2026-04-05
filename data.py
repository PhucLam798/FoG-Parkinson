import socket
import csv
import time
from datetime import datetime

ESP_IP = "192.168.4.1"   # IP của ESP tay
PORT = 8888
FILENAME = "data_sensor.csv"

def connect_to_esp():
    """Kết nối lại khi bị ngắt"""
    while True:
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(0.5)  # Timeout 0.5s
            sock.connect((ESP_IP, PORT))
            print(f"Đã kết nối tới ESP_TAY tại {ESP_IP}:{PORT}")
            return sock
        except Exception as e:
            print(f"Không kết nối được ESP ({e})")
            time.sleep(1)  # Delay retry để tránh loop nhanh

def main():
    with open(FILENAME, "a", newline="") as f:
        writer = csv.writer(f)
        
        # Header với 7 cột (thêm thời gian)
        writer.writerow(["time", "ax_foot", "ay_foot", "az_foot",
                         "ax_hand", "ay_hand", "az_hand"])

        sock = connect_to_esp()
        buffer = ""
        start_time = time.time()  # Thời điểm bắt đầu chương trình

        while True:
            try:
                data = sock.recv(2048).decode(errors="ignore")  
                if not data:
                    raise ConnectionResetError("Mất kết nối tới ESP")

                buffer += data
                while "\n" in buffer:
                    line, buffer = buffer.split("\n", 1)
                    line = line.strip()
                    if line:
                        vals = line.split(",")
                        if len(vals) == 6:
                            # Tính thời gian từ lúc bắt đầu (giây)
                            current_time = time.time() - start_time + 1
                            
                            # Thêm thời gian vào đầu danh sách
                            row_data = [round(current_time, 3)] + vals
                            
                            writer.writerow(row_data)
                            f.flush()  # đảm bảo ghi ngay vào file
                            print(row_data)

            except (socket.timeout, ConnectionResetError, OSError):
                sock.close()
                sock = connect_to_esp()

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n🛑 Dừng chương trình.")
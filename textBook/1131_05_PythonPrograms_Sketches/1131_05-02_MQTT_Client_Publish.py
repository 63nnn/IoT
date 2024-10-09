import paho.mqtt.client as mqtt  # paho-mqtt-2.0.0
import random
import json
import datetime
import time

# 設置日期時間的格式
ISOTIMEFORMAT = "%m/%d %H:%M:%S"

# 連線設定
# 初始化地端程式
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)  # 初始化地端程式

client.connect("140.128.99.71", 1883, 60)  # 設定連線資訊(IP, Port, 連線時間)
client.username_pw_set("course", "iot999")  # 設定登入帳號密碼

while True:
    t0 = random.randint(0, 30)
    t = datetime.datetime.now().strftime(ISOTIMEFORMAT)
    payload = {"Temperature": t0, "Time": t}
    print(json.dumps(payload))
    # 要發布的主題和內容
    client.publish(
        "ST019/Team06/Sensor", json.dumps(payload)
    )  # NN 改為組別號碼 01, 02, ...
    time.sleep(5)

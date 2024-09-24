import paho.mqtt.client as mqtt     # paho-mqtt-2.0.0

# 當地端程式連線伺服器得到回應時，要做的動作
def on_connect(client, userdata, flags, reason_code, properties):
    # 將訂閱主題寫在on_connet中
    # 如果我們失去連線或重新連線時
    # 地端程式將會重新訂閱
    if reason_code.is_failure:
        print(f"Failed to connect: {reason_code}. loop_forever() will retry connection")
    else:
        print(f"Connected with result code: {reason_code}")
        # we should always subscribe from on_connect callback to be sure
        # our subscribed is persisted across reconnections.
        client.subscribe("ST019/Team99/Sensor")  # NN 改為組別號碼 01, 02, ...

# 當接收到從伺服器發送的訊息時要進行的動作
def on_message(client, userdata, msg):
	# print(msg.topic+" "+msg.payload.decode('utf-8'))   # 轉換編碼utf-8才看得懂中文
    print(msg.topic+" "+str(msg.payload))

# 連線設定
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)  # 初始化地端程式 
client.on_connect = on_connect    # 設定連線的動作
client.on_message = on_message    # 設定接收訊息的動作

client.username_pw_set("course","iot999") # 設定登入帳號密碼
client.connect("140.128.99.71", 1883, 60) # 設定連線資訊(IP, Port, 連線時間)

# 開始連線，執行設定的動作和處理重新連線問題
# 也可以手動使用其他loop函式來進行連接
client.loop_forever()
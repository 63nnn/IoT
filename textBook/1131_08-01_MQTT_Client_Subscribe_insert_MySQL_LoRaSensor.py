import json
import paho.mqtt.client as mqtt
import datetime as datetimeLibrary
import mysql.connector
from mysql.connector import Error

LORA_NODE_MAC = "0000000033000016"


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
        client.subscribe("GIOT-GW/UL/1C497BFA3280")  # << subscribe here


# 當接收到從伺服器發送的訊息時要進行的動作
def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.payload))

    d = json.loads(msg.payload)[0]
    gwid = d["gwid"]
    macAddr = d["macAddr"]
    receive_data = d["data"]

    # 判斷LoRa node的Mac是否正確
    if macAddr != LORA_NODE_MAC:
        print("MQTT - Filter - this LoRa node mac is wrong")
        return

    data_length = len(receive_data)
    if data_length != 26:
        print("MQTT - Parse data - this data length is wrong")
        return
    print(f"\n{len(receive_data)}\n")

    temperature = receive_data[0:2] + "." + receive_data[2:4]
    humidity = receive_data[4:6] + "." + receive_data[6:8]
    tvoc = receive_data[8:14]
    co2 = receive_data[14:20]
    pm25 = receive_data[20:26]

    TIME_FORMAT = "%Y-%m-%d %H:%M:%S"
    time = datetimeLibrary.datetime.now().strftime(TIME_FORMAT)
    timestamp = int(datetimeLibrary.datetime.strptime(time, TIME_FORMAT).timestamp())

    # 將資料儲存到資料庫
    try:
        # 連接 MySQL/MariaDB 資料庫
        connection = mysql.connector.connect(
            host="localhost",  # 主機名稱
            database="thu_air",  # 資料庫名稱
            user="root",  # 帳號
            password="",
            port=58408,
        )

        # 新增資料
        sql = "INSERT INTO `history` (gateway_id,lora_node_mac,temperature,humidity,tvoc,co2,pm25,timestamp,datetime) VALUES(%s,%s,%s,%s,%s,%s,%s,%s,%s);"
        new_data = (
            gwid,
            macAddr,
            temperature,
            humidity,
            tvoc,
            co2,
            pm25,
            timestamp,
            time,
        )
        cursor = connection.cursor()
        cursor.execute(sql, new_data)

        # 確認資料有存入資料庫
        connection.commit()

    except Error as e:
        print("DB connection failure：", e)

    finally:
        if connection.is_connected():
            cursor.close()
            connection.close()


# 連線設定
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)  # 初始化地端程式
client.on_connect = on_connect  # 設定連線的動作
client.on_message = on_message  # 設定接收訊息的動作

client.username_pw_set("course", "iot999")  # 設定登入帳號密碼
client.connect("140.128.99.71", 1883, 60)  # 設定連線資訊(IP, Port, 連線時間)

# 開始連線，執行設定的動作和處理重新連線問題
# 也可以手動使用其他loop函式來進行連接
client.loop_forever()

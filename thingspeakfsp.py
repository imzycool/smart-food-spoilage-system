import serial
import time
import requests

# replace with your channel write api key and number
write_api_key = "Z9BKIEH1VHA8AGYK"
channel_number = 3188674

# serial port for arduino
serial_port = '/dev/tty.usbmodem101'
baud_rate = 9600

ser = serial.Serial(serial_port, baud_rate)
time.sleep(2)

def update_thingspeak(temp, hum, gas, moisture, spoil_status):
    # send numeric values to thingspeak fields
    url = (
        f"https://api.thingspeak.com/update?"
        f"api_key={write_api_key}&field1={temp}&field2={hum}"
        f"&field3={gas}&field4={moisture}&field5={spoil_status}"
    )
    try:
        response = requests.get(url)
        if response.status_code == 200:
            print("thingspeak updated")
        else:
            print("failed to update thingspeak")
    except:
        print("failed to update thingspeak")

try:
    while True:
        temps, hums, gases, moistures, statuses = [], [], [], [], []

        start_time = time.time()
        while time.time() - start_time < 15:
            try:
                moisture_line = ser.readline().decode('utf-8').strip()
                temp_hum_line = ser.readline().decode('utf-8').strip()
                gas_line = ser.readline().decode('utf-8').strip()
                status_line = ser.readline().decode('utf-8').strip()

                if not moisture_line or not temp_hum_line or not gas_line or not status_line:
                    continue

                moisture = int(moisture_line.split(":")[1].strip())
                temp = float(temp_hum_line.split("Temp:")[1].split("C")[0].strip())
                hum = float(temp_hum_line.split("Humidity:")[1].replace("%","").strip())
                gas = int(gas_line.split(":")[1].strip())
                spoil_status = 1 if "Spoiled" in status_line else 0

                temps.append(temp)
                hums.append(hum)
                gases.append(gas)
                moistures.append(moisture)
                statuses.append(spoil_status)

            except:
                continue

        avg_temp = round(sum(temps)/len(temps), 2) if temps else 0
        avg_hum = round(sum(hums)/len(hums), 2) if hums else 0
        avg_gas = round(sum(gases)/len(gases), 0) if gases else 0
        avg_moisture = round(sum(moistures)/len(moistures), 0) if moistures else 0
        avg_spoil_status = statuses[-1] if statuses else 0

        print(f"temp:{avg_temp} hum:{avg_hum} gas:{avg_gas} moisture:{avg_moisture} spoil_status:{avg_spoil_status}")

        update_thingspeak(avg_temp, avg_hum, avg_gas, avg_moisture, avg_spoil_status)

except KeyboardInterrupt:
    ser.close()


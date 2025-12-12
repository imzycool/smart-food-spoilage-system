import serial # talks to arduino
import time # for using delays
import requests # to send data to the internet

# ThingSpeak configuration
write_api_key = "Z9BKIEH1VHA8AGYK"
channel_number = 3188674

# Serial port and baud rate
serial_port = '/dev/tty.usbmodem101'  
baud_rate = 9600 

# connects to arduino
ser = serial.Serial(serial_port, baud_rate)
time.sleep(2)  # waits for Arduino to reset

# fucntion to send data to thingspeak
def update_thingspeak(temp, hum, gas, moisture, spoil_status):
    # makes the website link with out data
    url = (
        f"https://api.thingspeak.com/update?"
        f"api_key={write_api_key}&field1={temp}&field2={hum}"
        f"&field3={gas}&field4={moisture}&field5={spoil_status}"
    )
    try:
        # sends data to thingspeak
        response = requests.get(url)
        if response.status_code == 200:
            print("ThingSpeak updated")
        else:
            print("Failed to update ThingSpeak")
    except:
        print("Failed to update ThingSpeak")

try:
    while True:
        start_time = time.time()
        # lists to store sensor readings
        temps, hums, gases, moistures, statuses = [], [], [], [], []

        # Collect data for 15 seconds
        while time.time() - start_time < 15:
            try:
                # reads data from arduino
                line = ser.readline().decode('utf-8').strip()
                if not line:
                    continue

                # Moisture line
                if "Moisture Level:" in line:
                    moisture = int(line.split(":")[1].strip())
                    moistures.append(moisture)

                # Temp & Humidity line
                elif "Temp:" in line:
                    temp = float(line.split("Temp:")[1].split("C")[0].strip())
                    hum = float(line.split("Humidity:")[1].replace("%","").strip())
                    temps.append(temp)
                    hums.append(hum)

                # Gas line
                elif "Gas Level:" in line:
                    gas = int(line.split(":")[1].strip())
                    gases.append(gas)

                # Food status line
                elif "Food Status:" in line:
                    # 0 = fresh and 1 = spoiled
                    spoil_status = 1 if "Spoiled" in line else 0
                    statuses.append(spoil_status)

            except Exception as e:
                continue # skips errors

        # Use last reading for each value (average values)
        avg_temp = round(sum(temps)/len(temps), 2) if temps else 0
        avg_hum = round(sum(hums)/len(hums), 2) if hums else 0
        avg_gas = round(sum(gases)/len(gases), 0) if gases else 0
        avg_moisture = round(sum(moistures)/len(moistures), 0) if moistures else 0
        avg_spoil_status = statuses[-1] if statuses else 0

        print(f"temp:{avg_temp} hum:{avg_hum} gas:{avg_gas} moisture:{avg_moisture} spoil_status:{avg_spoil_status}")

        update_thingspeak(avg_temp, avg_hum, avg_gas, avg_moisture, avg_spoil_status)

except KeyboardInterrupt:
    ser.close() # closes serial connection



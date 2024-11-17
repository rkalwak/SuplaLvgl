# Supla control panel

## Hardware

7 Cal ESP32-S3 monitor TFT dla Arduino HMI 8M PSRAM 16M Flash Lvgl WiFi Bluetooth 800*480 capacitive touch

[AliExpress](https://pl.aliexpress.com/item/1005006237268179.html)

## Software

Define secrets.ini file with content below:
[env:esp32s3dev]
build_flags = 
	-D wifiSSID='"ssid"'
	-D wifiPass='"password"'
    -D topicTemp='"supla/<MQTT_USERNAME>/devices/<DEVICE_ID>/channels/<CHANNEL_ID>/state/temperature"'
    -D topicWind='"supla/<MQTT_USERNAME>/devices/<DEVICE_ID>/channels/<CHANNEL_ID>/state/value"'
    -D topicGarageGate='"supla/<MQTT_USERNAME>/devices/<DEVICE_ID>/channels/<CHANNEL_ID>/execute_action"'
    -D topicWindDirection='"supla/<MQTT_USERNAME>/devices/<DEVICE_ID>/channels/<CHANNEL_ID>/state/temperature"'
    -D topicPressure='"supla/<MQTT_USERNAME>/devices/<DEVICE_ID>/channels/<CHANNEL_ID>/state/value"'
    -D topicHumidity='"supla/<MQTT_USERNAME>/devices/<DEVICE_ID>/channels/<CHANNEL_ID>/state/humidity"'
    -D mqttServer='"mqtt<SERVER>.supla.org"'
    -D mqttLogin='"<MQTT_USERNAME>"'
    -D mqttPasswor='"<MQTT_PASSWORD>"'


    https://api.open-meteo.com/v1/forecast?latitude=51.0126&longitude=18.4357&hourly=temperature_2m,relative_humidity_2m,rain,snowfall,snow_depth,weather_code,surface_pressure,wind_speed_10m,wind_gusts_10m&timezone=auto&forecast_days=3
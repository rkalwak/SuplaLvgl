# Supla control panel

In constant development.

![image1](screen1.jpg)

## Hardware

7 Cal ESP32-S3 monitor TFT dla Arduino HMI 8M PSRAM 16M Flash Lvgl WiFi Bluetooth 800*480 capacitive touch

[AliExpress](https://pl.aliexpress.com/item/1005006237268179.html)

## Software used

LVGL 8.3 running on latest esp32-s3-devkitc boards, compiled using Platform.IO with libs:

	moononournation/GFX Library for Arduino@^1.5.0
	knolleary/PubSubClient@^2.8
	bblanchon/ArduinoJson@^7.2.1
	squix78/JsonStreamingParser@^1.0.5

## Software settings

Define secrets.ini file with content below:

```
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
```

### Software features

Receiving data from Supla MQTT server.
Calling action in Supla via MQTT.

#### Displaying weather from my weather station

1. Outside temperature
2. Humidity
3. Wind speed
4. Wind direction
5. Pressure
6. Rain
7. Light

#### Ability to turn on/off light

Sending commands to light switches.

#### Ability to open gates

Sending commands to gate switches.

#### Weather forecast from Open-Meteo.com

Retrieving forecast from Open-Meteo over HTTP request.

    https://api.open-meteo.com/v1/forecast?latitude=51.0126&longitude=18.4357&hourly=temperature_2m,relative_humidity_2m,rain,snowfall,snow_depth,weather_code,surface_pressure,wind_speed_10m,wind_gusts_10m&timezone=auto&forecast_days=3
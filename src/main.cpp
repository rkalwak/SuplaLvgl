
/*******************************************************************************
 * LVGL Widgets
 * This is a widgets demo for LVGL - Light and Versatile Graphics Library
 * import from: https://github.com/lvgl/lv_demos.git
 *
 * Dependent libraries:
 * LVGL: https://github.com/lvgl/lvgl.git

 * Touch libraries:
 * FT6X36: https://github.com/strange-v/FT6X36.git
 * GT911: https://github.com/TAMCTec/gt911-arduino.git
 * XPT2046: https://github.com/PaulStoffregen/XPT2046_Touchscreen.git
 *
 * LVGL Configuration file:
 * Copy your_arduino_path/libraries/lvgl/lv_conf_template.h
 * to your_arduino_path/libraries/lv_conf.h
 * Then find and set:
 * #define LV_COLOR_DEPTH     16
 * #define LV_TICK_CUSTOM     1
 *
 * For SPI display set color swap can be faster, parallel screen don't set!
 * #define LV_COLOR_16_SWAP   1
 *
 * Optional: Show CPU usage and FPS count
 * #define LV_USE_PERF_MONITOR 1
 ******************************************************************************/
// #include "lv_demo_widgets.h"
#include <lvgl.h>
#include <demos/lv_demos.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ui/ui.h>
#include <mqttCallback.h>
#include "openMeteo.h"
const char *garageGatePayload = "OPEN_CLOSE";
//
WiFiClientSecure wifiClient;
PubSubClient client(wifiClient); // lib required for mqtt
MqttCallback callbackObject;
#include <Arduino_GFX_Library.h>
#define TFT_BL 2
#define GFX_BL DF_GFX_BL // default backlight pin, you may replace DF_GFX_BL to actual backlight pin

Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
    41 /* DE */, 40 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
    15 /* R0 */, 7 /* R1 */, 6 /* R2 */, 5 /* R3 */, 4 /* R4 */,
    9 /* G0 */, 46 /* G1 */, 3 /* G2 */, 8 /* G3 */, 16 /* G4 */, 1 /* G5 */,
    14 /* B0 */, 21 /* B1 */, 47 /* B2 */, 48 /* B3 */, 45 /* B4 */,
    0 /* hsync_polarity */, 180 /* hsync_front_porch */, 30 /* hsync_pulse_width */, 16 /* hsync_back_porch */,
    0 /* vsync_polarity */, 12 /* vsync_front_porch */, 13 /* vsync_pulse_width */, 10 /* vsync_back_porch */);
Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
    800 /* width */, 480 /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */);

#include "touch.h"

static uint32_t screenWidth;
static uint32_t screenHeight;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;

OpenWeatherMapOneCallData openWeatherMapOneCallData;
OpenWeatherMapOneCall oneCallClient;
void getHeapMemory()
{
  char temp[300];
  sprintf(temp, "Heap: Free:%i, Min:%i, Size:%i, Alloc:%i",
          ESP.getFreeHeap(), ESP.getMinFreeHeap(), ESP.getHeapSize(), ESP.getMaxAllocHeap());
  Serial.println(temp);
}

void getPsram()
{
  char temp[300];
  sprintf(temp, "PSRAM (total): %i \nFree PSRAM: %i \nMinPSR: %i \nAllocPSR: %i",
          ESP.getPsramSize(), ESP.getFreePsram(), ESP.getMinFreePsram(), ESP.getMaxAllocPsram());
  Serial.println(temp);
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("ESP32_clientID1", mqttLogin, mqttPassword))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // ... and resubscribe
      client.subscribe(topicTemp);
      client.subscribe(topicWind);
      client.subscribe(topicWindDirection);
      client.subscribe(topicPressure);
      client.subscribe(topicHumidity);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      // delay(5000);
    }
  }
}

void connectmqtt()
{

  client.connect("ESP32_clientID1", mqttLogin, mqttPassword); // ESP will connect to mqtt broker with clientID
  {
    Serial.println("connected to MQTT");

    client.subscribe(topicTemp);
    client.subscribe(topicWind);
    client.subscribe(topicWindDirection);
    client.subscribe(topicPressure);
    client.subscribe(topicHumidity);

    if (!client.connected())
    {
      reconnect();
    }
  }
}

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

  lv_disp_flush_ready(disp);
}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  if (touch_has_signal())
  {
    if (touch_touched())
    {
      data->state = LV_INDEV_STATE_PR;

      /*Set the coordinates*/
      data->point.x = touch_last_x;
      data->point.y = touch_last_y;
      /*
      Serial.print( "Data x " );
      Serial.println( data->point.x );
      Serial.print( "Data y " );
      Serial.println( data->point.y );
      */
    }
    else if (touch_released())
    {
      data->state = LV_INDEV_STATE_REL;
    }
  }
  else
  {
    data->state = LV_INDEV_STATE_REL;
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  callbackObject.callback(topic, payload, length);
}

void connecWifiMqtt()
{
  Serial.println("connecting to wifi");
  WiFi.begin(wifiSSID, wifiPass);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("connected to wifi");
  wifiClient.setInsecure();

  client.setServer(mqttServer, 8883); // connecting to mqtt server
  client.setCallback(callback);
  delay(5000);
  Serial.println("Connecting to mqtt");
  connectmqtt();
  Serial.println("Connected to mqtt");
}
void onGateClicked(lv_event_t *e)
{
  Serial.println("Gate clicked");
  // client.publish(topicGate, "OPEN_CLOSE");
}

void onGarageGateClicked(lv_event_t *e)
{
  Serial.println("Garage Gate clicked");
  // client.publish(topicGarageGate, "OPEN_CLOSE");
}

void onGarageOutsideLightClick(lv_event_t *e)
{
  refreshForecast(e);
  auto ui_ChartTemperature_series_temp = lv_chart_get_series_next(ui_ChartTemperature, NULL);
  auto ui_ChartTemperature_series_hum = lv_chart_get_series_next(ui_ChartTemperature, ui_ChartTemperature_series_temp);
  for (int i = 0; i < 24; i++)
  {
    int16_t t = static_cast<u_int16_t>(round(openWeatherMapOneCallData.hourly[i].temp));
    int16_t p = static_cast<u_int16_t>(round(openWeatherMapOneCallData.hourly[i].pressure));
    lv_chart_set_next_value(ui_ChartTemperature, ui_ChartTemperature_series_temp, p);
    lv_chart_set_next_value(ui_ChartTemperature, ui_ChartTemperature_series_hum, t);
  }
}

void onGarageInsideLightClick(lv_event_t *e)
{
}
float OPEN_WEATHER_MAP_LOCATTION_LAT = 51.0126;
float OPEN_WEATHER_MAP_LOCATTION_LON = 18.4357;
void refreshForecast(lv_event_t *e)
{
  oneCallClient.setMetric(true);
  oneCallClient.setLanguage("pl");
  oneCallClient.update(&openWeatherMapOneCallData, "Supla", OPEN_WEATHER_MAP_LOCATTION_LAT, OPEN_WEATHER_MAP_LOCATTION_LON);
  Serial.println("Forecast");
  Serial.println(openWeatherMapOneCallData.daily[1].tempDay);
}

void setup()
{
  Serial.begin(115200);
  getHeapMemory();
  getPsram();
  connecWifiMqtt();
  Serial.println("After wifi and mqtt");
  getHeapMemory();
  getPsram();

  Serial.println("LVGL Widgets Demo");
  // Init Display
  gfx->begin();

#ifdef TFT_BL
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  ledcAttach(TFT_BL, 300, 8);
  ledcWrite(TFT_BL, 255); // Screen brightness can be modified by adjusting this parameter. (0-255)

#endif

  gfx->fillScreen(RED);
  // delay(500);
  // gfx->fillScreen(GREEN);
  // delay(500);
  // gfx->fillScreen(BLUE);
  // delay(500);
  // gfx->fillScreen(BLACK);
  delay(500);
  lv_init();

  // Init touch device
  pinMode(TOUCH_GT911_RST, OUTPUT);
  digitalWrite(TOUCH_GT911_RST, LOW);
  delay(10);
  digitalWrite(TOUCH_GT911_RST, HIGH);
  delay(10);
  touch_init();
  //  touch.setTouch( calData );

  screenWidth = gfx->width();
  screenHeight = gfx->height();
  Serial.println("Before lvgl buffor allocation");
  getHeapMemory();
  getPsram();
  disp_draw_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * screenWidth * screenHeight / 4, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

  if (!disp_draw_buf)
  {
    Serial.println("LVGL disp_draw_buf allocate failed!");
    getHeapMemory();
    getPsram();
  }
  else
  {
    lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, screenWidth * screenHeight / 4);

    Serial.println("After lvgl buffor allocation");
    getHeapMemory();
    getPsram();
    /* Initialize the display */
    lv_disp_drv_init(&disp_drv);
    /* Change the following line to your display resolution */
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /* Initialize the (dummy) input device driver */
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    ui_init();
    lv_chart_set_update_mode(ui_ChartTemperature, LV_CHART_UPDATE_MODE_SHIFT);
    // lv_chart_set_range(ui_ChartTemperature, LV_CHART_AXIS_PRIMARY_X, -15, 30);
    // lv_chart_set_range(ui_ChartTemperature, LV_CHART_AXIS_PRIMARY_Y, 0, 15);
    // lv_chart_set_range(ui_ChartTemperature, LV_CHART_AXIS_SECONDARY_Y, -5, 15);
    Serial.println("Setup done");
  }
}

void loop()
{

  if (!client.connected())
  {
    reconnect();
  }

  client.loop();

  lv_timer_handler(); /* let the GUI do its work */
  delay(5);
}

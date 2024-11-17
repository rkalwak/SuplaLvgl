#include "mqttCallback.h"
MqttCallback::MqttCallback()
{
}
void MqttCallback::callback(char *topic, byte *payload, unsigned int length)
{
    const char *windDirections[9] = {"Polnocny", "Polnocno wschodni", "Wschodni", "Poludniowo wschodni", "Poludniowy", "Poludniowo zachodni", "Zachodni", "Polnocno zachodni", "Polnocny"};
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    char buffer[length];
    int i = 0;
    for (i; i < length; i++)
    {
        Serial.print((char)payload[i]);
        buffer[i] = payload[i];
    }
    buffer[i] = '\0';
    float f = atof(buffer);

    if (strcmp(topic, topicTemp) == 0)
    {
        lv_label_set_text_fmt(ui_labelTempValue, "%.2F °C", f);
        Serial.println(f);
    }
    else if (strcmp(topic, topicWind) == 0)
    {
        lv_label_set_text_fmt(ui_labelWindValue, "%.2f m/s", f);
    }
    else if (strcmp(topic, topicWindDirection) == 0)
    {
        int windDirection = (f + 11.25) / 45;
        const char *windDirectionText = windDirections[windDirection];
        lv_label_set_text(ui_LabelWindDirectionValue, windDirectionText);
    }
    else if (strcmp(topic, topicHumidity) == 0)
    {
        lv_label_set_text_fmt(ui_LabelHumidityValue, "%.2f %", f);
    }
    else if (strcmp(topic, topicPressure) == 0)
    {
        lv_label_set_text_fmt(ui_LabelPressureValue, "%.2f hPa", f);
    }
    else
    {
    }

    Serial.println();
}


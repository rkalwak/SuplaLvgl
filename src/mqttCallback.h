#include "Arduino.h"
#include <ui/ui.h>


class MqttCallback
{
public:
   MqttCallback();
   void callback(char *topic, byte *payload, unsigned int length);

protected:
   const char *windDirections[9] = {"Polnocny", "Polnocno wschodni", "Wschodni", "Poludniowo wschodni", "Poludniowy", "Poludniowo zachodni", "Zachodni", "Polnocno zachodni", "Polnocny"};
   
  
};
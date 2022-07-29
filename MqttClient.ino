#include "MqttClient.h"

MqttClient client;

void setup()
{
  Serial.begin(9600);
  Serial.println();
}

void loop() 
{
  client.Run();
}

void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if(topic=="home/reset"){
    counterLaser = 0;
    counterPiezo = 0;
    counterUS = 0;
    itoa(counterLaser,sLaser, 10);
    itoa(counterPiezo,sPiezo, 10);
    itoa(counterUS,sUS, 10);
    client.publish("home/count2", sLaser);
    client.publish("home/count3", sPiezo);
    client.publish("home/count4", sUS);
    Serial.println("Remise à zero des compteurs");
  }
  Serial.println();
}



void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("ESP8266Piezo_laser")) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("home/reset");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

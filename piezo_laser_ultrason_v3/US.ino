void read_sensor() {
duration = pulseIn(pinSonar, HIGH);
distance = duration / 58.8;
}

void read_sensor_moyenne() {
duration = pulseIn(pinSonar, HIGH);
distanceTab[1]=duration / 58.8;

for(int j=1;j<element+1;j++){
  distanceMoyenneTab += distanceTab[j]/element;
}
}

void print_data() {
//Serial.print("distance = ");
Serial.println(distanceMoyenneTab);
//Serial.println(distance1);
//Serial.println(" cm");
}

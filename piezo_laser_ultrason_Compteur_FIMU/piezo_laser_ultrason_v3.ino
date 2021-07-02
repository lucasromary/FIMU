
//////////////////////////////////////////////// MQTT + WIFI /////////////////////////////////////////////////////
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//const char* ssid = "raspi-webgui";
//const char* password = "ChangeMe";
//const char* mqtt_server = "10.3.141.1";

const char* mqtt_server = "192.168.43.78";
const char* ssid = "LucasR";
const char* password = "lucasromary3";

WiFiClient espClient;
PubSubClient client(espClient);

//////////////////////////////////////////////// Capteurs /////////////////////////////////////////////////////
bool statePiezo1 = LOW;
bool statePiezo2 = LOW;
bool statePiezo3 = LOW;
const int pinPiezo1 = 5;
const int pinPiezo2 = 12;
const int pinPiezo3 = 13;

bool stateLaser = LOW;
bool lastStateLaser = LOW;
const int pinLaser = 14;
bool tokenLaser=0;

int pinSonar = 4; 
int distanceMin=90;
int distanceMax=125;
double distance = 0;
const int element = 10;
int distanceTab[element+1];
double distanceMoyenneTab;
int i =0;

double lastDistance = 0;
long duration = 0;
bool stateUS = LOW;
bool laststateUS = LOW;
int counterUS = 0;
int token = 0;

int counterPiezo = 0;
int counterLaser = 0;
char sLaser[8];
char sPiezo[8];
char sUS[8];

unsigned long timer1 = 0;
unsigned long previous_millis1 = 0;
unsigned long previous_millis3 = 0;
unsigned long intervalPiezo = 0;
unsigned long intervalLaser = 0;
unsigned long intervalUS = 0;
unsigned long temporaireHIGHLaser = 0;
unsigned long temporaireLOWLaser = 0;

unsigned long temporaireHIGHUS = 0;
unsigned long temporaireLOWUS = 0;

void setup() {
  Serial.begin(115200);
  /*
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear(); // Set all pixel colors to 'off'
*/
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(pinPiezo1, INPUT);
  pinMode(pinPiezo2, INPUT);
  pinMode(pinPiezo3, INPUT);
  pinMode(pinLaser,INPUT); 
  pinMode(pinSonar,INPUT); 
}

// For this project, you don't need to change anything in the loop function. Basically it ensures that you ESP is connected to your broker
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop()){
    client.connect("ESP8266Piezo_laser2");
  }
// On lit les données des capteurs et on calcul les intervals pour le comptage
  timer1 = millis();
  stateLaser = digitalRead(pinLaser);
  statePiezo1 = digitalRead(pinPiezo1);
  read_sensor_moyenne();
  //Serial.println(stateLaser);
  //Serial.println(statePiezo1);
  print_data();
/////////////////////// US //////////////////////////////
if (distanceMoyenneTab <distanceMin && token == 0) {
    temporaireLOWUS = millis();
    //Serial.print("temporaireLOWUS = ");
    //Serial.println(temporaireLOWUS);
    token = 1;
  }
  
  //if(capteurValue == LOW && interval > 100 && interval <= 600){
  if(distanceMoyenneTab > distanceMax && token == 1){
    temporaireHIGHUS = millis();
    //Serial.print("temporaireHIGHUS = ");
    //Serial.println(temporaireHIGHUS);
    token = 0;
    intervalUS = temporaireHIGHUS-temporaireLOWUS;  
    //Serial.println(intervalUS);
  }

  
/////////////////////// LASER //////////////////////////////
  if (stateLaser == 0 && tokenLaser == 0) {
    temporaireHIGHLaser = millis();
    //Serial.print("token = ");
    //Serial.print(tokenLaser);
    //Serial.print("temporaireHIGHLaser = ");
    //Serial.println(temporaireHIGHLaser);
    tokenLaser=1;
  }

  //if(capteurValue == LOW && interval > 100 && interval <= 600){
  if(stateLaser == 1 && tokenLaser == 1){
    temporaireLOWLaser = millis();
    //Serial.print("token = ");
    //Serial.print(tokenLaser);
    //Serial.print(" temporaireLOWLaser = ");
    //Serial.println(temporaireLOWLaser);
    intervalLaser = temporaireLOWLaser-temporaireHIGHLaser;  
    //Serial.println(intervalLaser);
    tokenLaser=0;
  }
  
/////////////////////// ////// //////////////////////////////
// On regarde si le passage est validé
  if(intervalLaser > 130){

      counterLaser += 1;
      itoa(counterLaser,sLaser, 10);
      client.publish("home/count3", sLaser);
 
      //Serial.print("interval : ");
      //Serial.println(intervalLaser);
      Serial.print("Compteur Laser: ");
      Serial.println(counterLaser);
      
      temporaireHIGHLaser = 0;
      temporaireLOWLaser = 0;
      intervalLaser = 0;

    }
 
  if(timer1-previous_millis1 > 1000 && statePiezo1 == 1){
    counterPiezo += 1;
    // onversion puis envoie MQTT
    itoa(counterPiezo,sPiezo, 10);
    client.publish("home/count2", sPiezo);
    Serial.print("Compteur piezo: ");
    Serial.println(counterPiezo);
    //Serial.print("Interval: ");
    //Serial.println(timer1-previous_millis1);
    previous_millis1 = timer1;
  }


  if(intervalUS > 500){ //600
     
      counterUS = counterUS +1; 
      itoa(counterUS,sUS, 10);
      client.publish("home/count4", sUS);
      //Serial.print("interval : ");
      //Serial.println(intervalUS);
      Serial.print("Compteur US : ");
      Serial.println(counterUS);
      
      temporaireHIGHUS = 0;
      temporaireLOWUS = 0;
      intervalUS=0;
    }

  lastStateLaser = stateLaser;

  for(int j=element; j>0;j--){
      distanceTab[j]=distanceTab[j-1];
      //Serial.println(distanceTab[j]);
  }
  distanceMoyenneTab=0;
} 



//////////////////////////////////////////////// MQTT + WIFI /////////////////////////////////////////////////////
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* mqtt_server = "192.168.43.78";
const char* ssid = "LucasR";
const char* password = "lucasromary3";

//const char* ssid = "raspi-webgui";
//const char* password = "ChangeMe";
//const char* mqtt_server = "10.3.141.1";

WiFiClient espClient;
PubSubClient client(espClient);
//////////////////////////////////////////////// RFID /////////////////////////////////////////////////////

#include <SPI.h>
#include <RFID.h>
RFID monModuleRFID(2,16);

int UID[10];
int last_UID[10] = {0};
int UID_pro[10]={126,4,99,195,218};
int UID_pro2[10]={14,35,96,195,142};
int check = 0;
int check_pro = 0;
int check_pro2 = 0;
int compteur = 0;
int compteur_pro = 0;
char s[8];
char s_pro[8];

int modulo = 0;
int modulo2 = 0;
//int participant_UID[5][3] = {0};
int j=0;
void setup()
{
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  Serial.println("SPI init");
  SPI.begin();
  Serial.println("RFID init");
  monModuleRFID.init();  
  Serial.println("init finished");

}

void loop()
{
  
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop()){
    client.connect("ESP8266NFC");
  }
  
    if (monModuleRFID.isCard()) {  
          if (monModuleRFID.readCardSerial()) {    
            Serial.println("//////////////////////////////////////////////////////////////////");    
            Serial.println("////////////////////// LECTURE DE LA CARTE ///////////////////////"); 
            Serial.println("//////////////////////////////////////////////////////////////////");   
            Serial.print("UID = "); 
            for(int i=0;i<=4;i++){
              UID[i]=monModuleRFID.serNum[i];

              Serial.print(UID[i],DEC);
              Serial.print(".");
            }
            Serial.println("");
            delay(100);
          }
          Serial.println("////////////////////// On check la carte ///////////////////////");
          Serial.print("Last_UID = ");
          for(int i=0;i<=4;i++){
            if(UID[i]==last_UID[i]){
              check +=1;          
            }
            if(UID[i]==UID_pro[i]){
              check_pro +=1;          
            }
            if(UID[i]==UID_pro2[i]){
              check_pro2 +=1;          
            }
            Serial.print(last_UID[i],DEC);
            Serial.print(".");
            last_UID[i] = UID[i];
          }
          Serial.println("");
          Serial.println("////////////////////// COMPTEUR ///////////////////////");
          if(check != 5){
            if(check_pro==5 && modulo == 0){
              compteur_pro += 1;
              itoa(compteur_pro,s_pro, 10);
              client.publish("home/count_pro", s_pro);
              Serial.println("On incrémente pro");
              client.publish("home/lucas", "1");
              modulo = 1;
              check_pro = 0;
            }
            else if(check_pro2==5 && modulo2 == 0){
              compteur_pro += 1;
              itoa(compteur_pro,s_pro, 10);
              client.publish("home/count_pro", s_pro);
              Serial.println("On incrémente pro");
              client.publish("home/olivier", "1");
              modulo2 = 1;
              check_pro2 = 0;
            }
            else if(check_pro==5 && modulo == 1){
              compteur_pro -= 1;
              itoa(compteur_pro,s_pro, 10);
              client.publish("home/count_pro", s_pro);
              Serial.println("On décrémente pro");
              client.publish("home/lucas", "2");
              modulo = 0;
              check_pro = 0;
            }
            else if(check_pro2==5 && modulo2 == 1){
              compteur_pro -= 1;
              itoa(compteur_pro,s_pro, 10);
              client.publish("home/count_pro", s_pro);
              Serial.println("On décrémente pro");
              client.publish("home/olivier", "2");
              modulo2 = 0;
              check_pro2 = 0;
            }
            
            else{
              compteur += 1;
              itoa(compteur,s, 10);
              client.publish("home/count", s);
              Serial.println("On incrémente");
            }
          }
          Serial.print("compteur = ");
          Serial.print(compteur);
          Serial.print( "compteur_pro = ");
          Serial.println(compteur_pro);
          check = 0;
          check_pro = 0;
          check_pro2 = 0;
          monModuleRFID.halt();
    }
    delay(1);

// On peut afficher les participant
/*
for(int j=0;j<=2;j++){
 for(int i=0;i<=4;i++){
    Serial.print(participant_UID[i][j],DEC);
    Serial.print(".");
}
Serial.println("");
}
Serial.println("");
Serial.println("");
}
*/

}

#include <WiFi.h>
#include <FirebaseESP32.h>
#include <Wire.h>

#define FIREBASE_HOST "-"
#define FIREBASE_AUTH "-"
#define WIFI_SSID "-"
#define WIFI_PASSWORD "-"

#define relay 5

//LCD I2C
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);


//thingspeak
String apiKey = "WLH0O3MKHTO6HDGG";
const char* server = "api.thingspeak.com";

WiFiClient client;

float V, PP, PB;
FirebaseData firebaseData;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD );
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  pinMode(relay, OUTPUT);
  
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  
  
  //LCD
  lcd.begin(); //initialize the lcd for 16 chars 2 line, turn o backlight
  lcd.backlight();
  //delay(250);
  //lcd.noBacklight();
  lcd.setCursor(0, 0);
  lcd.print(" TEKNIK KOMPUTER ");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print(" Water Pressure Sensor ");
  delay(1000);
}
//int n = 1;
void loop() {
  lcd.clear();
  int sensorVal = analogRead(34);
  lcd.setCursor(0, 0);
  lcd.print("Tekanan Sumur Bor");
  Serial.print("Sensor Value: ");
  Serial.print(sensorVal);

  //float voltage = (sensorVal*5.0)/1024.0;
  V = (sensorVal * 3.3) / 4096.0;
  PP = (3.0 * ((float)V - 0.32468)) * 1000000.0;
  PB = PP / 10e5;

   if (PB >2){
  digitalWrite(relay, LOW);
  }
  if (PB <1.5) {
    digitalWrite(relay, HIGH);
    }
  
  String data = String(PB) + String("Bars");
  Serial.print(data);
  Serial.println();
  Serial.print("  Volts: ");
  Serial.print(V, 5);
  int u;
  lcd.setCursor(0, 1);
  lcd.print(" pressure : ");
  lcd.print(abs(PB), 1.0);
  lcd.print(" bars ");
  for (u = 0 ; u < 16 ; u++) {
    lcd.scrollDisplayLeft();
  //Serial.println(abs(PB),1.0);

      delay(200);
  }


//  PB=Firebase.getInt(firebaseData,+"/Pompa_Status");
//gae tes coba lebih dari 0 ae tekanane yo 

// cek pompa hidup atau mati
  if (PB>2){
    Firebase.setString(firebaseData,"/Sumber Kolak 4/Pompa", "Mati"); 
    } if (PB<1.5) {
      Firebase.setString(firebaseData,"/Sumber Kolak 4/Pompa", "Hidup");
      }
    
//cek tekanan, tekanane kan ndek var PB = PP / 10e5;
  if (PB>2) {
    //    jika tekanan tinggi
// insert firebase
    Firebase.setString(firebaseData,"/Sumber Kolak 4/Status", "Tekanan Tinggi"); 
  } if (PB<1.5) { 
//    jika tekanan normal
    Firebase.setString(firebaseData, "/Sumber Kolak 4/Status", "Tekanan Normal"); 
  } if (PB<1) { 
//    jika tekanan rendah
    Firebase.setString(firebaseData, "/Sumber Kolak 4/Status", "Tekanan Rendah"); 
  }
  
  Firebase.setString(firebaseData, "/Sumber Kolak 4/Tekanan", data);
  Firebase.setString(firebaseData, "/Sumber Kolak 4/Nama", "Sumber Kolak 4");
  //Firebase.setString(firebaseData, "/Sumber Kolak 4/Relay", "mati");
  //Firebase.setString(firebaseData, "/Arjasa/Relay", "hidup");  
  
//if (PB < 0) {
//    data = 0;
//} else if (PB > 1) {
//    string status = "Aktif";
//} else {
//    string status = "Tidak Aktif";
//}
//  

if (client.connect(server,80)) {
  String postStr = apiKey;
  postStr +="&field1=";
  postStr += String(data);
  postStr += "\r\n\r\n";
  
  client.print("POST /update HTTP/1.1\n");
  client.print("Host: api.thingspeak.com\n");
  client.print("Connection: close\n");
  client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
  client.print("Content-Type: application/x-www-form-urlencoded\n");
  client.print("Content-Length: ");
  client.print(postStr.length());
  client.print("\n\n");
  client.print(postStr);
  
  Serial.print (data);
  Serial.println();
  Serial.println("mengirim ke thingspeak");
  }

  //Firebase.setString(firebaseData, "/Tekanan", data);
  //Firebase.setString(firebaseData, "/Status", status);
////
//if (PB<0){
//  Firebase.setString(PB ==0);  }

  //    Serial.print("  Pressure = ");
     // Serial.println(abs(pressure_bar),1);
  //    Serial.println(" bars ");

  delay(100);
}

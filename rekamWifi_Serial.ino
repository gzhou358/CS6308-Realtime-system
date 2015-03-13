#define CCx 1
#include <WiFi.h>
#include <WiFiClient.h>
// variable used
String inData;
char charBuf[20];
char pinnumber[3];
char valuenumber[5];
int command = 0;
int pin = 0;
int i;
int value;

#ifdef CCx
char ssid[] = "asd123";     //  your network SSID (name)
char pass[] = "12345678";  // your network password
uint16_t port = 1200;     // port number of the server
IPAddress server(192, 168, 10, 102);   // IP Address of the server
WiFiClient client;
int status = WL_IDLE_STATUS;     // the Wifi radio's status
#endif

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  // These Pin Modes are specific to each Launchpad and can be reconfigured here. To Use a different Launchpad/Uncomment Relevant Code here and in get/put functions
    pinMode(2, INPUT);    //analogread
    pinMode(3, INPUT);    //digitalread
    pinMode(5, INPUT);    //digitalread
    pinMode(6, INPUT);    //analogread
    pinMode(7, OUTPUT);   //digitalwrite
    pinMode(8, OUTPUT);   //digitalwrite
    pinMode(9, OUTPUT);  //analogwrite
    pinMode(10, OUTPUT);  //analogwrite
    pinMode(29, OUTPUT);   //analogwrite
    
#ifdef CCx
    // attempt to connect to Wifi network:
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);
    int tries = 0;
    while ( status != WL_CONNECTED) {
      status = WiFi.status();
      // wait .1 seconds for connection:
      delay(100);
      Serial.print(".");
      if (tries++ > 300) {
        Serial.println("Couldn't connect to your wifi network");
        Serial.println("check your ssid and password.");
        while(1);
      }
    }
  
    // you're connected now, so print out the data:
    Serial.println("\nYou're connected to the network");
    IPAddress empty(0,0,0,0);
    while (WiFi.localIP() == empty); //wait for IP assignment
    
    // attempt to connect to the server
    Serial.println("Attempting to connect to server");
    tries = 0;
    while (client.connect(server, port) == false) {
      Serial.print(".");
      if (tries++ > 100) {
        Serial.println("\nThe server isn't responding");
        while(1);
      }
      delay(100);
    }
    Serial.println("\nThe server responded");
#endif
//    establishContact(); // send a byte to establish contact until receiver responds 
}

String help(){
 //   Serial.println("see rekamshort.ino");
    return "see rekamshort.ino";
}

// Gets an analog value from a pin 
int get(int pin){
    //Read multi time to get correct data.
    if ((pin==2)||(pin==6)){
      analogRead(pin);
      analogRead(pin);
      analogRead(pin);
      analogRead(pin);
      analogRead(pin);
      value = analogRead(pin);
      Serial.print("analogRead ");
      Serial.println(value);
      return value;
    }
    else if ((pin==3)||(pin==5)){
      digitalRead(pin);
      digitalRead(pin);
      digitalRead(pin);
      digitalRead(pin);
      digitalRead(pin);
      value = digitalRead(pin);
      Serial.print("digitalRead ");
      Serial.println(value);
      return value;
    }
    else { 
      Serial.println("choose correct pin");
      return -1;
    }
}

// Puts an analog or digital value on a pin from 0 100 percent of VCC 
int put(int pin, int value){ 
  if(value<0){value=0;}
  if ((pin==9)||(pin==10)){
    analogWrite(pin,value);
    Serial.print("analogWrite ");
    Serial.println(value);
    return value;
  } 
  else if ((pin==7)||(pin==8)){
    digitalWrite(pin,value);
    Serial.print("digitalWrite ");
    Serial.println(value);
    return value;
  }
  else { 
    Serial.println("choose correct pin");
    return -1;
  } 
} 

// This is generic rekam1 VM code and should be the same across all Launchpads              
void loop()
{
  if (Serial.available())
  {
    char received = Serial.read();
    inData += received;  
    // Process message when new line or carriage return character is recieved
    if(received == '\n' || received == '\r'){
      inData.toCharArray(charBuf, 20);
      //charBuf is the string you input.
      Serial.print(charBuf);
      
      if(!strncmp(charBuf,"get ",4)){
        command=1;
      }else if(!strncmp(charBuf,"put ",4)){
        command=2;
      }else if(!strncmp(charBuf,"?",1)){
        command=3;
      }else {  command = 0;}
      for(i=0;i<3;i++){
        pinnumber[i]=charBuf[i+4];
      }
      pin = atoi(pinnumber);
       for(i=0;i<5;i++){
        if(pin<10){ 
          valuenumber[i]=charBuf[i+6];
        }else{ 
          valuenumber[i]=charBuf[i+7];
        }
      }
      value=atoi(valuenumber);
      inData = ""; // Clear recieved buffer
      // charBuf to command + pin + value;
      if(command==1){  get(pin);  }
      if(command==2){  put(pin, value);  }
      if(command==3){  Serial.println(help());  }
      if(command==0){  
        Serial.println("wrong command");
      }
    }
  }
  if (client.available()) {
    client.read((uint8_t*)charBuf, client.available());
    Serial.print("wifi command: ");
    Serial.print(charBuf);
//    client.print(charBuf);
    if(!strncmp(charBuf,"get ",4)){
      command=1;
    }else if(!strncmp(charBuf,"put ",4)){
      command=2;
    }else if(!strncmp(charBuf,"?",1)){
      command=3;
    }else {  command = 0;}
    for(i=0;i<3;i++){
      pinnumber[i]=charBuf[i+4];
    }
    pin = atoi(pinnumber);
     for(i=0;i<5;i++){
      if(pin<10){ 
        valuenumber[i]=charBuf[i+6];
      }else{ 
        valuenumber[i]=charBuf[i+7];
      }
    }
    value=atoi(valuenumber);
    inData = ""; // Clear recieved buffer
    // charBuf to command + pin + value;
    if(command==1){
      client.print("Read value ");
      client.println(get(pin));  
    }
    if(command==2){
      put(pin, value);  
      client.print("Put value ");
      client.println(put(pin, value)); 
    }
    if(command==3){  
      client.println(help());
      Serial.println(help());  
    }
    if(command==0){  
      client.println("wrong command");  
    }
    //clean buffer
    for (int i =0; i<20; i++)
    {  charBuf[i] = NULL;  }
  }
}

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.println("ENTER A COMMAND"); // send an initial string
    delay(1000);
  }
}

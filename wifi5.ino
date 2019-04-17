#include <SoftwareSerial.h>
int flag=0;
SoftwareSerial wifi = SoftwareSerial(2, 3);
String network_name="SSID"; //name of network to connect, must be in quotes
String network_pwd="pwd"; //network password, must be in quotes

void setup()
{
  wifi.begin(115200);//ESP8266 default baud rate
  Serial.begin(57600);//Set Arduino serial monitor to 57600
  WIFI_SETUP();//This function will setup the server on wifi and print the IP Address to the serial monitor
}

void loop()
{
  if (Serial.available()) { //This if statement allows you to type AT commands directly into the serial monitor
    String command = Serial.readString();
    wifi.print(command);
    wifi.write(10);
    wifi.write(13);
  }
  if(flag==1){//This flag will close any active connections to the webpage hosted on the ESP8266 (this may be optional)
    wifi.println("AT+CIPCLOSE=0");
    delay(100);
    wifi.println("AT+CIPCLOSE=1");
    delay(100);
    wifi.println("AT+CIPCLOSE=2");
    delay(100);
    wifi.println("AT+CIPCLOSE=3");
    delay(100);
    wifi.readString();
    delay(100);
    flag=0;
  }
  if (wifi.available()) {//if a request has been sent to the server send an HTML header and a message to the browser
    Serial.println(wifi.readString());//print the request
    if(flag==0){
    wifi.print("AT+CIPSEND=0,63");
    wifi.write(13);
    wifi.write(10);
    wifi.write(13);
    wifi.write(10);
    delay(100);
    wifi.print("HTTP/1.1 200 OK");
    wifi.write(13);
    wifi.write(10);
    wifi.print("Content-Type: text/plain");
    wifi.write(13);
    wifi.write(10);
    wifi.write(13);
    wifi.write(10);
    wifi.print("Hello From Arduino");
    wifi.write(13);
    wifi.write(10);
    wifi.write(13);
    wifi.write(10);
    flag=1;//flag for closing active connections
    delay(1000);//when this is removed, the web browser does not load the web page. there must be a way to test if the transmission is complete before closing the active connection.
    }
  }
}

void WIFI_SETUP() {
  Serial.println();
  Serial.println("SETUP.. 1/7");
  wifi.println("AT+UART_CUR=57600,8,1,0,0"); //there is too much noise if not soldered, slowing baud rate seems to help
  wifi.begin(57600);// change baud rate to setting above
  delay(1000);// delay for time to change baud rate
  if (wifi.available()) { //empty the serial buffer after reset
    wifi.readString(); //this is gibberish to remove from the buffer if baud rate was changed
    Serial.println("Baud Rate Changed..2/7");
  }
  wifi.println("AT+CWMODE_CUR=1"); //SET AS STATION
  delay(200);//delay to allow setting change
  if (wifi.available()) { //print the response from ESP8266
    Serial.println("SET AS STATION..3/7");
    Serial.print(wifi.readString());
  }
  wifi.println("AT+CWDHCP_CUR=1,1"); //SET DHCP ENABLED ON STATION MODE
  delay(200);// delay top allow setting change
  if (wifi.available()) { //print the response from ESP8266
    Serial.println("DHCP ENABLED..4/7");
    Serial.print(wifi.readString());
  }
  Serial.println("INTERNET CONNECTION STARTING.. 5/7");
  wifi.print("AT+CWJAP_CUR="); //CONNECT TO AN ACCESS POINT
  wifi.write(34);
  wifi.print(network_name);
  wifi.write(34);
  wifi.print(",");
  wifi.write(34);
  wifi.print(network_pwd);
  wifi.write(34);
  wifi.println();
  delay(200);// delay to allow setting change
  while (!wifi_op_complete('K')) { //function to prevent buffer overflow and response truncation from ESP8266 (OK is the last word and 'K' is the last letter)
  }
  wifi.println("AT+CIPMUX=1"); //ALLOW MULTIPLE CONNECTIONS (may be optional)
  delay(200);//delay to allow setting change
  if (wifi.available()) { //empty the serial buffer after reset
    Serial.println("ALLOW MULTIPLE CONNECTIONS.. 6/7");
    Serial.print(wifi.readString());
  }
  wifi.println("AT+CIPSERVER=1,80"); //START SERVER ON PORT 80, this allows connection to IP:PORT (type IP:PORT in web browser to view page, IP is all that is needed if PORT is 80).
  delay(200);//delay to allow setting change
  if (wifi.available()) { //empty the serial buffer after reset
    Serial.println("SERVER STARTING.. 7/7");
    Serial.print(wifi.readString());
    Serial.println("SYSTEM READY..");
    Serial.println();
  }
  wifi.println("AT+CIFSR"); //Display IP and MAC Address
  Serial.println("TYPE IP ADDRESS BELOW IN LOCAL NETWORK BROWSER TO TEST");
}
bool wifi_op_complete(char K){//function to test for the end of transmission and print buffer while waiting
  char next_char=wifi.peek();
  if(next_char!=-1){
  Serial.write(next_char);
  wifi.read();
  }
  if(next_char==K){
    Serial.println();
    return true;
  }
  else return false;
}

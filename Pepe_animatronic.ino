#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <Arduino.h>
#include <Audio.h>
#include <Servo.h>
#include "Pepe_playlist.h" // This is a seperate file that just contains an array of mp3 locations

/* Put your SSID & Password */
const char* ssid = "Pepe-controller";  // Enter SSID here
const char* password = "1234";  //Enter Password here

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);

bool eyes_left_status = false ; // Variable that holds the position of the eyes for the web-app visuals
bool eyes_right_status = false ; // Variable that holds the position of the eyes for the web-app visuals
bool eyes_middle_status = true ; // Variable that holds the position of the eyes for the web-app visuals
bool mouth_open_status = false ; // Variable that holds the position of the mouth for the web-app visuals

Servo MouthServo;  // create servo object to control a servo
Servo EyeServo;  // create servo object to control a servo
const int MouthPin = 16;
const int EyePin = 17;
int pos_mouth = 0;    // variable to store the servo starting position for mouth
int pos_eyes = 45;    // variable to store the servo starting position for eyes (left-right)

const int buttonPin = 35;  // The pin input for the button
int buttonState = 0; // The button "starting" state

#define MAX98357A_I2S_DOUT  25 // The pins for the Audio driver module
#define MAX98357A_I2S_BCLK 27 // The pins for the Audio driver module
#define MAX98357A_I2S_LRC  26 // The pins for the Audio driver module
Audio audio; // Initiate audio object

long randNumber; // Generate random numbers so that when the button is pressed, different audio plays

void setup() {
  Serial.begin(115200);
  /*
  WiFi.begin(ssid, password); //connect to your local wi-fi network
  while (WiFi.status() != WL_CONNECTED) { //check wi-fi is connected to wi-fi network
    delay(1000);
    Serial.print(".");
  }
  //Serial.println("WiFi connected..!");
  //Serial.print("Got IP: ");  Serial.println(WiFi.localIP());
  */ 
  //This part is for a local "app"  which become difficult as it needs some jpg's for the webapp
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  


  // The following commands check the current webpage, and depending on the page, will trigger a handler function
  server.on("/", handle_OnConnect);
  server.on("/eyesleft", handle_eyesleft);
  server.on("/eyesright", handle_eyesright);
  server.on("/eyesmiddle", handle_eyesmiddle);
  server.on("/mouthswitch", handle_mouthswitch);
  server.on("/speak", handle_TextCommand);
  server.on("/get", handle_TextCommand);
  server.on("/radio", handle_htmlCommand);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");

  MouthServo.attach(MouthPin);  // Attaches the servo on pin ## to the servo object
  MouthServo.write(pos_mouth); // Put the servo at it's neutral position
  EyeServo.attach(EyePin); // attaches the servo on pin ## to the servo object
  EyeServo.write(pos_eyes); // Put the servo at it's neutral position
 
  pinMode(buttonPin, INPUT_PULLUP); // Initiate a button on 

  // Initiate SD card, check if it mounted properly
  if(!SD.begin(5)){
    Serial.println("Card Mount Failed");
    return;
  }
  // Check if there is an SD card inserted
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }

  // Initiate the audio object
  audio.setPinout(MAX98357A_I2S_BCLK, MAX98357A_I2S_LRC, MAX98357A_I2S_DOUT);
  audio.setVolume(10);

  // Initiate the random number generation
  randomSeed(analogRead(0));
}

void loop() {
  // If the button is pressed, it will trigger an animation + audio event
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    change_EyesOnButton();
    play_AudioMP3(random(0, 20));
    change_MoutnOnButton(1);
  }
  // Check the web-app for changes
  server.handleClient();
  audio.loop();
}

void change_EyePosition(int pos_eyes) {
    EyeServo.write(pos_eyes);
}

void change_MouthPosition(int pos_mouth){
  MouthServo.write(pos_mouth);
//  for (pos = 0; pos <= 75; pos += 1) { // goes from 0 degrees to 75 degrees
//    myservo.write(pos);              // tell servo to go to position in variable 'pos'
//    delay(15);                       // waits 15ms for the servo to reach the position
}

void play_AudioMP3(int song_numb){
    //#####
    //#####
    // I need some sort of list of which file to play... and a RNG to pick it
    //#####
    //#####
    audio.connecttoFS(SD, "/SupriseMofo.wav");
    audio.loop();
}

// The eyes needs to move up and down for as long as the audio plays
void change_EyesOnButton(){
    change_EyePosition(0);
    delay(20);
    change_EyePosition(90);
    delay(20);
    change_EyePosition(45);
}

// The mouth needs to move up and down for as long as the audio plays
void change_MoutnOnButton(int duration){
    //#####
    //#####
    // This needs to sync up with the audio to randomly move
    //#####
    //#####
}

void handle_OnConnect() {
  Serial.println("GPIO4 Status: OFF | GPIO5 Status: OFF");
  server.send(200, "text/html", SendHTML(eyes_left_status,eyes_right_status,eyes_middle_status,mouth_open_status)); 
}

void handle_eyesleft() {
  Serial.println("Eyes turning left");
  change_EyePosition(0);
  eyes_left_status=true; eyes_middle_status=false; eyes_right_status=false;
  server.send(200, "text/html", SendHTML(eyes_left_status,eyes_middle_status,eyes_right_status,mouth_open_status)); 
}

void handle_eyesright() {
  Serial.println("Eyes turning right");
  change_EyePosition(90);
  eyes_left_status=false; eyes_middle_status=false; eyes_right_status=true;
  server.send(200, "text/html", SendHTML(eyes_left_status,eyes_middle_status,eyes_right_status,mouth_open_status)); 
}

void handle_eyesmiddle() {
  Serial.println("Eyes turning to centre");
  change_EyePosition(45);
  eyes_left_status=false; eyes_middle_status=true; eyes_right_status=false;
  server.send(200, "text/html", SendHTML(eyes_left_status,eyes_middle_status,eyes_right_status,mouth_open_status)); 
}

void handle_mouthswitch() {
  Serial.println("Mouth opens");
  if(mouth_open_status){ mouth_open_status=false; change_MouthPosition(0); } else{ mouth_open_status=true; change_MouthPosition(75);}
  server.send(200, "text/html", SendHTML(eyes_left_status,eyes_middle_status,eyes_right_status,mouth_open_status)); 
}

void handle_speak(){
  Serial.println("Speaks");
  play_AudioMP3(random(0, 20));
  server.send(200, "text/html", SendHTML(eyes_left_status,eyes_middle_status,eyes_right_status,mouth_open_status)); 
}

void handle_TextCommand(){
//  Serial.println(HTML_STR_INPUT1);
  String incomming1 = server.arg("INPUT1");        // get request argument value by name
  Serial.println(incomming1);
//  audio.connecttospeech(incomming1, "en");
  server.send(200, "text/html", SendHTML(eyes_left_status,eyes_middle_status,eyes_right_status,mouth_open_status)); 
}

void handle_htmlCommand(){
//  Serial.println(HTML_STR_INPUT1);
  String incomming1 = server.arg("INPUT1");        // get request argument value by name
  Serial.println(incomming1);
  audio.connecttohost(incomming1);
  server.send(200, "text/html", SendHTML(eyes_left_status,eyes_middle_status,eyes_right_status,mouth_open_status)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(uint8_t eyes_left_status,uint8_t eyes_middle_status,uint8_t eyes_right_status,uint8_t mouth_open_status){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Pepechu controller</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="h1 {text-align: center;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  
  // A default button style of the three eye positions
  ptr +=".button {height: 150px; display: inline-block;background-color: rgba(235,53,53,0.2);border: none;color: white;text-decoration: none; font-size: 25px; cursor: pointer; width: 32%; }\n";
  // A longer button variant for the cheeks/nose area (that allows you to activate a speach sequence), and a mouth one that opens ans closes the mouth
  ptr +=".button_long {width: 100%; height: 70px;}\n";
  
  // This will be the main pepe background image
  ptr +=".image {width: 270px;height: 580px;margin-bottom: 0px;background-image:  url('data:image/png;base64,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');\n";
  ptr +="background-size: cover;background-repeat: no-repeat; object-fit: cover; width: 100%;}\n";
  // These are the different pepe overlay images depending on which triggers have been pushed
  ptr +=".img_eye_mid {width: 270px;height: 580px;margin-bottom: 0px;background-image:  url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAQ4AAAHgBAMAAACm0Z5kAAAAAXNSR0IArs4c6QAAABVQTFRFAAAAAAAAHx4KNjUW/N7f/+rq////HAYCWQAAAAF0Uk5TAEDm2GYAAATDSURBVHja7ZhLjts4EIYdCck6voEhjLwW4BsE0n5ASL1uNNC6/xES60GRVJEsehqTzfdtumGRVX8VX0VeLgAAAAAAAAAAACDzcOmKu9de/1dF+FbKTZ27d6+mokv/kg0i/YtWhcK4OoiE1bSZrsiDVm1doKROe8qZeuS7d8pkdPkmL307lKiGRKX1ZRkaJ+qk1ZFmtbZ72o9+Xcot65JJWB6lLET7Y7GvEhli4/9soDCpa+ySjrK9MjbHCo8fjbTCSF6wcm5fGEikfV1+Br0+SxORn3+s0+f+SXhgIX/si6E/suVDKCUw7FnN95Z8nrTJZ2bwaxCOI0sOXyhphDXXqQ5375B6RIJL9nbDq7v49MgVO/XhxPNms1MnTwcvRmnt68oHz4+oo1bVSbEJslvRHLjWUy3M2lpn4BHbgzp9+XBsyY9TQmutgUdsgihj8bYsp/mWG+1elnJVF10LRB1FFr7ivF0bO2ld/i26KsUan2RUjc/tVD4EOoKSItk/PoaBvNBKaOkhnLePgv6687aJcVOd+1W0f/a8dXXEzQjpPaPtLuvoErmYt7/TFMYkcDR0eko5qbvCYO7zx65jcXDL9N+auT0LU7oGY8J0zFtY7dODSRmqjkZeT0urSekWTJPUsfiIh3G0EXU0+ZTaYKLjsvpYGsRkbC1MdFyyKX1a6R0bwjxdjIxRIWuDwQ3lPE+XWPp4Stcx6QdJhxfMOKxaBRnt/nmKm/jTZuwTY7O2GP6YyeiYhqedXyczVfPPM45xyulYnIiR2GAi4+Lno++flkwgpGra5899n8/H6kcUsn3fc57Mx7QOn5/Y6vlx7KdjJid0rGMnCLFLQadjnwOeheNDXoddVCcd04FCx+bs5qTjiEMxLrZVqGMq07G6M46OdirIxyEk0DGV6djD/tca+O4lSq1j+hodb9bA55fo+CHomOfsuExv77uBzyk7LptBX8d7Tsc8S0Imn7d5tfNj/gy+SDJWg2U67s9uH1+owxpM6bj8NR25eTrP4sC8rsMaTOow7V/RYVpRx5DT0Z50bP1POkxOxyjqWLblcRj+Nx3jssKF2mEpT4bIPA3rwn2zcPYPu3MEdWFkno5rwXW7CAkZY+vl7tfJlt7aqZpezIdnw18vg5COtXzwNmV3WKykNnR2c/N51nH3jcz+0SBWhnuRbCf77KfUKQulY9tZSqNrwx3c2S+lIqWyE5IRtqDDhiP35pdjk/P9rCMYWhO7Ohxt0jrGQTJkw9g+53SY+KXOuycFa+7DsTGIdykr04nlfl77eRn2OnfScfeObFtLy3fKcfCKhw9RR/pCdwlup02wf7R7wEa8K29NBm/BiDuhydy0bUZS1dgYv2gvjoYpZ8Jknx6qTbJJFIUmcV2vmugks+kwmheQamttooVl5vnCmwOCCZN7iLl671IJI5p3qVgoXv9rWkfstXB9yVG/08XKftGjq+On4slP/VqseGz8FtFxzT7Z3S568p2v1wsAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABTyG9l/ybbgi11LAAAAAElFTkSuQmCC');\n"; 
  ptr +="background-size: cover; background-repeat: no-repeat; object-fit: cover; width: 100%;}\n";
  ptr +=".img_eye_left {width: 270px;height: 580px;margin-bottom: 0px;background-image:  url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAQ4AAAHgBAMAAACm0Z5kAAAAAXNSR0IArs4c6QAAAB5QTFRFAAAArDIyAAAAHx4KNjUW+NbX++Hh/N7f/+rq/////fIMoAAAAAJ0Uk5TAA0IIrGFAAAFDElEQVR42u2Yu67jNhBADVjF/YyFqpQO3NidCn9DkDqFwB9Y2OXCF5GU2pX+NtabjyE1dFKeAyxgeMXhmeGIpO/hAAAAAAAAAAAAADI3myp7eOGM/1TCjZIfKhxefVqKKv3NbhLpb7QWiuDqJBJR02GqrBm0tkWGSZGeaS/UbX94pSxGtf/IR/+3maiWROX6sYZmEnXRishjhXZ4eh79eyk/WeQ0YX6Wsoj2y+y5cjTEh/9zgMyiTrlLHnl7ZazHMo8fjVpmJh9ECZ/PTCTyfJF/Bn3epYnMwy+L9LkfiHsR9o99MfXb7vXBV/ECO1H3R0tzBm7ymel966VjacnpC1ca4Z2rVIe7c0jdIsklR9vpFVW8PfYuO8U2iTPbWp0ieTo4OUrvvu764MwjehSqe1KsQZYomgN3nakQurbQBbjF9qBKf33YtuRbUNBCG+AWaxBlLs6WZT0+10a7l6WmKrJ+FogeWRH+j/N2etgq6/gx66dS7OFQ43h2OAXXB8/Du1KkhifW0NfzwvihbsJ5e9MP156348DOwYuVOvdXCX/4/nlreyxhLl3SJIZditRw2aNySnEWCtJ1jQmSCnEXwxn/7RSlqPaS6SMe9ZTVSVkMoaZGV9IpmWv/kj3MnNRJreF5zImcdSvb970fpB4xa3VVaxJ6mKVRTpoGCz0a43dJTKP3RZz+2vrkpGiwdV1kj3rMKKLxSnnU9rujWNneDtKOEYxb3G8hzlkqx5TLfVpT47zEJ+XSLh6PzgnRzKU9HXY34NmjvU8BtqqarvmWK3KOenSPIc7DXyJfJKIxeDyGVJqh2c26QI1Y0ojGVI/7vX20D7/XOrewMY15XQb/dwZmDdKIa3tOeIxxHkHPDyKKCKPHmMVQjM2jFptsx2OphnFfHasgx7SHu3+8g0xRsj2WZXWarfv+XenRBR5zp3xYj67eWm343O0vi+PR1PWS0Pwhsz8eQkrGCvOl9NjGzmX95Wh86T0ax2MJ0yk9jJ9P6NH3inWxX5d6i/MlecwBxf6Ie/S9JNIFNG5ycY8l4KWTkTwO76PtzSvDo6nHnot6rAEvkTREj+98j+ljtodJehzHYb3CYxg/74xJj34JaHvUxm3z52+Hjz3qaVHM9OmQ5bGs5lLX0OMselzk5go2kK/5Jp7waDeVbX3KH4HHNd/D3j/2PFrvwB5lnoHHVJC1T4N74byLWPtQ+8e7xGuc49qdkT5t1xN7OhjkcoyBNo9reF8f+bndL4cbhZXPdsN+xd6Xh1PSoSil4OFctdfSeB6t9fnPMZB3QF2ttV3WuRM96mF0mb4nb63iny1Op/xj53Nc5355+7p/larn4U+pGt7PBsfjHvFwAx0Dj0irzz32lKthi3geW6c7b4tf1l2PpcnNjsa2Mku0i7uy9qn9DFfX649wJzTbwjzL2KrYFbk6R/Zaj8axKMXR11fUY72LpavhvH7269IG7fGMtHrqZ3a9nC2N2atGcN0Vd+TR4of2zw52c0wedWp86m9Kf6+/bsdAZZksaqQczfLvr3JHo/QjXbYzcrldj0FK/d+lLv4r74wv0x5bJP/2U5aaklo1tT3KGXFGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAT/gXZtE+l4ame28AAAAASUVORK5CYII=');\n";
  ptr +="background-size: cover; background-repeat: no-repeat; object-fit: cover; width: 100%;}\n";
  ptr +=".img_eye_right {width: 270px;height: 580px;margin-bottom: 0px;background-image:  url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAQ4AAAHgBAMAAACm0Z5kAAAAAXNSR0IArs4c6QAAABhQTFRFAAAAAAAAHx4KNjUW++Hh/N7f/+rq////82GdogAAAAF0Uk5TAEDm2GYAAAT8SURBVHja7Zi7juM4EEV7LWDi7T8wOrBTAf4FQX9gxQ2D1uRO+PtryXrwUcWHu4FNzgkGMx6xdOuySBb18QEAAAAAAAAAAAAyF5e2enjjjX9XhB+lPlQ8vH3Xijb9SzaJ9C+lKgqCFyeRiJoO01a9oVRtU6GkSb8pF+qSH94WmtHmH3nr/3YlRVNSpPVtGSUvKTatUR5rSoen31O+LuUnm5oirM9SFlL6Y/W7amSID/84QKWpr9wlHXV7pVZjlcdPibTKTN6IEj9fmYjyfFN/Br1fpYnM4x+b9LkfCQ8i5I99MfVLtn0IpQSBvaj50dI7I23ymRn8GqTjyJLTF1oaYc21RYe7d0hdlOSSo930mlYvj1yz0+wv8d62udMkTwcvR2ntl7UP3ntEHU1Rn6QVyBql5MDd3tQIVduUBbhoe1Bb3j7sW/IlMrQpDXDRCqQwF2/Lch5fvCndy1KvaqquBaKOqgi/cd6+HnZsnf9adVXSHo5lHL48jlH7EOgIWorU8MQchvKCMGGoi3DeXtLDo1QKztt52LghREqd+4dg+JObkEqb0RGFuaX8/ZDnczSdFOGY09F6VpzGfhTsmMnI2HIwSiaJ4yVIZhrZazLSlhx2J42WSd7SPZmxE3PJBTpMVuo6bJmlUzJd1wVCQh0PPdCXK6OLdJzto8SSJZklD9MrOqzVhHjVLZSptbZgbtYoa4Cul8vDakL8RdYndaiW7gW2BViEhDoWdwUZJ3fHEPzY50W3ZI+yJ3KVy9SKYQ6+jHVeBzcVm90AdhnGMfQ6SDrEfJ4yuq6PDLkPkqWqEHm53YdnGFWHJ+Qwz6qn4z7lcR8TOr5SU+voUOZFCrNsfp3jyZTF9ZryIxLiFrozL7OpCR1Hx47XltHP07GU6P2q7ciqjtFbcJsl5TpmP808sOvWVF7FUaHDK/TeeAWX0DEGEcz6Z+/k8baOxdwSHd9BgM4vsJ/54Vd9UsdqyB+3xvcCu2fr4/hLOr49GU8Jxt1Jf6ijE3RYW6JjOQr6gnl5BUzqMLEOayUhsQ4FSYZ0Wv7xnDBdeEadp2GPX9SxBgzPqNO2DUX8DzrEGLfFRWFitvXi9St+JybreAV8xGf2z3Xsy9WMhTqU3sH40zLkdXw79WVGXcdN1PGIW6HZEF/HfRhyOrY403CzS+jLdAgd2dw+TOeTsylfB6VO978dHT/vw/NYWk+YbecI+kK3TsXOcLoxzEflGG6GkY7z7ozXOFz7cVnwpr+KfoQx5D755Je6tym707JLOnp+3p0deZB0nN0gD+3qELTbQRNjPUut0Be6V2IzBulsRfHYL2P6/eUkdmU3YQuKD+3nv3u1t4x1JO6W7t200xuQTUfqThm2ILGOY/KWfVrd6PT7vlXvUet4t8V1cjmX3SsdR+al0yk6zo/E/TYo9tWPW7BejvkL/+m1EZnd3PIPD6/xJt6Vb/7+kf0Cctg+BnWOu/l7mDc53r48VH6I+fQ/0RmnWba2VIXzXWtUW/bojbKOJdKU1xrr7+2N73TByomzUHT8m/v6mbciTiVRV/8oOj4z35OLvq9VDP78/AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgEr+A7lJlgcN35skAAAAAElFTkSuQmCC');\n"; 
  ptr +="background-size: cover; background-repeat: no-repeat; object-fit: cover; width: 100%;}\n";
  ptr +=".img_mouth_close {width: 270px;height: 580px;margin-bottom: 0px;background-image:  url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAQ4AAAHgAgMAAAApkWvEAAAAAXNSR0IArs4c6QAAAAlQTFRFAAAAMh0VrFQy8wTWLAAAAAF0Uk5TAEDm2GYAAAEzSURBVHja7dW7kYMwFAVQE7gEtp4twYEhUAn0QwkEUpWbrMeWEBjh2WTnnMyMdHkP63O5AAAAAAAAAAAAAADwT/XDi++PZucOZz0mjOkpPh7eGipINcuBih4DprRj3q2oezs/T7rXM0JqUE05XMWzmtv6m6ZWS/l1u7ZefjsqGurH9owUi1L66URImvOQM4WkFG8HQ+IwHg0J++tzq9mdkGzbhvXvAyHxdcq49WgzZG7ZgFM9ZN47BdbnwVQLeT+9iAplyJjmYWxcrsO0Wiftm2cIRchXal+zMcRi75xa9ssfbMBrOBMylIfSmW7KU7by58z53bdueHXIXrNBm7doyF5SOe3LjPv6WnpNWSr3V5e98b59v+2O6Y7c/f279wAAAAAAAAAAAAAAAAAAAAAAAMDTDzyKiJik61j/AAAAAElFTkSuQmCC');\n";
  ptr +="background-size: cover; background-repeat: no-repeat; object-fit: cover; width: 100%;}\n";
  ptr +=".img_mouth_open {width: 270px;height: 580px;margin-bottom: 0px;background-image:  url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAQ4AAAHgBAMAAACm0Z5kAAAAAXNSR0IArs4c6QAAAA9QTFRFAAAAMh0VrDIyrFQy13u6f6tMWwAAAAF0Uk5TAEDm2GYAAAOCSURBVHja7dtrkqsqFIbhOIMQewLJDKiMwKrMf0wnF7tVLmsBAqb2eZ+f2Sn5skC52Pt0AgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA/yDjO7LtmO9I0TDJ8Lz0eE9ln98+N0ph7jlMgyTvOo/3PLZy7wxFKZYk54op7qXqJCkvRdWi7CuFU5RdtRjvddjSmgy1arFvoAwVa1H+aGuRIj+J/ui0wqwyVnvIahdTJzjtJ6TGGNUQP4+YSY1i04JEL/L3c+MhNlGkCyUNDrkvtBTbJJGLnQti2KwIoTBjdhA/xvLLHvmiZVGCeDFKAwTi5ATZlrCgL+Q+Sr5p1jEqpgglsUbqlu3YfNS1HbNCx/x+ybRIsYxbtSBzjrodEugeJcfcLW1KsSmK3DGfcthmtVhqMooF+f3nR2tajk+5fprnmOaW5OHxaE8cIOQI5+g3PuQcHe6XlBy2eZDfSUbO0e15quWwDaeXzwQzJuXQ57kptFZMW8VOq1VIbIL5Wxso8/603jN5K1EpyGatGn+uO8sgE73a5fZxnVt9JVt9JqZYFkImYXVq4z9tafLp/Z1XjOWjcJDJ3ULEV6jOaj2SZFq3+Wl1k+yV7Sdhtyms2N0djw0lcRp99433SSjFmLzLDWzmvPtichp9d43zyXWdPbK/FDcwoa2+3d4ZXqPPVt1kt+XLsTMRef+ibrK9Pni3evOjacchuRtLP8wtIcdNO5Ep2Gi75UzMoZwonQvPP5ZLpOW474whnAfN10gbH8r52v5jut05cg7qhJJYk5BD+CE5J+2DEGRnjswjZaEkSTkqFEM75g88PdOGR+FrmCF21leSY54cCl8GDaGnYn4OYyq8HPMO8G1CjtF7HVDvTaEw0cnTXNWXuOssao5GGdxEeo52rX9jjos6PHrkOCXkOJHjgBxGvW275FBvmD7DlBzeANFy9Pojsss33C7kyByovYYpObIGyLXjX39+SQ6xY/p1y9fkEJ/s/bpFHCDX/2MOYYD0HB5fkyPeMV27RVgrm745Ypu6a99uiRakczmiI7V3OSIF6V6O8Ai59i/HqyAX/ziqf4z3SZV7GnVAObwgR8VwghwX4xPk8ndSeViM03zKe+v7f6OiJTEHF2OpyQkAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAHTwH8TS7PYjsVbZAAAAAElFTkSuQmCC');\n";
  ptr +="background-size: cover; background-repeat: no-repeat; object-fit: cover; width: 100%;}\n";

  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";

  // Change overlay images depending on which trigger has been activated, eyes are three seperate triggers... mouths is an on/off switch
  if(eyes_middle_status){
      ptr +="<div class=\"image\"><div class=\"img_eye_mid\">";
  }else if(eyes_left_status){
    ptr +="<div class=\"image\"><div class=\"img_eye_left\">";
  }else{
    ptr +="<div class=\"image\"><div class=\"img_eye_right\">";
  }
  if(mouth_open_status){
    ptr +="<div class=\"img_mouth_open\">\n";
  }else{
    ptr +="<div class=\"img_mouth_close\">\n";
  }

  // Add buttons with 100% alpha = invisible
  ptr +="<a class=\"button\" href=\"/eyesleft\"></a>\n";
  ptr +="<a class=\"button\" href=\"/eyesmiddle\"></a>\n";
  ptr +="<a class=\"button\" href=\"/eyesright\"></a>\n";
  ptr +="<a class=\"button button_long\" href=\"/speak\"></a>\n";
  ptr +="<a class=\"button button_long\" href=\"/mouthswitch\"></a>\n";
  ptr +="<br /><br /><br /><br /><br />\n";
  ptr +="<form action=\"/get\">\n";
  ptr +="  Pepe: <input type=\"text\" name=\"INPUT1\">\n";
  ptr +="  <input type=\"submit\" value=\"Submit\">\n";
  ptr +="</form><br>\n";
  ptr +="</div></div></div>\n";

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

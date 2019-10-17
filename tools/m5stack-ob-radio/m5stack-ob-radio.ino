/**************************************************************
 *
 * For this example, you need to install PubSubClient library:
 *   https://github.com/knolleary/pubsubclient
 *   or from http://librarymanager/all#PubSubClient
 *
 * TinyGSM Getting Started guide:
 *   http://tiny.cc/tiny-gsm-readme
 *
 * For more MQTT examples, see PubSubClient library
 *
 **************************************************************
 * Use Mosquitto client tools to work with MQTT
 *   Ubuntu/Linux: sudo apt-get install mosquitto-clients
 *   Windows:      https://mosquitto.org/download/
 *
 * Subscribe for messages:
 *   mosquitto_sub -h test.mosquitto.org -t HSHCup/punches -t <topic2>
 * Toggle led:
 *   mosquitto_pub -h test.mosquitto.org -t GsmClientTest/led -q 1 -m "toggle"
 *
 * You can use Node-RED for wiring together MQTT-enabled devices
 *   https://nodered.org/
 * Also, take a look at these additional Node-RED modules:
 *   node-red-contrib-blynk-ws
 *   node-red-dashboard
 *
 **************************************************************/
//#fialovy/zluty RX, modry/zeleny TX

#include <M5Stack.h>
// Select your modem:
#define TINY_GSM_MODEM_SIM800
// #define TINY_GSM_MODEM_SIM808
// #define TINY_GSM_MODEM_SIM900
// #define TINY_GSM_MODEM_UBLOX
// #define TINY_GSM_MODEM_BG96
// #define TINY_GSM_MODEM_A6
// #define TINY_GSM_MODEM_A7
// #define TINY_GSM_MODEM_M590
// #define TINY_GSM_MODEM_ESP8266
// #define TINY_GSM_MODEM_XBEE

// Set serial for debug console (to the Serial Monitor, speed 115200)
#define SerialSRR Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on Mega, Leonardo, Micro
#include <HardwareSerial.h>
HardwareSerial Serial2(1); //cannot be 2 - https://github.com/vshymanskyy/TinyGSM/issues/91#issuecomment-356024747

#define SerialAT Serial2



#include <PubSubClient.h>
#include <TinyGsmClient.h>
TinyGsm modem(SerialAT);
TinyGsmClient client(modem);

// Your GPRS credentials
// Leave empty, if missing user or pass
const char apn[]  = "internet.t-mobile.cz";
const char user[] = "";
const char pass[] = "";

// MQTT details
const char* broker_url = "test.mosquitto.org";
const int broker_port = 1883;

String topicOB = "HSH/2018";
String topic_punch = topicOB + "/punch";

PubSubClient mqtt(client);

long lastReconnectAttempt = 0;

void setup() {
  M5.begin();
  m5.Speaker.mute();
  M5.Lcd.printf("Mqtt Ob\n"); // just to see that the program has started
  M5.Lcd.printf("Open SerialSSR\n"); 
  SerialSRR.begin(115200);
  delay(1000);
  // see https://github.com/vshymanskyy/TinyGSM/issues/91#issuecomment-356024747
  M5.Lcd.printf("Open Modem serial\n"); 
  SerialAT.begin(9600, SERIAL_8N1, 16, 17, false);
  delay(1000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  //SerialMon.println("Initializing modem...");
  M5.Lcd.printf("Initializing modem...\n"); 
  modem.restart();
  //modem.init();

  String modem_info = modem.getModemInfo();
  M5.Lcd.printf("%s\n", modem_info); 
  //SerialMon.print("Modem: ");
  //SerialMon.println(modemInfo);

  // Unlock your SIM card with a PIN
  //modem.simUnlock("1234");

  //SerialMon.print("Waiting for network...");
  M5.Lcd.printf("Waiting for network ... "); 
  if (!modem.waitForNetwork()) {
    M5.Lcd.printf("FAIL\n"); 
    //SerialMon.println(" fail");
    while (true);
  }
  M5.Lcd.printf("OK\n"); 
  //SerialMon.println(" OK");

  //SerialMon.print("Connecting to ");
  //SerialMon.print(apn);
  M5.Lcd.printf("Connecting to APN %s ... ", apn); 
  if (!modem.gprsConnect(apn, user, pass)) {
    M5.Lcd.printf("FAIL\n"); 
    //SerialMon.println(" fail");
    while (true);
  }
  M5.Lcd.printf("OK\n"); 
  //SerialMon.println(" OK");

  // MQTT Broker setup
  mqtt.setServer(broker_url, broker_port);
  mqtt.setCallback(mqttCallback);
}

boolean mqttConnect() {
  //SerialMon.print("Connecting to ");
  //SerialMon.print(broker);

  // Connect to MQTT Broker
  boolean status = mqtt.connect("M5StackOBRadio");

  // Or, if you want to authenticate MQTT:
  //boolean status = mqtt.connect("GsmClientName", "mqtt_user", "mqtt_pass");

  if (status == false) {
    //SerialMon.println(" fail");
    return false;
  }
  //SerialMon.println(" OK");
  mqtt.publish(topicOB.c_str(), "Control started", true); //true for retain
  //mqtt.subscribe(topicOB);
  return mqtt.connected();
}

int readN(int n, char *buff)
{
  int ret = 0;
  while(ret<n) {
    int n1 = SerialSRR.available();
    int needed = n - ret;
    if(n1 > needed)
      n1 = needed;
    for(int i=0; i<n1; i++) {
      int fb = SerialSRR.read();
      buff[ret++] = fb;
    }
  }
  return ret;
}

void loop() {

  if (!mqtt.connected()) {
    //SerialMon.println("=== MQTT NOT CONNECTED ===");
    // Reconnect every 10 seconds
    unsigned long t = millis();
    if (t - lastReconnectAttempt > 10000L) {
      lastReconnectAttempt = t;
      M5.Lcd.printf("Connecting MQTT broker %s:%d ... ", broker_url, broker_port); 
      if (mqttConnect()) {
        lastReconnectAttempt = 0;
      }
    }
    delay(100);
    return;
  }

  // receive SRR data
  if (SerialSRR.available() > 0) {
    int fb = SerialSRR.read();
    // punch layout: STX command, length, data, crc, ETX, example:  02 ef 01 08 ea 09 03
    if(fb != 2) { 
      M5.Lcd.printf("Invalid data received: %0x", fb);
      SerialSRR.flush(); 
    }
    else {
      String msg = "{";
      #define BUFF_LEN 32
      char buff[BUFF_LEN];
      
      readN(1, buff);
      int cmd = buff[0];
      msg += "\"cmd\":" + String(cmd);
      
      readN(1, buff);
      int length = buff[0];

      if(length < BUFF_LEN) {
        readN(length, buff);
      }
      else {
        M5.Lcd.printf("Invalid length received: %d", length);
        SerialSRR.flush(); 
      }

      msg += ", \"data\":\"";
      for(int i=0; i<length; i++) {
        String s = String((int)buff[i], HEX);
        if(s.length() == 1)
          s = "0" + s;
          msg += s;
      }
      msg += "\"";

      readN(2, buff);
      int crc = 256 * (int)buff[0] + (int)buff[1];
      msg += ", \"crc\":" + String(crc);

      readN(1, buff);
      if(buff[0] == 0x03) {
        msg += "}";
      }
      else {
        M5.Lcd.printf("Invalid ETX received: %x", (int)buff[0]);
        SerialSRR.flush(); 
        msg = "";
      }
      if(msg.length() > 0) {
        mqtt.publish(topic_punch.c_str(), msg.c_str(), false);
        M5.Lcd.printf("Punch received: %s", msg);   
      }
    }
  }

  mqtt.loop();
  M5.update();
}

void mqttCallback(char* topic, byte* payload, unsigned int len) {
  //SerialMon.print("Message arrived [");
  //SerialMon.print(topic);
  //SerialMon.print("]: ");
  //SerialMon.write(payload, len);
  //SerialMon.println();
  M5.Lcd.printf("Message arrived %s", payload);
}



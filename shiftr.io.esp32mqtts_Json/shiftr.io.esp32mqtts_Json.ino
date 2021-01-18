#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
//#include <ESPmDNS.h>
#include <ArduinoJson.h>

const char* ssid = " MI RED WIFI";  const char* password = "MI PASSWORD";

//este es el token que me provee shiftr
//mqtt://tecaperupaul:nCDPLPtU95E@tecaperupaul.cloud.shiftr.io
//hay que separar el nombre de dominio, nombre de usuario y password

/* this is the MDNS name of PC where you installed MQTT Server */
const char* serverHostname = "tecaperupaul.cloud.shiftr.io";
const char* username = "tecaperupaul";
const char* password_mqtt = "nCDPLPtU95E";
const int port = 8883; //este es el puerto segun la documentacion de shiftr.io

//para obtener los certificados hay que ejecutar el comando
// openssl s_client -showcerts -connect tecaperupaul.cloud.shiftr.io:8883


const char* ca_cert = \ 
"-----BEGIN CERTIFICATE-----\n" \
"MIIFODCCBCCgAwIBAgISA2nw4dZ0EDcHe0anqSc+0CmRMA0GCSqGSIb3DQEBCwUA\n" \
"MDIxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MQswCQYDVQQD\n" \
"EwJSMzAeFw0yMDEyMDQyMTEyNTVaFw0yMTAzMDQyMTEyNTVaMBoxGDAWBgNVBAMT\n" \
"D2Nsb3VkLnNoaWZ0ci5pbzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \
"AOatHPqduVxiHXOSgDw3bLHMEjL1GtMA+PleYAG2YeF6FS7kkjeOxZleC3uiTcNj\n" \
"gXpD5BgK2MrzICVcEGot51zewSoGJmpiBR2TqJMBUBngZtGUUmGxSEofcc5Vd1ef\n" \
"nnpA8pFj8yHE4M63/xJScHUhtaD55kG2Ig8AVWuO/P0qhojtO7tjXEr5VX3b0VEi\n" \
"Mn3nXRfjyuunAaTu2jJqdu3fHC89IlqmH5hJycjjuDHKKBe6MVZ4Xjol7fQX8AqI\n" \
"oWrjLUQgE0k4+x6QbwM+XerXKgwebvvEagJajajtf4EcN5WxRnYtHryQyKH/P02b\n" \
"ewQCU7BKqA8TBGvpUnuoo/kCAwEAAaOCAl4wggJaMA4GA1UdDwEB/wQEAwIFoDAd\n" \
"BgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwDAYDVR0TAQH/BAIwADAdBgNV\n" \
"HQ4EFgQUdPUH6SRDT2+KYFewAeax8IgWoI0wHwYDVR0jBBgwFoAUFC6zF7dYVsuu\n" \
"UAlA5h+vnYsUwsYwVQYIKwYBBQUHAQEESTBHMCEGCCsGAQUFBzABhhVodHRwOi8v\n" \
"cjMuby5sZW5jci5vcmcwIgYIKwYBBQUHMAKGFmh0dHA6Ly9yMy5pLmxlbmNyLm9y\n" \
"Zy8wLQYDVR0RBCYwJIIRKi5jbG91ZC5zaGlmdHIuaW+CD2Nsb3VkLnNoaWZ0ci5p\n" \
"bzBMBgNVHSAERTBDMAgGBmeBDAECATA3BgsrBgEEAYLfEwEBATAoMCYGCCsGAQUF\n" \
"BwIBFhpodHRwOi8vY3BzLmxldHNlbmNyeXB0Lm9yZzCCAQUGCisGAQQB1nkCBAIE\n" \
"gfYEgfMA8QB2AESUZS6w7s6vxEAH2Kj+KMDa5oK+2MsxtT/TM5a1toGoAAABdi/Q\n" \
"s9cAAAQDAEcwRQIhAJCcYhoykpn3Uhjjzdi9XTuWeimGPhpR2czzB3jQOS1/AiBC\n" \
"CPo7RybZmfa3bp+lGQeZbkLUzz8KG+g2uNULEvJubAB3APZclC/RdzAiFFQYCDCU\n" \
"Vo7jTRMZM7/fDC8gC8xO8WTjAAABdi/QtAgAAAQDAEgwRgIhAPLQ3D16BFS6o24f\n" \
"ibHi1dhg2wvdpbNW37hr9ca2kgBoAiEA3SsPTqEgThNZW0ZvVnGjx/1eGmfbU93M\n" \
"zNtxtDgwK9gwDQYJKoZIhvcNAQELBQADggEBAHzUKS3YEMvzJIqVq4bpQsNr1J0f\n" \
"GOJ3/9hNGNrChTeGTUY8hUC7I5bMG+Z11Zr6m3/Lbv0Z3pIUJT1oWp4ikTZiE4dh\n" \
"e7qo/XBGZXANeKK858mfWLjMCuXVQzFbj3o0296TjhKWJyklDqmTECdIjwi+c/cB\n" \
"VFq5L/nrv7XVKDYrOp3LuKJ5LpJQYckPus7WOs8lPPueiWl7zvxeWwYwpbLLGiti\n" \
"zB0Oeat26TRUgElUfkBY3sH586/I5wSKVcLUGOPDV7rCCUj0CP3kwbPSauqmYGp/\n" \
"u2N1E+AZGSY/dtD/FxXJxZ5Jpl6Qm1s6EgDYYuuv+lkcqfEZlrUEexqPTe0=\n" \
"-----END CERTIFICATE-----\n" \
"-----BEGIN CERTIFICATE-----\n" \
"MIIEZTCCA02gAwIBAgIQQAF1BIMUpMghjISpDBbN3zANBgkqhkiG9w0BAQsFADA/\n" \
"MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
"DkRTVCBSb290IENBIFgzMB4XDTIwMTAwNzE5MjE0MFoXDTIxMDkyOTE5MjE0MFow\n" \
"MjELMAkGA1UEBhMCVVMxFjAUBgNVBAoTDUxldCdzIEVuY3J5cHQxCzAJBgNVBAMT\n" \
"AlIzMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuwIVKMz2oJTTDxLs\n" \
"jVWSw/iC8ZmmekKIp10mqrUrucVMsa+Oa/l1yKPXD0eUFFU1V4yeqKI5GfWCPEKp\n" \
"Tm71O8Mu243AsFzzWTjn7c9p8FoLG77AlCQlh/o3cbMT5xys4Zvv2+Q7RVJFlqnB\n" \
"U840yFLuta7tj95gcOKlVKu2bQ6XpUA0ayvTvGbrZjR8+muLj1cpmfgwF126cm/7\n" \
"gcWt0oZYPRfH5wm78Sv3htzB2nFd1EbjzK0lwYi8YGd1ZrPxGPeiXOZT/zqItkel\n" \
"/xMY6pgJdz+dU/nPAeX1pnAXFK9jpP+Zs5Od3FOnBv5IhR2haa4ldbsTzFID9e1R\n" \
"oYvbFQIDAQABo4IBaDCCAWQwEgYDVR0TAQH/BAgwBgEB/wIBADAOBgNVHQ8BAf8E\n" \
"BAMCAYYwSwYIKwYBBQUHAQEEPzA9MDsGCCsGAQUFBzAChi9odHRwOi8vYXBwcy5p\n" \
"ZGVudHJ1c3QuY29tL3Jvb3RzL2RzdHJvb3RjYXgzLnA3YzAfBgNVHSMEGDAWgBTE\n" \
"p7Gkeyxx+tvhS5B1/8QVYIWJEDBUBgNVHSAETTBLMAgGBmeBDAECATA/BgsrBgEE\n" \
"AYLfEwEBATAwMC4GCCsGAQUFBwIBFiJodHRwOi8vY3BzLnJvb3QteDEubGV0c2Vu\n" \
"Y3J5cHQub3JnMDwGA1UdHwQ1MDMwMaAvoC2GK2h0dHA6Ly9jcmwuaWRlbnRydXN0\n" \
"LmNvbS9EU1RST09UQ0FYM0NSTC5jcmwwHQYDVR0OBBYEFBQusxe3WFbLrlAJQOYf\n" \
"r52LFMLGMB0GA1UdJQQWMBQGCCsGAQUFBwMBBggrBgEFBQcDAjANBgkqhkiG9w0B\n" \
"AQsFAAOCAQEA2UzgyfWEiDcx27sT4rP8i2tiEmxYt0l+PAK3qB8oYevO4C5z70kH\n" \
"ejWEHx2taPDY/laBL21/WKZuNTYQHHPD5b1tXgHXbnL7KqC401dk5VvCadTQsvd8\n" \
"S8MXjohyc9z9/G2948kLjmE6Flh9dDYrVYA9x2O+hEPGOaEOa1eePynBgPayvUfL\n" \
"qjBstzLhWVQLGAkXXmNs+5ZnPBxzDJOLxhF2JIbeQAcH5H0tZrUlo5ZYyOqA7s9p\n" \
"O5b85o3AM/OJ+CktFBQtfvBhcJVd9wvlwPsk+uyOy2HI7mNxKKgsBTt375teA2Tw\n" \
"UdHkhVNcsAKX1H7GNNLOEADksd86wuoXvg==\n" \
"-----END CERTIFICATE-----\n" ;



/* create an instance of WiFiClientSecure */
WiFiClientSecure espClient;
PubSubClient client(espClient);

/*LED GPIO pin*/
const char led = 2;//

/* topics */
//#define COUNTER_TOPIC    "smarthome/room1/counter"
#define LED_TOPIC     "test" /* 1=on, 0=off */

long lastMsg = 0;
char msg[20];
int counter = 0;

void receivedCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  Serial.println(topic);

  Serial.print("payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  //DynamicJsonDocument doc(1024);
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  int humedad = doc["humedad"];
  /* we got '1' -> on */
  if ( humedad < 30 ) {
    digitalWrite(13, LOW);
    for(int i=0;i<128;i++){
      digitalWrite( led, not digitalRead(led) ); 
      delay(90);
      }
      digitalWrite(led, LOW);
      digitalWrite(13, HIGH);
  } else {
    /* we got '0' -> on */
    digitalWrite(led, LOW);
  }

}

void mqttconnect() {
  /* Loop until reconnected */
  while (!client.connected()) {
    Serial.print("MQTT connecting ...");
    /* client ID */
    String clientId = "ESP32Client";
    /* connect now */
    //if (client.connect(clientId.c_str(),"tecaperupaul","EagDBkNwDL7imJxx")) {
    if (client.connect(clientId.c_str(),username,password_mqtt)) {
      Serial.println("connected");
      /* subscribe topic */
      client.subscribe(LED_TOPIC);
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
}

void setup() {
  pinMode(led, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  Serial.begin(115200);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  delay(3000);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  /* set led as output to control led on-off */
  

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  espClient.setCACert(ca_cert);
  //client.setServer("tecaperupaul.cloud.shiftr.io", 8883); 
  client.setServer(serverHostname, port); 
  client.setCallback(receivedCallback);
  
}
void loop() {
  /* if client was disconnected then try to reconnect again */
  if (!client.connected()) {
    mqttconnect();
  }
  /* this function will listen for incomming 
  subscribed topic-process-invoke receivedCallback */
  client.loop();
  /* we increase counter every 3 secs
  we count until 3 secs reached to avoid blocking program if using delay()*/
  /*long now = millis();
  if (now - lastMsg > 3000) {
    lastMsg = now;
    if (counter < 100) {
      counter++;
      snprintf (msg, 20, "%d", counter);
      // publish the message 
      client.publish(COUNTER_TOPIC, msg);
    }else {
      counter = 0;  
    }
  }*/
}

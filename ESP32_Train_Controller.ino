#include <WiFi.h>
#include <PubSubClient.h>
#include <Preferences.h>

// WiFi details
#include "secrets.h"

// GPIO definitions
#define IN1 16
#define IN2 17
#define PWM_CHANNEL 0
#define PWM_FREQ 1000
#define PWM_RESOLUTION 8

Preferences prefs;

// config variables
String train_name = "DefaultTrain";
int maxSpeed = 255;
int accelRate = 1;

// dynamic unique id
char train_id[8]; // 4 hex characters
char topic_speed[64];
char topic_direction[64];
char topic_status[64];
char topic_name[64];
char topic_config_name[64];
char topic_config_maxspeed[64];
char topic_config_accelrate[64];

WiFiClient espClient;
PubSubClient client(espClient);

int targetSpeed = 0;
int currentSpeed = 0;
String direction = "stop";

// watchdog
unsigned long lastMsgTime = 0;
unsigned long watchdogTimeout = 5000;

void buildTopics() {
  sprintf(topic_speed, "train/%s/speed", train_id);
  sprintf(topic_direction, "train/%s/direction", train_id);
  sprintf(topic_status, "train/%s/status", train_id);
  sprintf(topic_name, "train/%s/name", train_id);
  sprintf(topic_config_name, "train/%s/config/friendlyname", train_id);
  sprintf(topic_config_maxspeed, "train/%s/config/maxspeed", train_id);
  sprintf(topic_config_accelrate, "train/%s/config/accelrate", train_id);
}

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i=0; i<length; i++) msg += (char)payload[i];

  if (String(topic) == topic_speed) {
    targetSpeed = constrain(msg.toInt(), 0, 255);
  }
  if (String(topic) == topic_direction) {
    direction = msg;
  }
  if (String(topic) == topic_config_name) {
    prefs.putString("name", msg);
    train_name = prefs.getString("name", "DefaultTrain");
    client.publish(topic_name, train_name.c_str());
  }
  if (String(topic) == topic_config_maxspeed) {
    int ms = constrain(msg.toInt(), 0, 255);
    prefs.putInt("maxspeed", ms);
    maxSpeed = ms;
  }
  if (String(topic) == topic_config_accelrate) {
    int ar = constrain(msg.toInt(), 1, 10);
    prefs.putInt("accelrate", ar);
    accelRate = ar;
  }
  lastMsgTime = millis();
}

void reconnect() {
  while (!client.connected()) {
if (client.connect(train_id, mqtt_user, mqtt_pass)) {
      client.subscribe(topic_speed);
      client.subscribe(topic_direction);
      client.subscribe(topic_config_name);
      client.subscribe(topic_config_maxspeed);
      client.subscribe(topic_config_accelrate);
      client.publish(topic_name, train_name.c_str());
    } else {
      delay(2000);
    }
  }
}

void updateMotor() {
  if (targetSpeed > maxSpeed) targetSpeed = maxSpeed;

  if (currentSpeed < targetSpeed) currentSpeed += accelRate;
  if (currentSpeed > targetSpeed) currentSpeed -= accelRate;
  if (abs(currentSpeed - targetSpeed) < accelRate) currentSpeed = targetSpeed;

  if (direction == "forward") {
    digitalWrite(IN2, LOW);
    ledcWrite(PWM_CHANNEL, currentSpeed);
  } else if (direction == "reverse") {
    digitalWrite(IN2, HIGH);
    ledcWrite(PWM_CHANNEL, currentSpeed);
  } else {
    digitalWrite(IN2, LOW);
    ledcWrite(PWM_CHANNEL, 0);
  }

  static unsigned long lastStatus = 0;
  if (millis() - lastStatus > 1000) {
    char report[64];
    snprintf(report, sizeof(report), "speed=%d direction=%s", currentSpeed, direction.c_str());
    client.publish(topic_status, report);
    lastStatus = millis();
  }
}

void setup() {
  Serial.begin(115200);

  uint64_t chipid = ESP.getEfuseMac();
  uint16_t shortid = (uint16_t)(chipid & 0xFFFF);
  sprintf(train_id, "%04X", shortid);

  prefs.begin("trainprefs", false);
  train_name = prefs.getString("name", "DefaultTrain");
  maxSpeed = prefs.getInt("maxspeed", 255);
  accelRate = prefs.getInt("accelrate", 1);

  buildTopics();

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(IN1, PWM_CHANNEL);
  ledcWrite(PWM_CHANNEL, 0);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  lastMsgTime = millis();
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  updateMotor();

  if (millis() - lastMsgTime > watchdogTimeout) {
    targetSpeed = 0;
    direction = "stop";
  }
  delay(10);
}

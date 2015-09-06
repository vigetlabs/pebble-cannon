#include <ArduinoPebbleSerial.h>
#include <Servo.h>

static const uint16_t SUPPORTED_SERVICES[] = {0x1001};
static const uint8_t NUM_SERVICES = 2;
// static uint8_t pebble_buffer[GET_PAYLOAD_BUFFER_SIZE(200)];
static uint8_t pebble_buffer[256];
static bool is_connected = false;

Servo xServo;
Servo yServo;
Servo fireServo;

bool xRead = false;
bool yRead = false;
bool fire  = false;

int16_t xAxis;
int16_t yAxis;

int xMax = 150;
int xMin = 30;

int yMax = 110;
int yMin = 90;

int xPosition = 90;
int yPosition = 100;
int minTriggerPosition = 0;
int maxTriggerPosition = 50;

void setup() {
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);

  for (int i = 0; i < 3; i++) {
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
  }

  // Teensy 3.0/3.1 uses hardware serial mode (pins 0/1) with RX/TX shorted together
  ArduinoPebbleSerial::begin_hardware(pebble_buffer, sizeof(pebble_buffer), Baud57600,
                                      SUPPORTED_SERVICES, NUM_SERVICES);

  xServo.attach(3);
  yServo.attach(4);
  fireServo.attach(5);

  xServo.write(xPosition);
  yServo.write(yPosition);
  fireServo.write(minTriggerPosition);
}

void loop() {
  determineState();
  display();
}

void determineState() {
  checkForConnection();
  readFromPebble();
}

void checkForConnection() {
  if (ArduinoPebbleSerial::is_connected()) {
    if (!is_connected) {
      Serial.println("Connected to the smartstrap!");
      is_connected = true;
    }
  } else {
    if (is_connected) {
      Serial.println("Disconnected from the smartstrap!");
      is_connected = false;
    }
  }
}

void readFromPebble() {
  size_t length;
  uint16_t service_id;
  uint16_t attribute_id;
  RequestType type;

  if (ArduinoPebbleSerial::feed(&service_id, &attribute_id, &length, &type)) {
    if (service_id == 0x1001) {
      if (attribute_id == 0x1001) {
        xAxis = *(int16_t*)pebble_buffer;
        xRead = true;
      }
      else if (attribute_id == 0x1002) {
        yAxis = *(int16_t*)pebble_buffer;
        yRead = true;
      }
      else if (attribute_id == 0x1006) {
        fire = true;
      }
    }
  }
}

void display() {
  if (xRead && yRead) {
    xRead = false;
    yRead = false;
    Serial.printf("%d\t%d\n", xAxis, yAxis);

    xPosition = map(xAxis, -1000, 1000, xMax, xMin);
    yPosition = map(yAxis, -1000, 1000, yMax, yMin);

    xServo.write(xPosition);
    yServo.write(yPosition);
  }

  if (fire) {
    fireTheCannon();
    fire = false;
  }
}

void fireTheCannon() {
  Serial.println("FIRE CANNON!");
  fireServo.write(maxTriggerPosition);
  delay(750);
  fireServo.write(minTriggerPosition);
}

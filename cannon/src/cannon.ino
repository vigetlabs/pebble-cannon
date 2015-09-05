#include <ArduinoPebbleSerial.h>

static const uint16_t SUPPORTED_SERVICES[] = {0x1001};
static const uint8_t NUM_SERVICES = 2;
// static uint8_t pebble_buffer[GET_PAYLOAD_BUFFER_SIZE(200)];
static uint8_t pebble_buffer[256];

void setup() {
  // General init
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
}

bool xRead = false;
bool zRead = false;
bool yRead = false;

int16_t xAxis;
int16_t yAxis;
int16_t zAxis;

void loop() {
  // Let the ArduinoPebbleSerial code do its processing
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
      else if (attribute_id == 0x1003) {
        zAxis = *(int16_t*)pebble_buffer;
        zRead = true;
      }
      else if (attribute_id == 0x1004) {
        Serial.println("TAPP!");
      }
    }
  }

  if (xRead && yRead && zRead) {
    xRead = false;
    zRead = false;
    yRead = false;
    Serial.printf("%d\t%d\t%d\n", xAxis, yAxis, zAxis);
  }

  static bool is_connected = false;
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

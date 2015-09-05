#include <pebble.h>

#define TIMEOUT_MS 1000
#define MAX_READ_SIZE 100

static Window *s_main_window;
static TextLayer *s_status_layer;
static TextLayer *s_attr_text_layer;
static TextLayer *s_raw_text_layer;
static char s_text_buffer1[20];
static char s_text_buffer2[20];
static SmartstrapAttribute *x_attr_attribute;
static SmartstrapAttribute *y_attr_attribute;
static SmartstrapAttribute *z_attr_attribute;
static SmartstrapAttribute *tap_attr_attribute;
static SmartstrapAttribute *compass_attr_attribute;


static void prv_update_text(void) {
  if (smartstrap_service_is_available(0x1001)) {
    text_layer_set_text(s_status_layer, "Fire at will!");
  } else {
    text_layer_set_text(s_status_layer, "Connecting...");
  }
}

static void prv_did_read(SmartstrapAttribute *attr, SmartstrapResult result,
                              const uint8_t *data, size_t length) {
  // APP_LOG(APP_LOG_LEVEL_ERROR, "did_read(<%p>, %d)", attr, result);
}

static void prv_did_write(SmartstrapAttribute *attr, SmartstrapResult result) {
  // APP_LOG(APP_LOG_LEVEL_ERROR, "did_write(<%p>, %d)", attr, result);
}

static void prv_send_request(void *context) {
}

static void prv_availablility_status_changed(SmartstrapServiceId service_id, bool is_available) {
  // APP_LOG(APP_LOG_LEVEL_DEBUG, "Availability for 0x%x is %d", service_id, is_available);
  prv_update_text();
}

static void prv_notified(SmartstrapAttribute *attr) {
  // APP_LOG(APP_LOG_LEVEL_ERROR, "notified(<%p>)", attr);
}

static void prv_main_window_load(Window *window) {
  s_status_layer = text_layer_create(GRect(0, 15, 144, 40));
  text_layer_set_font(s_status_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  prv_update_text();
  text_layer_set_text_color(s_status_layer, GColorBlack);
  text_layer_set_background_color(s_status_layer, GColorClear);
  text_layer_set_text_alignment(s_status_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_status_layer, GTextOverflowModeWordWrap);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_status_layer));

  s_attr_text_layer = text_layer_create(GRect(0, 60, 144, 40));
  text_layer_set_font(s_attr_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  // text_layer_set_text(s_attr_text_layer, "-");
  text_layer_set_text_color(s_attr_text_layer, GColorBlack);
  text_layer_set_background_color(s_attr_text_layer, GColorClear);
  text_layer_set_text_alignment(s_attr_text_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_attr_text_layer, GTextOverflowModeWordWrap);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_attr_text_layer));

  s_raw_text_layer = text_layer_create(GRect(0, 100, 144, 40));
  text_layer_set_font(s_raw_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  // text_layer_set_text(s_raw_text_layer, "-");
  text_layer_set_text_color(s_raw_text_layer, GColorBlack);
  text_layer_set_background_color(s_raw_text_layer, GColorClear);
  text_layer_set_text_alignment(s_raw_text_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_raw_text_layer, GTextOverflowModeWordWrap);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_raw_text_layer));
}

static void prv_main_window_unload(Window *window) {
  text_layer_destroy(s_status_layer);
}

static void prv_init(void) {
  s_main_window = window_create();

  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = prv_main_window_load,
    .unload = prv_main_window_unload
  });
  window_stack_push(s_main_window, true);
  SmartstrapHandlers handlers = (SmartstrapHandlers) {
    .availability_did_change = prv_availablility_status_changed,
    .did_write = prv_did_write,
    .did_read = prv_did_read,
    .notified = prv_notified
  };
  smartstrap_subscribe(handlers);
  smartstrap_set_timeout(50);
  x_attr_attribute = smartstrap_attribute_create(0x1001, 0x1001, 16);
  y_attr_attribute = smartstrap_attribute_create(0x1001, 0x1002, 16);
  z_attr_attribute = smartstrap_attribute_create(0x1001, 0x1003, 16);
  tap_attr_attribute = smartstrap_attribute_create(0x1001, 0x1004, 16);
  compass_attr_attribute = smartstrap_attribute_create(0x1001, 0x1005, 16);
  // app_timer_register(1000, prv_send_request, NULL);
}

static void prv_deinit(void) {
  window_destroy(s_main_window);
  // compass_service_unsubscribe();
  // accel_tap_service_unsubscribe();
  accel_data_service_unsubscribe();
  smartstrap_unsubscribe();
}

static void send_x_data(int16_t xdata) {
  SmartstrapResult result;

  if (!smartstrap_service_is_available(smartstrap_attribute_get_service_id(x_attr_attribute))) {
    // APP_LOG(APP_LOG_LEVEL_DEBUG, "x_attr_attribute is not available");
    return;
  }

  // get the write buffer
  uint8_t *buffer = NULL;
  size_t length = 0;
  result = smartstrap_attribute_begin_write(x_attr_attribute, &buffer, &length);
  if (result != SmartstrapResultOk) {
    // APP_LOG(APP_LOG_LEVEL_ERROR, "Write of x_attr_attribute failed with result %d", result);
    return;
  }

  // write the data into the buffer
  memset(buffer, 0, sizeof(buffer));
  memcpy(buffer, &xdata, 16);

  // send it off
  result = smartstrap_attribute_end_write(x_attr_attribute, 16, true);
  if (result != SmartstrapResultOk) {
  }
}

static void send_y_data(int16_t ydata) {
  SmartstrapResult result;

  if (!smartstrap_service_is_available(smartstrap_attribute_get_service_id(y_attr_attribute))) {
    // APP_LOG(APP_LOG_LEVEL_DEBUG, "y_attr_attribute is not available");
    return;
  }

  // get the write buffer
  uint8_t *buffer = NULL;
  size_t length = 0;
  result = smartstrap_attribute_begin_write(y_attr_attribute, &buffer, &length);
  if (result != SmartstrapResultOk) {
    // APP_LOG(APP_LOG_LEVEL_ERROR, "Write of y_attr_attribute failed with result %d", result);
    return;
  }

  // write the data into the buffer
  memset(buffer, 0, sizeof(buffer));
  memcpy(buffer, &ydata, 16);

  // send it off
  result = smartstrap_attribute_end_write(y_attr_attribute, 16, true);
  if (result != SmartstrapResultOk) {
  }
}

static void send_z_data(int16_t zdata) {
  SmartstrapResult result;

  if (!smartstrap_service_is_available(smartstrap_attribute_get_service_id(z_attr_attribute))) {
    // APP_LOG(APP_LOG_LEVEL_DEBUG, "z_attr_attribute is not available");
    return;
  }

  // get the write buffer
  uint8_t *buffer = NULL;
  size_t length = 0;
  result = smartstrap_attribute_begin_write(z_attr_attribute, &buffer, &length);
  if (result != SmartstrapResultOk) {
    // APP_LOG(APP_LOG_LEVEL_ERROR, "Write of z_attr_attribute failed with result %d", result);
    return;
  }

  // write the data into the buffer
  memset(buffer, 0, sizeof(buffer));
  memcpy(buffer, &zdata, 16);

  // send it off
  result = smartstrap_attribute_end_write(z_attr_attribute, 16, true);
  if (result != SmartstrapResultOk) {
  }
}

static void send_compass_data(int16_t compass_data) {
  SmartstrapResult result;

  if (!smartstrap_service_is_available(smartstrap_attribute_get_service_id(compass_attr_attribute))) {
    // APP_LOG(APP_LOG_LEVEL_DEBUG, "x_attr_attribute is not available");
    return;
  }

  // get the write buffer
  uint8_t *buffer = NULL;
  size_t length = 0;
  result = smartstrap_attribute_begin_write(compass_attr_attribute, &buffer, &length);
  if (result != SmartstrapResultOk) {
    // APP_LOG(APP_LOG_LEVEL_ERROR, "Write of x_attr_attribute failed with result %d", result);
    return;
  }

  // write the data into the buffer
  memset(buffer, 0, sizeof(buffer));
  memcpy(buffer, &compass_data, 16);

  // send it off
  result = smartstrap_attribute_end_write(compass_attr_attribute, 16, true);
  if (result != SmartstrapResultOk) {
  }
}

static void send_tap() {
  SmartstrapResult result;
  int16_t tap = 1;

  APP_LOG(APP_LOG_LEVEL_DEBUG, "TAP DAT");

  if (!smartstrap_service_is_available(smartstrap_attribute_get_service_id(tap_attr_attribute))) {
    // APP_LOG(APP_LOG_LEVEL_DEBUG, "tap_attr_attribute is not available");
    return;
  }

  // get the write buffer
  uint8_t *buffer = NULL;
  size_t length = 0;
  result = smartstrap_attribute_begin_write(tap_attr_attribute, &buffer, &length);
  if (result != SmartstrapResultOk) {
    // APP_LOG(APP_LOG_LEVEL_ERROR, "Write of tap_attr_attribute failed with result %d", result);
    return;
  }

  // write the data into the buffer
  memset(buffer, 0, sizeof(buffer));
  memcpy(buffer, &tap, 16);

  // send it off
  result = smartstrap_attribute_end_write(tap_attr_attribute, 16, true);
  if (result != SmartstrapResultOk) {
  }
}

static void send_data(AccelData data) {
  send_x_data(data.x);
  send_y_data(data.y);
  // send_z_data(data.z);
}

static void data_handler(AccelData *data, uint32_t num_samples) {
  send_data(data[0]);
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
  switch (axis) {
  case ACCEL_AXIS_X:
    if (direction > 0) {
      APP_LOG(APP_LOG_LEVEL_INFO, "X axis positive.");
    } else {
      APP_LOG(APP_LOG_LEVEL_INFO, "X axis negative.");
    }
    break;
  case ACCEL_AXIS_Y:
    if (direction > 0) {
      APP_LOG(APP_LOG_LEVEL_INFO, "Y axis positive.");
    } else {
      APP_LOG(APP_LOG_LEVEL_INFO, "Y axis negative.");
    }
    break;
  case ACCEL_AXIS_Z:
    if (direction > 0) {
      APP_LOG(APP_LOG_LEVEL_INFO, "Z axis positive.");
    } else {
      APP_LOG(APP_LOG_LEVEL_INFO, "Z axis negative.");
    }
    break;
  }
  // send_tap();
}

static void compass_heading_handler(CompassHeadingData heading_data) {
  if (heading_data.compass_status != CompassStatusDataInvalid) {
    int16_t heading = (int16_t)TRIGANGLE_TO_DEG(heading_data.magnetic_heading);
    // send_compass_data(heading);
  }

  // Display state of the compass
  static char s_valid_buf[64];
  switch (heading_data.compass_status) {
    case CompassStatusDataInvalid:
      snprintf(s_valid_buf, sizeof(s_valid_buf), "%s", "Compass is calibrating!\n\nMove your arm to aid calibration.");
      break;
    case CompassStatusCalibrating:
      snprintf(s_valid_buf, sizeof(s_valid_buf), "%s", "Fine tuning...");
      break;
    case CompassStatusCalibrated:
      snprintf(s_valid_buf, sizeof(s_valid_buf), "%s", "Calibrated");
      break;
  }

  APP_LOG(APP_LOG_LEVEL_DEBUG, s_valid_buf);
}

int main(void) {
  prv_init();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "STARTING APP");
  if (x_attr_attribute && y_attr_attribute && z_attr_attribute) {
    // compass_service_set_heading_filter(2 * (TRIG_MAX_ANGLE / 360));
    // compass_service_subscribe(&compass_heading_handler);
    // accel_tap_service_subscribe(tap_handler);
    uint32_t num_samples = 3;
    accel_data_service_subscribe(num_samples, data_handler);
    app_event_loop();
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "ENDING APP");
  prv_deinit();
}

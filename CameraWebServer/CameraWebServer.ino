#include <Arduino.h>
#include "esp_camera.h"
#include <WiFi.h>

// ===========================
// Select camera model
// ===========================
#include "board_config.h"

// ===========================
// WiFi credentials
// ===========================
const char *ssid = "eduroam";

// Try these credentials first
const char *identity = "***";
const char *username = "**";
const char *password = "***";

void startCameraServer();
void setupLedFlash();

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // --------------------------------------------------
  // CAMERA INITIALIZATION
  // --------------------------------------------------

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_LATEST;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 25;
  config.fb_count = 4;

  if (config.pixel_format == PIXFORMAT_JPEG) {
    if (psramFound()) {
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  }

  esp_err_t err = esp_camera_init(&config);

  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%x\n", err);
    return;
  }

#if defined(LED_GPIO_NUM)
  setupLedFlash();
#endif

  // --------------------------------------------------
  // WIFI SCAN
  // --------------------------------------------------

  Serial.println("Scanning for WiFi networks...");

  int n = WiFi.scanNetworks();

  for (int i = 0; i < n; i++) {
    Serial.printf(
      "%d: %s (%d dBm)\n",
      i,
      WiFi.SSID(i).c_str(),
      WiFi.RSSI(i)
    );
  }

  // --------------------------------------------------
  // WPA2 ENTERPRISE CONNECTION
  // --------------------------------------------------

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);

  Serial.println();
  Serial.println("Connecting to eduroam...");

  WiFi.begin(
    ssid,
    WPA2_AUTH_PEAP,
    identity,
    username,
    password
  );

  unsigned long startAttemptTime = millis();

  while (
    WiFi.status() != WL_CONNECTED &&
    millis() - startAttemptTime < 30000
  ) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("FAILED TO CONNECT");
    Serial.printf("WiFi Status: %d\n", WiFi.status());
    return;
  }

  // --------------------------------------------------
  // CONNECTED
  // --------------------------------------------------

  Serial.println("WiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  startCameraServer();

  Serial.println();
  Serial.print("Camera Ready! Open: http://");
  Serial.println(WiFi.localIP());
}

void loop() {
  delay(10000);
}
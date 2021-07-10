#include "esp_camera.h"
#include <WiFi.h>

#include "camera_pins.h"
#include "settings.h"

const char* ssid = WIFI_SSID;
const char* password = WIFI_PW;

const IPAddress ip = WIFI_IP;
const IPAddress gateway = WIFI_GATEWAY;
const IPAddress subnet = WIFI_SUBNET;

void startCameraServer();

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

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
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  
 
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
    
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }


  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

  WiFi.config(ip, gateway, subnet);
  Serial.print("Connecting: ");
  WiFi.begin(ssid, password);

  wl_status_t wifi_status = WL_DISCONNECTED;
  uint32_t wifi_try_counter = 0;

  while (wifi_status != WL_CONNECTED) {
    delay(100);
    wifi_try_counter++;
    wifi_status = WiFi.status();
    Serial.print(wifi_status);

    if (wifi_try_counter > 50) {
      Serial.println("\nRestarting... (Connecting took too long)");
      ESP.restart();
    }

  }
  
  Serial.print("  CONNECTED");
  Serial.print("\n\n\n");
  

  startCameraServer();


  Serial.print("IP: ");
  Serial.print(WiFi.localIP());
}

void loop() {
  
  delay(10000);
}
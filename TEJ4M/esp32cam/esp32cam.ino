#include <esp32cam.h>
#include <WebServer.h>
#include <WiFi.h>

#define AP_SSID "kevinserver"
#define AP_PASS "123456"

WebServer server(80);

void handleCapture(){
  auto img = esp32cam::capture();
  if(img == nullptr){
    server.send(500, "", "");
    return;
  }

  server.setContentLength(img->size());
  server.send(200, "image/jpeg");
  WiFiClient client = server.client();
  img->writeTo(client);
}

void setup() {
  // put your setup code here, to run once:
  auto res = esp32cam::Resolution::find(640, 480);
  esp32cam::Config cfg;
  cfg.setPins(esp32cam::pins::FreeNove);
  cfg.setResolution(res);
  cfg.setJpeg(80);
  esp32cam::Camera.begin(cfg);
  WiFi.softAP(AP_SSID, AP_PASS);
  server.on("/capture.jpg",handleCapture);
  server.begin();
        
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();

}

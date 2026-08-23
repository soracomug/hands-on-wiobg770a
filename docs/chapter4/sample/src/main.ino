#include <Adafruit_TinyUSB.h>
#include <ArduinoHttpClient.h>
#include <WioCellular.h>

static constexpr auto SEARCH_ACCESS_TECHNOLOGY =
    WioCellularNetwork::SearchAccessTechnology::LTEM;
static constexpr auto LTEM_BAND = WioCellularNetwork::ALL_LTEM_BAND;

static const char APN[] = "soracom.io";
static const char HTTP_HOST[] = "uni.soracom.io";
static const char HTTP_PATH[] = "/";
static constexpr int HTTP_PORT = 80;

static constexpr int POWER_ON_TIMEOUT = 1000 * 20;
static constexpr int NETWORK_TIMEOUT = 1000 * 60 * 3;
static constexpr int HTTP_TIMEOUT = 1000 * 10;
static constexpr int SEND_INTERVAL = 1000 * 60;
static constexpr int CONSOLE_WAIT_TIMEOUT = 1000 * 10;

static bool connectCellular();
static bool sendToHarvest();
static void stopWithBlink();

void setup() {
  Serial.begin(115200);
  const uint32_t start = millis();
  while (!Serial && millis() - start < CONSOLE_WAIT_TIMEOUT) {
    delay(10);
  }

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.println();
  Serial.println("=== Wio BG770A / SORACOM Harvest Data ===");

  if (!connectCellular()) {
    Serial.println("[ERROR] Cellular connection failed.");
    stopWithBlink();
  }

  digitalWrite(LED_BUILTIN, LOW);
  sendToHarvest();
}

void loop() {
  WioCellular.doWorkUntil(SEND_INTERVAL);
  sendToHarvest();
}

static bool connectCellular() {
  Serial.println("[1/5] Configure APN, RAT, and LTE-M band");
  WioNetwork.config.apn = APN;
  WioNetwork.config.searchAccessTechnology = SEARCH_ACCESS_TECHNOLOGY;
  WioNetwork.config.ltemBand = LTEM_BAND;

  Serial.println("[2/5] Start WioCellular");
  WioCellular.begin();

  Serial.println("[3/5] Power on BG770A");
  const auto result = WioCellular.powerOn(POWER_ON_TIMEOUT);
  if (result != WioCellularResult::Ok) {
    Serial.printf("      Failed: %s\n", WioCellularResultToString(result));
    return false;
  }

  Serial.println("[4/5] Start WioNetwork");
  WioNetwork.begin();

  Serial.println("[5/5] Wait for network registration");
  if (!WioNetwork.waitUntilCommunicationAvailable(NETWORK_TIMEOUT)) {
    Serial.println("      Timed out");
    return false;
  }

  Serial.println("[READY] Cellular connection established.");
  return true;
}

static bool sendToHarvest() {
  const String payload =
      String("{\"uptime_ms\":") + millis() +
      ",\"mcu_temp_c\":" + String(readCPUTemperature(), 1) + "}";

  Serial.println();
  Serial.printf("[HTTP] POST http://%s%s\n", HTTP_HOST, HTTP_PATH);
  Serial.printf("       Payload: %s\n", payload.c_str());

  WioCellularArduinoTcpClient<WioCellularModule> tcpClient{
      WioCellular, WioNetwork.config.pdpContextId};
  HttpClient httpClient{tcpClient, HTTP_HOST, HTTP_PORT};
  httpClient.setTimeout(HTTP_TIMEOUT);

  const int error =
      httpClient.post(HTTP_PATH, "application/json", payload.c_str());
  if (error != 0) {
    Serial.printf("[ERROR] HTTP POST failed: %d\n", error);
    httpClient.stop();
    return false;
  }

  const int statusCode = httpClient.responseStatusCode();
  const String responseBody = httpClient.responseBody();
  httpClient.stop();

  Serial.printf("[HTTP] Status code: %d\n", statusCode);
  if (responseBody.length() > 0) {
    Serial.printf("       Response: %s\n", responseBody.c_str());
  }

  if (statusCode >= 200 && statusCode < 300) {
    Serial.println("[SUCCESS] Data was accepted by Unified Endpoint.");
    return true;
  }

  Serial.println("[ERROR] Unexpected HTTP status code.");
  return false;
}

static void stopWithBlink() {
  while (true) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(500);
  }
}

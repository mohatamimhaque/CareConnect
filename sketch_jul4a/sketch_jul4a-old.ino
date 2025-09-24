#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "MAX30100_PulseOximeter.h"

// ----------------- Pin Configuration -----------------
#define DHTPIN 18
#define DHTTYPE DHT11
#define DS18B20_PIN 4              
#define I2C_SDA 21
#define I2C_SCL 22
#define REPORTING_PERIOD_MS 2000

// ----------------- Sensor Instances ------------------
DHT dht(DHTPIN, DHTTYPE);
OneWire (DSoneWire18B20_PIN);
DallasTemperature ds18b20(&oneWire);
PulseOximeter pox;
WebServer server(80);


const char* ssid = "Hj";         //  WiFi SSID
const char* password = "12345678"; //  WiFi password




// ----------------- Global Variables ------------------
bool maxSensorDetected = false;
uint32_t lastReportTime = 0;
float dhtTemp = NAN;
float humidity = NAN;
float bodyTemp = NAN;
float bpm = 0;
float spo2 = 0;


// ----------------- Beat Detection Callback -----------
void onBeatDetected() {
  Serial.println("Beat detected!");
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);

  unsigned long startAttemptTime = millis();
  const unsigned long timeout = 10000;

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
    Serial.print(".");
    delay(100);  
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected. IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi");
  }
}


void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>Care Connect</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
  <style>
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
    }

    body {
  background: url('https://raw.githubusercontent.com/mohatamimhaque/image/7d429f09c2a15c20a04b0b8c9f056df9ab981cbe/26837.jpg') no-repeat center center fixed;    background-size: cover;
    background-position: center;
    background-repeat: no-repeat;
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      color: #f1f1f1;
      display: flex;
      justify-content: center;
      align-items: center;
      min-height: 100vh;
      position: relative;
    }
    #wrapper{
        position:fixed;
        width:100%;
        height:100%;
        background: rgba(0,0,0,0.6);
        
    }

    .dashboard {
      background: #1e1e1e;
      padding: 30px;
      border-radius: 12px;
      box-shadow: 0 0 20px rgba(0,0,0,0.6);
      max-width: 600px;
      width: 100%;
      position: relative;
    }

    .dashboard h1 {
      text-align: center;
      margin-bottom: 25px;
      font-size: 1.6rem;
      color: #4dd0e1;
    }

    .reading {
      font-size: 1.0rem;
      margin: 15px 0;
      padding: 10px 15px;
      border-radius: 8px;
      background-color: #2c2c2c;
      display: flex;
      justify-content: space-between;
      align-items: center;
    }

    #status {
      margin-top: 20px;
      text-align: center;
      font-size: 1rem;
      color: #ffa726;
    }

    #chart {
      position: fixed;
      top: 0;
      left: 0;
      width: 100%;
      height: 100%;
      background: rgba(26, 26, 26, 0.85);
      display: flex;
      justify-content: center;
      align-items: center;
      display: none;
      z-index: 1000;
    }

    .chart-card {
      position: relative;
      width: 600px;
      height: 500px;
      background: #2c2c2c;
      padding: 28px;
      border-radius: 12px;
    }

    canvas {
      background-color: #1e1e1e;
      border-radius: 12px;
    }

    .close-btn {
      position: absolute;
      top: 0;
      right: 6px;
      background: transparent;
      border: none;
      color: #ccc;
      font-size: 16px;
      cursor: pointer;
      transition: color 0.3s;
      padding: 12px;
    }

    .close-btn:hover {
      color: #ff5252;
    }

    .show-chart-btn {
      display: block;
      margin: 25px auto 0;
      padding: 10px 18px;
      font-size: 1rem;
      background-color: #4dd0e1;
      color: #000;
      border: none;
      border-radius: 8px;
      cursor: pointer;
      transition: background-color 0.3s;
    }

    .show-chart-btn:hover {
      background-color: #26c6da;
    }

    @media (max-width: 600px) {
      .reading {
        font-size: 1rem;
      }
      .chart-card {
        width: 90%;
        height: auto;
      }
    }
  </style>
</head>
<body>
<div id="wrapper">

</div>
  <div class="dashboard">
    <h1>Health Monitor</h1>
    <div class="reading"><span>🌡 Room Temp:</span> <span id="dhtTemp">--°C</span></div>
    <div class="reading"><span>💧 Humidity:</span> <span id="humidity">-- %</span></div>
    <div class="reading"><span>🌡 Body Temp:</span> <span id="bodyTemp">-- °C</span></div>
    <div class="reading"><span>❤️ Heart Rate:</span> <span id="bpm">-- BPM</span></div>
    <div class="reading"><span>🩸 SpO2:</span> <span id="spo2">-- %</span></div>
    <p id="status">Status: Waiting for data...</p>

<button class="show-chart-btn" onclick="window.open('http://localhost/careConnect/', '_blank')">📊 Show Chart</button>
  </div>



  <script>
     

    function fetchData() {
      fetch('/data')
        .then(response => response.json())
        .then(data => {

          document.getElementById('dhtTemp').innerText = data.dhtTemp !== null ? `${data.dhtTemp.toFixed(1)} °C` : 'DHT11 data not available';
          document.getElementById('humidity').innerText = data.humidity !== null ? `${data.humidity.toFixed(1)} %` : 'DHT11 data not available';
          document.getElementById('bodyTemp').innerText = data.bodyTemp !== null ? `${data.bodyTemp.toFixed(1)} °C` : 'DS18B20 data not available';
          if (data.bpm >= 50 && data.bpm <= 200 && data.spo2 >= 60 && data.spo2 <= 100) {
          
            document.getElementById('bpm').innerText = `${data.bpm.toFixed(0)} BPM`;
                    document.getElementById('spo2').innerText = `${data.spo2.toFixed(0)} %`;
                    document.getElementById('status').innerText = 'Status: Data Updated ✔';
                    
          } else {
             document.getElementById('bpm').innerText = '-- BPM';
                    document.getElementById('spo2').innerText = '-- %';
                   
                    document.getElementById('status').innerText = '⌛ Waiting for valid MAX30100 data...';
          }

           fetch('http://localhost/careConnect/store.php', {
            method: 'POST',
            headers: {
              'Content-Type': 'application/json',
            },
            body: JSON.stringify({
              dhtTemp: data.dhtTemp !== null ? parseFloat(data.dhtTemp).toFixed(1) : 0,
              humidity: data.humidity !== null ? parseFloat(data.humidity).toFixed(1) : 0,
              bodyTemp: data.bodyTemp !== null ? parseFloat(data.bodyTemp).toFixed(1) : 0,
              bpm: (data.bpm >= 50 && data.bpm <= 200 && data.spo2 >= 60 && data.spo2 <= 100) ? parseFloat(data.bpm).toFixed(1) : 0,
              spo2: (data.bpm >= 50 && data.bpm <= 200 && data.spo2 >= 60 && data.spo2 <= 100) ? parseFloat(data.spo2).toFixed(1) : 0
            }),
          })
          .then(response => response.json())
          .then(result => {
            console.log('Store response:', result);
          })
          .catch(error => {
            console.error('Error storing data:', error);
          });





        })
        .catch(err => {
          document.getElementById('status').innerText = '❌ Failed to fetch data from ESP32';
        });
    }
    setInterval(fetchData, 2000); // Match SENSOR_READ_INTERVAL
    fetchData();
  </script>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}



void handleData() {
  String json = "{";
  json += "\"dhtTemp\": " + String(isnan(dhtTemp) ? "null" : String(dhtTemp, 1)) + ",";
  json += "\"humidity\": " + String(isnan(humidity) ? "null" : String(humidity, 1)) + ",";
  json += "\"bodyTemp\": " + String(bodyTemp == DEVICE_DISCONNECTED_C ? "null" : String(bodyTemp, 1)) + ",";
  json += "\"bpm\": " + String(bpm) + ",";
  json += "\"spo2\": " + String(spo2);
  json += "}";
  server.send(200, "application/json", json);
}






void setup() {
  Serial.begin(115200);
  delay(1000);
  connectToWiFi();

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
  Serial.println("Web server started");



  Serial.println("Setup started");

  // ---------- Initialize DHT11 ----------
  dht.begin();
  Serial.println("DHT11 initialized");

  // ---------- Initialize DS18B20 ----------
  ds18b20.begin();
  Serial.println("DS18B20 initialized");

  // ---------- Initialize I2C for MAX30100 ----------
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(100000);  
  Serial.println("Wire.begin() done");

  // ---------- Scan I2C for MAX30100 ----------
  Serial.println("Scanning I2C for MAX30100...");
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.print("Found I2C device at: 0x");
      Serial.println(address, HEX);
      if (address == 0x57) {
        Serial.println("MAX30100 detected at address 0x57");
        maxSensorDetected = true;
        break;
      }
    }


  }

  // ---------- Initialize MAX30100 ----------
  if (maxSensorDetected) {
    Serial.println("Initializing MAX30100...");
    if (!pox.begin()) {
      Serial.println("Sensor init failed!");
      maxSensorDetected = false;
    } else {
      Serial.println("MAX30100 ready.");
      pox.setOnBeatDetectedCallback(onBeatDetected);
      pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
    }
  } else {
    Serial.println("MAX30100 not found! Check wiring and power.");
  }


}

void loop() {
      server.handleClient();

  // ---------- MAX30100 update ----------
  if (maxSensorDetected) {
    pox.update();  // Must be called frequently
  }

  if (millis() - lastReportTime >= REPORTING_PERIOD_MS) {
    lastReportTime = millis();
    Serial.println("----- Health Monitor Data -----");

    // ---------- Read DHT11 ----------
     dhtTemp = dht.readTemperature();
     humidity = dht.readHumidity();

    if (isnan(dhtTemp) || isnan(humidity)) {
      Serial.println("DHT11 not responding");
    } else {
      Serial.print("Room Temp (DHT11): "); Serial.print(dhtTemp); Serial.println(" °C");
      Serial.print("Humidity: "); Serial.print(humidity); Serial.println(" %");
    }

    // ---------- Read DS18B20 ----------
    ds18b20.requestTemperatures();
     bodyTemp = ds18b20.getTempCByIndex(0);
    if (bodyTemp == DEVICE_DISCONNECTED_C) {
      Serial.println("DS18B20 not connected");
    } else {
      Serial.print("Body Temp (DS18B20): "); Serial.print(bodyTemp); Serial.println(" °C");
    }

    // ---------- Read MAX30100 ----------
    if (maxSensorDetected) {
       bpm = pox.getHeartRate();
       spo2 = pox.getSpO2();
      Serial.print("Heart Rate: "); Serial.print(bpm); Serial.println(" BPM");
      Serial.print("SpO2: "); Serial.print(spo2); Serial.println(" %");
    } else {
      Serial.println("MAX30100 unavailable");
    }

    Serial.println("----------------------------------\n");
  }
}

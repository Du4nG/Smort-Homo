#include <ESP8266WiFi.h>

const char* ssid     = "MinhPhu";      // Yep, that's Mập.
const char* password = "minhphu2001";

WiFiServer server(81); // Phải mở một cái (NAT) port để có thể truy cập từ mạng KHÔNG phải nội bộ, tức là dùng mạng ở đâu cũng được lun, này phải truy cập router để mở nha.

String header;

String light = "off";
String fan = "off";

const int relay1 = 5;
const int relay2 = 4;

//Tank bảo delay ngu lắm.
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);

  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);

  WiFi.begin(ssid, password); // Câu wifi

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("IP nè con đĩ: ");
  Serial.println(WiFi.localIP()); // Muốn search thủ công thì dùng Advanced IP Scanner như cụ Bùi Hà Đức.
  
  server.begin();
}

void loop(){ // Ảduino ft. HTML,JS | Official MV

  WiFiClient client = server.available();

  if (client) {
    String currentLine = "";
    currentTime = millis();
    previousTime = currentTime;

    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();

      // Ủi vô nhà nè
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();


            // Xài ngắt pls
            if (header.indexOf("GET /5/on") >= 0) {
              light = "on";
              digitalWrite(relay1, HIGH);
            } else if (header.indexOf("GET /5/off") >= 0) {
              light = "off";
              digitalWrite(relay1, LOW);
            } else if (header.indexOf("GET /4/on") >= 0) {
              fan = "on";
              digitalWrite(relay2, HIGH);
            } else if (header.indexOf("GET /4/off") >= 0) {
              fan = "off";
              digitalWrite(relay2, LOW);
            }



            // Top 10 cách trở thành Designer. Top 1:
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #69BF64; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            client.println("<body><h1>Clever Dung</h1>");



            client.println("<p>Light is " + light + "</p>"); // Trạng thái
            if (light=="off") 
              client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
            else 
              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");



            client.println("<p>Only Fan is " + fan + "</p>"); // Vẫn là trạng thái
            if (fan=="off") 
              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
             else 
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
            client.println("</body></html>");
            break;



          // Khúc này wtf
          } else currentLine = "";
        } else if (c != '\r') 
            currentLine += c;
      }
    }
    header = "";


    // Sủi
    client.stop();
  }
}

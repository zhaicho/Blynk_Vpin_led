#include <ESP8266WiFi.h>
#include <espnow.h>

// 連接WiFi；
char ssid[] = "Hou lan ging";
char pass[] = "houyaoying";

#define relay 0

// 結構範例接收數據必須匹配發送方結構
typedef struct struct_message
{
    int Vpin1;
    int Vpin2;
} struct_message;

struct_message myData;

void setup()
{
    pinMode(relay, OUTPUT);
    // digitalWrite(relay,LOW);
    Serial.begin(9600);

    // 將設備設置為Wi-Fi工作站
    WiFi.mode(WIFI_STA);
    Serial.print("Connecting to:");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println("");
    Serial.println("CONNECTED!!!");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // Init ESP-NOW
    if (esp_now_init() != 0)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // 一旦Espnow成功初始化，我們將註冊發送回饋函數至獲取數據包的狀態
    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
    esp_now_register_recv_cb(OnDataRecv);
}

void loop()
{
}

// 收到數據時將執行的回饋函數
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len)
{
    memcpy(&myData, incomingData, sizeof(myData));
    Serial.println(myData.Vpin2);
    digitalWrite(relay, myData.Vpin2); // relay跟收到的信號；
}
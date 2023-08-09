#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL6l2PpFRkH"
#define BLYNK_TEMPLATE_NAME "LED"
#define BLYNK_AUTH_TOKEN "TBY3kEZd_HZlOLUkRvVyOWmQfRPA1-Fv"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <espnow.h>

// 連接WiFi；
char ssid[] = "Hou lan ging";
char pass[] = "houyaoying";

// esp互傳mac碼；
uint8_t broadcastAddress[] = {0x80, 0x64, 0x6F, 0x87, 0x92, 0xB9};

// 建立傳送結構；必須與接受方結構匹配；
typedef struct struct_message
{
    int Vpin;
} struct_message;

struct_message myData;

// 定義引脚；
#define relay 0

void setup()
{
    // 將設備設置為Wi-Fi工作站
    digitalWrite(LED_BUILTIN, HIGH);
    WiFi.mode(WIFI_STA);

    // Init ESP-NOW
    if (esp_now_init() != 0)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // 一旦Espnow成功初始化，我們將註冊發送回饋函數至獲取數據包的狀態
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    esp_now_register_send_cb(OnDataSent);

    // 註冊同行
    esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
    Serial.begin(9600);
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    pinMode(relay, OUTPUT);
}

void loop()
{
    Blynk.run();
}

BLYNK_CONNECTED() // 當設備成功連接至 Blynk 伺服器後執行
{
    Blynk.syncVirtual(V1, V2); // 同步虛擬腳位 V1，V2會觸發執行 BLYNK_WRITE(V1,V2)
}

// 虛擬引脚V1設置；
BLYNK_WRITE(V1) // 當 Blynk 虛擬腳位 V1 有改變時便觸發
{
    int pinValue1 = param.asInt();   // 以整數型態讀取虛擬腳位 V1 的值
    digitalWrite(relay, !pinValue1); // 低電位觸發relay
}

// 虛擬引脚V2設置；
BLYNK_WRITE(V2) // 當 Blynk 虛擬腳位 V2 有改變時便觸發
{
    int pinValue2 = param.asInt(); // 以整數型態讀取虛擬腳位 V2 的值
    myData.Vpin = !pinValue2;      // send出數值與讀取的相反；
    // 從ESP-NOW傳送訊息
    esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
}

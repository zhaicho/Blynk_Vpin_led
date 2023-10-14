#define BLYNK_PRINT Serial
// Blynk 鏈接key；
#define BLYNK_TEMPLATE_ID "BLYNK_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "LYNK_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "BLYNK_AUTH_TOKEN"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <espnow.h>

// 連接WiFi；
char ssid[] = "Hou lan ging";
char pass[] = "houyaoying";

// esp互傳mac碼；
uint8_t broadcastAddress1[] = {0x80, 0x64, 0x6F, 0x87, 0x00, 0x00};
uint8_t broadcastAddress2[] = {0x80, 0x64, 0x6F, 0x87, 0x00, 0x00};

void setup()
{
  // 將設備設置為Wi-Fi工作站
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

  // 配對所有廣播MCU；
  esp_now_add_peer(broadcastAddress1, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  esp_now_add_peer(broadcastAddress2, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop()
{
  Blynk.run();
}

// 建立傳送結構；必須與接受方結構匹配；
typedef struct struct_message
{
  int Vpin1;
  int Vpin2;
} struct_message;

struct_message myData;

// 發送數據時的回饋函數
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus)
{
  Serial.print("傳送: ");
  if (sendStatus == 0)
  {
    Serial.println("成功！");
  }
  else
  {
    Serial.println("失敗！！！");
  }
}

BLYNK_CONNECTED() // 當設備成功連接至 Blynk 伺服器後執行
{
  Blynk.syncVirtual(V1, V2); // 同步虛擬腳位 V1，V2會觸發執行 BLYNK_WRITE(V1,V2)
}

// 虛擬引脚V1設置；
BLYNK_WRITE(V1) // 當 Blynk 虛擬腳位 V1 有改變時便觸發
{
  int pinValue1 = param.asInt(); // 以整數型態讀取虛擬腳位 V1 的值
  myData.Vpin1 = !pinValue1;      // send出數值與讀取的相反；

  // 從ESP-NOW傳送訊息
  esp_now_send(broadcastAddress1, (uint8_t *)&myData, sizeof(myData));
  Serial.print("Vpin1 state:");
  Serial.println(myData.Vpin1);
}

// 虛擬引脚V2設置；
BLYNK_WRITE(V2) // 當 Blynk 虛擬腳位 V2 有改變時便觸發
{

  int pinValue2 = param.asInt(); // 以整數型態讀取虛擬腳位 V2 的值
  myData.Vpin2 = !pinValue2;      // send出數值與讀取的相反；

  // 從ESP-NOW傳送訊息
  esp_now_send(broadcastAddress2, (uint8_t *)&myData, sizeof(myData));
  Serial.print("Vpin2 state:");
  Serial.println(myData.Vpin2);
}

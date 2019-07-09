#include <U8x8lib.h>
#include <LoRa.h>

// WIFI_LoRa_32 ports
// GPIO5  -- SX1278's SCK
// GPIO19 -- SX1278's MISO
// GPIO27 -- SX1278's MOSI
// GPIO18 -- SX1278's CS
// GPIO14 -- SX1278's RESET
// GPIO26 -- SX1278's IRQ(Interrupt Request)

#define SS      18
#define RST     14
#define DI0     26
#define BAND    868E6

// the OLED used 8 x 16
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

void setup() {

  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(SS, RST, DI0);

  Serial.begin(115200);
  while (!Serial); //if just the the basic function, must connect to a computer
  delay(1000);

  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  u8x8.drawString(0, 0, "   KG LoRa RX   ");
  u8x8.drawString(0, 1, "================");

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    u8x8.drawString(0, 4, "Starting LoRa failed!");
    while (1);
  }
  u8x8.drawString(0, 4, "----------------");
  u8x8.drawString(0, 5, "  waiting for");
  u8x8.drawString(0, 6, "    packet");
}

int rxCounter = 0;
ulong lastHeartbeat = 0;

void loop() {

  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    u8x8.clearLine(5);
    u8x8.clearLine(6);
    u8x8.clearLine(7);

    rxCounter++;
    // received a packet
    Serial.print("Received packet '");
    u8x8.drawString(0, 2, "RX Count:");
    u8x8.setCursor(10, 2);
    u8x8.printf("%6d", rxCounter);

    // read packet
    int col = 0;
    int row = 5;
    while (LoRa.available() && row < 8) {
      char b = (char)LoRa.read();
      Serial.print(b);
      u8x8.drawGlyph(col, row, b);

      col++;
      if (col == 16) {
        col = 0;
        row++;
      }
    }
    
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());

    u8x8.drawString(0, 3, "RSSI: ");
    u8x8.setCursor(10, 3);
    u8x8.printf("%6d", LoRa.packetRssi());
  }

  // Flash heartbeat on display
  if (millis() > lastHeartbeat + 1500) {
    lastHeartbeat = millis();
    u8x8.drawString(15, 0, " ");
  } else if (millis() > lastHeartbeat + 750) {
    u8x8.drawString(15, 0, "*");
  }
}
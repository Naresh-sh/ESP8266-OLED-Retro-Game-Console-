#include <ESP8266WiFi.h>
#if LWIP_FEATURES && !LWIP_IPV6
#define HAVE_NETIF_SET_DEFAULT 1
#include <lwip/napt.h>
#include <lwip/dns.h>
#define NAPT 1000
#define NAPT_PORT 10
#endif
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Buttons
#define UP_BUTTON     D6
#define DOWN_BUTTON   D7
#define OK_BUTTON     D5
#define LEFT_BUTTON   D3
#define RIGHT_BUTTON  D4

// Menu list
const char* games[] = {
  "1. Dino Run",
  "2. Snake",
  "3. Flappy stone",
  "4. Tic Tac Toe",
  "5. Pong",
  "6. Super Mario",
  "7. Car Dodger",
  "8. Tappy Tap",
  "9. Space Invaders", 
  "10. tetris lite",
  "11. brick killer",
  "12. play 2048",
  "13. doodle jump",
  "14. Maze Solver",
  "15. Pac-Man",
  "16. Frogger",
  "17. Helicopter",
  "18. Settings"
}; 

const int totalGames = sizeof(games) / sizeof(games[0]);
int selectedGame = 0;
int menuStartIndex = 0;
bool inGame = false;
bool unlocked = false;
unsigned long lastDebounce = 0;

// Password (UP, UP, UP, UP)
const int passwordSequence[] = {UP_BUTTON, UP_BUTTON, UP_BUTTON, UP_BUTTON};
const int passwordLength = sizeof(passwordSequence) / sizeof(passwordSequence[0]);
int passwordIndex = 0;

// ===================== BITMAPS =====================
// Custom Booting Image (128x64px)
// Custom Booting Images (128x64px)
static const unsigned char PROGMEM epd_bitmap_tauka1 [] = {
	0x92, 0x49, 0x24, 0x92, 0x49, 0x25, 0xbf, 0xff, 0xff, 0xff, 0xb6, 0xdb, 0x6d, 0xb6, 0xdb, 0x6d, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdb, 0x7f, 0xff, 0xed, 0x40, 0x00, 0x00, 0x49, 0x24, 0x93, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x24, 0xc2, 0x48, 0x33, 0x30, 0x00, 0x24, 0x00, 0x11, 0x21, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x65, 0x9a, 0x67, 0x92, 0x81, 0x24, 0x81, 0x24, 0xc2, 0x0d, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x19, 0x35, 0x98, 0xcc, 0x90, 0x00, 0x08, 0x00, 0x08, 0x91, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x93, 0x65, 0x9a, 0x64, 0x92, 0x02, 0x02, 0x49, 0x20, 0x41, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x66, 0x4c, 0x67, 0x32, 0x48, 0x10, 0x40, 0x00, 0x04, 0x09, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x2c, 0xdb, 0x64, 0x9a, 0x48, 0x40, 0x10, 0x92, 0x41, 0x21, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x23, 0x22, 0x59, 0x89, 0x20, 0x00, 0x82, 0x00, 0x10, 0x05, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xd9, 0xad, 0x92, 0x65, 0x24, 0x02, 0x00, 0x24, 0x84, 0x91, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x48, 0x99, 0x2c, 0x94, 0x80, 0x08, 0x08, 0x00, 0x20, 0x01, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x26, 0xc6, 0x65, 0x32, 0x48, 0x20, 0x20, 0x92, 0x09, 0x21, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xb2, 0x64, 0x93, 0x4b, 0x02, 0x00, 0x00, 0x00, 0x80, 0x01, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x83, 0x32, 0x9b, 0x28, 0x90, 0x00, 0x82, 0x08, 0x10, 0x45, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x49, 0x18, 0xc3, 0x84, 0x00, 0x00, 0x08, 0x40, 0x41, 0x01, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x41, 0xc1, 0x27, 0x93, 0x00, 0x08, 0x00, 0x04, 0x00, 0x01, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x92, 0x24, 0x0f, 0xc0, 0x10, 0x20, 0x20, 0x80, 0x08, 0x11, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x21, 0x8d, 0xc4, 0x00, 0x00, 0x00, 0x01, 0x00, 0x41, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x21, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x82, 0x10, 0x10, 0x01, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x37, 0xf2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x42, 0x00, 0x68, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x70, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xec, 0x68, 0x10, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0xff, 0xf8, 0x20, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x09, 0xfc, 0x50, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x09, 0xff, 0xf0, 0x40, 0x00, 0x10, 0x80, 0x00, 0x01, 
	0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x01, 0x07, 0xff, 0xe0, 0x60, 0x00, 0x40, 0x08, 0x40, 0x01, 
	0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x01, 0x97, 0xff, 0xe0, 0x60, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc7, 0xff, 0xe0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x80, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x00, 0x00, 0x80, 0x04, 0x00, 0x00, 0x0c, 0x7f, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x00, 0x00, 0x00, 0x20, 0x04, 0x00, 0x02, 0x7f, 0xff, 0xc0, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x3f, 0xff, 0x90, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x00, 0x08, 0x04, 0x00, 0x40, 0x00, 0x03, 0xff, 0xff, 0xe0, 0x04, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x01, 0xfc, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x80, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0xfb, 0xbf, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x80, 0x40, 0x01, 0x02, 0x00, 0x00, 0x03, 0xff, 0xff, 0x80, 0x10, 0x00, 0x20, 0x00, 0x00, 0x01, 
	0x00, 0x01, 0x00, 0x00, 0x08, 0x08, 0x01, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x80, 0x08, 0x00, 0x10, 0x20, 0x80, 0x00, 0x3f, 0xf9, 0x80, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x82, 0x00, 0x04, 0x00, 0x80, 0x02, 0x40, 0x0f, 0xf6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x37, 0xcd, 0x80, 0x10, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x00, 0x44, 0x00, 0x00, 0x00, 0x80, 0x00, 0x11, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x80, 0x00, 0x01, 0x02, 0x08, 0x10, 0x10, 0x1c, 0xda, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x80, 0x80, 0x00, 0x08, 0x00, 0x04, 0x00, 0x26, 0xcc, 0x80, 0x02, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x02, 0x01, 0x02, 0x00, 0x10, 0x00, 0x40, 0x2b, 0x35, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x00, 0x08, 0x00, 0x20, 0x41, 0x00, 0x01, 0x29, 0xb5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x84, 0x00, 0x04, 0x00, 0x00, 0x42, 0x08, 0x76, 0xcd, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x80, 0x10, 0x00, 0x41, 0x00, 0x00, 0x82, 0x7a, 0x6b, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x02, 0x42, 0x00, 0x04, 0x24, 0x08, 0x01, 0xff, 0x6f, 0xb0, 0x10, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x10, 0x00, 0x01, 0x00, 0x00, 0x40, 0x01, 0xfd, 0xbf, 0xfc, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x80, 0x00, 0x04, 0x08, 0x08, 0x10, 0x03, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x80, 0x90, 0x00, 0x21, 0x02, 0x02, 0x03, 0xef, 0xfe, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x17, 0xff, 0xff, 0xf8, 0x40, 0x80, 0x00, 0x00, 0x00, 0x00, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x30, 0x0f, 0xbf, 0x6f, 0xec, 0x04, 0xf0, 0x40, 0x00, 0x00, 0x00, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0xd0, 0x0f, 0xff, 0xff, 0x78, 0x01, 0xbc, 0x00, 0x00, 0x00, 0x01, 
	0x00, 0x00, 0x00, 0x00, 0x03, 0x50, 0x49, 0xff, 0xf9, 0x98, 0x21, 0xaf, 0x80, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x0d, 0xf0, 0x1f, 0x7f, 0xee, 0xf8, 0x81, 0xeb, 0x60, 0x00, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x19, 0x90, 0x1f, 0xef, 0xff, 0xf8, 0x07, 0x7b, 0x70, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0x36, 0xe1, 0x1b, 0xff, 0xbf, 0xf0, 0x11, 0xdd, 0x98, 0x00, 0x00, 0x01, 
	0xbf, 0xff, 0xff, 0xfd, 0xb7, 0xfd, 0xdf, 0xff, 0xff, 0xff, 0xdb, 0xf7, 0xf9, 0x24, 0x92, 0x21
};

static const unsigned char PROGMEM epd_bitmap_kaneki [] = {
	0xff, 0xff, 0xff, 0xff, 0x03, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x30, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x0c, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xfe, 0xe4, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x43, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x30, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x52, 0xff, 0xf7, 0x7f, 0xff, 0xff, 0xff, 0x82, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x14, 0xff, 0xbf, 0x7f, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x65, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0x70, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x1b, 0xff, 0x76, 0xff, 0xff, 0xff, 0xff, 0x04, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x93, 0xff, 0xfe, 0xbf, 0x7f, 0xff, 0xff, 0x80, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x27, 0xfe, 0xef, 0xbf, 0x7f, 0xff, 0xff, 0x92, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x0b, 0xff, 0xed, 0xee, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x07, 0xff, 0x7d, 0x6e, 0xff, 0xff, 0xff, 0xc8, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x47, 0xde, 0xe7, 0x77, 0x7f, 0x7f, 0xff, 0xc2, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x1f, 0xfe, 0xfe, 0xdd, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x0d, 0xfd, 0xda, 0xdb, 0x7f, 0xb6, 0xff, 0xb0, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x2d, 0xdd, 0xee, 0xee, 0xff, 0x7f, 0xff, 0xc8, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x07, 0x77, 0xe6, 0xb6, 0xf7, 0x7b, 0xbf, 0xc2, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x43, 0xbc, 0xda, 0xdb, 0x37, 0x3e, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x13, 0xaf, 0xda, 0x4f, 0x7d, 0x9b, 0x9f, 0xe4, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x02, 0x6d, 0xc6, 0x55, 0xf6, 0x0d, 0xfb, 0xe1, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x23, 0xdd, 0x76, 0xd6, 0x78, 0x05, 0x6f, 0xc9, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x09, 0xb7, 0x20, 0x0b, 0x68, 0x21, 0xdf, 0xca, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x01, 0xb6, 0xaf, 0x8a, 0x68, 0x04, 0xf7, 0x66, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x24, 0xdb, 0x9e, 0xe6, 0xd8, 0x03, 0xbb, 0x35, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x01, 0x6d, 0x9e, 0xf1, 0xb6, 0x0b, 0xaf, 0xb5, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x11, 0xb6, 0x6f, 0xec, 0xf3, 0xff, 0xed, 0x9a, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x04, 0xb7, 0x7f, 0x8e, 0xfd, 0xff, 0x7b, 0x62, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x41, 0x39, 0x9a, 0x62, 0xff, 0xff, 0x4f, 0x38, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x12, 0x4e, 0xd2, 0x59, 0xff, 0xff, 0xed, 0x92, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x33, 0x75, 0x94, 0xff, 0xff, 0xa6, 0xc4, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x24, 0xb3, 0x74, 0x96, 0xff, 0xff, 0xbb, 0x64, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x18, 0xfa, 0x62, 0xff, 0xff, 0x99, 0x31, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x12, 0x04, 0xfb, 0x28, 0xff, 0xff, 0xb2, 0xc0, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x92, 0x7c, 0x9b, 0x7f, 0xff, 0x62, 0x8a, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x20, 0x01, 0x3c, 0xc4, 0x7f, 0xff, 0x01, 0xa0, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x09, 0x24, 0xbf, 0x35, 0xff, 0xfe, 0xc4, 0x44, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x9f, 0x92, 0x7f, 0xde, 0xa3, 0x51, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x20, 0x90, 0x3f, 0x4a, 0x6c, 0xfd, 0x89, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x04, 0x01, 0x2f, 0x4f, 0xff, 0xfd, 0x01, 0xa8, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x01, 0x08, 0x27, 0x31, 0xff, 0xfb, 0x00, 0x82, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x10, 0x00, 0x8f, 0x93, 0xff, 0xf2, 0x04, 0xd0, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x42, 0x0b, 0xcc, 0x07, 0xe6, 0x11, 0x04, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x22, 0x00, 0x02, 0xc9, 0xbf, 0xcc, 0x00, 0x61, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x08, 0x03, 0x72, 0x7f, 0x90, 0x00, 0x88, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x04, 0x00, 0x01, 0x1a, 0x7f, 0x20, 0x00, 0x0a, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x41, 0x00, 0x8c, 0xbc, 0x60, 0x01, 0x26, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x10, 0x00, 0x00, 0x62, 0xb1, 0x10, 0x00, 0x40, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x08, 0x00, 0x00, 0x02, 0x00, 0x00, 0x98, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x10, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x10, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x62, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
};

static const unsigned char PROGMEM epd_bitmap_kurona [] = {
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x48, 0x90, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x92, 0x09, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x02, 0x20, 0x40, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x01, 0x48, 0x86, 0x00, 0x31, 0xb1, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x02, 0x0c, 0x5b, 0x80, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x84, 0x44, 0xd0, 0x98, 0xc3, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x21, 0x07, 0x40, 0x48, 0x31, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x08, 0x21, 0x20, 0x01, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x82, 0x24, 0x08, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x21, 0x00, 0xc0, 0x80, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x04, 0x08, 0x12, 0x12, 0xfc, 0xf9, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x40, 0x00, 0x79, 0xf1, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x02, 0x04, 0x20, 0xc8, 0x4c, 0x61, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x06, 0xb3, 0xf8, 0x02, 0x00, 0x05, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x33, 0xfe, 0xd8, 0x60, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x1e, 0x78, 0x0c, 0x24, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x7c, 0xb3, 0x2c, 0x20, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x61, 0xbe, 0x58, 0x32, 0x44, 0x49, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x01, 0xbf, 0xfe, 0x10, 0x7c, 0xf9, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x1e, 0x9f, 0xde, 0x11, 0x2c, 0x21, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x47, 0xbf, 0xfb, 0x08, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x17, 0x3f, 0xfe, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x26, 0x5f, 0xfe, 0x10, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x03, 0x5f, 0xfe, 0x01, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x48, 0x3f, 0xfc, 0x08, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x01, 0x9f, 0xfc, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x24, 0x5f, 0xf8, 0x10, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x02, 0x4d, 0xe8, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x9f, 0xf0, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x23, 0xa0, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x01, 0x27, 0x80, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x17, 0x30, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x04, 0x40, 0x00, 0x84, 0x25, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x21, 0x18, 0x00, 0x24, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x04, 0x00, 0x09, 0x38, 0x02, 0x00, 0x13, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x05, 0x00, 0x04, 0xb8, 0x08, 0x01, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x04, 0x00, 0x20, 0xb0, 0x60, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x08, 0x41, 0x7c, 0x00, 0x35, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x03, 0x10, 0x7d, 0x10, 0x3f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x12, 0x3c, 0x04, 0x9b, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x50, 0x7c, 0x82, 0x3f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x21, 0x7c, 0x92, 0x3f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0xa0, 0x7c, 0xc1, 0xbf, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x04, 0xf4, 0xc0, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x40, 0x18, 0xc0, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x30, 0x40, 0xe4, 0xbf, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x98, 0x90, 0xe0, 0x9f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x0e, 0x20, 0xe0, 0x1f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x02, 0x43, 0x01, 0xc1, 0x3f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x90, 0x00, 0x00, 0x83, 0x80, 0xc0, 0x1f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x04, 0x00, 0x00, 0x81, 0x82, 0x40, 0x1d, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc1, 0x00, 0x00, 0x00, 0x61, 0x80, 0x00, 0x41, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x40, 0x00, 0x02, 0x72, 0xc5, 0x08, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x10, 0x00, 0x08, 0x38, 0xc1, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc1, 0x00, 0x00, 0x01, 0x1c, 0xe0, 0x40, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x08, 0x00, 0x04, 0x0e, 0xe2, 0xc0, 0x21, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x20, 0x00, 0x00, 0x0f, 0xf0, 0x80, 0x81, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x01, 0x17, 0xf9, 0x80, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x80, 0x00, 0x04, 0x47, 0xfd, 0x80, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x49, 0xff, 0x10, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc1, 0x10, 0x00, 0x08, 0x3e, 0xff, 0x00, 0x11, 0xff, 0xff, 0xff, 0xff
};

static const unsigned char PROGMEM epd_bitmap_tachi [] = {
	0xff, 0xff, 0xff, 0xff, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x90, 0x60, 0xa0, 0x0b, 0x40, 0x00, 0x4f, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x40, 0x10, 0x90, 0x20, 0x40, 0x12, 0x57, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x61, 0x10, 0x44, 0x04, 0x80, 0x00, 0x37, 0x77, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x19, 0x63, 0x20, 0x12, 0x20, 0x24, 0x4f, 0xdd, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x46, 0x08, 0x9a, 0xc9, 0x09, 0x01, 0x2e, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x92, 0x42, 0x20, 0x44, 0x48, 0x27, 0xb6, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x26, 0x52, 0x0a, 0xc8, 0x96, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x11, 0x11, 0x82, 0x12, 0x41, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x04, 0x48, 0x84, 0x20, 0x01, 0x18, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x24, 0x40, 0x10, 0x4a, 0x64, 0x24, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x25, 0x14, 0x81, 0x31, 0x09, 0x84, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x91, 0x24, 0xa4, 0x24, 0x90, 0x11, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x12, 0x49, 0x24, 0x98, 0x24, 0xc0, 0x48, 0x80, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x48, 0x92, 0x48, 0x9a, 0x4f, 0xfe, 0x26, 0x24, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x4c, 0x92, 0x42, 0x42, 0x5f, 0xfe, 0x91, 0x24, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x23, 0x24, 0x91, 0x24, 0xbf, 0xfe, 0x11, 0x12, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x90, 0x64, 0x91, 0x20, 0xbf, 0xff, 0x24, 0xc9, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x56, 0x49, 0x24, 0x19, 0x7f, 0xff, 0x24, 0x24, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x42, 0x19, 0x21, 0x81, 0x7f, 0xff, 0x13, 0x24, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x29, 0x92, 0x88, 0x64, 0xff, 0xff, 0x10, 0x92, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x99, 0x66, 0x02, 0x00, 0xff, 0xff, 0x26, 0x49, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x44, 0x09, 0x12, 0x13, 0x81, 0xf1, 0x89, 0x24, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x66, 0xd9, 0x00, 0xc0, 0xff, 0xff, 0x19, 0x24, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x32, 0x44, 0xe0, 0x05, 0xff, 0xff, 0x24, 0x93, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x99, 0x36, 0x01, 0x84, 0x07, 0xe1, 0x84, 0x90, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x8d, 0x93, 0x41, 0xc9, 0x3b, 0xe6, 0x12, 0x4c, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x64, 0xc9, 0x03, 0xd9, 0x9f, 0xe3, 0x32, 0x63, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x52, 0x6c, 0xe3, 0x71, 0xff, 0xff, 0x0c, 0x92, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x9b, 0x26, 0x21, 0xf3, 0xff, 0xff, 0x21, 0x94, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xa9, 0x99, 0x99, 0x79, 0xff, 0xff, 0x1a, 0x49, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x66, 0x65, 0x91, 0xf1, 0xff, 0xff, 0x12, 0x4a, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x54, 0xa6, 0x60, 0xf3, 0xff, 0xfe, 0x45, 0x32, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x99, 0x99, 0x48, 0xf9, 0xff, 0xdf, 0x35, 0x89, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xab, 0x59, 0x52, 0x99, 0xff, 0xfe, 0x08, 0x64, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x66, 0x66, 0x90, 0x03, 0xff, 0xfe, 0x9b, 0x16, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x54, 0xa6, 0xa1, 0x11, 0xff, 0xfc, 0x92, 0xd1, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x99, 0x99, 0x64, 0x19, 0xff, 0xf9, 0x24, 0x99, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xae, 0x6d, 0x42, 0x09, 0xff, 0x73, 0x2d, 0x26, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x63, 0x65, 0x98, 0x11, 0xff, 0xe4, 0x99, 0x64, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x59, 0x96, 0x61, 0x19, 0x7f, 0xc6, 0x26, 0x59, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x9c, 0xb2, 0xc4, 0x19, 0x97, 0xc3, 0x29, 0x93, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xa6, 0xac, 0x90, 0xb8, 0xf1, 0x84, 0x36, 0xac, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x6a, 0xcd, 0xa2, 0x79, 0xbc, 0x05, 0x24, 0xa5, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x5a, 0x53, 0x69, 0x7c, 0xcc, 0x16, 0x4d, 0x75, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x93, 0x77, 0x79, 0xb8, 0xfb, 0x76, 0x59, 0x4c, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xb5, 0x8f, 0xfe, 0xfc, 0xfb, 0x7f, 0x66, 0x9b, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x64, 0xb7, 0xff, 0xfd, 0xef, 0x9f, 0x66, 0xb2, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x5b, 0x67, 0xff, 0xfe, 0x7f, 0xff, 0xd9, 0x64, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x9b, 0x3f, 0xff, 0xfe, 0x7f, 0xfe, 0xe9, 0x4d, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xa4, 0xdf, 0xff, 0xff, 0xbf, 0xff, 0xee, 0x9b, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x6d, 0x5f, 0xff, 0xff, 0xff, 0xff, 0xf2, 0xb2, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x59, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xf5, 0x64, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x93, 0x7f, 0xff, 0xff, 0xfe, 0xff, 0xfd, 0x4d, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xb6, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x5b, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x65, 0xff, 0xff, 0xff, 0xef, 0x7f, 0xff, 0x92, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x4f, 0xff, 0xff, 0x7f, 0xff, 0xfe, 0xff, 0x34, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xb3, 0xff, 0xff, 0xff, 0x7f, 0xf7, 0xff, 0xcd, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x9f, 0xff, 0xff, 0xff, 0xdf, 0xff, 0x5f, 0xa6, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x4f, 0xff, 0xff, 0xff, 0xfe, 0xfd, 0x3f, 0xb2, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x6f, 0xff, 0xfe, 0x1f, 0xfb, 0xf9, 0x9f, 0xc9, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x9f, 0xff, 0xfe, 0xc0, 0xff, 0xf2, 0xdf, 0xdb, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x5f, 0xff, 0xfe, 0x76, 0x0f, 0xc2, 0x5f, 0xc8, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x5f, 0xff, 0xfe, 0x19, 0xa0, 0x49, 0x2f, 0xe6, 0xff, 0xff, 0xff, 0xff
};

static const unsigned char PROGMEM epd_bitmap_tachibana [] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xe8, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xea, 0x40, 0x00, 0x10, 0x00, 0x13, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xef, 0xc0, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xd0, 0x04, 0x00, 0xb5, 0x70, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xca, 0x00, 0x09, 0x5b, 0x58, 0x15, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xa0, 0x40, 0x11, 0xad, 0xae, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xa0, 0x00, 0x10, 0xea, 0xf5, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0x94, 0x00, 0x13, 0x37, 0x5a, 0x81, 0x77, 0x77, 0x77, 0x77, 0x77, 
	0x77, 0x77, 0x77, 0x77, 0x77, 0xa0, 0x80, 0x21, 0xda, 0xaf, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0x40, 0x00, 0x32, 0xab, 0x75, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xdd, 0xa8, 0x00, 0x22, 0xda, 0xd5, 0x42, 0xda, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x7f, 0x82, 0x00, 0x53, 0x6d, 0x5e, 0xc1, 0x7f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0x50, 0x00, 0x65, 0x56, 0xeb, 0x41, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x40, 0x52, 0xdb, 0x55, 0xc2, 0x7f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0x52, 0x00, 0xd3, 0x55, 0x5e, 0xa1, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0x40, 0x00, 0xa5, 0x6a, 0xa1, 0x66, 0x7f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xfd, 0x90, 0x00, 0xa6, 0xb0, 0x15, 0x4b, 0x7f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0x44, 0x00, 0x05, 0xd7, 0xd6, 0xed, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xa0, 0x80, 0xa5, 0x5a, 0x09, 0x4e, 0xbf, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xee, 0x88, 0x01, 0x46, 0xa8, 0x04, 0xb7, 0x7f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0x40, 0x00, 0x05, 0xb5, 0x17, 0x7a, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0x52, 0x00, 0x26, 0xdb, 0x2d, 0xae, 0x7f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x7f, 0x40, 0x00, 0x2a, 0xaa, 0xd6, 0xf4, 0xbf, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0x10, 0x22, 0xcb, 0x6d, 0xab, 0x58, 0xbf, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0x44, 0x03, 0x4d, 0xb6, 0xdd, 0x6a, 0x7f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xfb, 0xf7, 0x40, 0x21, 0x8a, 0xab, 0x6b, 0xb0, 0xbf, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0x28, 0x02, 0xd6, 0xda, 0xb6, 0xd0, 0xbf, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0x41, 0x23, 0x4b, 0x6d, 0xad, 0x40, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xdf, 0xbf, 0x24, 0x02, 0x9d, 0xaa, 0xdb, 0xa2, 0x3f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xfe, 0x80, 0x23, 0x96, 0xb6, 0xb5, 0x50, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xa8, 0x05, 0x5a, 0xd5, 0xde, 0x90, 0x7f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xfe, 0xfd, 0xfe, 0x80, 0x27, 0x97, 0xed, 0x6b, 0xa2, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 
	0xbb, 0xbb, 0xbb, 0xff, 0xf7, 0x52, 0x05, 0x5a, 0x56, 0xad, 0x10, 0x7f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x25, 0x97, 0xb5, 0xdb, 0x55, 0x7f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0x54, 0x06, 0xac, 0xdb, 0x6d, 0x20, 0xbf, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xbf, 0x40, 0x27, 0x37, 0x6d, 0x5a, 0x92, 0x7f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xa9, 0x05, 0x2a, 0xaa, 0xee, 0x51, 0x7f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xfe, 0xd0, 0x26, 0x7b, 0xdb, 0x59, 0x10, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xfd, 0xfb, 0xea, 0x03, 0x56, 0x55, 0xb4, 0xa5, 0x7b, 0xbb, 0xbb, 0xbb, 0xbb, 
	0xbb, 0xbb, 0xbb, 0xbf, 0xff, 0xa8, 0x20, 0xbb, 0xda, 0xe8, 0x50, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x84, 0xd6, 0xaf, 0x55, 0x12, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xdf, 0xaa, 0x22, 0xbb, 0x75, 0xb0, 0xa2, 0xdf, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x24, 0xdd, 0xad, 0x6a, 0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xed, 0x20, 0x66, 0xda, 0xb2, 0x52, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xfe, 0xfd, 0xf5, 0x95, 0x7b, 0x6d, 0x69, 0x27, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0x42, 0x58, 0xd2, 0xf5, 0x56, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xb4, 0xef, 0x25, 0xad, 0x6f, 0xf7, 0x77, 0x77, 0x77, 0x77, 
	0x77, 0x77, 0x77, 0x77, 0xef, 0xf7, 0xe5, 0x75, 0x8e, 0xfd, 0x76, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xbd, 0x5e, 0xd5, 0xad, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xf7, 0x76, 0xf6, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x7f, 0xfe, 0xfa, 0xba, 0xad, 0xad, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xfd, 0xff, 0xfb, 0xd7, 0xdb, 0x77, 0xfe, 0xee, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xf7, 0xff, 0xed, 0x7a, 0xb6, 0xde, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xfb, 0xbf, 0xad, 0xff, 0xdd, 0xad, 0xb5, 0x5f, 0xbb, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xaa, 0xb5, 0x76, 0xdf, 0xf6, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfb, 0xdb, 0xab, 0x69, 0x5f, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xdf, 0xff, 0xf7, 0xad, 0x6d, 0x75, 0xbb, 0xb6, 0xee, 0xee, 0xee, 0xee, 0xee, 
	0xee, 0xee, 0xee, 0xff, 0xf7, 0xde, 0xf7, 0x5a, 0xae, 0xd6, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xdf, 0x7b, 0x5d, 0xae, 0xd5, 0x55, 0xba, 0xdf, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x7f, 0xfe, 0xf6, 0xb2, 0xb5, 0x57, 0x77, 0xf6, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xfd, 0xdb, 0xbf, 0xd5, 0x4a, 0xbd, 0xda, 0xdf, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xf7, 0xfd, 0xea, 0xed, 0x6a, 0xd7, 0x7d, 0xfd, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xfb, 0xbe, 0xab, 0x7d, 0xbb, 0x57, 0x7b, 0xd6, 0xb7, 0xff, 0xff, 0xff, 0xff
};

static const unsigned char PROGMEM epd_bitmap_roxy [] = {
	0xff, 0xff, 0xff, 0xff, 0x1e, 0x7b, 0xd8, 0x7e, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x7c, 0xcc, 0xc3, 0xf0, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xf3, 0xb7, 0x0f, 0x80, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe6, 0xb4, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xcc, 0xd1, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x3b, 0x47, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x6b, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x9a, 0x38, 0x00, 0x00, 0x00, 0x00, 0x5a, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xf4, 0xf0, 0x00, 0x00, 0x00, 0x07, 0x7f, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x21, 0xc0, 0x00, 0x00, 0x00, 0x31, 0xa5, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xa7, 0x00, 0x00, 0x00, 0x01, 0x9c, 0x88, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x9e, 0x00, 0x00, 0x00, 0x06, 0x52, 0x6b, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x38, 0x00, 0x00, 0x00, 0x32, 0x4b, 0x11, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x60, 0x00, 0x00, 0x00, 0x89, 0x88, 0xc4, 0xc0, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x03, 0x6e, 0x26, 0x36, 0x00, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x19, 0x1e, 0x79, 0x22, 0x40, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x4c, 0xc5, 0x1d, 0x89, 0x80, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xc4, 0x47, 0x9e, 0x64, 0xa0, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x06, 0x33, 0x37, 0xcf, 0x16, 0x20, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x13, 0x21, 0x93, 0xcf, 0x92, 0x48, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x24, 0xcc, 0x4b, 0xe8, 0x29, 0x48, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xac, 0x92, 0x27, 0x81, 0x49, 0x26, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x02, 0x89, 0x32, 0x97, 0x0f, 0x56, 0x80, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x08, 0xc9, 0x48, 0x9e, 0x07, 0xd0, 0xc0, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x26, 0x2c, 0x4d, 0x2c, 0x03, 0xcd, 0x10, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x58, 0xad, 0x21, 0x2d, 0x03, 0x62, 0x20, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x01, 0x48, 0x9e, 0xca, 0x4f, 0x09, 0xca, 0x82, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x02, 0x4a, 0x9e, 0xe9, 0x1f, 0xab, 0xc9, 0x30, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x0b, 0x30, 0xdf, 0x64, 0xdf, 0xeb, 0xa5, 0x08, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x13, 0x25, 0xcf, 0x79, 0x3f, 0xef, 0x94, 0x18, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x4c, 0xc8, 0xef, 0xbd, 0x1f, 0xf7, 0xb2, 0x04, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x3c, 0x92, 0xee, 0xbc, 0xcf, 0xff, 0xb0, 0x24, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x99, 0x24, 0xfb, 0x04, 0x8f, 0x7f, 0xe8, 0x3e, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x53, 0x2c, 0xfc, 0x07, 0xff, 0xfe, 0xe8, 0x7e, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x52, 0x42, 0xf0, 0xff, 0xff, 0xfb, 0xd8, 0x1c, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x08, 0x98, 0xe3, 0xff, 0xff, 0xdf, 0xf8, 0xd8, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x8c, 0x93, 0xef, 0xff, 0xfd, 0xff, 0xf9, 0x38, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x61, 0x64, 0x5f, 0xff, 0xbf, 0xff, 0xf1, 0x38, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x14, 0x09, 0x7f, 0xfd, 0xff, 0xbe, 0xf8, 0x98, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x93, 0xa2, 0x3f, 0xdf, 0xf7, 0xfb, 0xea, 0x58, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x28, 0x98, 0x9f, 0xff, 0xff, 0x7f, 0xf2, 0x58, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x4c, 0x44, 0x9f, 0xff, 0x7f, 0xff, 0xf1, 0x38, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x5f, 0x24, 0xae, 0xf7, 0xfd, 0xff, 0x64, 0x98, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x1f, 0x22, 0x27, 0xff, 0xef, 0xb7, 0xc8, 0x98, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x07, 0x19, 0x13, 0xff, 0xff, 0xff, 0xca, 0x78, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x83, 0xc4, 0xdd, 0xdd, 0xff, 0x7f, 0x22, 0x3a, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x61, 0xc4, 0x0f, 0xff, 0xbb, 0xfd, 0x12, 0x98, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x40, 0xd9, 0x23, 0xfb, 0xff, 0xef, 0x48, 0xb8, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x98, 0xc3, 0x39, 0xdf, 0xf7, 0xbe, 0x24, 0x3a, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xac, 0x48, 0xb0, 0x7f, 0x7e, 0xf8, 0x24, 0xb9, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x64, 0x12, 0x34, 0x0f, 0xdf, 0xf8, 0x12, 0x18, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x52, 0x24, 0xb8, 0xc1, 0xfd, 0xe1, 0x48, 0xca, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x9b, 0x04, 0x1e, 0x00, 0x03, 0x00, 0x48, 0x08, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x64, 0x83, 0x1c, 0x20, 0x10, 0x22, 0x26, 0x08, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x4c, 0x88, 0x4e, 0x22, 0x04, 0x80, 0x20, 0x08, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x33, 0x00, 0x0f, 0x18, 0x80, 0xc4, 0x92, 0x08, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x92, 0x65, 0x8f, 0x84, 0x20, 0x00, 0x84, 0x8c, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xcc, 0x80, 0x27, 0xc4, 0x28, 0x08, 0x60, 0x0c, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x25, 0x92, 0x03, 0xc9, 0x86, 0x01, 0x12, 0x0c, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x24, 0x40, 0x83, 0xe2, 0x00, 0x80, 0x72, 0x1c, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xcb, 0x48, 0x60, 0x24, 0x90, 0x00, 0xa3, 0x9c, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x32, 0x63, 0x10, 0x16, 0x44, 0x00, 0xc1, 0xcc, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x21, 0x80, 0x10, 0x13, 0x90, 0x01, 0xc1, 0xcc, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x88, 0x30, 0x90, 0x08, 0xf2, 0x01, 0x89, 0xcc, 0xff, 0xff, 0xff, 0xff
};

static const unsigned char PROGMEM epd_bitmap_touka [] = {
	0xfe, 0x00, 0x36, 0xff, 0xe7, 0xc1, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x7f, 
	0xfe, 0x6d, 0x99, 0xdf, 0xd7, 0xb4, 0xfc, 0x00, 0x0d, 0x00, 0x01, 0xb6, 0x00, 0x0d, 0x00, 0x7f, 
	0xfe, 0x48, 0x4d, 0xdb, 0xd7, 0xa4, 0xb6, 0x24, 0x99, 0x08, 0x0c, 0x01, 0x90, 0x84, 0x80, 0x7f, 
	0xff, 0x33, 0x7d, 0x6f, 0xef, 0xcb, 0x7a, 0x00, 0x30, 0x80, 0x80, 0x40, 0x10, 0x26, 0x24, 0x7f, 
	0xfe, 0x66, 0x36, 0xef, 0xef, 0xe2, 0x6c, 0x49, 0x04, 0x10, 0x20, 0x00, 0x00, 0x12, 0x41, 0xff, 
	0xfe, 0x59, 0x98, 0xbf, 0xdf, 0xb2, 0xbc, 0x00, 0x40, 0x00, 0x02, 0x08, 0x0a, 0x49, 0x99, 0x7f, 
	0xfe, 0x5b, 0x68, 0xdf, 0xc7, 0x99, 0xb6, 0x04, 0x90, 0x82, 0x00, 0x82, 0x62, 0x3d, 0xa6, 0x7f, 
	0xfe, 0x36, 0x6c, 0xd7, 0x63, 0x64, 0x68, 0x20, 0x80, 0x20, 0x08, 0x20, 0x09, 0x36, 0x6f, 0xff, 
	0xfe, 0xb5, 0xb1, 0x75, 0xb3, 0x66, 0xda, 0x09, 0x40, 0x00, 0x41, 0x09, 0x85, 0x9f, 0x99, 0xff, 
	0xfe, 0x9d, 0xb9, 0x6d, 0xa5, 0x99, 0xb4, 0x82, 0x40, 0x00, 0x00, 0x04, 0x24, 0xe9, 0xf7, 0x7f, 
	0xfe, 0x6b, 0x6c, 0xdb, 0x6d, 0xc9, 0x24, 0x22, 0x04, 0x01, 0x04, 0x92, 0x6b, 0x3e, 0x6f, 0x7f, 
	0xff, 0xeb, 0x6e, 0xdb, 0x73, 0x66, 0x6b, 0x88, 0x80, 0x00, 0x10, 0x4a, 0xc9, 0x97, 0xad, 0xff, 
	0xff, 0x34, 0xdb, 0x6d, 0xbe, 0x66, 0xcb, 0xc4, 0x10, 0x04, 0x03, 0x09, 0x92, 0xd9, 0xb7, 0x7f, 
	0xff, 0xd6, 0xdf, 0x6d, 0xcf, 0x99, 0x37, 0xc4, 0x40, 0x10, 0x08, 0x94, 0x92, 0x6e, 0xd7, 0xff, 
	0xfe, 0xcb, 0x75, 0xb6, 0x79, 0x93, 0x27, 0xc4, 0x00, 0x00, 0x42, 0x34, 0x41, 0xee, 0xd9, 0xff, 
	0xfe, 0x63, 0x36, 0xdb, 0xb6, 0x64, 0xcf, 0x94, 0x00, 0x81, 0x04, 0x23, 0x11, 0x33, 0x6f, 0x7f, 
	0xff, 0x11, 0xae, 0xcb, 0xb6, 0xcc, 0x91, 0x08, 0x40, 0x04, 0x11, 0x20, 0x20, 0x5d, 0xa7, 0xff, 
	0xff, 0x1c, 0xc9, 0xb4, 0xcd, 0x93, 0x20, 0x09, 0x04, 0x00, 0x08, 0x48, 0x89, 0x36, 0xdb, 0xff, 
	0xfe, 0xc6, 0x76, 0x66, 0x69, 0x32, 0x62, 0x39, 0x20, 0x48, 0x8a, 0x42, 0x21, 0x2e, 0xdb, 0xff, 
	0xfe, 0x12, 0x13, 0x3b, 0xb6, 0x49, 0x88, 0x14, 0x00, 0x02, 0x20, 0x92, 0x60, 0x2b, 0x6e, 0xff, 
	0xff, 0x19, 0x99, 0x9b, 0x99, 0x8c, 0x80, 0x94, 0x81, 0x02, 0x24, 0x80, 0x44, 0x9b, 0x6f, 0xff, 
	0xfe, 0x84, 0xcc, 0xef, 0xfc, 0xa6, 0x64, 0x2c, 0x88, 0x48, 0x08, 0x25, 0x80, 0x36, 0xd3, 0x7f, 
	0xfe, 0x24, 0x66, 0x6f, 0xe6, 0xb3, 0x10, 0x22, 0x80, 0x01, 0x49, 0x08, 0xa2, 0x6d, 0x99, 0xff, 
	0xfe, 0x41, 0x03, 0x7f, 0xfd, 0x99, 0x92, 0x0a, 0x21, 0x04, 0x41, 0x02, 0xa0, 0x89, 0x6c, 0x7f, 
	0xfe, 0x00, 0x18, 0x92, 0x59, 0x4c, 0xc8, 0x99, 0x80, 0x48, 0x12, 0x66, 0x84, 0x36, 0x61, 0x7f, 
	0xfe, 0x88, 0x80, 0x9b, 0x5b, 0x66, 0x24, 0x84, 0x44, 0x08, 0x90, 0x1b, 0xc1, 0x44, 0x90, 0x7f, 
	0xfe, 0x02, 0x24, 0x49, 0x94, 0x93, 0x24, 0x65, 0x00, 0x92, 0x05, 0xb9, 0xc0, 0xc1, 0x24, 0xff, 
	0xfe, 0x30, 0x81, 0x24, 0xa6, 0xd9, 0x93, 0x01, 0x22, 0x10, 0xa5, 0xf8, 0x04, 0x19, 0x24, 0x7f, 
	0xfe, 0x0c, 0x80, 0x04, 0xaa, 0x4c, 0xd8, 0x32, 0x48, 0x48, 0x93, 0xf7, 0x21, 0x26, 0x51, 0x7f, 
	0xfe, 0x42, 0x4f, 0xc2, 0x2b, 0x33, 0x62, 0x88, 0x84, 0x2c, 0x5f, 0xfb, 0x80, 0x89, 0x99, 0x7f, 
	0xfe, 0x18, 0x4f, 0xe0, 0x49, 0xb5, 0x24, 0x89, 0x33, 0x16, 0x7f, 0xde, 0x06, 0x4a, 0xa6, 0x7f, 
	0xfe, 0x01, 0x1b, 0xb1, 0x04, 0xcc, 0xd9, 0x24, 0x49, 0x1f, 0x7f, 0x7e, 0x42, 0x26, 0x64, 0xff, 
	0xfe, 0x44, 0x5f, 0xf0, 0x26, 0x4a, 0x92, 0x46, 0xcc, 0x9f, 0x7f, 0xfc, 0xc3, 0x25, 0x59, 0x7f, 
	0xfe, 0x11, 0x5f, 0x62, 0x23, 0x32, 0xa4, 0xd9, 0x32, 0x8f, 0xff, 0xf9, 0x24, 0x9f, 0x5b, 0x7f, 
	0xfe, 0x90, 0x5f, 0xe0, 0xc9, 0xb6, 0x68, 0xc1, 0xdb, 0x2f, 0xef, 0xf3, 0x6c, 0x9f, 0xa4, 0xff, 
	0xfe, 0xa2, 0x9f, 0xa4, 0x18, 0xc9, 0x9b, 0x71, 0xdc, 0xc7, 0xfb, 0xe4, 0xcb, 0x2f, 0xb4, 0xff, 
	0xfe, 0x60, 0xa6, 0xc1, 0xa6, 0x6d, 0xb7, 0x6e, 0xf6, 0x93, 0xff, 0x2d, 0xb8, 0x5f, 0xdb, 0x7f, 
	0xfe, 0x40, 0x6d, 0x00, 0x25, 0xb6, 0xed, 0xeb, 0xbf, 0x33, 0xfd, 0xbb, 0xf6, 0x5f, 0xc9, 0x7f, 
	0xfe, 0x12, 0x59, 0x00, 0x91, 0x97, 0xbb, 0x33, 0xed, 0xcc, 0xf0, 0xde, 0xf6, 0x2f, 0x3d, 0xff, 
	0xfe, 0x01, 0x96, 0x42, 0x06, 0x7d, 0xdb, 0xdc, 0xff, 0xc8, 0x42, 0xde, 0xfb, 0x25, 0xe6, 0x7f, 
	0xfe, 0x04, 0x64, 0x80, 0x01, 0x8f, 0x76, 0xcf, 0xde, 0xf3, 0x30, 0xb7, 0x6c, 0x99, 0xfb, 0xff, 
	0xfe, 0x10, 0xdb, 0x00, 0x48, 0xf7, 0xb7, 0xe6, 0xff, 0xbd, 0x90, 0x6f, 0x79, 0x9b, 0x3c, 0xff, 
	0xfe, 0x00, 0x12, 0x11, 0x02, 0x1c, 0xed, 0x9b, 0xb7, 0xf9, 0xc9, 0x7f, 0x98, 0x47, 0xd6, 0x7f, 
	0xfe, 0x05, 0x24, 0x84, 0x21, 0x6f, 0xdb, 0x6f, 0xfd, 0xfb, 0x69, 0x3f, 0xa6, 0x24, 0xd9, 0x7f, 
	0xfe, 0x20, 0x8c, 0x84, 0x89, 0x3b, 0x7a, 0x6c, 0xdf, 0x6f, 0x24, 0x7f, 0xcc, 0x19, 0xb4, 0x7f, 
	0xfe, 0x08, 0xb2, 0x5b, 0xc8, 0xbf, 0x6e, 0x9b, 0xf7, 0xff, 0x9c, 0x9e, 0xd4, 0x82, 0x24, 0xff, 
	0xfe, 0x02, 0x79, 0x0b, 0x66, 0x67, 0xf9, 0xb7, 0x7d, 0x9a, 0xf9, 0x3f, 0xe6, 0x24, 0x90, 0x7f, 
	0xfe, 0x46, 0x7c, 0xa7, 0xe4, 0xde, 0x9f, 0x67, 0xdf, 0xee, 0xf9, 0x6f, 0xe7, 0x11, 0x13, 0x7f, 
	0xfe, 0x11, 0xfc, 0x24, 0x99, 0x7f, 0xf6, 0x5c, 0xfb, 0x69, 0xf4, 0x4f, 0xb3, 0x49, 0x48, 0x7f, 
	0xfe, 0x14, 0x79, 0x1f, 0xd2, 0x77, 0x3e, 0x9f, 0xbf, 0xff, 0xf6, 0x3f, 0xf3, 0x26, 0x49, 0xff, 
	0xfe, 0x46, 0x52, 0x43, 0x64, 0xfd, 0xae, 0xa7, 0xff, 0xf7, 0xb8, 0xae, 0xf9, 0x90, 0x92, 0x7f, 
	0xfe, 0x03, 0x10, 0x11, 0x25, 0xfe, 0xcf, 0x2d, 0xef, 0xff, 0xec, 0x2f, 0xf8, 0x92, 0x24, 0x7f, 
	0xfe, 0x20, 0xc4, 0x8c, 0x08, 0xdb, 0x77, 0x5f, 0xfd, 0xbc, 0xe6, 0x37, 0xfe, 0x84, 0x0c, 0xff, 
	0xfe, 0x08, 0x20, 0x20, 0xc2, 0xe5, 0x37, 0x56, 0xfe, 0xcd, 0xbc, 0x97, 0xd8, 0x81, 0x00, 0xff, 
	0xfe, 0x02, 0x09, 0x02, 0x12, 0x74, 0xcf, 0x77, 0xfb, 0xfd, 0xf9, 0x0f, 0xfe, 0x22, 0x40, 0x7f, 
	0xfe, 0x00, 0x00, 0x18, 0x01, 0xf2, 0x79, 0xf9, 0xbf, 0x77, 0xd8, 0x0b, 0xfe, 0x0a, 0x00, 0x7f, 
	0xfe, 0x00, 0x00, 0x00, 0x44, 0x79, 0x3e, 0xde, 0x37, 0xdf, 0xc2, 0x8b, 0xfe, 0xc4, 0x80, 0x7f, 
	0xfe, 0x00, 0x00, 0x00, 0x01, 0x9c, 0xbe, 0xf7, 0x07, 0xfe, 0x88, 0x8b, 0xef, 0x04, 0x80, 0x7f, 
	0xfe, 0x00, 0x00, 0x00, 0x03, 0xb6, 0xfa, 0x0d, 0x40, 0xfa, 0x24, 0x07, 0xff, 0x21, 0x00, 0x7f, 
	0xfe, 0x08, 0x80, 0x00, 0x01, 0x89, 0x28, 0x01, 0x10, 0x13, 0x25, 0x25, 0xff, 0x80, 0x00, 0x7f, 
	0xfe, 0x20, 0x0b, 0x84, 0x01, 0x49, 0x20, 0x32, 0x44, 0x04, 0x90, 0x85, 0xff, 0xc0, 0x00, 0x7f, 
	0xfe, 0x04, 0x47, 0x91, 0x11, 0xd4, 0xc9, 0x08, 0x91, 0x64, 0x46, 0x05, 0xfd, 0xd0, 0x04, 0x7f, 
	0xfe, 0x53, 0x16, 0xc4, 0x44, 0x92, 0x48, 0x09, 0x08, 0x11, 0x10, 0x05, 0xff, 0xe0, 0x00, 0x7f, 
	0xfe, 0x48, 0x93, 0x84, 0x00, 0xc9, 0x30, 0x44, 0x62, 0x90, 0x91, 0x04, 0xff, 0x60, 0x00, 0x7f
};

static const unsigned char PROGMEM epd_bitmap_selfie [] = {
	0xff, 0xfd, 0xb6, 0xdd, 0xf6, 0xb6, 0xad, 0x6d, 0xb6, 0xad, 0xfb, 0xdb, 0xad, 0xb7, 0xff, 0xff, 
	0xff, 0xfe, 0xd5, 0x6b, 0x5a, 0xdb, 0x65, 0xaa, 0xda, 0xd6, 0x5d, 0x6d, 0x76, 0xda, 0xaf, 0xff, 
	0xff, 0xfa, 0xab, 0x5d, 0x6b, 0x55, 0xae, 0x95, 0x57, 0x5b, 0x66, 0xb6, 0xdb, 0x6d, 0xff, 0xff, 
	0xff, 0xfd, 0xb5, 0x66, 0xaa, 0xad, 0x55, 0x7a, 0xb5, 0xd5, 0x5b, 0x6b, 0x6d, 0x56, 0x9f, 0xff, 
	0xff, 0xfe, 0xdd, 0xb5, 0xd5, 0xb5, 0xb5, 0xfd, 0xaa, 0x6a, 0xb5, 0xbd, 0x5b, 0x6a, 0xef, 0xff, 
	0xff, 0xfb, 0x6b, 0x5b, 0x6e, 0xd6, 0xd5, 0xb6, 0xdb, 0xab, 0xbe, 0xf6, 0xed, 0xb6, 0xbf, 0xff, 
	0xff, 0xfd, 0xde, 0xed, 0xaa, 0xaa, 0xab, 0xff, 0xea, 0xb5, 0x4b, 0x5d, 0x5a, 0xda, 0xdf, 0xff, 
	0xff, 0xfe, 0xeb, 0x56, 0xd5, 0x5b, 0x5b, 0xbb, 0x75, 0x5a, 0xea, 0xef, 0x6f, 0x6b, 0x5f, 0xff, 
	0xff, 0xfb, 0x5d, 0xbb, 0x5b, 0x56, 0xaf, 0xef, 0xff, 0x6b, 0x55, 0x55, 0xb5, 0xaa, 0xaf, 0xff, 
	0xff, 0xfd, 0xeb, 0x6d, 0x55, 0x6a, 0xfd, 0xfe, 0xdb, 0xad, 0x5b, 0xfa, 0xdf, 0xed, 0x6f, 0xff, 
	0xff, 0xfb, 0x3d, 0xb6, 0xad, 0x5b, 0x77, 0x77, 0xff, 0xd5, 0x6e, 0xaf, 0x75, 0x56, 0xbf, 0xff, 
	0xff, 0xfd, 0xd6, 0xd5, 0xd6, 0xad, 0xff, 0xdf, 0x5b, 0x6b, 0x53, 0xfb, 0xbf, 0xfa, 0xcf, 0xff, 
	0xff, 0xfa, 0xab, 0x56, 0xb5, 0x75, 0xb7, 0xfd, 0xef, 0xed, 0xaf, 0xfd, 0x6a, 0xab, 0x7f, 0xff, 
	0xff, 0xfd, 0x6d, 0xb5, 0x55, 0x95, 0xfc, 0xb7, 0xfd, 0xb5, 0x53, 0x56, 0xda, 0xfd, 0x9f, 0xff, 
	0xff, 0xfb, 0xb6, 0xdb, 0xad, 0x5b, 0xd7, 0xff, 0x77, 0xf6, 0xbb, 0xff, 0x6d, 0xaa, 0xef, 0xff, 
	0xff, 0xfd, 0x55, 0x6d, 0x55, 0x55, 0xfe, 0xfb, 0xdf, 0x7a, 0xcd, 0xb7, 0xb6, 0xf7, 0xbf, 0xff, 
	0xff, 0xfb, 0xaa, 0xab, 0x6d, 0xaa, 0xdf, 0xaf, 0xbd, 0xea, 0xb7, 0x7d, 0xab, 0xad, 0x6f, 0xff, 
	0xff, 0xfd, 0x6b, 0x55, 0xaa, 0xb7, 0xfa, 0xfe, 0xf7, 0xfa, 0xcd, 0xdf, 0xfa, 0xff, 0xbf, 0xff, 
	0xff, 0xfa, 0xb5, 0x5a, 0xad, 0x5d, 0x6f, 0xfb, 0xde, 0xba, 0xab, 0xf6, 0xae, 0xb5, 0x6f, 0xff, 
	0xff, 0xfb, 0x55, 0x55, 0x55, 0x57, 0xfe, 0xdf, 0xf0, 0x5d, 0x57, 0x5f, 0xfd, 0xde, 0xbf, 0xff, 
	0xff, 0xfa, 0xdb, 0x6a, 0xb6, 0xdf, 0xb7, 0xf6, 0xc1, 0x06, 0xad, 0xf6, 0xde, 0xb5, 0xdf, 0xff, 
	0xff, 0xfd, 0x55, 0xad, 0x55, 0x4d, 0xc9, 0x7f, 0xd4, 0x62, 0xab, 0x5f, 0xf6, 0xfa, 0xaf, 0xff, 
	0xff, 0xfa, 0xaa, 0xb5, 0xaa, 0xb6, 0x00, 0xee, 0xd9, 0xa5, 0x5f, 0xed, 0xbb, 0xaf, 0x7f, 0xff, 
	0xff, 0xfb, 0x55, 0x55, 0x55, 0x5c, 0x47, 0x7f, 0xf0, 0x36, 0xb5, 0xbb, 0xdf, 0xfa, 0xdf, 0xff, 
	0xff, 0xfd, 0x5b, 0x55, 0x53, 0x59, 0x95, 0xf6, 0xf5, 0x77, 0x5f, 0x6e, 0xf5, 0xb7, 0x6f, 0xff, 
	0xff, 0xfa, 0xad, 0x56, 0xaa, 0xd1, 0x83, 0xdf, 0xb8, 0xae, 0xeb, 0xf7, 0x6f, 0xfd, 0xbf, 0xff, 
	0xff, 0xfa, 0xaa, 0xb5, 0x55, 0x59, 0xa5, 0x7b, 0xfe, 0xbe, 0xbd, 0x5a, 0xfb, 0x6e, 0xdf, 0xff, 
	0xff, 0xfa, 0xaa, 0xdb, 0xe9, 0x5b, 0x91, 0xfe, 0xdb, 0x76, 0xef, 0xf7, 0xbf, 0xfb, 0xef, 0xff, 
	0xff, 0xfa, 0xaa, 0xbe, 0xfd, 0x4c, 0xe5, 0xdb, 0xfd, 0xdf, 0x5a, 0xad, 0xea, 0xae, 0xbf, 0xff, 
	0xff, 0xfa, 0xaa, 0xab, 0xad, 0x6b, 0xdd, 0xff, 0xb7, 0x7a, 0xf7, 0xfe, 0xff, 0xfb, 0xef, 0xff, 
	0xff, 0xfa, 0xd5, 0x56, 0xfb, 0x57, 0xef, 0x6e, 0xfd, 0xee, 0xaa, 0xab, 0xab, 0x7f, 0x7f, 0xff, 
	0xff, 0xfd, 0x55, 0x55, 0xdd, 0xae, 0xbb, 0xfb, 0xef, 0xbb, 0xd7, 0xdd, 0xff, 0xd5, 0xdf, 0xff, 
	0xff, 0xfa, 0xaa, 0xaa, 0x76, 0xfb, 0xee, 0xdf, 0xbe, 0xed, 0x6d, 0x6b, 0x55, 0xff, 0xff, 0xff, 
	0xff, 0xfa, 0xab, 0x55, 0x9f, 0x5f, 0x7b, 0xfd, 0xf7, 0xbc, 0xf6, 0xde, 0xff, 0x77, 0x5f, 0xff, 
	0xff, 0xfa, 0xaa, 0xaa, 0xab, 0xad, 0xdf, 0x6f, 0x7d, 0xf5, 0xab, 0x75, 0xdb, 0xdd, 0xff, 0xff, 
	0xff, 0xfa, 0xaa, 0xaa, 0xaa, 0xff, 0x77, 0xfa, 0x9f, 0x6a, 0xb5, 0xaf, 0x76, 0xf7, 0x5f, 0xff, 
	0xff, 0xfa, 0xaa, 0xaa, 0xaa, 0xab, 0xdd, 0xab, 0x57, 0xfd, 0xd2, 0xd5, 0xdd, 0xfd, 0xef, 0xff, 
	0xff, 0xfa, 0xaa, 0xaa, 0xb5, 0x56, 0xff, 0xf5, 0xad, 0xb5, 0x7a, 0xbf, 0x6b, 0x5b, 0x7f, 0xff, 
	0xff, 0xfa, 0xaa, 0xaa, 0xd5, 0x49, 0xed, 0xad, 0x5f, 0xf6, 0xa5, 0xd5, 0xb5, 0xfe, 0xdf, 0xff, 
	0xff, 0xfa, 0xaa, 0xaa, 0xaa, 0xa5, 0xbf, 0xf5, 0xbe, 0xd5, 0xb2, 0xb6, 0xdb, 0x6b, 0x6f, 0xff, 
	0xff, 0xfa, 0xaa, 0xaa, 0xa9, 0x52, 0xf6, 0xfe, 0xf7, 0xda, 0xd7, 0x6f, 0x6d, 0xb5, 0xbf, 0xff, 
	0xff, 0xfa, 0xaa, 0xaa, 0xaa, 0xa8, 0x5f, 0xaf, 0xbd, 0xab, 0x4a, 0xb5, 0xb5, 0x6e, 0xef, 0xff, 
	0xff, 0xfa, 0xaa, 0xaa, 0xa9, 0x56, 0xbb, 0xfb, 0xef, 0x55, 0x6b, 0x5e, 0xd6, 0xd5, 0x7f, 0xff, 
	0xff, 0xfa, 0xaa, 0xaa, 0xaa, 0xaa, 0x8e, 0xee, 0xfd, 0x2d, 0x5a, 0xd7, 0x6b, 0x77, 0xaf, 0xff, 
	0xff, 0xfa, 0xaa, 0xaa, 0xa9, 0x55, 0x53, 0xbf, 0xb5, 0x55, 0x55, 0x5b, 0xbd, 0xaa, 0xdf, 0xff, 
	0xff, 0xfa, 0xaa, 0xaa, 0xa8, 0xaa, 0xa8, 0x7b, 0xf5, 0x6a, 0xaa, 0xb6, 0xd5, 0x5b, 0x5f, 0xff, 
	0xff, 0xfa, 0xaa, 0xaa, 0xaa, 0x55, 0x54, 0x5e, 0xda, 0x95, 0x6d, 0x6d, 0x56, 0xaa, 0xaf, 0xff, 
	0xff, 0xfa, 0xaa, 0xaa, 0xab, 0x6a, 0xaa, 0xeb, 0x55, 0x55, 0x2d, 0xaa, 0xbb, 0x6d, 0xff, 0xff, 
	0xff, 0xfa, 0xaa, 0xaa, 0xaa, 0x95, 0x57, 0x55, 0xbe, 0xfb, 0xda, 0x6a, 0xca, 0xdb, 0x4f, 0xff, 
	0xff, 0xfa, 0xaa, 0xaa, 0xab, 0xfa, 0xa8, 0xbd, 0x6b, 0xae, 0xf7, 0x95, 0x5d, 0xee, 0xbf, 0xff, 
	0xff, 0xfa, 0xaa, 0xaa, 0xaf, 0x7f, 0x56, 0xd6, 0xdd, 0xf5, 0xaf, 0xeb, 0x66, 0xb7, 0xef, 0xff, 
	0xff, 0xfa, 0xaa, 0xaa, 0xac, 0x07, 0xb7, 0x6b, 0x7e, 0x7f, 0xde, 0xad, 0x5b, 0x5a, 0xbf, 0xff, 
	0xff, 0xfd, 0x55, 0x5a, 0xb5, 0xf8, 0xed, 0xfa, 0xeb, 0xb6, 0xa5, 0xf5, 0xaa, 0xab, 0x5f, 0xff, 
	0xff, 0xfa, 0xb6, 0xd5, 0xef, 0xff, 0x3b, 0xbf, 0xff, 0xdf, 0x95, 0x1a, 0xd6, 0xdd, 0xef, 0xff, 
	0xff, 0xfb, 0x55, 0x6d, 0x57, 0xef, 0xde, 0xed, 0xbb, 0x7c, 0x52, 0x57, 0x6b, 0x6a, 0xbf, 0xff, 
	0xff, 0xfd, 0x6b, 0x56, 0xba, 0xbf, 0xe7, 0xff, 0xef, 0xea, 0x4a, 0xad, 0xdd, 0xbd, 0xaf, 0xff, 
	0xff, 0xfa, 0xb5, 0xb5, 0xae, 0xbd, 0x7a, 0xb5, 0xbe, 0xfa, 0xa9, 0x2e, 0xb6, 0xd6, 0xff, 0xff, 
	0xff, 0xfd, 0xda, 0xda, 0xfa, 0x55, 0xbb, 0xff, 0xfb, 0xb4, 0x24, 0xa5, 0xed, 0xbb, 0x5f, 0xff, 
	0xff, 0xfb, 0x57, 0x6f, 0x51, 0x24, 0xac, 0xad, 0x6f, 0xf1, 0x55, 0x57, 0x5b, 0x6d, 0xdf, 0xff, 
	0xff, 0xfd, 0xba, 0xb5, 0xa5, 0x52, 0x55, 0x5f, 0xfd, 0x55, 0x12, 0x8a, 0xf7, 0xb6, 0xbf, 0xff, 
	0xff, 0xfe, 0xd7, 0xda, 0xca, 0x8a, 0x92, 0x4a, 0xdb, 0xf5, 0x25, 0x57, 0x5d, 0x7b, 0xef, 0xff, 
	0xff, 0xfb, 0xfa, 0xb7, 0x51, 0x54, 0xaa, 0xaf, 0xef, 0xb4, 0x94, 0x92, 0xef, 0xde, 0xbf, 0xff, 
	0xff, 0xfe, 0xaf, 0xdd, 0xca, 0x52, 0x49, 0x05, 0x7e, 0x0a, 0x8a, 0xfd, 0xba, 0xb5, 0xef, 0xff, 
	0xff, 0xfb, 0xfa, 0xf7, 0x55, 0x2a, 0x2a, 0x54, 0xb6, 0xa2, 0x49, 0xd7, 0xef, 0xef, 0x7f, 0xff
};

// Array of all booting bitmaps
static const unsigned char* const boot_bitmaps[] = {
  epd_bitmap_tauka1,
  epd_bitmap_kaneki,
  epd_bitmap_kurona,
  epd_bitmap_tachi,
  epd_bitmap_tachibana,
  epd_bitmap_roxy,
  epd_bitmap_touka,
  epd_bitmap_selfie
};
const int num_boot_bitmaps = 8;

// Dino Run Bitmaps
static const unsigned char PROGMEM dino_run1[] = {
  0x0f, 0xc0, 0x1b, 0xc0, 0x1f, 0xc0, 0x1f, 0x00,
  0x1f, 0x80, 0x9f, 0xc0, 0xdf, 0xc0, 0xff, 0x80,
  0x7f, 0x00, 0x3e, 0x00, 0x12, 0x00, 0x13, 0x00
};
static const unsigned char PROGMEM dino_run2[] = {
  0x0f, 0xc0, 0x1b, 0xc0, 0x1f, 0xc0, 0x1f, 0x00,
  0x1f, 0x80, 0x9f, 0xc0, 0xdf, 0xc0, 0xff, 0x80,
  0x7f, 0x00, 0x1c, 0x00, 0x08, 0x00, 0x18, 0x00
};
static const unsigned char PROGMEM dino_duck1[] = {
  0x03, 0xfc, 0x9f, 0xfc, 0xdf, 0xfc, 0xff, 0xf8,
  0x7f, 0xf0, 0x3f, 0xc0, 0x1b, 0x00, 0x1b, 0x00
};
static const unsigned char PROGMEM dino_duck2[] = {
  0x03, 0xfc, 0x9f, 0xfc, 0xdf, 0xfc, 0xff, 0xf8,
  0x7f, 0xf0, 0x3f, 0xc0, 0x11, 0x00, 0x1b, 0x00
};
static const unsigned char PROGMEM cactus_small[] = {
  0x30, 0x30, 0xb0, 0xb0, 0xf0, 0xf0, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30
};
static const unsigned char PROGMEM cactus_double[] = {
  0x30, 0xc0, 0x30, 0xc0, 0xb1, 0xc0, 0xb1, 0xc0,
  0xf3, 0xc0, 0xf3, 0xc0, 0x30, 0xc0, 0x30, 0xc0,
  0x30, 0xc0, 0x30, 0xc0, 0x30, 0xc0, 0x30, 0xc0
};
static const unsigned char PROGMEM bird_wingup[] = {
  0x0e, 0x1f, 0x3b, 0x7f, 0xff, 0x3e, 0x08, 0x10
};
static const unsigned char PROGMEM bird_wingdown[] = {
  0x0e, 0x1f, 0x3b, 0x7f, 0xff, 0x7e, 0x48, 0x40
};

// Flappy Stone Bitmaps
static const unsigned char PROGMEM flappy_bird_sprite[] = {
  0x3c, 0x42, 0xad, 0x85, 0x81, 0x42, 0x3c, 0x18
};

// ---- Mario Mini Bitmaps (redesigned for a clear, readable walk-cycle) ----
static const uint8_t PROGMEM mario_stand_12x14[] = {
  0x0F,0x00,0x1F,0x80,0x1F,0x80,0x36,0xC0,0x1F,0x80,0x3F,0xF0,0x3F,0xF0,0x1F,0x80,
  0x3F,0xF0,0x36,0xC0,0x19,0x80,0x19,0x80,0x19,0x80,0x39,0xC0
};
static const uint8_t PROGMEM mario_run1_12x14[] = {
  0x0F,0x00,0x1F,0x80,0x1F,0x80,0x36,0xC0,0x1F,0x80,0x3F,0xF0,0x3F,0xF0,0x1F,0x80,
  0x3F,0xF0,0x36,0xC0,0x38,0x00,0x78,0x00,0x03,0xC0,0x07,0xE0
};
static const uint8_t PROGMEM mario_run2_12x14[] = {
  0x0F,0x00,0x1F,0x80,0x1F,0x80,0x36,0xC0,0x1F,0x80,0x3F,0xF0,0x3F,0xF0,0x1F,0x80,
  0x3F,0xF0,0x36,0xC0,0x03,0xC0,0x07,0xE0,0x38,0x00,0x78,0x00
};
static const uint8_t PROGMEM mario_jump_12x14[] = {
  0x0F,0x00,0x1F,0x80,0x1F,0x80,0x36,0xC0,0x5F,0xA0,0xDF,0xB0,0x3F,0xF0,0x1F,0x80,
  0x3F,0xF0,0x36,0xC0,0x60,0x60,0xC0,0x30,0x00,0x00,0x00,0x00
};
static const uint8_t PROGMEM pipe_16x18[] = {
  0xFF,0xFF,0x81,0x01,0xBD,0xBD,0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,0x81,0x01,0x81,0x01,
  0x81,0x01,0x81,0x01,0x81,0x01,0x81,0x01,0x81,0x01,0x81,0x01,0x81,0x01,0xFF,0xFF,
  0x00,0x00,0x00,0x00
};
static const uint8_t PROGMEM goomba_12x10[] = {
  0x1E,0x00,0x3F,0x00,0x7F,0x80,0x6D,0x80,0x7F,0x80,0x3F,0x00,0x3F,0x00,
  0x7F,0x80,0x41,0x80,0x41,0x80
};
static const uint8_t PROGMEM coin_8x8[] = {
  0x3C,0x42,0xBD,0xA5,0xA5,0xBD,0x42,0x3C
};
static const uint8_t PROGMEM cloud_16x8[] = {
  0x0E,0x00,0x1F,0x80,0x3F,0xC0,0x7F,0xE0,
  0x7F,0xE0,0x3F,0xC0,0x1F,0x80,0x0E,0x00
};
static const uint8_t PROGMEM brick_8x8[] = {
  0xFF,0x81,0xBD,0x81,0xBD,0x81,0xFF,0x00
};
static const uint8_t PROGMEM dust_4x4[] = {
  0x60, 0x90, 0x90, 0x60
};

// Car Dodger Bitmaps
static const unsigned char PROGMEM carSprite12x12[] = {
  0x18, 0x00, 0x3c, 0x00, 0x7e, 0x00, 0xff, 0x00,
  0xe7, 0x00, 0xe7, 0x00, 0xff, 0x00, 0xbd, 0x00,
  0xbd, 0x00, 0xff, 0x00, 0x7e, 0x00, 0x3c, 0x00
};
static const unsigned char PROGMEM obstacleCarSprite[] = {
  0x3c, 0x00, 0x7e, 0x00, 0xdb, 0x00, 0xff, 0x00,
  0xe7, 0x00, 0xe7, 0x00, 0xff, 0x00, 0xbd, 0x00,
  0xbd, 0x00, 0xff, 0x00, 0x7e, 0x00, 0x3c, 0x00
};

// Space Invaders Bitmaps (Vertical Orientation)
static const unsigned char PROGMEM playerShipVertical[] = {
  0x10, 0x10, 0x38, 0x38, 0x7c, 0x7c, 0xfe, 0xfe,
  0xfe, 0xfe, 0x7c, 0x7c, 0x38, 0x38, 0x10, 0x10
};
static const unsigned char PROGMEM invader[] = {
  0x1c, 0x00, 0x3e, 0x00, 0x7f, 0x00, 0xdb, 0x80,
  0xff, 0x80, 0x24, 0x00, 0x42, 0x00, 0x81, 0x00
};

// Doodle Jump Bitmaps
static const unsigned char PROGMEM doodlerSprite[] = {
  0x18, 0x3c, 0x7e, 0xff, 0xbd, 0x24, 0x42, 0x81
};
static const unsigned char PROGMEM rocketSprite[] = {
  0x18, 0x3c, 0x3c, 0xff, 0xff, 0xff, 0xdb, 0x99
};

// Pac-Man Bitmaps
static const unsigned char PROGMEM pacman_right[] = {
  0x3c, 0x7e, 0xef, 0xf0, 0xf0, 0xff, 0x7e, 0x3c
};
static const unsigned char PROGMEM pacman_left[] = {
  0x3c, 0x7e, 0xf7, 0x0f, 0x0f, 0xff, 0x7e, 0x3c
};
static const unsigned char PROGMEM pacman_up[] = {
  0x3c, 0x7e, 0xdb, 0xc3, 0xc3, 0xff, 0x7e, 0x3c
};
static const unsigned char PROGMEM pacman_down[] = {
  0x3c, 0x7e, 0xff, 0xc3, 0xc3, 0xdb, 0x7e, 0x3c
};
static const unsigned char PROGMEM ghost_sprite[] = {
  0x3c, 0x7e, 0xdb, 0xff, 0xff, 0xff, 0xff, 0xa5
};
static const unsigned char PROGMEM ghost_scared[] = {
  0x3c, 0x7e, 0xbd, 0xa5, 0xff, 0xbd, 0xff, 0x5a
};

// Frogger Bitmaps
static const unsigned char PROGMEM frog_sprite[] = {
  0x66, 0xff, 0x3c, 0x7e, 0x7e, 0x3c, 0xff, 0x66
};
static const unsigned char PROGMEM car_sprite[] = {
  0x3c, 0x7e, 0xff, 0xdb, 0xff, 0xff, 0xbd, 0x3c
};
static const unsigned char PROGMEM log_sprite[] = {
  0x7f, 0xfe, 0xff, 0xff, 0x80, 0x01, 0x80, 0x01,
  0x80, 0x01, 0x80, 0x01, 0xff, 0xff, 0x7f, 0xfe
};
static const unsigned char PROGMEM turtle_sprite[] = {
  0x3c, 0x7e, 0xdb, 0xff, 0xff, 0xdb, 0x7e, 0x3c
};

// Function declarations
void handlePassword();
int readButton();
void drawMenu();
void handleMenuInput();
void showSelectedGame();
void gameOver(int score);
void waitForRelease();

// Game declarations
void dinoRun();
void snakeGame();
void flappystone();
void ticTacToe();
void pongGame();
void marioMiniGame();
void carDodgerGame();
void tappyTapGame();
void spaceInvaderGame();
void tetrislite();
void brickkillerGame();
void play2048();
void playDoodleJump();
void mazeSolverGame();
void pacmanGame();
void froggerGame();
void helicopterGame();
void setOledBrightness(uint8_t contrast);
void brightnessMenu();
String showVirtualKeyboard(const String& title);
void wifiRepeaterMenu();
void settingsMenu();

// ===================== ARDUINO INITIALIZATION =====================
void setup() {
  Wire.begin(D1, D2);
  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);
  pinMode(OK_BUTTON, INPUT_PULLUP);
  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
  
  // Initialize EEPROM and read the index of the next boot image & saved brightness
  EEPROM.begin(64);
  int selectedBitmap = EEPROM.read(0);
  if (selectedBitmap >= num_boot_bitmaps) {
    selectedBitmap = 0;
  }

  int savedBrightness = EEPROM.read(1);
  if (savedBrightness < 20 || savedBrightness > 255) {
    savedBrightness = 150;
    EEPROM.write(1, savedBrightness);
    EEPROM.commit();
  }
  setOledBrightness(savedBrightness);

  display.drawBitmap(0, 0, boot_bitmaps[selectedBitmap], 128, 64, SSD1306_WHITE);
  display.display();

  // Save the next boot image index (sequential / series-wise)
  EEPROM.write(0, (selectedBitmap + 1) % num_boot_bitmaps);
  EEPROM.commit();
  delay(1000);
}

void loop() {
  if (!unlocked) {
    handlePassword();
    return;
  }

  if (!inGame) {
    drawMenu();
    handleMenuInput();
  } else {
    showSelectedGame();
  }
}

void handlePassword() {
  static int lastRead = HIGH;
  int currentButton = readButton();

  if (currentButton != -1 && lastRead == HIGH) {
    if (currentButton == passwordSequence[passwordIndex]) {
      passwordIndex++;
      if (passwordIndex >= passwordLength) {
        unlocked = true;
        // Silent unlock directly to the game menu
        display.clearDisplay();
        display.display();
        delay(200);
      }
    } else {
      passwordIndex = 0;
    }
  }

  lastRead = (currentButton == -1) ? HIGH : LOW;
}

int readButton() {
  if (digitalRead(UP_BUTTON) == LOW) return UP_BUTTON;
  if (digitalRead(DOWN_BUTTON) == LOW) return DOWN_BUTTON;
  if (digitalRead(OK_BUTTON) == LOW) return OK_BUTTON;
  return -1;
}

void drawMenu() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Select Game:");
  for (int i = 0; i < 6; i++) {
    int index = menuStartIndex + i;
    if (index >= totalGames) break;
    if (index == selectedGame) display.print("> ");
    else display.print("  ");
    display.println(games[index]);
  }
  display.display();
}

void handleMenuInput() {
  if (millis() - lastDebounce > 200) {
    if (digitalRead(UP_BUTTON) == LOW) {
      selectedGame = (selectedGame - 1 + totalGames) % totalGames;
      lastDebounce = millis();
    }
    if (digitalRead(DOWN_BUTTON) == LOW) {
      selectedGame = (selectedGame + 1) % totalGames;
      lastDebounce = millis();
    }
    if (digitalRead(OK_BUTTON) == LOW) {
      inGame = true;
      lastDebounce = millis();
      // IMPORTANT FIX (#1): make sure OK is fully released before we hand
      // control to the game loop, otherwise the very first frame of the
      // game can see OK still LOW and instantly re-trigger an action.
      waitForRelease();
    }

    if (selectedGame < menuStartIndex) menuStartIndex = selectedGame;
    else if (selectedGame >= menuStartIndex + 6) menuStartIndex = selectedGame - 5;
  }
}

void showSelectedGame() {
  switch (selectedGame) {
    case 0: dinoRun(); break;
    case 1: snakeGame(); break;
    case 2: flappystone(); break;
    case 3: ticTacToe(); break;
    case 4: pongGame(); break;
    case 5: marioMiniGame(); break;
    case 6: carDodgerGame(); break;
    case 7: tappyTapGame(); break;
    case 8: spaceInvaderGame(); break;
    case 9: tetrislite(); break;
    case 10: brickkillerGame(); break;
    case 11: play2048(); break;
    case 12: playDoodleJump(); break;
    case 13: mazeSolverGame(); break;
    case 14: pacmanGame(); break;
    case 15: froggerGame(); break;
    case 16: helicopterGame(); break;
    case 17: settingsMenu(); break;
    default: inGame = false; break;
  }
}

// FIX (#1 - core of "game khatam hote hi menu me forward nahi hota"):
// A lot of games were ending right after a button was pressed (jump / shoot /
// tap). If that button was still held down the instant we returned to the
// menu, handleMenuInput() would see it as a fresh press and instantly launch
// the game again - it LOOKED like "menu me wapas nahi ja raha". Waiting here
// for a full release before we hand control back fixes every game in one place.
void waitForRelease() {
  unsigned long start = millis();
  while ((digitalRead(UP_BUTTON) == LOW || digitalRead(DOWN_BUTTON) == LOW ||
          digitalRead(OK_BUTTON) == LOW || digitalRead(LEFT_BUTTON) == LOW ||
          digitalRead(RIGHT_BUTTON) == LOW)) {
    delay(10);
    if (millis() - start > 1500) break; // safety timeout, never hang forever
  }
  delay(60); // small extra debounce
}

void gameOver(int score) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(10, 15);
  display.println("Game Over!");
  display.setTextSize(1);
  display.setCursor(20, 40);
  display.print("Score: ");
  display.print(score);
  display.display();
  delay(1200);
  waitForRelease();     // FIX (#1): guarantees a clean return to the menu
  inGame = false;       // centralised here so every caller behaves the same
}

// ===================== GAME 1: DINO RUN =====================
void dinoRun() {
  float dinoY = 42;
  float velocity = 0;
  
  float gravity = 0.55;
  float jump = -5.8; 
  
  float obsX = SCREEN_WIDTH;
  int obsType = 0; 
  int obsW = 6;
  int obsH = 12;
  int obsY = 42;
  
  int score = 0;
  float speed = 3.0;
  
  float starX[3] = {30, 80, 120};
  int starY[3] = {10, 22, 15};
  float cloudX = 90;
  int cloudY = 8;
  
  bool running = true;
  bool animFrame = false;
  unsigned long animTimer = 0;

  while(digitalRead(OK_BUTTON) == LOW || digitalRead(UP_BUTTON) == LOW) delay(10);

  while (running) {
    bool isJumping = (digitalRead(OK_BUTTON) == LOW || digitalRead(UP_BUTTON) == LOW);
    bool isDucking = (digitalRead(DOWN_BUTTON) == LOW && dinoY >= 42);

    if (isJumping && dinoY >= 42) {
      velocity = jump;
    }

    velocity += gravity;
    dinoY += velocity;

    if (dinoY > 42) {
      dinoY = 42;
      velocity = 0;
    }

    if (millis() - animTimer > 100) {
      animFrame = !animFrame;
      animTimer = millis();
    }

    speed = 3.0 + (score / 12.0);
    if (speed > 7.5) speed = 7.5;

    obsX -= speed;
    if (obsX < -16) {
      obsX = SCREEN_WIDTH + random(30, 70); 
      obsType = random(0, 4);
      if (obsType == 0) { obsW = 6; obsH = 12; obsY = 42; }
      else if (obsType == 1) { obsW = 12; obsH = 12; obsY = 42; }
      else if (obsType == 2) { obsW = 8; obsH = 8; obsY = 28; } 
      else { obsW = 8; obsH = 8; obsY = 44; } 
      score++;
    }

    for (int i = 0; i < 3; i++) {
      starX[i] -= speed * 0.3;
      if (starX[i] < 0) { starX[i] = SCREEN_WIDTH; starY[i] = random(5, 25); }
    }
    cloudX -= speed * 0.5;
    if (cloudX < -16) { cloudX = SCREEN_WIDTH; cloudY = random(4, 12); }

    int dinoW = isDucking ? 16 : 12;
    int dinoH = isDucking ? 8 : 12;
    int dinoCurY = isDucking ? 46 : (int)dinoY;

    int ax1 = 10, ay1 = dinoCurY, ax2 = 10 + dinoW, ay2 = dinoCurY + dinoH;
    int bx1 = (int)obsX, by1 = obsY, bx2 = (int)obsX + obsW, by2 = obsY + obsH;

    if (!(ax2 - 2 <= bx1 || ax1 + 2 >= bx2 || ay2 - 1 <= by1 || ay1 + 1 >= by2)) {
      running = false;
    }

    display.clearDisplay();

    for (int i = 0; i < 3; i++) display.drawPixel((int)starX[i], starY[i], SSD1306_WHITE);
    display.drawBitmap((int)cloudX, cloudY, cloud_16x8, 16, 8, SSD1306_WHITE);
    display.drawFastHLine(0, 54, SCREEN_WIDTH, SSD1306_WHITE);

    if (obsType == 0) display.drawBitmap((int)obsX, obsY, cactus_small, 6, 12, SSD1306_WHITE);
    else if (obsType == 1) display.drawBitmap((int)obsX, obsY, cactus_double, 12, 12, SSD1306_WHITE);
    else display.drawBitmap((int)obsX, obsY, animFrame ? bird_wingup : bird_wingdown, 8, 8, SSD1306_WHITE);

    const unsigned char* dinoBmp = isDucking ? (animFrame ? dino_duck1 : dino_duck2) : (animFrame ? dino_run1 : dino_run2);
    display.drawBitmap(10, dinoCurY, dinoBmp, dinoW, dinoH, SSD1306_WHITE);



    display.display();
    delay(15); 
  }

  gameOver(score);
  inGame = false;
}

// ===================== GAME 2: SNAKE =====================
void snakeGame() {
  const int cellSize = 4;
  const int gridWidth = 30;
  const int gridHeight = 13;
  const int offsetX = 4;
  const int offsetY = 10;

  struct Point { int x, y; };
  Point snake[100];
  int snakeLength = 3;
  snake[0] = {gridWidth / 2, gridHeight / 2};
  snake[1] = {gridWidth / 2 - 1, gridHeight / 2};
  snake[2] = {gridWidth / 2 - 2, gridHeight / 2};

  int dx = 1, dy = 0;
  bool running = true;
  int score = 0;

  Point food;
  auto respawnFood = [&]() {
    bool onBody = true;
    while (onBody) {
      food = { (int)random(0, gridWidth), (int)random(0, gridHeight) };
      onBody = false;
      for (int i = 0; i < snakeLength; i++) {
        if (snake[i].x == food.x && snake[i].y == food.y) { onBody = true; break; }
      }
    }
  };
  respawnFood();

  while (running) {
    if (digitalRead(UP_BUTTON) == LOW && dy != 1) { dx = 0; dy = -1; }
    else if (digitalRead(DOWN_BUTTON) == LOW && dy != -1) { dx = 0; dy = 1; }
    else if (digitalRead(LEFT_BUTTON) == LOW && dx != 1) { dx = -1; dy = 0; }
    else if (digitalRead(RIGHT_BUTTON) == LOW && dx != -1) { dx = 1; dy = 0; }

    Point newHead = { (snake[0].x + dx + gridWidth) % gridWidth, (snake[0].y + dy + gridHeight) % gridHeight };

    for (int i = 0; i < snakeLength; i++) {
      if (snake[i].x == newHead.x && snake[i].y == newHead.y) {
        running = false;
        break;
      }
    }

    for (int i = snakeLength; i > 0; i--) {
      snake[i] = snake[i - 1];
    }
    snake[0] = newHead;

    if (newHead.x == food.x && newHead.y == food.y) {
      snakeLength = min(snakeLength + 1, 99);
      score++;
      respawnFood();
    }

    display.clearDisplay();
    display.drawRect(offsetX - 2, offsetY - 2, gridWidth * cellSize + 4, gridHeight * cellSize + 4, SSD1306_WHITE);
    display.fillRect(food.x * cellSize + offsetX, food.y * cellSize + offsetY, cellSize, cellSize, SSD1306_WHITE);

    for (int i = 0; i < snakeLength; i++) {
      display.fillRect(snake[i].x * cellSize + offsetX, snake[i].y * cellSize + offsetY, cellSize, cellSize, SSD1306_WHITE);
    }


    display.display();

    delay(120 - min(score * 3, 80));
  }

  gameOver(score);
  inGame = false;
}

// ===================== GAME 3: FLAPPY STONE =====================
void flappystone() {
  const int birdX = 20;
  float birdY = 30;
  float birdVelocity = 0;
  float gravity = 0.2;
  float jumpStrength = -2.5;

  const int pipeWidth = 16;
  float pipeX = SCREEN_WIDTH;
  int pipeGapY = random(10, 32);
  const int gapHeight = 28; // Increased from 22 for easier gameplay

  int score = 0;
  bool playing = true;
  bool lastOK = false;

  while (playing) {
    bool currentOK = (digitalRead(OK_BUTTON) == LOW);
    if (currentOK && !lastOK) {
      birdVelocity = jumpStrength;
    }
    lastOK = currentOK;

    birdVelocity += gravity;
    birdY += birdVelocity;

    if (birdY < 0) { birdY = 0; birdVelocity = 0; }
    if (birdY > SCREEN_HEIGHT - 8) break;

    pipeX -= 1.5; // Smooth sub-pixel movement
    if (pipeX + pipeWidth < 0) {
      pipeX = SCREEN_WIDTH;
      pipeGapY = random(10, 32);
      score++;
    }

    if (pipeX < birdX + 8 && pipeX + pipeWidth > birdX) {
      if (birdY < pipeGapY || birdY + 8 > pipeGapY + gapHeight) {
        break;
      }
    }

    display.clearDisplay();
    display.drawFastHLine(0, SCREEN_HEIGHT - 1, SCREEN_WIDTH, SSD1306_WHITE);

    display.fillRect((int)pipeX + 2, 0, pipeWidth - 4, pipeGapY - 4, SSD1306_WHITE);
    display.fillRect((int)pipeX, pipeGapY - 4, pipeWidth, 4, SSD1306_WHITE);
    display.fillRect((int)pipeX, pipeGapY + gapHeight, pipeWidth, 4, SSD1306_WHITE);
    display.fillRect((int)pipeX + 2, pipeGapY + gapHeight + 4, pipeWidth - 4, SCREEN_HEIGHT - (pipeGapY + gapHeight + 4), SSD1306_WHITE);

    display.drawBitmap(birdX, (int)birdY, flappy_bird_sprite, 8, 8, SSD1306_WHITE);



    display.display();
    delay(15); // Faster loop = much smoother frame rate
  }

  gameOver(score);
  inGame = false;
}

void ticTacToe() {
  char board[3][3];
  memset(board, ' ', sizeof(board));
  int cursorX = 0, cursorY = 0;
  bool isXturn = true;
  bool playing = true;
  int playMode = 0; 
  
  display.clearDisplay();
  display.setCursor(15, 10);
  display.print("Select Mode:");
  display.setCursor(15, 30);
  display.print("> 1 Player (AI)");
  display.setCursor(15, 45);
  display.print("  2 Players");
  display.display();

  delay(200);
  while(true) {
    if (digitalRead(UP_BUTTON) == LOW || digitalRead(DOWN_BUTTON) == LOW) {
      playMode = 1 - playMode;
      display.clearDisplay();
      display.setCursor(15, 10);
      display.print("Select Mode:");
      display.setCursor(15, 30);
      display.print(playMode == 0 ? "> 1 Player (AI)" : "  1 Player (AI)");
      display.setCursor(15, 45);
      display.print(playMode == 1 ? "> 2 Players" : "  2 Players");
      display.display();
      delay(200);
    }
    if (digitalRead(OK_BUTTON) == LOW) {
      delay(200);
      break;
    }
  }

  auto getWinner = [&]() {
    for (int i = 0; i < 3; i++) {
      if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2]) return board[i][0];
      if (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i]) return board[0][i];
    }
    if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2]) return board[0][0];
    if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0]) return board[0][2];
    return ' ';
  };

  auto isFull = [&]() {
    for (int r=0; r<3; r++)
      for (int c=0; c<3; c++)
        if (board[r][c] == ' ') return false;
    return true;
  };

  auto makeAIMove = [&]() {
    for (int r=0; r<3; r++) {
      for (int c=0; c<3; c++) {
        if (board[r][c] == ' ') {
          board[r][c] = 'O';
          if (getWinner() == 'O') return;
          board[r][c] = ' ';
        }
      }
    }
    for (int r=0; r<3; r++) {
      for (int c=0; c<3; c++) {
        if (board[r][c] == ' ') {
          board[r][c] = 'X';
          if (getWinner() == 'X') {
            board[r][c] = 'O';
            return;
          }
          board[r][c] = ' ';
        }
      }
    }
    if (board[1][1] == ' ') { board[1][1] = 'O'; return; }
    while (true) {
      int r = random(0, 3);
      int c = random(0, 3);
      if (board[r][c] == ' ') { board[r][c] = 'O'; return; }
    }
  };

  while (playing) {
    display.clearDisplay();

    display.drawLine(42, 0, 42, 64, SSD1306_WHITE);
    display.drawLine(85, 0, 85, 64, SSD1306_WHITE);
    display.drawLine(0, 21, 128, 21, SSD1306_WHITE);
    display.drawLine(0, 42, 128, 42, SSD1306_WHITE);

    for (int r = 0; r < 3; r++) {
      for (int c = 0; c < 3; c++) {
        int px = c * 42 + 21;
        int py = r * 21 + 10;
        if (board[r][c] == 'X') {
          display.drawLine(px-8, py-6, px+8, py+6, SSD1306_WHITE);
          display.drawLine(px-8, py+6, px+8, py-6, SSD1306_WHITE);
        } else if (board[r][c] == 'O') {
          display.drawCircle(px, py, 7, SSD1306_WHITE);
        }
      }
    }

    display.drawRect(cursorX * 42 + 2, cursorY * 21 + 2, 38, 17, SSD1306_WHITE);
    display.display();

    if (!isXturn && playMode == 0) {
      delay(400);
      makeAIMove();
      char winner = getWinner();
      if (winner != ' ') {
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(20, 20);
        display.print("AI Wins!");
        display.display();
        delay(2000);
        playing = false;
      } else if (isFull()) {
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(20, 20);
        display.print("Draw!");
        display.display();
        delay(2000);
        playing = false;
      }
      isXturn = true;
      continue;
    }

    if (digitalRead(LEFT_BUTTON) == LOW && cursorX > 0) { cursorX--; delay(150); }
    if (digitalRead(RIGHT_BUTTON) == LOW && cursorX < 2) { cursorX++; delay(150); }
    if (digitalRead(UP_BUTTON) == LOW && cursorY > 0) { cursorY--; delay(150); }
    if (digitalRead(DOWN_BUTTON) == LOW && cursorY < 2) { cursorY++; delay(150); }
    
    if (digitalRead(OK_BUTTON) == LOW && board[cursorY][cursorX] == ' ') {
      board[cursorY][cursorX] = isXturn ? 'X' : 'O';
      char winner = getWinner();
      if (winner != ' ') {
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(20, 20);
        display.print(winner); display.print(" Wins!");
        display.display();
        delay(2000);
        playing = false;
      } else if (isFull()) {
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(20, 20);
        display.print("Draw!");
        display.display();
        delay(2000);
        playing = false;
      }
      isXturn = !isXturn;
      delay(200);
    }
  }

  waitForRelease();
  inGame = false;
}

// ===================== GAME 5: PONG =====================
void pongGame() {
  int mode = 0;
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(15, 10);
  display.print("Select Mode:");
  display.setCursor(15, 30);
  display.print("> Play with AI");
  display.setCursor(15, 45);
  display.print("  Play with Friend");
  display.display();

  delay(200);
  while(true) {
    if (digitalRead(UP_BUTTON) == LOW || digitalRead(DOWN_BUTTON) == LOW) {
      mode = 1 - mode;
      display.clearDisplay();
      display.setCursor(15, 10);
      display.print("Select Mode:");
      display.setCursor(15, 30);
      display.print(mode == 0 ? "> Play with AI" : "  Play with AI");
      display.setCursor(15, 45);
      display.print(mode == 1 ? "> Play with Friend" : "  Play with Friend");
      display.display();
      delay(200);
    }
    if (digitalRead(OK_BUTTON) == LOW) {
      delay(200);
      break;
    }
  }

  int paddleHeight = 16;
  int paddleWidth = 3;
  int ballSize = 3;
  
  float leftPaddleY = SCREEN_HEIGHT / 2 - paddleHeight / 2;
  float rightPaddleY = SCREEN_HEIGHT / 2 - paddleHeight / 2;
  
  float ballX = SCREEN_WIDTH / 2;
  float ballY = SCREEN_HEIGHT / 2;
  
  float ballSpeedX = (random(0, 2) == 0) ? 1.6 : -1.6;
  float ballSpeedY = random(-10, 11) / 10.0;
  
  int p1Score = 0;
  int p2Score = 0;
  bool playing = true;

  int starX[6];
  int starY[6];
  for (int i=0; i<6; i++) {
    starX[i] = random(0, SCREEN_WIDTH);
    starY[i] = random(0, SCREEN_HEIGHT);
  }

  auto resetBall = [&]() {
    ballX = SCREEN_WIDTH / 2;
    ballY = SCREEN_HEIGHT / 2;
    ballSpeedX = (random(0, 2) == 0) ? 1.6 : -1.6;
    ballSpeedY = random(-10, 11) / 10.0;
    delay(500);
  };

  while (playing) {
    if (digitalRead(UP_BUTTON) == LOW && leftPaddleY > 0) leftPaddleY -= 2.2;
    if (digitalRead(DOWN_BUTTON) == LOW && leftPaddleY + paddleHeight < SCREEN_HEIGHT) leftPaddleY += 2.2;

    if (mode == 1) {
      if (digitalRead(LEFT_BUTTON) == LOW && rightPaddleY > 0) rightPaddleY -= 2.2;
      if (digitalRead(RIGHT_BUTTON) == LOW && rightPaddleY + paddleHeight < SCREEN_HEIGHT) rightPaddleY += 2.2;
    } else {
      // Smart imperfect AI
      if (random(0, 100) < 85) { 
        float diff = ballY - (rightPaddleY + paddleHeight / 2.0);
        float speed = max(1.0f, abs(ballSpeedX) * 0.65f);
        if (diff > 2 && rightPaddleY + paddleHeight < SCREEN_HEIGHT) rightPaddleY += speed;
        else if (diff < -2 && rightPaddleY > 0) rightPaddleY -= speed;
      }
    }

    ballX += ballSpeedX;
    ballY += ballSpeedY;

    if (ballY <= 0) {
      ballY = 0;
      ballSpeedY = -ballSpeedY + (random(-10, 11) / 100.0);
    } else if (ballY + ballSize >= SCREEN_HEIGHT) {
      ballY = SCREEN_HEIGHT - ballSize;
      ballSpeedY = -ballSpeedY + (random(-10, 11) / 100.0);
    }

    if (ballSpeedX < 0 && ballX <= paddleWidth && ballX >= 0) {
      if (ballY + ballSize >= leftPaddleY && ballY <= leftPaddleY + paddleHeight) {
        ballX = paddleWidth;
        ballSpeedX = -ballSpeedX;
        if (ballSpeedX < 3.2) ballSpeedX *= 1.05;

        float hitPos = (ballY + ballSize / 2.0) - (leftPaddleY + paddleHeight / 2.0);
        ballSpeedY = (hitPos / (paddleHeight / 2.0)) * 1.8 + (random(-20, 21) / 100.0);
      }
    }

    if (ballSpeedX > 0 && ballX + ballSize >= SCREEN_WIDTH - paddleWidth && ballX + ballSize <= SCREEN_WIDTH) {
      if (ballY + ballSize >= rightPaddleY && ballY <= rightPaddleY + paddleHeight) {
        ballX = SCREEN_WIDTH - paddleWidth - ballSize;
        ballSpeedX = -ballSpeedX;
        if (abs(ballSpeedX) < 3.2) ballSpeedX *= 1.05;

        float hitPos = (ballY + ballSize / 2.0) - (rightPaddleY + paddleHeight / 2.0);
        ballSpeedY = (hitPos / (paddleHeight / 2.0)) * 1.8 + (random(-20, 21) / 100.0);
      }
    }

    if (ballX < 0) {
      p2Score++;
      if (p2Score >= 3) playing = false;
      else resetBall();
    } else if (ballX > SCREEN_WIDTH) {
      p1Score++;
      if (p1Score >= 3) playing = false;
      else resetBall();
    }

    for (int i=0; i<6; i++) {
      starX[i] -= 1;
      if (starX[i] < 0) { starX[i] = SCREEN_WIDTH; starY[i] = random(0, SCREEN_HEIGHT); }
    }

    display.clearDisplay();

    for (int i=0; i<6; i++) display.drawPixel(starX[i], starY[i], SSD1306_WHITE);

    display.fillRect(0, (int)leftPaddleY, paddleWidth, paddleHeight, SSD1306_WHITE);
    display.fillRect(SCREEN_WIDTH - paddleWidth, (int)rightPaddleY, paddleWidth, paddleHeight, SSD1306_WHITE);
    display.fillRect((int)ballX, (int)ballY, ballSize, ballSize, SSD1306_WHITE);
    


    display.display();
    delay(15); 
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(15, 10);
  if (p1Score >= 3) display.print("P1 Wins!");
  else display.print("P2 Wins!");
  display.setTextSize(1);
  display.setCursor(20, 42);
  display.print("Final Score: "); display.print(p1Score); display.print(" - "); display.print(p2Score);
  display.display();
  delay(1500);
  waitForRelease();
  
  inGame = false;
}

// ===================== GAME 6: SUPER MARIO (MINI) =====================
// FIX (#2): players couldn't tell if Mario was walking or standing still.
// Rebuilt with the same core rules the original game uses:
//  - acceleration/friction instead of an instant fixed speed (so Mario
//    visibly speeds up and slides to a stop, like the real game)
//  - variable-height jump: holding the button longer while rising reduces
//    gravity, exactly like Super Mario Bros' jump curve
//  - the walk-cycle frame now advances based on DISTANCE travelled, not a
//    fixed timer, so the legs always visibly swap while Mario is moving and
//    freeze solid the instant he stops - unmistakably different from idle
//  - small dust puffs kick up from the feet while running, and a short
//    squash frame plays on landing, both classic platformer "juice" cues
void marioMiniGame() {
  const int GROUND_Y = 54;
  int camX = 0;
  int score = 0;
  int coins = 0;

  float marioX = 30;
  float marioY = GROUND_Y - 14;
  float marioVX = 0;
  float marioVY = 0;

  const float ACCEL = 0.35;      // ground acceleration
  const float FRICTION = 0.28;   // ground friction when no input
  const float MAX_SPEED = 2.4;
  const float GRAVITY_FULL = 0.42;
  const float GRAVITY_HELD = 0.20; // lighter gravity while rising + button held (variable jump)
  const float JUMP_FORCE = -6.4;

  bool onGround = true;
  bool faceRight = true;
  bool wasOnGround = true;
  unsigned long landSquashUntil = 0;

  float distanceWalked = 0;   // drives the walk-cycle, not millis()
  int walkFrame = 0;

  struct Dust { float x, y; int life; bool active; };
  const int maxDust = 6;
  Dust dust[maxDust];
  for (int i = 0; i < maxDust; i++) dust[i].active = false;
  auto spawnDust = [&](float x, float y) {
    for (int i = 0; i < maxDust; i++) {
      if (!dust[i].active) { dust[i] = {x, y, 8, true}; return; }
    }
  };

  struct Entity { float x, y; int w, h, type; bool active; };
  const int maxEntities = 6;
  Entity entities[maxEntities];

  auto spawnEntity = [&](int idx, int worldX) {
    int t = random(0, 3);
    entities[idx].type = t;
    entities[idx].active = true;
    entities[idx].x = worldX;
    if (t == 0) { entities[idx].w = 16; entities[idx].h = 16; entities[idx].y = GROUND_Y - 16; }
    else if (t == 1) { entities[idx].w = 12; entities[idx].h = 10; entities[idx].y = GROUND_Y - 10; }
    else { entities[idx].w = 8; entities[idx].h = 8; entities[idx].y = GROUND_Y - 24 - random(0, 8); }
  };

  for (int i = 0; i < maxEntities; i++) spawnEntity(i, 140 + i * 70);

  bool playing = true;

  auto overlap = [](float ax, float ay, int aw, int ah, float bx, float by, int bw, int bh) {
    return !(ax + aw <= bx || bx + bw <= ax || ay + ah <= by || by + bh <= ay);
  };

  while (digitalRead(OK_BUTTON) == LOW) delay(10);

  while (playing) {
    bool left = (digitalRead(LEFT_BUTTON) == LOW);
    bool right = (digitalRead(RIGHT_BUTTON) == LOW);
    bool jumpHeld = (digitalRead(OK_BUTTON) == LOW);

    // --- Horizontal physics: accelerate, then friction when released ---
    if (left) { marioVX -= ACCEL; faceRight = false; }
    else if (right) { marioVX += ACCEL; faceRight = true; }
    else {
      if (marioVX > 0) marioVX = max(0.0f, marioVX - FRICTION);
      else if (marioVX < 0) marioVX = min(0.0f, marioVX + FRICTION);
    }
    marioVX = constrain(marioVX, -MAX_SPEED, MAX_SPEED);

    float prevWorldX = marioX + camX;
    marioX += marioVX;
    if (marioX < 10) { marioX = 10; }
    if (marioX > 60) { camX += (marioX - 60); marioX = 60; if (camX < 0) camX = 0; }
    float worldX = marioX + camX;
    distanceWalked += fabs(worldX - prevWorldX);

    // --- Variable-height jump: still holding OK while moving upward = float longer ---
    if (jumpHeld && onGround) {
      marioVY = JUMP_FORCE;
      onGround = false;
    }
    float g = (marioVY < 0 && jumpHeld) ? GRAVITY_HELD : GRAVITY_FULL;
    marioVY += g;
    marioY += marioVY;

    // --- Ground / brick collisions ---
    int firstBrickX = (camX / 80) * 80;
    wasOnGround = onGround;
    onGround = false;
    for (int bx = firstBrickX - 80; bx < firstBrickX + 240; bx += 80) {
      int sx = bx - camX;
      if (marioVY > 0 && overlap(marioX, marioY, 12, 14, sx, GROUND_Y - 24, 8, 8)) {
        marioY = GROUND_Y - 24 - 14;
        marioVY = 0;
        onGround = true;
      }
      if (marioVY < 0 && overlap(marioX, marioY, 12, 14, sx, GROUND_Y - 24, 8, 8)) {
        marioVY = 0;
        score += 10;
      }
    }
    if (marioY + 14 >= GROUND_Y) {
      marioY = GROUND_Y - 14;
      marioVY = 0;
      onGround = true;
    }
    if (onGround && !wasOnGround) {
      landSquashUntil = millis() + 90; // brief squash frame on landing
      spawnDust(marioX + 2, marioY + 14);
      spawnDust(marioX + 8, marioY + 14);
    }

    // --- Running dust puffs (only while actually moving on ground) ---
    static unsigned long dustTimer = 0;
    if (onGround && fabs(marioVX) > 1.2 && millis() - dustTimer > 90) {
      dustTimer = millis();
      spawnDust(marioX + (faceRight ? 0 : 10), marioY + 13);
    }
    for (int i = 0; i < maxDust; i++) {
      if (dust[i].active) { dust[i].y += 0.3; dust[i].life--; if (dust[i].life <= 0) dust[i].active = false; }
    }

    // --- Entities (pipes / goombas / coins) ---
    for (int i = 0; i < maxEntities; i++) {
      if (!entities[i].active) continue;
      if (entities[i].type == 1) entities[i].x -= 0.8;

      int sx = entities[i].x - camX;
      if (sx < -30) { spawnEntity(i, camX + SCREEN_WIDTH + random(20, 60)); continue; }

      if (overlap(marioX, marioY, 12, 14, sx, entities[i].y, entities[i].w, entities[i].h)) {
        if (entities[i].type == 1) {
          if (marioVY > 0 && marioY + 14 - entities[i].y <= 6) {
            marioVY = JUMP_FORCE * 0.5;
            entities[i].active = false;
            score += 100;
          } else {
            playing = false;
          }
        } else if (entities[i].type == 2) {
          entities[i].active = false;
          score += 50;
          coins++;
        } else {
          playing = false;
        }
      }
    }

    // --- Walk-cycle frame driven by DISTANCE, not time (the actual fix) ---
    int newFrame = ((int)(distanceWalked / 5)) % 2;
    walkFrame = newFrame;

    display.clearDisplay();
    display.drawFastHLine(0, GROUND_Y, SCREEN_WIDTH, SSD1306_WHITE);

    for (int bx = firstBrickX - 80; bx < firstBrickX + 240; bx += 80) {
      int sx = bx - camX;
      if (sx > -8 && sx < SCREEN_WIDTH) display.drawBitmap(sx, GROUND_Y - 24, brick_8x8, 8, 8, SSD1306_WHITE);
    }

    for (int i = 0; i < maxEntities; i++) {
      if (!entities[i].active) continue;
      int sx = entities[i].x - camX;
      if (sx >= -16 && sx < SCREEN_WIDTH) {
        if (entities[i].type == 0) display.drawBitmap(sx, entities[i].y, pipe_16x18, 16, 18, SSD1306_WHITE);
        else if (entities[i].type == 1) display.drawBitmap(sx, entities[i].y, goomba_12x10, 12, 10, SSD1306_WHITE);
        else display.drawBitmap(sx, entities[i].y, coin_8x8, 8, 8, SSD1306_WHITE);
      }
    }

    for (int i = 0; i < maxDust; i++) {
      if (dust[i].active) display.drawBitmap((int)dust[i].x, (int)dust[i].y, dust_4x4, 4, 4, SSD1306_WHITE);
    }

    const uint8_t* marioBmp;
    bool squashing = millis() < landSquashUntil;
    if (!onGround) marioBmp = mario_jump_12x14;
    else if (squashing) marioBmp = mario_stand_12x14; // brief stop-frame on landing
    else if (fabs(marioVX) > 0.3) marioBmp = (walkFrame == 0) ? mario_run1_12x14 : mario_run2_12x14;
    else marioBmp = mario_stand_12x14;

    int drawY = (int)marioY + (squashing ? 1 : 0);
    if (faceRight) {
      display.drawBitmap((int)marioX, drawY, marioBmp, 12, 14, SSD1306_WHITE);
    } else {
      // simple horizontal mirror so facing left is visually distinct too
      for (int yy = 0; yy < 14; yy++) {
        for (int xx = 0; xx < 12; xx++) {
          int byteIdx = yy * 2 + (xx / 8);
          uint8_t rowByte = pgm_read_byte(&marioBmp[byteIdx]);
          bool bit = (rowByte >> (7 - (xx % 8))) & 1;
          if (bit) display.drawPixel((int)marioX + (11 - xx), drawY + yy, SSD1306_WHITE);
        }
      }
    }

    display.setCursor(2, 2);
    display.setTextSize(1);
    display.print("Coins:"); display.print(coins);

    display.display();
    delay(18);
  }

  gameOver(score);
  inGame = false;
}

// ===================== GAME 7: CAR DODGER =====================
void carDodgerGame() {
  const int ROAD_LEFT = 2;
  const int ROAD_RIGHT = 126;
  const int CAR_W = 12, CAR_H = 12;

  float carX = (SCREEN_WIDTH - CAR_W) / 2.0;
  const float carY = SCREEN_HEIGHT - CAR_H - 2;

  struct Enemy { float x, y, speed; int type; bool active; }; 
  const int maxEnemies = 2;
  Enemy enemies[maxEnemies];
  
  auto spawnEnemy = [&](int idx, float startY) {
    enemies[idx].active = true;
    enemies[idx].type = random(0, 3);
    int w = (enemies[idx].type == 2) ? 14 : 12;
    enemies[idx].x = random(ROAD_LEFT + 2, ROAD_RIGHT - w - 2);
    enemies[idx].y = startY;
    enemies[idx].speed = 1.5 + (random(0, 10) / 10.0);
  };

  spawnEnemy(0, -30);
  spawnEnemy(1, -70);

  int score = 0;
  float roadOffset = 0;
  bool running = true;

  while(running) {
    if (digitalRead(LEFT_BUTTON) == LOW) carX -= 2.2;
    if (digitalRead(RIGHT_BUTTON) == LOW) carX += 2.2;

    carX = constrain(carX, ROAD_LEFT + 2, ROAD_RIGHT - CAR_W - 2);

    roadOffset = fmod(roadOffset + 1.8, 12.0);

    for (int i=0; i<maxEnemies; i++) {
      enemies[i].y += enemies[i].speed;
      if (enemies[i].y > SCREEN_HEIGHT) {
        score++;
        spawnEnemy(i, -random(20, 60));
        enemies[i].speed = 1.5 + (score / 16.0) + (random(0, 10) / 10.0);
        if (enemies[i].speed > 4.5) enemies[i].speed = 4.5;
      }

      int ew = (enemies[i].type == 2) ? 14 : 12;
      int eh = (enemies[i].type == 2) ? 16 : 12;

      if (!(carX + CAR_W <= enemies[i].x || enemies[i].x + ew <= carX ||
            carY + CAR_H <= enemies[i].y || enemies[i].y + eh <= carY)) {
        running = false;
      }
    }

    display.clearDisplay();

    display.drawFastVLine(ROAD_LEFT, 0, SCREEN_HEIGHT, SSD1306_WHITE);
    display.drawFastVLine(ROAD_RIGHT, 0, SCREEN_HEIGHT, SSD1306_WHITE);
    
    for (int y = -roadOffset; y < SCREEN_HEIGHT; y += 12) {
      display.drawFastVLine(43, y, 6, SSD1306_WHITE);
      display.drawFastVLine(85, y, 6, SSD1306_WHITE);
    }

    for (int i=0; i<maxEnemies; i++) {
      if (enemies[i].y >= -16) {
        if (enemies[i].type == 2) {
          display.fillRect((int)enemies[i].x, (int)enemies[i].y, 14, 16, SSD1306_WHITE);
        } else {
          display.drawBitmap((int)enemies[i].x, (int)enemies[i].y, obstacleCarSprite, CAR_W, CAR_H, SSD1306_WHITE);
        }
      }
    }

    display.drawBitmap((int)carX, (int)carY, carSprite12x12, CAR_W, CAR_H, SSD1306_WHITE);

    display.display();
    delay(15);
  }

  gameOver(score);
  inGame = false;
}

// ===================== GAME 8: TAPPY TAP (Tower Stack) =====================
// FIX (#3): the old version wasn't really behaving like the classic
// "Stack" game it was going for - blocks just piled straight up until they
// ran off the top of the screen, there was no reward for a clean stack, and
// a mis-drop that clipped a single pixel behaved identically to a perfect
// one. Rewritten with the real Stack rules:
//   - the camera/tower view scrolls down as you build higher, so the tower
//     never runs out of screen (the actual game does this too)
//   - a "PERFECT" drop (edges line up within a small tolerance) keeps the
//     block at full width and pays a combo bonus instead of always shrinking
//   - the sliced-off overhang visibly breaks away and falls off screen
//   - speed ramps up with height, and a running combo counter is shown
void tappyTapGame() {
  const int rowHeight = 6;
  const int maxRows = 200; // logical tower height, way beyond one screen

  int stackedX[maxRows];
  int stackedWidth[maxRows];

  int score = 0;
  int combo = 0;
  int currentRow = 0;

  float blockX = 10;
  int blockWidth = 40;
  float blockSpeed = 1.6;
  int dir = 1;

  // camera offset in rows: how many completed rows have scrolled off
  int viewOffset = 0;
  const int visibleRows = 9; // how many rows fit on screen at once

  bool playing = true;
  bool released = false;

  struct Chip { float x, y, w; float vy; bool active; };
  Chip chip = {0, 0, 0, 0, false};

  while (digitalRead(OK_BUTTON) == LOW) delay(10);

  while (playing) {
    blockX += dir * blockSpeed;
    if (blockX <= 4) { blockX = 4; dir = 1; }
    else if (blockX + blockWidth >= 124) { blockX = 124 - blockWidth; dir = -1; }

    bool okPressed = (digitalRead(OK_BUTTON) == LOW);
    if (okPressed && released) {
      released = false;
      int bx = (int)blockX;

      if (currentRow == 0) {
        stackedX[0] = bx;
        stackedWidth[0] = blockWidth;
        currentRow++;
        score += 10;
        blockX = 10;
        blockSpeed = 1.6;
      } else {
        int belowX = stackedX[currentRow - 1];
        int belowW = stackedWidth[currentRow - 1];

        int left = max(bx, belowX);
        int right = min(bx + blockWidth, belowX + belowW);
        int newW = right - left;

        if (newW <= 0) {
          playing = false; // total miss
        } else {
          const int PERFECT_TOLERANCE = 2; // px - "official" Stack-style snap
          bool perfect = (abs(bx - belowX) <= PERFECT_TOLERANCE);

          if (perfect) {
            // Perfect drop: snap to the row below, no shrink, combo bonus
            left = belowX;
            newW = belowW;
            combo++;
            score += 20 + combo * 5;
          } else {
            // Normal drop: crop to the overlap and drop the sliced-off chip
            combo = 0;
            score += 10;
            int cutW = blockWidth - newW;
            if (cutW > 0) {
              float cutX = (bx < belowX) ? bx : (left + newW);
              chip = {cutX, (float)(54 - (currentRow - viewOffset) * rowHeight), (float)cutW, 0.4f, true};
            }
          }

          stackedX[currentRow] = left;
          stackedWidth[currentRow] = newW;
          blockWidth = newW;

          if (currentRow < maxRows - 1) currentRow++;

          // scroll the camera once the tower grows past the visible area
          if (currentRow - viewOffset >= visibleRows) {
            viewOffset = currentRow - visibleRows + 1;
          }

          blockX = random(4, max(5, 120 - blockWidth));
          blockSpeed = 1.6 + (currentRow * 0.08);
          if (blockSpeed > 4.2) blockSpeed = 4.2;
        }
      }
    }
    if (!okPressed) released = true;

    if (chip.active) {
      chip.y += chip.vy;
      chip.vy += 0.5;
      if (chip.y > 64) chip.active = false;
    }

    display.clearDisplay();
    display.drawRect(2, 2, 124, 60, SSD1306_WHITE);

    for (int i = viewOffset; i < currentRow; i++) {
      int ry = 54 - (i - viewOffset) * rowHeight;
      if (ry < 4) continue;
      display.fillRect(stackedX[i], ry, stackedWidth[i], rowHeight - 1, SSD1306_WHITE);
    }

    int activeY = 54 - (currentRow - viewOffset) * rowHeight;
    if (activeY > 4) {
      display.fillRect((int)blockX, activeY, blockWidth, rowHeight - 1, SSD1306_WHITE);
    }

    if (chip.active) {
      display.fillRect((int)chip.x, (int)chip.y, (int)chip.w, rowHeight - 1, SSD1306_WHITE);
    }

    display.setCursor(4, 2);
    display.print("H:"); display.print(currentRow);
    if (combo > 1) {
      display.setCursor(70, 2);
      display.print("Combo x"); display.print(combo);
    }

    display.display();
    delay(15);
  }

  gameOver(score);
  inGame = false;
}

// ===================== GAME 9: SPACE INVADERS =====================
// FIX (#4): the Scout enemy's sine-wave path called the floating point
// sin() function every single frame for every Scout on screen. On this
// hardware that's the single biggest cause of the frame lag - sin() is
// comparatively expensive on an 8-bit-ish MCU without an FPU. Replaced with
// a small precomputed lookup table (fastSinY) so the wave motion is now a
// plain array read instead of a trig call, with no visible gameplay change.
static const int8_t PROGMEM sinTableX100[240] = {
  0, 17, 33, 48, 62, 74, 84, 92, 97, 100, 100, 97, 91, 83, 72, 60, 46, 30, 14, -3, -19, -35, -50, -64, -76, -85, -93, -98, -100, -99, -96, -90, -81, -71, -58, -43, -28, -12, 5, 22, 37, 52, 66, 77, 87, 94, 98, 100, 99, 95, 89, 80, 69, 56, 41, 26, 9, -8, -24, -40, -54, -68, -79, -88, -95, -99, -100, -99, -94, -88, -78, -67, -54, -39, -23, -7, 10, 26, 42, 56, 69, 80, 89, 95, 99, 100, 98, 93, 86, 77, 65, 52, 37, 21, 4, -13, -29, -44, -59, -71, -82, -90, -96, -99, -100, -98, -93, -85, -75, -63, -49, -34, -18, -2, 15, 31, 47, 61, 73, 83, 91, 97, 100, 100, 97, 92, 84, 73, 61, 47, 32, 16, -1, -17, -34, -49, -63, -75, -85, -92, -97, -100, -99, -96, -91, -82, -72, -59, -45, -29, -13, 3, 20, 36, 51, 64, 76, 86, 93, 98, 100, 99, 96, 89, 81, 70, 57, 43, 27, 11, -6, -22, -38, -53, -66, -78, -87, -94, -98, -100, -99, -95, -88, -79, -68, -55, -40, -25, -8, 8, 25, 41, 55, 68, 79, 88, 95, 99, 100, 98, 94, 87, 78, 66, 53, 38, 22, 6, -11, -27, -43, -57, -70, -81, -90, -96, -99, -100, -98, -93, -86, -76, -64, -51, -36, -20, -3, 13, 30, 45, 59, 72, 82, 91, 96, 99, 100, 97, 92, 85
};
inline float fastSinY(float y) {
  int idx = ((int)y) % 240;
  if (idx < 0) idx += 240;
  int8_t v = (int8_t)pgm_read_byte(&sinTableX100[idx]);
  return v / 100.0f;
}

void spaceInvaderGame() {
  float playerX = 60.0;
  const float playerY = 46.0;
  int lives = 3;
  int score = 0;
  int wave = 1;
  int enemiesKilled = 0;
  
  bool doubleShot = false;
  unsigned long doubleShotTimer = 0;
  bool shield = false;
  unsigned long shieldTimer = 0;

  float starX[8];
  float starY[8];
  for (int i = 0; i < 8; i++) {
    starX[i] = random(0, SCREEN_WIDTH);
    starY[i] = random(0, SCREEN_HEIGHT);
  }

  const int maxBullets = 6;
  float bulletX[maxBullets];
  float bulletY[maxBullets];
  bool bulletActive[maxBullets];
  for (int i = 0; i < maxBullets; i++) bulletActive[i] = false;
  unsigned long lastFireTime = 0;

  const int maxEnemyBullets = 4;
  float eBulletX[maxEnemyBullets];
  float eBulletY[maxEnemyBullets];
  bool eBulletActive[maxEnemyBullets];
  for (int i = 0; i < maxEnemyBullets; i++) eBulletActive[i] = false;

  struct Enemy {
    float x, y;
    float vx, vy;
    float centerX;
    int type;
    int hp;
    bool active;
    unsigned long lastShotTime;
  };
  const int maxEnemies = 3;
  Enemy enemies[maxEnemies];

  auto spawnEnemy = [&](int idx) {
    enemies[idx].active = true;
    enemies[idx].type = random(0, 3);
    enemies[idx].hp = (enemies[idx].type == 1) ? 2 : 1;
    enemies[idx].x = random(10, SCREEN_WIDTH - 26);
    enemies[idx].y = -random(15, 60);
    enemies[idx].centerX = enemies[idx].x;
    enemies[idx].vx = (random(0, 2) == 0) ? 0.6 : -0.6;
    enemies[idx].vy = 0.5 + (wave * 0.1) + (random(0, 5) / 10.0);
    enemies[idx].lastShotTime = millis() + random(500, 2000);
  };

  for (int i = 0; i < maxEnemies; i++) spawnEnemy(i);

  bool bossActive = false;
  float bossX = 48.0;
  float bossY = -20.0;
  float bossVx = 1.0;
  int bossHP = 0;
  unsigned long bossLastShot = 0;

  float pUpX = 0, pUpY = 0;
  int pUpType = -1;
  bool pUpActive = false;

  const int maxParticles = 10;
  float partX[maxParticles];
  float partY[maxParticles];
  float partVx[maxParticles];
  float partVy[maxParticles];
  int partLife[maxParticles];
  for (int i = 0; i < maxParticles; i++) partLife[i] = 0;

  auto spawnExplosion = [&](float x, float y) {
    for (int i = 0; i < maxParticles; i++) {
      partX[i] = x;
      partY[i] = y;
      partVx[i] = (random(-150, 151) / 100.0);
      partVy[i] = (random(-150, 151) / 100.0);
      partLife[i] = random(8, 16);
    }
  };

  while (digitalRead(OK_BUTTON) == LOW || digitalRead(UP_BUTTON) == LOW) delay(10);

  bool playing = true;

  while (playing && lives > 0) {
    if (digitalRead(LEFT_BUTTON) == LOW) playerX -= 2.2;
    if (digitalRead(RIGHT_BUTTON) == LOW) playerX += 2.2;
    playerX = constrain(playerX, 0, SCREEN_WIDTH - 8);

    if (digitalRead(OK_BUTTON) == LOW && (millis() - lastFireTime > 250)) {
      lastFireTime = millis();
      if (doubleShot) {
        int s1 = -1, s2 = -1;
        for (int i = 0; i < maxBullets; i++) {
          if (!bulletActive[i]) {
            if (s1 == -1) s1 = i;
            else if (s2 == -1) { s2 = i; break; }
          }
        }
        if (s1 != -1 && s2 != -1) {
          bulletActive[s1] = true; bulletX[s1] = playerX - 1; bulletY[s1] = playerY;
          bulletActive[s2] = true; bulletX[s2] = playerX + 7; bulletY[s2] = playerY;
        }
      } else {
        for (int i = 0; i < maxBullets; i++) {
          if (!bulletActive[i]) {
            bulletActive[i] = true;
            bulletX[i] = playerX + 3;
            bulletY[i] = playerY;
            break;
          }
        }
      }
    }

    for (int i = 0; i < 8; i++) {
      starY[i] += 1.2;
      if (starY[i] >= SCREEN_HEIGHT) { starY[i] = 0; starX[i] = random(0, SCREEN_WIDTH); }
    }

    for (int i = 0; i < maxParticles; i++) {
      if (partLife[i] > 0) { partX[i] += partVx[i]; partY[i] += partVy[i]; partLife[i]--; }
    }

    if (doubleShot && (millis() > doubleShotTimer)) doubleShot = false;
    if (shield && (millis() > shieldTimer)) shield = false;

    for (int i = 0; i < maxBullets; i++) {
      if (bulletActive[i]) {
        bulletY[i] -= 3.5;
        if (bulletY[i] < 0) bulletActive[i] = false;
      }
    }

    for (int i = 0; i < maxEnemyBullets; i++) {
      if (eBulletActive[i]) {
        eBulletY[i] += 2.0;
        if (eBulletY[i] > SCREEN_HEIGHT) eBulletActive[i] = false;

        if (eBulletX[i] >= playerX && eBulletX[i] <= playerX + 8 &&
            eBulletY[i] >= playerY && eBulletY[i] <= playerY + 16) {
          eBulletActive[i] = false;
          if (shield) {
            shield = false;
          } else {
            lives--;
            spawnExplosion(playerX + 4, playerY + 8);
            delay(500); // trimmed from 800ms - was adding to the "laggy" feel
          }
        }
      }
    }

    if (pUpActive) {
      pUpY += 1.0;
      if (pUpY > SCREEN_HEIGHT) pUpActive = false;

      if (pUpX + 6 >= playerX && pUpX <= playerX + 8 &&
          pUpY + 6 >= playerY && pUpY <= playerY + 16) {
        pUpActive = false;
        if (pUpType == 0) { doubleShot = true; doubleShotTimer = millis() + 8000; }
        else { shield = true; shieldTimer = millis() + 10000; }
      }
    }

    if (!bossActive) {
      for (int i = 0; i < maxEnemies; i++) {
        if (!enemies[i].active) continue;

        if (enemies[i].type == 0) {
          // Scout: sine wave, now via lookup table instead of sin()
          enemies[i].y += enemies[i].vy;
          enemies[i].centerX += enemies[i].vx;
          if (enemies[i].centerX < 10 || enemies[i].centerX > SCREEN_WIDTH - 26) enemies[i].vx = -enemies[i].vx;
          enemies[i].x = enemies[i].centerX + fastSinY(enemies[i].y) * 12.0;
        } else if (enemies[i].type == 1) {
          enemies[i].y += enemies[i].vy * 0.7;
          enemies[i].x += enemies[i].vx;
          if (enemies[i].x < 10 || enemies[i].x > SCREEN_WIDTH - 26) enemies[i].vx = -enemies[i].vx;
        } else {
          enemies[i].y += enemies[i].vy * 1.3;
          if (enemies[i].y > 10 && enemies[i].y < playerY - 10) {
            if (enemies[i].x < playerX) enemies[i].x += 0.8;
            else if (enemies[i].x > playerX) enemies[i].x -= 0.8;
          }
        }

        if (enemies[i].y > SCREEN_HEIGHT) spawnEnemy(i);

        if (enemies[i].active && enemies[i].y > 0 && (millis() > enemies[i].lastShotTime)) {
          enemies[i].lastShotTime = millis() + random(1500, 3000);
          for (int b = 0; b < maxEnemyBullets; b++) {
            if (!eBulletActive[b]) {
              eBulletActive[b] = true;
              eBulletX[b] = enemies[i].x + 8;
              eBulletY[b] = enemies[i].y + 8;
              break;
            }
          }
        }

        if (enemies[i].x + 16 >= playerX && enemies[i].x <= playerX + 8 &&
            enemies[i].y + 8 >= playerY && enemies[i].y <= playerY + 16) {
          spawnExplosion(enemies[i].x + 8, enemies[i].y + 4);
          spawnEnemy(i);
          if (shield) shield = false;
          else { lives--; delay(500); }
        }

        for (int b = 0; b < maxBullets; b++) {
          if (bulletActive[b]) {
            if (bulletX[b] >= enemies[i].x && bulletX[b] <= enemies[i].x + 16 &&
                bulletY[b] >= enemies[i].y && bulletY[b] <= enemies[i].y + 8) {
              bulletActive[b] = false;
              enemies[i].hp--;
              if (enemies[i].hp <= 0) {
                spawnExplosion(enemies[i].x + 8, enemies[i].y + 4);
                if (random(0, 10) < 2 && !pUpActive) {
                  pUpActive = true;
                  pUpX = enemies[i].x + 4;
                  pUpY = enemies[i].y;
                  pUpType = random(0, 2);
                }
                spawnEnemy(i);
                score += 20;
                enemiesKilled++;
                if (enemiesKilled >= 15) {
                  bossActive = true;
                  bossX = 48.0;
                  bossY = -30.0;
                  bossHP = 15 + wave * 5;
                  bossLastShot = millis() + 1000;
                  for (int k = 0; k < maxEnemies; k++) enemies[k].active = false;
                }
              }
            }
          }
        }
      }
    } else {
      if (bossY < 8.0) bossY += 0.5;
      else {
        bossX += bossVx;
        if (bossX < 10 || bossX > SCREEN_WIDTH - 42) bossVx = -bossVx;
      }

      if (millis() > bossLastShot) {
        bossLastShot = millis() + random(1000, 2000);
        int fired = 0;
        for (int b = 0; b < maxEnemyBullets; b++) {
          if (!eBulletActive[b]) {
            eBulletActive[b] = true;
            eBulletX[b] = bossX + 8 + (fired * 16);
            eBulletY[b] = bossY + 12;
            fired++;
            if (fired >= 2) break;
          }
        }
      }

      for (int b = 0; b < maxBullets; b++) {
        if (bulletActive[b]) {
          if (bulletX[b] >= bossX && bulletX[b] <= bossX + 32 &&
              bulletY[b] >= bossY && bulletY[b] <= bossY + 12) {
            bulletActive[b] = false;
            bossHP--;
            if (bossHP <= 0) {
              spawnExplosion(bossX + 16, bossY + 6);
              spawnExplosion(bossX + 8, bossY + 6);
              spawnExplosion(bossX + 24, bossY + 6);
              score += 200;
              wave++;
              enemiesKilled = 0;
              bossActive = false;
              for (int k = 0; k < maxEnemies; k++) spawnEnemy(k);
            }
          }
        }
      }
    }

    display.clearDisplay();

    for (int i = 0; i < 8; i++) display.drawPixel((int)starX[i], (int)starY[i], SSD1306_WHITE);

    for (int i = 0; i < maxParticles; i++) {
      if (partLife[i] > 0) display.drawPixel((int)partX[i], (int)partY[i], SSD1306_WHITE);
    }

    display.drawBitmap((int)playerX, (int)playerY, playerShipVertical, 8, 16, SSD1306_WHITE);
    if (shield) display.drawCircle((int)playerX + 4, (int)playerY + 8, 10, SSD1306_WHITE);

    for (int i = 0; i < maxBullets; i++) {
      if (bulletActive[i]) display.drawFastVLine((int)bulletX[i], (int)bulletY[i], 3, SSD1306_WHITE);
    }
    for (int i = 0; i < maxEnemyBullets; i++) {
      if (eBulletActive[i]) display.fillRect((int)eBulletX[i], (int)eBulletY[i], 2, 2, SSD1306_WHITE);
    }

    if (pUpActive) {
      if ((millis() / 150) % 2 == 0) {
        display.drawRect((int)pUpX, (int)pUpY, 6, 6, SSD1306_WHITE);
        display.setCursor((int)pUpX + 1, (int)pUpY - 1);
        display.print(pUpType == 0 ? "D" : "S");
      }
    }

    if (!bossActive) {
      for (int i = 0; i < maxEnemies; i++) {
        if (enemies[i].active && enemies[i].y >= 0) display.drawBitmap((int)enemies[i].x, (int)enemies[i].y, invader, 16, 8, SSD1306_WHITE);
      }
    } else {
      int bx = (int)bossX;
      int by = (int)bossY;
      display.fillRect(bx, by + 4, 32, 8, SSD1306_WHITE);
      display.fillRect(bx + 8, by, 16, 4, SSD1306_WHITE);
      display.fillRect(bx + 14, by + 12, 4, 2, SSD1306_WHITE);

      display.drawRect(34, 2, 60, 4, SSD1306_WHITE);
      int maxHP = 15 + wave * 5;
      int barW = (bossHP * 58) / maxHP;
      display.fillRect(35, 3, barW, 2, SSD1306_WHITE);
    }

    display.setTextSize(1);
    display.setCursor(95, 2);
    display.print("L:"); display.print(lives);

    display.display();
    delay(15);
  }

  gameOver(score);
  inGame = false;
}

// ===================== GAME 10: TETRIS LITE =====================
// FIX (#5): enlarged the side panel / NEXT-piece preview box a bit so it
// isn't so cramped, without shrinking the playfield.
void tetrislite() {
  const int ORG_X = 8; 
  const int ORG_Y = 2;
  
  const int CELL_W = 5;
  const int CELL_H = 3;
  const int COLS = 10;
  const int ROWS = 20;
  const int W_PX = COLS * CELL_W; 
  const int H_PX = ROWS * CELL_H; 
  
  const int RightUI_X = 60;          // was 68 - panel now a bit wider/bigger
  const int RightUI_W = 66;          // was 58

  uint8_t board[ROWS][COLS];
  memset(board, 0, sizeof(board));

  const uint16_t SHAPES[7][4] = {
    { 0x0f00, 0x2222, 0x00f0, 0x4444 }, 
    { 0x8e00, 0x6440, 0x0e20, 0x44c0 }, 
    { 0x2e00, 0x4460, 0x0e80, 0xc440 }, 
    { 0x6600, 0x6600, 0x6600, 0x6600 }, 
    { 0x6c00, 0x4620, 0x06c0, 0x8c40 }, 
    { 0x4e00, 0x4640, 0x0e40, 0x4c40 }, 
    { 0xc600, 0x2640, 0x0c60, 0x4c80 }  
  };

  auto cellAtMask = [&](uint16_t mask, int x, int y) {
    return (mask >> (15 - (y * 4 + x))) & 1;
  };

  struct Piece { int type, rot, r, c; } cur, nxt;

  auto getPiece = [&]() {
    Piece p;
    p.type = random(0, 7); p.rot = 0; p.r = 0; p.c = 3;
    return p;
  };

  cur = getPiece();
  nxt = getPiece();

  auto collides = [&](Piece p) {
    uint16_t mask = SHAPES[p.type][p.rot];
    for (int y = 0; y < 4; y++) {
      for (int x = 0; x < 4; x++) {
        if (cellAtMask(mask, x, y)) {
          int rr = p.r + y;
          int cc = p.c + x;
          if (cc < 0 || cc >= COLS || rr >= ROWS) return true;
          if (rr >= 0 && board[rr][cc]) return true;
        }
      }
    }
    return false;
  };

  auto lockPiece = [&](Piece p) {
    uint16_t mask = SHAPES[p.type][p.rot];
    for (int y = 0; y < 4; y++) {
      for (int x = 0; x < 4; x++) {
        if (cellAtMask(mask, x, y)) {
          int rr = p.r + y;
          int cc = p.c + x;
          if (rr >= 0 && rr < ROWS && cc >= 0 && cc < COLS) board[rr][cc] = 1;
        }
      }
    }
  };

  auto clearLines = [&]() {
    int cleared = 0;
    for (int r = ROWS - 1; r >= 0; ) {
      bool full = true;
      for (int c = 0; c < COLS; c++) {
        if (!board[r][c]) { full = false; break; }
      }
      if (full) {
        cleared++;
        for (int rr = r; rr > 0; rr--) {
          for (int cc = 0; cc < COLS; cc++) board[rr][cc] = board[rr - 1][cc];
        }
        for (int cc = 0; cc < COLS; cc++) board[0][cc] = 0;
      } else {
        r--;
      }
    }
    return cleared;
  };

  auto ghostRow = [&](Piece p) {
    while (!collides(p)) p.r++;
    return p.r - 1;
  };

  int score = 0;
  int lines = 0;
  unsigned long lastFall = millis();
  unsigned long gravity = 600;

  bool lastL = false, lastR = false, lastU = false, lastOK = false;

  while (true) {
    bool L = (digitalRead(LEFT_BUTTON) == LOW);
    bool R = (digitalRead(RIGHT_BUTTON) == LOW);
    bool U = (digitalRead(UP_BUTTON) == LOW);
    bool D = (digitalRead(DOWN_BUTTON) == LOW);
    bool OK = (digitalRead(OK_BUTTON) == LOW);

    if (U && !lastU) {
      Piece test = cur;
      test.rot = (test.rot + 1) % 4;
      if (!collides(test)) cur = test;
      else {
        test.c -= 1;
        if (!collides(test)) cur = test;
        else {
          test.c += 2;
          if (!collides(test)) cur = test;
        }
      }
    }
    lastU = U;

    if (L && !lastL) { Piece test = cur; test.c--; if (!collides(test)) cur = test; }
    lastL = L;
    if (R && !lastR) { Piece test = cur; test.c++; if (!collides(test)) cur = test; }
    lastR = R;

    if (OK && !lastOK) {
      while (!collides(cur)) cur.r++;
      cur.r--;
      lockPiece(cur);
      int cl = clearLines();
      score += cl * 100;
      lines += cl;
      cur = nxt;
      nxt = getPiece();
      if (collides(cur)) break;
    }
    lastOK = OK;

    if (D) { Piece test = cur; test.r++; if (!collides(test)) cur = test; }

    if (millis() - lastFall > gravity) {
      lastFall = millis();
      Piece test = cur;
      test.r++;
      if (!collides(test)) cur = test;
      else {
        lockPiece(cur);
        int cl = clearLines();
        score += cl * 100;
        lines += cl;
        cur = nxt;
        nxt = getPiece();
        if (collides(cur)) break;
      }
    }

    display.clearDisplay();
    display.drawRect(ORG_X - 1, ORG_Y - 1, W_PX + 2, H_PX + 2, SSD1306_WHITE);

    for (int r = 0; r < ROWS; r++) {
      for (int c = 0; c < COLS; c++) {
        if (board[r][c]) {
          display.fillRect(ORG_X + c * CELL_W, ORG_Y + r * CELL_H, CELL_W, CELL_H, SSD1306_WHITE);
        }
      }
    }

    int gRow = ghostRow(cur);
    uint16_t mask = SHAPES[cur.type][cur.rot];
    for (int y = 0; y < 4; y++) {
      for (int x = 0; x < 4; x++) {
        if (cellAtMask(mask, x, y)) {
          int rr = gRow + y;
          int cc = cur.c + x;
          if (rr >= 0) {
            int tx = ORG_X + cc * CELL_W;
            int ty = ORG_Y + rr * CELL_H;
            display.drawRect(tx, ty, CELL_W, CELL_H, SSD1306_WHITE);
          }
        }
      }
    }

    for (int y = 0; y < 4; y++) {
      for (int x = 0; x < 4; x++) {
        if (cellAtMask(mask, x, y)) {
          int rr = cur.r + y;
          int cc = cur.c + x;
          if (rr >= 0) {
            display.fillRect(ORG_X + cc * CELL_W, ORG_Y + rr * CELL_H, CELL_W, CELL_H, SSD1306_WHITE);
          }
        }
      }
    }

    display.drawRect(RightUI_X, 2, RightUI_W, 60, SSD1306_WHITE);
    display.setCursor(RightUI_X + 5, 6);
    display.print("TETRIS");

    display.setCursor(RightUI_X + 5, 20);
    display.print("Score");
    display.setCursor(RightUI_X + 5, 30);
    display.print(score);

    display.setCursor(RightUI_X + 5, 41);
    display.print("NEXT");
    // NEXT box made a bit bigger than before (was 16x16 with 3px cells),
    // still fully inside the panel and the 64px-tall screen.
    display.drawRect(RightUI_X + 6, 47, 26, 14, SSD1306_WHITE);
    uint16_t nextMask = SHAPES[nxt.type][0];
    for (int y = 0; y < 4; y++) {
      for (int x = 0; x < 4; x++) {
        if (cellAtMask(nextMask, x, y)) {
          display.fillRect(RightUI_X + 9 + x * 4, 49 + y * 3, 3, 2, SSD1306_WHITE);
        }
      }
    }

    display.display();
    delay(10);
  }

  gameOver(score);
  inGame = false;
}

// ===================== GAME 11: BRICK KILLER =====================
// FIX (#6): full rebuild. Added 5 real difficulty tiers (Simple / Normal /
// Hard / Very Hard / Tricky), a proper lives system instead of instant
// game-over, sub-pixel smooth motion, and genuine randomness on every wall
// and paddle bounce (small angle jitter) so the ball never falls into a
// perfectly repeating, predictable loop like it used to.
void brickkillerGame() {
  const char* levelNames[5] = {"Simple", "Normal", "Hard", "Very Hard", "Tricky"};
  int level = 0;

  auto drawLevelMenu = [&]() {
    display.clearDisplay();
    display.setCursor(15, 4);
    display.print("Select Difficulty:");
    for (int i = 0; i < 5; i++) {
      display.setCursor(15, 14 + i * 10);
      display.print(i == level ? "> " : "  ");
      display.print(levelNames[i]);
    }
    display.display();
  };

  drawLevelMenu();
  delay(200);
  while (true) {
    if (digitalRead(UP_BUTTON) == LOW) { level = (level + 4) % 5; drawLevelMenu(); delay(180); }
    if (digitalRead(DOWN_BUTTON) == LOW) { level = (level + 1) % 5; drawLevelMenu(); delay(180); }
    if (digitalRead(OK_BUTTON) == LOW) { delay(200); break; }
  }

  // Per-difficulty tuning
  int paddleWidth   = 26 - level * 3;          // 26,23,20,17,14
  float ballSpeed    = 1.5 + level * 0.35;      // gets faster each tier
  int brickRows      = 3 + (level >= 2 ? 1 : 0) + (level >= 4 ? 1 : 0);
  float jitterAmount = 0.15 + level * 0.12;     // more unpredictable bounce at higher tiers
  bool movingBricks   = (level == 4);           // "Tricky": a couple of bricks drift sideways
  int lives = 3;

  const int paddleHeight = 3;
  float paddleX = (SCREEN_WIDTH - paddleWidth) / 2.0;
  const int paddleY = SCREEN_HEIGHT - 6;

  const int brickCols = 8;
  const int brickWidth = 14;
  const int brickHeight = 5;
  bool bricks[6][8];
  memset(bricks, 1, sizeof(bricks));

  // a couple of bricks (Tricky mode) get a horizontal drift
  float brickDriftOffset[6][8];
  memset(brickDriftOffset, 0, sizeof(brickDriftOffset));

  float ballX, ballY, ballVX, ballVY;
  auto resetBall = [&]() {
    ballX = SCREEN_WIDTH / 2;
    ballY = paddleY - 10;
    float angle = random(-40, 41) * (PI / 180.0); // launch angle randomness
    ballVX = ballSpeed * sin(angle);
    ballVY = -ballSpeed * cos(angle);
  };
  resetBall();

  int score = 0;
  bool playing = true;

  while (playing) {
    if (digitalRead(LEFT_BUTTON) == LOW && paddleX > 0) paddleX -= 3.2;
    if (digitalRead(RIGHT_BUTTON) == LOW && paddleX + paddleWidth < SCREEN_WIDTH) paddleX += 3.2;

    ballX += ballVX;
    ballY += ballVY;

    // Wall bounces with a touch of random jitter so it never repeats exactly
    if (ballX <= 0) { ballX = 0; ballVX = fabs(ballVX) + random(-10, 11) * jitterAmount * 0.01; }
    if (ballX + 3 >= SCREEN_WIDTH) { ballX = SCREEN_WIDTH - 3; ballVX = -fabs(ballVX) + random(-10, 11) * jitterAmount * 0.01; }
    if (ballY <= 0) { ballY = 0; ballVY = fabs(ballVY) + random(-10, 11) * jitterAmount * 0.01; }

    if (ballY + 3 >= SCREEN_HEIGHT) {
      lives--;
      if (lives <= 0) { playing = false; break; }
      resetBall();
      delay(400);
      continue;
    }

    // Paddle bounce: angle depends on hit position, plus jitter
    if (ballY + 3 >= paddleY && ballY + 3 <= paddleY + paddleHeight + 3 &&
        ballX + 3 >= paddleX && ballX <= paddleX + paddleWidth && ballVY > 0) {
      ballY = paddleY - 3;
      float hit = ((ballX + 1.5) - (paddleX + paddleWidth / 2.0)) / (paddleWidth / 2.0);
      float speedNow = sqrt(ballVX * ballVX + ballVY * ballVY);
      float angle = hit * 1.0 + (random(-15, 16) * jitterAmount * 0.02);
      angle = constrain(angle, -1.3, 1.3);
      ballVX = speedNow * sin(angle);
      ballVY = -fabs(speedNow * cos(angle));
      if (speedNow < ballSpeed * 2.2) { ballVX *= 1.02; ballVY *= 1.02; } // tiny ramp-up
    }

    // Moving bricks (Tricky only): a slow left-right drift on row 0
    if (movingBricks) {
      for (int c = 0; c < brickCols; c++) {
        if (bricks[0][c]) {
          brickDriftOffset[0][c] += 0.15;
        }
      }
    }

    for (int r = 0; r < brickRows; r++) {
      for (int c = 0; c < brickCols; c++) {
        if (!bricks[r][c]) continue;
        int bx = c * (brickWidth + 2) + 2 + (int)(movingBricks && r == 0 ? sin(brickDriftOffset[r][c]) * 6 : 0);
        int by = r * (brickHeight + 2) + 2;
        if (ballX + 3 > bx && ballX < bx + brickWidth &&
            ballY + 3 > by && ballY < by + brickHeight) {
          bricks[r][c] = false;
          ballVY = -ballVY + random(-10, 11) * jitterAmount * 0.01;
          score += 10;
        }
      }
    }

    bool win = true;
    for (int r = 0; r < brickRows; r++)
      for (int c = 0; c < brickCols; c++)
        if (bricks[r][c]) win = false;

    if (win) {
      display.clearDisplay();
      display.setCursor(15, 20);
      display.print(levelNames[level]);
      display.setCursor(15, 32);
      display.print("Cleared!");
      display.display();
      delay(1500);
      break;
    }

    display.clearDisplay();
    for (int r = 0; r < brickRows; r++) {
      for (int c = 0; c < brickCols; c++) {
        if (bricks[r][c]) {
          int bx = c * (brickWidth + 2) + 2 + (int)(movingBricks && r == 0 ? sin(brickDriftOffset[r][c]) * 6 : 0);
          display.fillRect(bx, r * (brickHeight + 2) + 2, brickWidth, brickHeight, SSD1306_WHITE);
        }
      }
    }
    display.fillRect((int)paddleX, paddleY, paddleWidth, paddleHeight, SSD1306_WHITE);
    display.fillRect((int)ballX, (int)ballY, 3, 3, SSD1306_WHITE);

    display.setCursor(2, SCREEN_HEIGHT - 8);
    display.print("Lives:"); display.print(lives);

    display.display();
    delay(15);
  }

  gameOver(score);
  inGame = false;
}

// ===================== GAME 12: PLAY 2048 =====================
// FIX (#7): the old 12x12 tiles were too small for 3-digit numbers - the
// text (6px per character) spilled past the tile edge into a neighbouring
// EMPTY cell (which is black), so a black-on-white digit turned into
// invisible black-on-black pixels the moment the score/tiles got big.
// Tiles are now wider (rectangular, since the screen is short), which is
// the "make it fuller-screen" ask, and every tile value now actually fits
// inside its own tile. Also added a session best-score tracker and a
// one-time "2048!" banner, like the official game's win condition.
void play2048() {
  int grid[4][4];
  memset(grid, 0, sizeof(grid));
  int score = 0;
  static int sessionBest = 0;
  bool wonBannerShown = false;

  const int TILE_W = 20, TILE_H = 14, GAP = 1;
  const int ORG_X = 2, ORG_Y = 3;

  auto addTile = [&]() {
    int empty[16];
    int count = 0;
    for (int i=0; i<16; i++) {
      if (grid[i/4][i%4] == 0) empty[count++] = i;
    }
    if (count > 0) {
      int idx = empty[random(0, count)];
      grid[idx/4][idx%4] = (random(0, 10) < 9) ? 2 : 4; // classic 90/10 spawn odds
    }
  };

  auto drawTileValue = [&](int x, int y, int val) {
    char buf[8];
    if (val >= 100000) sprintf(buf, "%dK", val / 1000);
    else if (val >= 10000) sprintf(buf, "%dK", val / 1000);
    else sprintf(buf, "%d", val);
    int len = strlen(buf);
    int textW = len * 6 - 1; // GFX default font advance is 6px/char
    int startX = x + (TILE_W - textW) / 2;
    int startY = y + (TILE_H - 8) / 2 + 1;
    if (startX < x + 1) startX = x + 1; // never let it creep outside the tile
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(startX, startY);
    display.print(buf);
  };

  addTile();
  addTile();

  auto slideLeft = [&]() {
    bool moved = false;
    for (int r=0; r<4; r++) {
      int lastMerge = -1;
      for (int c=1; c<4; c++) {
        if (grid[r][c] == 0) continue;
        int k = c;
        while (k > 0 && grid[r][k-1] == 0) {
          grid[r][k-1] = grid[r][k];
          grid[r][k] = 0;
          k--;
          moved = true;
        }
        if (k > 0 && grid[r][k-1] == grid[r][k] && lastMerge != k-1) {
          grid[r][k-1] *= 2;
          score += grid[r][k-1];
          if (grid[r][k-1] >= 2048 && !wonBannerShown) wonBannerShown = true;
          grid[r][k] = 0;
          lastMerge = k-1;
          moved = true;
        }
      }
    }
    return moved;
  };

  auto rotate = [&]() {
    int tmp[4][4];
    for (int r=0; r<4; r++)
      for (int c=0; c<4; c++)
        tmp[r][c] = grid[3-c][r];
    memcpy(grid, tmp, sizeof(grid));
  };

  bool announcedWin = false;

  while (true) {
    bool moved = false;
    if (digitalRead(LEFT_BUTTON) == LOW) { moved = slideLeft(); delay(180); }
    else if (digitalRead(RIGHT_BUTTON) == LOW) { rotate(); rotate(); moved = slideLeft(); rotate(); rotate(); delay(180); }
    else if (digitalRead(UP_BUTTON) == LOW) { rotate(); rotate(); rotate(); moved = slideLeft(); rotate(); delay(180); }
    else if (digitalRead(DOWN_BUTTON) == LOW) { rotate(); moved = slideLeft(); rotate(); rotate(); rotate(); delay(180); }

    if (moved) addTile();
    if (score > sessionBest) sessionBest = score;

    if (wonBannerShown && !announcedWin) {
      announcedWin = true;
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(18, 20);
      display.print("2048!");
      display.setTextSize(1);
      display.setCursor(10, 44);
      display.print("Keep going for more");
      display.display();
      delay(1400);
      waitForRelease();
    }

    display.clearDisplay();

    for (int r=0; r<4; r++) {
      for (int c=0; c<4; c++) {
        int tx = ORG_X + c * (TILE_W + GAP);
        int ty = ORG_Y + r * (TILE_H + GAP);
        if (grid[r][c] > 0) {
          display.fillRect(tx, ty, TILE_W, TILE_H, SSD1306_WHITE);
          drawTileValue(tx, ty, grid[r][c]);
        } else {
          display.drawRect(tx, ty, TILE_W, TILE_H, SSD1306_WHITE);
        }
      }
    }

    display.setTextColor(SSD1306_WHITE);
    int panelX = ORG_X + 4 * (TILE_W + GAP) + 2;
    display.drawRect(panelX, ORG_Y, SCREEN_WIDTH - panelX - 1, 4 * (TILE_H + GAP) - GAP, SSD1306_WHITE);
    display.setCursor(panelX + 4, ORG_Y + 3);
    display.print("2048");
    display.setCursor(panelX + 4, ORG_Y + 16);
    display.print("Score");
    display.setCursor(panelX + 4, ORG_Y + 25);
    display.print(score);
    display.setCursor(panelX + 4, ORG_Y + 40);
    display.print("Best");
    display.setCursor(panelX + 4, ORG_Y + 49);
    display.print(sessionBest);

    display.display();

    bool gameOverState = true;
    for (int r=0; r<4; r++) {
      for (int c=0; c<4; c++) {
        if (grid[r][c] == 0) gameOverState = false;
        if (r < 3 && grid[r][c] == grid[r+1][c]) gameOverState = false;
        if (c < 3 && grid[r][c] == grid[r][c+1]) gameOverState = false;
      }
    }
    if (gameOverState) break;
  }

  gameOver(score);
  inGame = false;
}

// ===================== GAME 13: DOODLE JUMP (NOKIA OG EDITION) =====================
void playDoodleJump() {
  static const unsigned char PROGMEM doodler_left[] = {
    0x38, 0x7c, 0xfe, 0xdf, 0xff, 0x7e, 0x24, 0x66
  };
  static const unsigned char PROGMEM doodler_right[] = {
    0x1c, 0x3e, 0x7f, 0xfb, 0xff, 0x7e, 0x24, 0x66
  };

  float doodlerX = 60.0;
  float doodlerY = 45.0;
  float velocityY = -4.0;
  bool facingRight = true;

  const float gravity = 0.32;
  const float jumpPower = -6.2;

  static int sessionHighScore = 0;
  int score = 0;
  int maxAltitude = 0;

  unsigned long rocketEnd = 0;

  struct BrokenPiece { float x, y, vx, vy; bool active; };
  BrokenPiece leftPiece = {0, 0, 0, 0, false};
  BrokenPiece rightPiece = {0, 0, 0, 0, false};

  struct Platform {
    float x, y;
    int type; // 0: Wood, 1: Broken Wood, 2: Moving
    float speed;
    bool broken;
    bool hasSpring;
    bool springExpanded;
    bool hasRocket;
    bool active;
  };

  const int maxPlats = 6;
  Platform platforms[maxPlats];

  auto resetPlatform = [&](int idx, float yPos) {
    platforms[idx].active = true;
    platforms[idx].x = random(4, SCREEN_WIDTH - 26);
    platforms[idx].y = yPos;
    platforms[idx].broken = false;
    platforms[idx].hasSpring = false;
    platforms[idx].springExpanded = false;
    platforms[idx].hasRocket = false;
    platforms[idx].speed = (random(0, 2) == 0 ? 1.0 : -1.0);

    int r = random(0, 100);
    if (r < 18) {
      platforms[idx].type = 1; // Broken wood
    } else if (r < 38) {
      platforms[idx].type = 2; // Moving
    } else {
      platforms[idx].type = 0; // Standard wood
      int pR = random(0, 100);
      if (pR < 14) {
        platforms[idx].hasSpring = true;
      } else if (pR < 22) {
        platforms[idx].hasRocket = true;
      }
    }
  };

  platforms[0] = {50, 56, 0, 0, false, false, false, false, true};
  for (int i = 1; i < maxPlats; i++) {
    resetPlatform(i, 56 - i * 11);
  }

  while (digitalRead(OK_BUTTON) == LOW || digitalRead(UP_BUTTON) == LOW) delay(10);
  bool running = true;

  while (running) {
    if (digitalRead(LEFT_BUTTON) == LOW) {
      doodlerX -= 2.4;
      facingRight = false;
    }
    if (digitalRead(RIGHT_BUTTON) == LOW) {
      doodlerX += 2.4;
      facingRight = true;
    }

    if (doodlerX < -6) doodlerX = SCREEN_WIDTH - 2;
    else if (doodlerX > SCREEN_WIDTH - 2) doodlerX = -6;

    if (millis() < rocketEnd) {
      velocityY = -8.5;
    } else {
      velocityY += gravity;
      if (velocityY > 6.5) velocityY = 6.5;
    }

    doodlerY += velocityY;

    for (int i = 0; i < maxPlats; i++) {
      if (platforms[i].type == 2 && platforms[i].active) {
        platforms[i].x += platforms[i].speed;
        if (platforms[i].x <= 2) { platforms[i].x = 2; platforms[i].speed = -platforms[i].speed; }
        else if (platforms[i].x >= SCREEN_WIDTH - 24) { platforms[i].x = SCREEN_WIDTH - 24; platforms[i].speed = -platforms[i].speed; }
      }
    }

    if (velocityY > 0 && millis() >= rocketEnd) {
      for (int i = 0; i < maxPlats; i++) {
        if (!platforms[i].active || platforms[i].broken) continue;

        if (doodlerX + 8 >= platforms[i].x && doodlerX <= platforms[i].x + 22 &&
            doodlerY + 8 >= platforms[i].y && doodlerY + 3 <= platforms[i].y + 4) {

          if (platforms[i].type == 1) {
            platforms[i].broken = true;
            leftPiece = {platforms[i].x, platforms[i].y, -0.6, 1.2, true};
            rightPiece = {platforms[i].x + 11, platforms[i].y, 0.6, 1.2, true};
          } else if (platforms[i].hasRocket) {
            platforms[i].hasRocket = false;
            rocketEnd = millis() + 2000;
            velocityY = -8.5;
          } else if (platforms[i].hasSpring) {
            platforms[i].springExpanded = true;
            velocityY = -12.5;
          } else {
            velocityY = jumpPower;
          }
        }
      }
    }

    if (doodlerY < 28) {
      float scroll = 28 - doodlerY;
      doodlerY = 28;
      maxAltitude += (int)scroll;
      score = maxAltitude;

      for (int i = 0; i < maxPlats; i++) {
        platforms[i].y += scroll;
        if (platforms[i].y > SCREEN_HEIGHT) {
          resetPlatform(i, platforms[i].y - SCREEN_HEIGHT);
        }
      }

      if (leftPiece.active) leftPiece.y += scroll;
      if (rightPiece.active) rightPiece.y += scroll;
    }

    if (leftPiece.active) {
      leftPiece.x += leftPiece.vx; leftPiece.y += leftPiece.vy; leftPiece.vy += 0.4;
      if (leftPiece.y > SCREEN_HEIGHT) leftPiece.active = false;
    }
    if (rightPiece.active) {
      rightPiece.x += rightPiece.vx; rightPiece.y += rightPiece.vy; rightPiece.vy += 0.4;
      if (rightPiece.y > SCREEN_HEIGHT) rightPiece.active = false;
    }

    if (doodlerY > SCREEN_HEIGHT) {
      running = false;
      break;
    }

    display.clearDisplay();

    for (int i = 0; i < maxPlats; i++) {
      if (!platforms[i].active || platforms[i].broken) continue;
      int px = (int)platforms[i].x;
      int py = (int)platforms[i].y;

      if (platforms[i].type == 0) {
        display.drawRect(px, py, 22, 4, SSD1306_WHITE);
        display.drawFastHLine(px + 2, py + 1, 18, SSD1306_WHITE);
        display.drawPixel(px + 6, py + 2, SSD1306_WHITE);
        display.drawPixel(px + 15, py + 2, SSD1306_WHITE);
      } else if (platforms[i].type == 1) {
        display.drawRect(px, py, 10, 4, SSD1306_WHITE);
        display.drawRect(px + 12, py, 10, 4, SSD1306_WHITE);
        display.drawLine(px + 10, py, px + 12, py + 3, SSD1306_WHITE);
      } else if (platforms[i].type == 2) {
        display.fillRect(px, py, 22, 4, SSD1306_WHITE);
        display.drawFastHLine(px + 1, py + 1, 20, SSD1306_BLACK);
      }

      if (platforms[i].hasSpring) {
        int sx = px + 8;
        if (platforms[i].springExpanded) {
          display.drawLine(sx, py, sx + 4, py - 6, SSD1306_WHITE);
          display.drawLine(sx + 4, py - 6, sx, py - 8, SSD1306_WHITE);
        } else {
          display.drawFastHLine(sx, py - 1, 6, SSD1306_WHITE);
          display.drawFastHLine(sx + 1, py - 2, 4, SSD1306_WHITE);
          display.drawFastHLine(sx, py - 3, 6, SSD1306_WHITE);
        }
      }

      if (platforms[i].hasRocket) {
        display.drawBitmap(px + 7, py - 8, rocketSprite, 8, 8, SSD1306_WHITE);
      }
    }

    if (leftPiece.active) display.drawRect((int)leftPiece.x, (int)leftPiece.y, 10, 4, SSD1306_WHITE);
    if (rightPiece.active) display.drawRect((int)rightPiece.x, (int)rightPiece.y, 10, 4, SSD1306_WHITE);

    const unsigned char* dSprite = facingRight ? doodler_right : doodler_left;
    display.drawBitmap((int)doodlerX, (int)doodlerY, dSprite, 8, 8, SSD1306_WHITE);

    if (millis() < rocketEnd) {
      display.drawBitmap((int)doodlerX, (int)doodlerY - 2, rocketSprite, 8, 8, SSD1306_WHITE);
      if ((millis() / 50) % 2 == 0) {
        display.drawLine((int)doodlerX + 2, (int)doodlerY + 8, (int)doodlerX + 2, (int)doodlerY + 14, SSD1306_WHITE);
        display.drawLine((int)doodlerX + 5, (int)doodlerY + 8, (int)doodlerX + 5, (int)doodlerY + 14, SSD1306_WHITE);
      } else {
        display.drawLine((int)doodlerX + 3, (int)doodlerY + 8, (int)doodlerX + 3, (int)doodlerY + 16, SSD1306_WHITE);
        display.drawLine((int)doodlerX + 4, (int)doodlerY + 8, (int)doodlerX + 4, (int)doodlerY + 16, SSD1306_WHITE);
      }
    }

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(2, 0);
    display.print("H:"); display.print(score);
    display.setCursor(80, 0);
    display.print("HI:"); display.print(sessionHighScore);

    display.display();
    delay(20);
  }

  if (score > sessionHighScore) sessionHighScore = score;
  gameOver(score);
  inGame = false;
}

// ===================== GAME 14: MAZE SOLVER (FRAMED WITH TOP HUD) =====================
void mazeSolverGame() {
  const int COLS = 29;
  const int ROWS = 11;
  uint8_t maze[ROWS][COLS];

  const int ox = 6;
  const int oy = 15;

  struct Point { int r, c; };
  Point stack[200];
  int top = -1;

  auto getNeighbors = [&](Point p, Point out[4]) {
    int cnt = 0;
    int dr[] = {-2, 2, 0, 0};
    int dc[] = {0, 0, -2, 2};
    for (int i = 0; i < 4; i++) {
      int nr = p.r + dr[i];
      int nc = p.c + dc[i];
      if (nr > 0 && nr < ROWS - 1 && nc > 0 && nc < COLS - 1 && maze[nr][nc] == 1) {
        out[cnt++] = {nr, nc};
      }
    }
    return cnt;
  };

  int level = 1;
  int baseTimer = 65;
  bool inLevel = true;

  while (inLevel) {
    memset(maze, 1, sizeof(maze));
    top = -1;

    maze[1][1] = 0;
    stack[++top] = {1, 1};

    while (top >= 0) {
      Point curr = stack[top];
      Point neighbors[4];
      int count = getNeighbors(curr, neighbors);
      if (count > 0) {
        Point next = neighbors[random(0, count)];
        maze[next.r][next.c] = 0;
        maze[(curr.r + next.r) / 2][(curr.c + next.c) / 2] = 0;
        stack[++top] = next;
      } else {
        top--;
      }
    }

    int extraOpenings = min(level - 1, 4);
    int attempts = 0;
    while (extraOpenings > 0 && attempts < 50) {
      attempts++;
      int rr = random(1, ROWS - 1);
      int cc = random(1, COLS - 1);
      if (maze[rr][cc] == 1) {
        bool horiz = (cc > 0 && cc < COLS - 1 && maze[rr][cc - 1] == 0 && maze[rr][cc + 1] == 0);
        bool vert  = (rr > 0 && rr < ROWS - 1 && maze[rr - 1][cc] == 0 && maze[rr + 1][cc] == 0);
        if (horiz || vert) { maze[rr][cc] = 0; extraOpenings--; }
      }
    }

    maze[ROWS - 2][COLS - 2] = 0;

    int playerR = 1, playerC = 1;
    int targetR = ROWS - 2, targetC = COLS - 2;

    int secondsLeft = baseTimer - (level * 4);
    if (secondsLeft < 15) secondsLeft = 15;

    display.clearDisplay();
    display.setCursor(2, 0);
    display.print("Lvl: "); display.print(level);
    display.setCursor(65, 0);
    display.print("Get Ready!");
    display.drawFastHLine(0, 9, 128, SSD1306_WHITE);

    display.drawRect(ox - 2, oy - 2, COLS * 4 + 4, ROWS * 4 + 4, SSD1306_WHITE);

    for (int r = 0; r < ROWS; r++) {
      for (int c = 0; c < COLS; c++) {
        if (maze[r][c] == 1) display.fillRect(ox + c * 4, oy + r * 4, 4, 4, SSD1306_WHITE);
      }
    }
    display.fillRect(ox + playerC * 4 + 1, oy + playerR * 4 + 1, 2, 2, SSD1306_WHITE);
    display.drawRect(ox + targetC * 4, oy + targetR * 4, 4, 4, SSD1306_WHITE);
    display.display();
    delay(900);

    unsigned long timerStart = millis();
    bool levelWon = false;

    while (true) {
      if (digitalRead(LEFT_BUTTON) == LOW && maze[playerR][playerC - 1] == 0) { playerC--; delay(120); }
      else if (digitalRead(RIGHT_BUTTON) == LOW && maze[playerR][playerC + 1] == 0) { playerC++; delay(120); }
      else if (digitalRead(UP_BUTTON) == LOW && maze[playerR - 1][playerC] == 0) { playerR--; delay(120); }
      else if (digitalRead(DOWN_BUTTON) == LOW && maze[playerR + 1][playerC] == 0) { playerR++; delay(120); }

      int elapsed = (millis() - timerStart) / 1000;
      int tLeft = secondsLeft - elapsed;

      if (tLeft <= 0) {
        inLevel = false;
        break;
      }

      if (playerR == targetR && playerC == targetC) {
        levelWon = true;
        level++;
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(20, 25);
        display.print("Level Complete!");
        display.display();
        delay(1200);
        break;
      }

      display.clearDisplay();

      // Top HUD: completely outside maze area
      display.setCursor(2, 0);
      display.print("Lvl:"); display.print(level);
      display.setCursor(48, 0);
      display.print("Exit:*");
      display.setCursor(92, 0);
      display.print("T:"); display.print(tLeft); display.print("s");
      display.drawFastHLine(0, 9, 128, SSD1306_WHITE);

      // Maze Outer Border (Clean frame)
      display.drawRect(ox - 2, oy - 2, COLS * 4 + 4, ROWS * 4 + 4, SSD1306_WHITE);

      for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
          if (maze[r][c] == 1) {
            display.fillRect(ox + c * 4, oy + r * 4, 4, 4, SSD1306_WHITE);
          }
        }
      }

      // Exit Target: Blinking star box (clearly inside the maze, not under the timer)
      if ((millis() / 200) % 2 == 0) {
        display.drawRect(ox + targetC * 4, oy + targetR * 4, 4, 4, SSD1306_WHITE);
        display.fillRect(ox + targetC * 4 + 1, oy + targetR * 4 + 1, 2, 2, SSD1306_WHITE);
      }

      // Player
      display.fillRect(ox + playerC * 4 + 1, oy + playerR * 4 + 1, 2, 2, SSD1306_WHITE);

      display.display();
      delay(15);
    }

    if (!levelWon) break;
  }

  gameOver(level * 100);
  inGame = false;
}

// ===================== GAME 15: PAC-MAN (OFFICIAL ARCADE ENGINE) =====================
/*
 * ==============================================================================
 *                         PAC-MAN COMPLETE ENGINE FLOWCHART
 * ==============================================================================
 *
 *     [ STAGE START ] -> Show Level & Fruit Name -> 1.5s "READY!" Pause
 *            |
 *            v
 *     [ INITIALIZE MAZE ] -> 14x8 Authentic Layout
 *            - Pellets (10pts), Energizers (50pts)
 *            - Pac-Man at (Col 6, Row 5), Blinky at (Col 6, Row 3), Pinky at (Col 7, Row 3)
 *            |
 *            v
 *  +---->[ READ INPUT ] -> Instant 180-deg reversal or buffer NextDir
 *  |         |
 *  |         v
 *  |  [ PAC-MAN SUB-PIXEL MOVE ]
 *  |    - Advance subpixel (0..7). When subpixel==0 (tile center):
 *  |      * Try buffered turn (NextDir) if path is clear
 *  |      * Stop if forward tile is wall
 *  |      * Side Tunnel Wrap: Col 0 <-> Col 13
 *  |      * Eat Pellet (+10) / Eat Energizer (+50, FRIGHTENED ON)
 *  |      * Fruit check (+Bonus)
 *  |         |
 *  |         v
 *  |  [ GHOST AI & SUB-PIXEL MOVE ]
 *  |    - Advance subpixel: normal (1px/tick), frightened (1px/2ticks), eyes (2px/tick)
 *  |    - At tile center:
 *  |      * Blinky targets Pac-Man tile (Chase) or Top-Right (Scatter)
 *  |      * Pinky targets 3 tiles ahead (Chase) or Top-Left (Scatter)
 *  |      * Scared ghosts flee towards corners
 *  |      * Eyes target Ghost Pen Door (Col 6, Row 3)
 *  |      * Pick non-reverse direction minimizing distance to target
 *  |         |
 *  |         v
 *  |  [ COLLISION CHECK ]
 *  |    - Distance < 5px:
 *  |      * Scared ghost -> EAT GHOST (+200), turns into Eyes!
 *  |      * Normal ghost -> PAC-MAN DIES (Lives--), death animation
 *  |         |
 *  |         v
 *  |  [ STAGE CLEAR CHECK ]
 *  |    - Pellets == 0 -> Stage Clear fanfare -> Next Stage (Faster, Fruit bonus++)
 *  |         |
 *  +--- Render OLED 128x64 with Side HUD (Score, Level, Lives, Mode)
 * ==============================================================================
 */

void pacmanGame() {
  const uint8_t baseMaze[8][14] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 3, 2, 2, 2, 1, 1, 1, 1, 2, 2, 2, 3, 1},
    {1, 2, 1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 2, 1},
    {0, 2, 1, 1, 2, 1, 4, 4, 1, 2, 1, 1, 2, 0}, // Tunnel at row 3
    {1, 2, 2, 2, 2, 1, 0, 0, 1, 2, 2, 2, 2, 1},
    {1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1},
    {1, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
  };

  const char* fruitNames[5] = {"CHERRY", "STRAWBERRY", "ORANGE", "APPLE", "KEY"};
  const int fruitScores[5] = {100, 300, 500, 700, 1000};

  uint8_t maze[8][14];
  int score = 0;
  int lives = 3;
  int stage = 1;

  // Directions: 0: NONE, 1: UP, 2: RIGHT, 3: DOWN, 4: LEFT
  const int8_t dx[] = {0, 0, 1, 0, -1};
  const int8_t dy[] = {0, -1, 0, 1, 0};

  while (lives > 0) {
    // Stage Intro Screen
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(35, 12);
    display.print("STAGE "); display.print(stage);
    display.setCursor(20, 28);
    display.print(fruitNames[min(stage - 1, 4)]);
    display.setCursor(42, 45);
    display.print("READY!");
    display.display();
    delay(1400);

    // Initialize Maze & Pellets
    memcpy(maze, baseMaze, sizeof(maze));
    int pelletsLeft = 0;
    for (int r = 0; r < 8; r++)
      for (int c = 0; c < 14; c++)
        if (maze[r][c] == 2 || maze[r][c] == 3) pelletsLeft++;

    int totalPellets = pelletsLeft;
    bool fruitSpawned = false;
    bool fruitActive = false;
    unsigned long fruitExpire = 0;

    // Pac-Man state (Col, Row, Sub-pixel 0..7, Dir, NextDir)
    int pacCol = 6, pacRow = 5;
    int pacSubX = 0, pacSubY = 0;
    int pacDir = 4, pacNextDir = 4;

    // Ghosts
    struct Ghost {
      int col, row;
      int subX, subY;
      int dir;
      bool frightened;
      bool isEyes;
      unsigned long releaseTime;
    };

    Ghost ghosts[2];
    ghosts[0] = {6, 3, 0, 0, 1, false, false, 0};                 // Blinky (Red)
    ghosts[1] = {7, 3, 0, 0, 1, false, false, millis() + 3000};   // Pinky (Pink)

    unsigned long frightenedEnd = 0;
    unsigned long scatterTimer = millis() + 6000;
    bool scatterMode = true;

    auto resetPositions = [&]() {
      pacCol = 6; pacRow = 5; pacSubX = 0; pacSubY = 0; pacDir = 4; pacNextDir = 4;
      ghosts[0] = {6, 3, 0, 0, 1, false, false, 0};
      ghosts[1] = {7, 3, 0, 0, 1, false, false, millis() + 2500};
    };

    while (digitalRead(OK_BUTTON) == LOW || digitalRead(UP_BUTTON) == LOW) delay(10);
    bool stageRunning = true;
    uint8_t tickCounter = 0;

    while (stageRunning && lives > 0 && pelletsLeft > 0) {
      tickCounter++;

      // 1. Controls
      if (digitalRead(UP_BUTTON) == LOW) pacNextDir = 1;
      else if (digitalRead(RIGHT_BUTTON) == LOW) pacNextDir = 2;
      else if (digitalRead(DOWN_BUTTON) == LOW) pacNextDir = 3;
      else if (digitalRead(LEFT_BUTTON) == LOW) pacNextDir = 4;

      // Instant 180 reversal
      if ((pacDir == 1 && pacNextDir == 3) || (pacDir == 3 && pacNextDir == 1) ||
          (pacDir == 2 && pacNextDir == 4) || (pacDir == 4 && pacNextDir == 2)) {
        pacDir = pacNextDir;
      }

      // Scatter / Chase cycle
      if (millis() > scatterTimer && millis() >= frightenedEnd) {
        scatterMode = !scatterMode;
        scatterTimer = millis() + (scatterMode ? 5000 : 12000);
      }

      // Fruit spawn check (spawns when ~20 pellets eaten)
      if (!fruitSpawned && (totalPellets - pelletsLeft >= 20)) {
        fruitSpawned = true;
        fruitActive = true;
        fruitExpire = millis() + 10000; // lasts 10s
      }
      if (fruitActive && millis() > fruitExpire) fruitActive = false;

      // 2. Move Pac-Man (Sub-pixel motion)
      if (pacSubX == 0 && pacSubY == 0) {
        // At exact tile center
        // Tunnel Wrap
        if (pacRow == 3) {
          if (pacCol < 0) { pacCol = 13; pacSubX = 0; }
          else if (pacCol > 13) { pacCol = 0; pacSubX = 0; }
        }

        // Eat Pellet
        if (maze[pacRow][pacCol] == 2) {
          maze[pacRow][pacCol] = 0;
          score += 10;
          pelletsLeft--;
        } else if (maze[pacRow][pacCol] == 3) {
          maze[pacRow][pacCol] = 0;
          score += 50;
          pelletsLeft--;
          frightenedEnd = millis() + max(3000, 8000 - stage * 800);
          ghosts[0].frightened = true;
          ghosts[1].frightened = true;
        }

        // Eat Fruit
        if (fruitActive && pacRow == 4 && pacCol == 6) {
          fruitActive = false;
          score += fruitScores[min(stage - 1, 4)];
        }

        // Try buffered turn
        int nextC = pacCol + dx[pacNextDir];
        int nextR = pacRow + dy[pacNextDir];
        if (nextC >= 0 && nextC < 14 && nextR >= 0 && nextR < 8 && maze[nextR][nextC] != 1) {
          pacDir = pacNextDir;
        }

        // Forward wall check
        int fwdC = pacCol + dx[pacDir];
        int fwdR = pacRow + dy[pacDir];
        if (fwdC < 0 || fwdC >= 14 || fwdR < 0 || fwdR >= 8 || maze[fwdR][fwdC] == 1) {
          pacDir = 0; // Stop
        }
      }

      // Step Pac-Man
      if (pacDir != 0) {
        if (pacDir == 1) { if (pacSubY > 0) pacSubY--; else { pacRow--; pacSubY = 7; } }
        else if (pacDir == 3) { if (pacSubY < 7) pacSubY++; else { pacRow++; pacSubY = 0; } }
        else if (pacDir == 4) { if (pacSubX > 0) pacSubX--; else { pacCol--; pacSubX = 7; } }
        else if (pacDir == 2) { if (pacSubX < 7) pacSubX++; else { pacCol++; pacSubX = 0; } }
      }

      int pacPixelX = pacCol * 8 + pacSubX;
      int pacPixelY = pacRow * 8 + pacSubY;

      // Frightened expiry
      if (millis() >= frightenedEnd) {
        ghosts[0].frightened = false;
        ghosts[1].frightened = false;
      }

      // 3. Move Ghosts
      for (int g = 0; g < 2; g++) {
        if (millis() < ghosts[g].releaseTime) continue;

        // Frightened ghosts move every 2nd tick (half speed)
        if (ghosts[g].frightened && !ghosts[g].isEyes && (tickCounter % 2 != 0)) continue;

        if (ghosts[g].subX == 0 && ghosts[g].subY == 0) {
          // At tile center -> decide direction
          int targetC = pacCol;
          int targetR = pacRow;

          if (ghosts[g].isEyes) {
            targetC = 6; targetR = 3; // Pen door
            if (ghosts[g].col == 6 && ghosts[g].row == 3) {
              ghosts[g].isEyes = false;
              ghosts[g].frightened = false;
            }
          } else if (ghosts[g].frightened) {
            // Flee: run to opposite of Pac-Man
            targetC = 13 - pacCol;
            targetR = 7 - pacRow;
          } else if (scatterMode) {
            targetC = (g == 0 ? 12 : 1);
            targetR = 1;
          } else if (g == 1) {
            // Pinky ambushes 3 tiles ahead
            targetC = constrain(pacCol + dx[pacDir] * 3, 1, 12);
            targetR = constrain(pacRow + dy[pacDir] * 3, 1, 6);
          }

          int opp = (ghosts[g].dir == 1 ? 3 : ghosts[g].dir == 2 ? 4 : ghosts[g].dir == 3 ? 1 : 2);
          int bestD = ghosts[g].dir;
          long bestDist = 999999;

          for (int d = 1; d <= 4; d++) {
            if (d == opp) continue; // No 180 reversal at intersections
            int nc = ghosts[g].col + dx[d];
            int nr = ghosts[g].row + dy[d];

            if (nc >= 0 && nc < 14 && nr >= 0 && nr < 8) {
              if (maze[nr][nc] == 1) continue; // Wall
              if (maze[nr][nc] == 4 && !ghosts[g].isEyes) continue; // Door

              long dist = (nc - targetC) * (nc - targetC) + (nr - targetR) * (nr - targetR);
              if (dist < bestDist) {
                bestDist = dist;
                bestD = d;
              }
            }
          }
          ghosts[g].dir = bestD;
        }

        // Step Ghost
        int gSteps = ghosts[g].isEyes ? 2 : 1;
        for (int s = 0; s < gSteps; s++) {
          if (ghosts[g].dir == 1) { if (ghosts[g].subY > 0) ghosts[g].subY--; else { ghosts[g].row--; ghosts[g].subY = 7; } }
          else if (ghosts[g].dir == 3) { if (ghosts[g].subY < 7) ghosts[g].subY++; else { ghosts[g].row++; ghosts[g].subY = 0; } }
          else if (ghosts[g].dir == 4) { if (ghosts[g].subX > 0) ghosts[g].subX--; else { ghosts[g].col--; ghosts[g].subX = 7; } }
          else if (ghosts[g].dir == 2) { if (ghosts[g].subX < 7) ghosts[g].subX++; else { ghosts[g].col++; ghosts[g].subX = 0; } }
        }

        int gPixelX = ghosts[g].col * 8 + ghosts[g].subX;
        int gPixelY = ghosts[g].row * 8 + ghosts[g].subY;

        // Collision with Pac-Man
        if (abs(pacPixelX - gPixelX) <= 5 && abs(pacPixelY - gPixelY) <= 5) {
          if (ghosts[g].frightened && !ghosts[g].isEyes) {
            score += 200;
            ghosts[g].isEyes = true;
            ghosts[g].frightened = false;
          } else if (!ghosts[g].isEyes) {
            // Death
            lives--;
            // Death animation
            for (int r = 4; r >= 0; r--) {
              display.clearDisplay();
              display.drawCircle(pacPixelX + 4, pacPixelY + 4, r, SSD1306_WHITE);
              display.display();
              delay(80);
            }
            delay(500);
            resetPositions();
            break;
          }
        }
      }

      // 4. Render
      display.clearDisplay();

      // Maze tiles
      for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 14; c++) {
          int px = c * 8, py = r * 8;
          if (maze[r][c] == 1) {
            display.drawRect(px, py, 8, 8, SSD1306_WHITE);
          } else if (maze[r][c] == 2) {
            display.fillRect(px + 3, py + 3, 2, 2, SSD1306_WHITE);
          } else if (maze[r][c] == 3) {
            if ((millis() / 180) % 2 == 0) display.fillRect(px + 2, py + 2, 4, 4, SSD1306_WHITE);
          } else if (maze[r][c] == 4) {
            display.drawFastHLine(px, py + 4, 8, SSD1306_WHITE);
          }
        }
      }

      // Fruit Bonus
      if (fruitActive) {
        display.fillRect(48 + 2, 32 + 2, 4, 4, SSD1306_WHITE);
        display.drawPixel(48 + 4, 32 + 1, SSD1306_WHITE); // Stem
      }

      // Pac-Man Sprite
      bool mouthOpen = (tickCounter / 3) % 2;
      if (mouthOpen && pacDir != 0) {
        const unsigned char* pBmp = pacman_right;
        if (pacDir == 4) pBmp = pacman_left;
        else if (pacDir == 1) pBmp = pacman_up;
        else if (pacDir == 3) pBmp = pacman_down;
        display.drawBitmap(pacPixelX, pacPixelY, pBmp, 8, 8, SSD1306_WHITE);
      } else {
        display.fillCircle(pacPixelX + 4, pacPixelY + 4, 3, SSD1306_WHITE);
      }

      // Ghost Sprites
      for (int g = 0; g < 2; g++) {
        int gx = ghosts[g].col * 8 + ghosts[g].subX;
        int gy = ghosts[g].row * 8 + ghosts[g].subY;

        if (ghosts[g].isEyes) {
          display.drawPixel(gx + 2, gy + 3, SSD1306_WHITE);
          display.drawPixel(gx + 5, gy + 3, SSD1306_WHITE);
        } else if (ghosts[g].frightened) {
          bool flash = (frightenedEnd - millis() < 2000) && ((millis() / 150) % 2 == 0);
          display.drawBitmap(gx, gy, flash ? ghost_sprite : ghost_scared, 8, 8, SSD1306_WHITE);
        } else {
          display.drawBitmap(gx, gy, ghost_sprite, 8, 8, SSD1306_WHITE);
        }
      }

      // Right HUD Bar
      display.drawFastVLine(112, 0, 64, SSD1306_WHITE);
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(115, 2);
      display.print("S");
      display.setCursor(115, 12);
      display.print(score);

      display.setCursor(115, 30);
      display.print("L:"); display.print(lives);

      display.setCursor(115, 48);
      if (millis() < frightenedEnd) display.print("PWR");
      else display.print(scatterMode ? "SCT" : "CHS");

      display.display();
      delay(20);
    }

    if (pelletsLeft == 0) {
      stage++;
      display.clearDisplay();
      display.setCursor(15, 25);
      display.print("STAGE CLEARED!");
      display.display();
      delay(1500);
    }
  }

  gameOver(score);
  inGame = false;
}
// ===================== GAME 16: FROGGER (AUTHENTIC ARCADE BALANCED) =====================
void froggerGame() {
  int frogX = 60;
  int frogY = 56;
  int frogDir = 0; // 0: UP, 1: RIGHT, 2: DOWN, 3: LEFT
  int score = 0;
  int lives = 3;
  int level = 1;

  // 5 Goal Bays at Top Riverbank
  bool goalBays[5] = {false, false, false, false, false};
  const int bayX[5] = {8, 34, 60, 86, 112};

  // Obstacle Positions: 2 items per lane, spaced 64px apart (generous 50px safe gap!)
  float laneOffset[5] = {0, 32, 10, 20, 50};

  unsigned long lifeStart = millis();

  auto resetFrog = [&]() {
    frogX = 60;
    frogY = 56;
    frogDir = 0;
    lifeStart = millis();
  };

  while (digitalRead(OK_BUTTON) == LOW || digitalRead(UP_BUTTON) == LOW) delay(10);
  bool running = true;

  while (running && lives > 0) {
    // Stage Intro / Level Display on new stage
    int timeLimit = max(18, 32 - level * 3);

    // Speed tuned by level: steady, fair, never rushing crazy fast
    float speedLog1 = 0.45 + (level * 0.08); // Lane 0: Right
    float speedTurtle = -0.55 - (level * 0.08); // Lane 1: Left
    float speedLog2 = 0.50 + (level * 0.08); // Lane 2: Right
    float speedCar = -0.65 - (level * 0.10); // Lane 3: Left (Fair, playable pace)
    float speedTruck = 0.40 + (level * 0.06); // Lane 4: Right (Slow, majestic truck)

    // Controls: 8px hops
    if (digitalRead(UP_BUTTON) == LOW) { frogY -= 8; frogDir = 0; delay(140); }
    else if (digitalRead(DOWN_BUTTON) == LOW && frogY < 56) { frogY += 8; frogDir = 2; delay(140); }
    else if (digitalRead(LEFT_BUTTON) == LOW && frogX > 2) { frogX -= 8; frogDir = 3; delay(140); }
    else if (digitalRead(RIGHT_BUTTON) == LOW && frogX < 118) { frogX += 8; frogDir = 1; delay(140); }

    // Advance lane offsets
    laneOffset[0] = fmod(laneOffset[0] + speedLog1 + 128.0, 128.0);
    laneOffset[1] = fmod(laneOffset[1] + speedTurtle + 128.0, 128.0);
    laneOffset[2] = fmod(laneOffset[2] + speedLog2 + 128.0, 128.0);
    laneOffset[3] = fmod(laneOffset[3] + speedCar + 128.0, 128.0);
    laneOffset[4] = fmod(laneOffset[4] + speedTruck + 128.0, 128.0);

    // Countdown Timer
    int elapsed = (millis() - lifeStart) / 1000;
    int secondsLeft = timeLimit - elapsed;
    if (secondsLeft <= 0) {
      lives--;
      resetFrog();
      delay(500);
      continue;
    }

    // ----------------- RIVER PHYSICS (Y = 11 to 28) -----------------
    if (frogY >= 11 && frogY <= 28) {
      bool onPlatform = false;
      float drift = 0;

      // Lane 0: Big Logs (Y=12, width=30, 2 logs at 64px interval)
      if (frogY <= 14) {
        for (int k = 0; k < 2; k++) {
          int lx = (int)fmod(laneOffset[0] + k * 64.0, 128.0);
          if (frogX + 5 > lx && frogX < lx + 30) {
            onPlatform = true;
            drift = speedLog1;
            break;
          }
        }
      }
      // Lane 1: Turtles (Y=20, width=18, 2 turtle groups at 64px interval)
      else if (frogY <= 22) {
        for (int k = 0; k < 2; k++) {
          int tx = (int)fmod(laneOffset[1] + k * 64.0, 128.0);
          if (frogX + 5 > tx && frogX < tx + 18) {
            onPlatform = true;
            drift = speedTurtle;
            break;
          }
        }
      }
      // Lane 2: Medium Logs (Y=28, width=26, 2 logs at 64px interval)
      else {
        for (int k = 0; k < 2; k++) {
          int lx = (int)fmod(laneOffset[2] + k * 64.0, 128.0);
          if (frogX + 5 > lx && frogX < lx + 26) {
            onPlatform = true;
            drift = speedLog2;
            break;
          }
        }
      }

      if (onPlatform) {
        frogX += (int)drift;
        if (frogX < 0 || frogX > 120) {
          lives--;
          resetFrog();
          delay(500);
          continue;
        }
      } else {
        // Drown in River!
        lives--;
        // Water splash animation
        for (int r = 1; r <= 5; r += 2) {
          display.drawCircle(frogX + 4, frogY + 4, r, SSD1306_WHITE);
          display.display();
          delay(60);
        }
        resetFrog();
        delay(400);
        continue;
      }
    }

    // ----------------- HIGHWAY ROADWAY (Y = 40 to 52) -----------------
    if (frogY >= 40 && frogY <= 52) {
      bool hit = false;
      // Lane 3: Fast Cars (Y=40, width=12, moving LEFT, 2 cars at 64px interval)
      if (frogY <= 44) {
        for (int k = 0; k < 2; k++) {
          int cx = (int)fmod(laneOffset[3] + k * 64.0, 128.0);
          if (frogX + 6 > cx && frogX + 2 < cx + 12) { hit = true; break; }
        }
      }
      // Lane 4: Cargo Trucks (Y=48, width=16, moving RIGHT, 2 trucks at 64px interval)
      else {
        for (int k = 0; k < 2; k++) {
          int tx = (int)fmod(laneOffset[4] + k * 64.0, 128.0);
          if (frogX + 6 > tx && frogX + 2 < tx + 16) { hit = true; break; }
        }
      }

      if (hit) {
        lives--;
        // Squish delay
        display.drawLine(frogX, frogY + 4, frogX + 8, frogY + 4, SSD1306_WHITE);
        display.display();
        delay(600);
        resetFrog();
        continue;
      }
    }

    // ----------------- GOAL BAY CHECK (Y <= 6) -----------------
    if (frogY <= 6) {
      int reachedBay = -1;
      for (int b = 0; b < 5; b++) {
        if (abs(frogX - bayX[b]) <= 6) { reachedBay = b; break; }
      }

      if (reachedBay != -1 && !goalBays[reachedBay]) {
        goalBays[reachedBay] = true;
        score += 250;
        resetFrog();

        bool allFilled = true;
        for (int b = 0; b < 5; b++) { if (!goalBays[b]) allFilled = false; }
        if (allFilled) {
          score += 1000;
          level++;
          for (int b = 0; b < 5; b++) goalBays[b] = false;
          display.clearDisplay();
          display.setCursor(20, 25);
          display.print("STAGE CLEARED!");
          display.display();
          delay(1400);
        }
      } else {
        lives--;
        resetFrog();
        delay(400);
      }
      continue;
    }

    // ----------------- RENDER SCREEN -----------------
    display.clearDisplay();

    // 1. Goal Riverbank (5 Bays)
    display.fillRect(0, 0, 128, 9, SSD1306_WHITE);
    for (int b = 0; b < 5; b++) {
      display.fillRect(bayX[b] - 2, 0, 12, 8, SSD1306_BLACK);
      if (goalBays[b]) {
        // Frog in bay
        display.fillRect(bayX[b] + 2, 2, 4, 4, SSD1306_WHITE);
      }
    }

    // 2. River Zone (Water with ripple pixels)
    for (int y = 14; y <= 28; y += 7) {
      for (int x = 4; x < 128; x += 20) {
        display.drawPixel(x, y, SSD1306_WHITE);
        display.drawPixel(x + 1, y, SSD1306_WHITE);
      }
    }

    // Lane 0: 2 Big Logs (moving Right)
    for (int k = 0; k < 2; k++) {
      int lx = (int)fmod(laneOffset[0] + k * 64.0, 128.0);
      display.fillRect(lx, 11, 30, 6, SSD1306_WHITE);
      display.drawPixel(lx + 1, 12, SSD1306_BLACK);
      display.drawPixel(lx + 28, 12, SSD1306_BLACK);
      display.drawFastHLine(lx + 4, 13, 22, SSD1306_BLACK);
    }

    // Lane 1: 2 Turtle Groups (moving Left)
    for (int k = 0; k < 2; k++) {
      int tx = (int)fmod(laneOffset[1] + k * 64.0, 128.0);
      display.fillCircle(tx + 4, 20, 3, SSD1306_WHITE);
      display.fillCircle(tx + 13, 20, 3, SSD1306_WHITE);
      display.drawPixel(tx - 1, 20, SSD1306_WHITE); // Head
    }

    // Lane 2: 2 Medium Logs (moving Right)
    for (int k = 0; k < 2; k++) {
      int lx = (int)fmod(laneOffset[2] + k * 64.0, 128.0);
      display.fillRect(lx, 26, 26, 6, SSD1306_WHITE);
      display.drawFastHLine(lx + 3, 28, 18, SSD1306_BLACK);
    }

    // 3. Center Safety Median (Cobblestones - Safe Zone)
    for (int x = 0; x < 128; x += 6) {
      display.drawPixel(x, 34, SSD1306_WHITE);
      display.drawPixel(x + 3, 36, SSD1306_WHITE);
    }

    // 4. Highway Roadway
    // Lane 3: 2 Sports Cars (facing & moving LEFT, 52px safe gap!)
    for (int k = 0; k < 2; k++) {
      int cx = (int)fmod(laneOffset[3] + k * 64.0, 128.0);
      display.fillRect(cx + 2, 40, 10, 5, SSD1306_WHITE);
      display.fillRect(cx, 42, 3, 2, SSD1306_WHITE); // Nose/headlight pointing LEFT
      display.drawPixel(cx + 3, 45, SSD1306_WHITE);  // Wheels
      display.drawPixel(cx + 9, 45, SSD1306_WHITE);
    }

    // Road dashed dividing line
    for (int x = 0; x < 128; x += 12) {
      display.drawFastHLine(x, 46, 6, SSD1306_WHITE);
    }

    // Lane 4: 2 Cargo Trucks (facing & moving RIGHT, 48px safe gap!)
    for (int k = 0; k < 2; k++) {
      int tx = (int)fmod(laneOffset[4] + k * 64.0, 128.0);
      display.fillRect(tx, 48, 12, 6, SSD1306_WHITE);     // Cargo bed
      display.fillRect(tx + 12, 50, 4, 4, SSD1306_WHITE); // Cab on right
      display.drawPixel(tx + 2, 54, SSD1306_WHITE);       // Wheels
      display.drawPixel(tx + 10, 54, SSD1306_WHITE);
      display.drawPixel(tx + 14, 54, SSD1306_WHITE);
    }

    // 5. Starting Sidewalk
    display.drawFastHLine(0, 55, 128, SSD1306_WHITE);

    // Draw Directional Frog
    int fx = frogX, fy = frogY;
    display.fillRect(fx + 2, fy + 2, 4, 4, SSD1306_WHITE);
    if (frogDir == 0) { // UP
      display.drawPixel(fx + 1, fy + 1, SSD1306_WHITE); display.drawPixel(fx + 6, fy + 1, SSD1306_WHITE);
      display.drawPixel(fx, fy + 5, SSD1306_WHITE); display.drawPixel(fx + 7, fy + 5, SSD1306_WHITE);
    } else if (frogDir == 2) { // DOWN
      display.drawPixel(fx + 1, fy + 6, SSD1306_WHITE); display.drawPixel(fx + 6, fy + 6, SSD1306_WHITE);
      display.drawPixel(fx, fy + 2, SSD1306_WHITE); display.drawPixel(fx + 7, fy + 2, SSD1306_WHITE);
    } else if (frogDir == 3) { // LEFT
      display.drawPixel(fx + 1, fy + 1, SSD1306_WHITE); display.drawPixel(fx + 1, fy + 6, SSD1306_WHITE);
      display.drawPixel(fx + 5, fy, SSD1306_WHITE); display.drawPixel(fx + 5, fy + 7, SSD1306_WHITE);
    } else { // RIGHT
      display.drawPixel(fx + 6, fy + 1, SSD1306_WHITE); display.drawPixel(fx + 6, fy + 6, SSD1306_WHITE);
      display.drawPixel(fx + 2, fy, SSD1306_WHITE); display.drawPixel(fx + 2, fy + 7, SSD1306_WHITE);
    }

    // Bottom HUD
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(2, 56);
    display.print(secondsLeft); display.print("s");
    display.setCursor(45, 56);
    display.print("Lv:"); display.print(level);
    display.setCursor(95, 56);
    display.print("L:"); display.print(lives);

    display.display();
    delay(20);
  }

  gameOver(score);
  inGame = false;
}
// ===================== GAME 17: HELICOPTER =====================
// FIX (#11): the single floating obstacle block felt random and repetitive.
// Added the classic paired top+bottom "gate" obstacle (like the original
// 2000s Helicopter game / Flappy-style pipes) alongside the wavy cave
// terrain, so there are two distinct hazards to react to instead of one.
void helicopterGame() {
  float copterY = 32;
  float copterVY = 0;
  const float gravity = 0.35;
  const float lift = -0.7;

  int roof[16];
  int floor[16];
  for (int i=0; i<16; i++) {
    roof[i] = random(4, 15);
    floor[i] = random(48, 58);
  }

  // Paired gate obstacle: a gap the copter must fly through
  float gateX = SCREEN_WIDTH + 40;
  int gateGapY = 28;
  int gateGapH = 20;
  const int gateWidth = 6;

  int score = 0;
  bool playing = true;
  float segmentOffset = 0;
  float gameSpeed = 2.0;

  while (playing) {
    bool fly = (digitalRead(OK_BUTTON) == LOW || digitalRead(UP_BUTTON) == LOW);
    if (fly) copterVY += lift;
    else copterVY += gravity;

    copterVY = constrain(copterVY, -4.0, 4.0);
    copterY += copterVY;

    segmentOffset += gameSpeed;
    if (segmentOffset >= 8.0) {
      segmentOffset = 0;
      for (int i=0; i<15; i++) {
        roof[i] = roof[i+1];
        floor[i] = floor[i+1];
      }
      roof[15] = random(4, 15);
      floor[15] = random(48, 58);
      score++;
    }

    gateX -= gameSpeed;
    if (gateX < -gateWidth) {
      gateX = SCREEN_WIDTH + random(40, 100);
      gateGapY = random(12, 34);
      gateGapH = max(14, 22 - score / 8); // gap tightens slightly as score grows
    }

    int segIdx = (20 + (int)segmentOffset) / 8;
    if (copterY < roof[segIdx] || copterY + 6 > floor[segIdx]) {
      playing = false; 
    }

    if (gateX < 28 && gateX + gateWidth > 20) {
      if (copterY < gateGapY || copterY + 6 > gateGapY + gateGapH) {
        playing = false;
      }
    }

    display.clearDisplay();

    for (int i=0; i<16; i++) {
      int tx = i * 8 - (int)segmentOffset;
      display.fillRect(tx, 0, 8, roof[i], SSD1306_WHITE);
      display.fillRect(tx, floor[i], 8, SCREEN_HEIGHT - floor[i], SSD1306_WHITE);
    }

    display.fillRect((int)gateX, 0, gateWidth, gateGapY, SSD1306_WHITE);
    display.fillRect((int)gateX, gateGapY + gateGapH, gateWidth, SCREEN_HEIGHT - (gateGapY + gateGapH), SSD1306_WHITE);

    int cy = (int)copterY;
    display.fillRect(20, cy + 2, 8, 3, SSD1306_WHITE); 
    display.drawLine(18, cy + 1, 23, cy + 1, SSD1306_WHITE); 
    display.drawLine(20, cy + 5, 26, cy + 5, SSD1306_WHITE); 

    display.setCursor(2, 2);
    display.print(score);

    display.display();
    delay(20);
  }

  gameOver(score);
  inGame = false;
}

// ===================== OLED BRIGHTNESS CONTROL =====================
void setOledBrightness(uint8_t contrast) {
  if (contrast < 20) contrast = 20; // Safe minimum limit: screen will NEVER go pitch black/0
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(contrast);
}

void brightnessMenu() {
  EEPROM.begin(64);
  int brightness = EEPROM.read(1);
  if (brightness < 20 || brightness > 255) brightness = 150;

  waitForRelease();
  bool saved = false;

  while (!saved) {
    if (digitalRead(LEFT_BUTTON) == LOW || digitalRead(DOWN_BUTTON) == LOW) {
      brightness -= 15;
      if (brightness < 20) brightness = 20; // Hard limit: never 0
      setOledBrightness(brightness);
      delay(120);
    }
    if (digitalRead(RIGHT_BUTTON) == LOW || digitalRead(UP_BUTTON) == LOW) {
      brightness += 15;
      if (brightness > 255) brightness = 255;
      setOledBrightness(brightness);
      delay(120);
    }
    if (digitalRead(OK_BUTTON) == LOW) {
      EEPROM.write(1, brightness);
      EEPROM.commit();
      saved = true;
      waitForRelease();
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(35, 25);
      display.print("SAVED!");
      display.display();
      delay(800);
      break;
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(18, 4);
    display.print("OLED BRIGHTNESS");
    display.drawFastHLine(0, 14, 128, SSD1306_WHITE);

    int pct = map(brightness, 20, 255, 10, 100);
    display.setCursor(48, 20);
    display.print(pct);
    display.print(" %");

    display.drawRect(14, 33, 100, 12, SSD1306_WHITE);
    int barW = map(brightness, 20, 255, 2, 96);
    display.fillRect(16, 35, barW, 8, SSD1306_WHITE);

    display.setCursor(4, 52);
    display.print("<-/-> Adj  OK: Save");

    display.display();
    delay(20);
  }
}

// ===================== ON-SCREEN KEYBOARD (QWERTY VERSION) =====================
String showVirtualKeyboard(const String& title) {
  String pass = "";
  const char* const kbdRows[] = {
    "1234567890",
    "qwertyuiop",
    "asdfghjkl-",
    "zxcvbnm._@"
  };

  int kbdRow = 0;
  int kbdCol = 0;
  bool caps = false;
  bool done = false;

  waitForRelease();

  while (!done) {
    if (digitalRead(UP_BUTTON) == LOW) {
      if (kbdRow > 0) kbdRow--;
      delay(150);
    }
    if (digitalRead(DOWN_BUTTON) == LOW) {
      if (kbdRow < 4) {
        kbdRow++;
        if (kbdRow == 4) kbdCol = map(kbdCol, 0, 9, 0, 3);
      }
      delay(150);
    }
    if (digitalRead(LEFT_BUTTON) == LOW) {
      if (kbdRow < 4) kbdCol = (kbdCol - 1 + 10) % 10;
      else kbdCol = (kbdCol - 1 + 4) % 4;
      delay(150);
    }
    if (digitalRead(RIGHT_BUTTON) == LOW) {
      if (kbdRow < 4) kbdCol = (kbdCol + 1) % 10;
      else kbdCol = (kbdCol + 1) % 4;
      delay(150);
    }

    if (digitalRead(OK_BUTTON) == LOW) {
      if (kbdRow < 4) {
        char ch = kbdRows[kbdRow][kbdCol];
        if (caps && ch >= 'a' && ch <= 'z') ch -= 32;
        if (pass.length() < 32) pass += ch;
      } else {
        if (kbdCol == 0) caps = !caps;
        else if (kbdCol == 1) {
          if (pass.length() > 0) pass.remove(pass.length() - 1);
        } else if (kbdCol == 2) {
          if (pass.length() < 32) pass += ' ';
        } else if (kbdCol == 3) {
          done = true;
        }
      }
      delay(180);
      waitForRelease();
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    String topTitle = title;
    if (topTitle.length() > 18) topTitle = topTitle.substring(0, 16) + "..";
    display.print(topTitle);

    display.setCursor(0, 10);
    display.print("Pass:");
    if (pass.length() <= 14) {
      display.print(pass);
    } else {
      display.print(".." + pass.substring(pass.length() - 12));
    }
    display.print("_");

    for (int r = 0; r < 4; r++) {
      int y = 20 + r * 9;
      for (int c = 0; c < 10; c++) {
        int x = 4 + c * 12;
        char ch = kbdRows[r][c];
        if (caps && ch >= 'a' && ch <= 'z') ch -= 32;

        if (kbdRow == r && kbdCol == c) {
          display.fillRect(x - 1, y - 1, 10, 9, SSD1306_WHITE);
          display.setTextColor(SSD1306_BLACK);
          display.setCursor(x, y);
          display.print(ch);
          display.setTextColor(SSD1306_WHITE);
        } else {
          display.setCursor(x, y);
          display.print(ch);
        }
      }
    }

    const char* actLabels[4] = {"CAP", "DEL", "SPC", "DONE"};
    int actX[4] = {2, 34, 66, 98};
    for (int a = 0; a < 4; a++) {
      if (kbdRow == 4 && kbdCol == a) {
        display.fillRect(actX[a], 56, 28, 8, SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK);
        display.setCursor(actX[a] + 2, 56);
        display.print(actLabels[a]);
        display.setTextColor(SSD1306_WHITE);
      } else {
        display.drawRect(actX[a], 56, 28, 8, SSD1306_WHITE);
        display.setCursor(actX[a] + 2, 56);
        display.print(actLabels[a]);
      }
    }

    display.display();
    delay(20);
  }

  waitForRelease();
  return pass;
}

// ===================== USE AS REPEATER =====================
void wifiRepeaterMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println("Scanning WiFi...");
  display.setCursor(10, 35);
  display.println("Please wait...");
  display.display();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  int n = WiFi.scanNetworks();
  if (n == 0) {
    display.clearDisplay();
    display.setCursor(10, 20);
    display.println("No WiFi Found!");
    display.setCursor(10, 40);
    display.println("Press OK...");
    display.display();
    waitForRelease();
    while (digitalRead(OK_BUTTON) != LOW) delay(10);
    waitForRelease();
    return;
  }

  if (n > 20) n = 20;

  int selected = 0;
  int topIndex = 0;
  bool chosen = false;

  waitForRelease();
  while (!chosen) {
    if (digitalRead(UP_BUTTON) == LOW) {
      selected = (selected - 1 + n) % n;
      delay(180);
    }
    if (digitalRead(DOWN_BUTTON) == LOW) {
      selected = (selected + 1) % n;
      delay(180);
    }
    if (digitalRead(OK_BUTTON) == LOW) {
      chosen = true;
      waitForRelease();
      break;
    }
    if (digitalRead(LEFT_BUTTON) == LOW) {
      waitForRelease();
      return;
    }

    if (selected < topIndex) topIndex = selected;
    else if (selected >= topIndex + 5) topIndex = selected - 4;

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Select WiFi ("); display.print(n); display.print("):");

    for (int i = 0; i < 5; i++) {
      int idx = topIndex + i;
      if (idx >= n) break;
      int y = 12 + i * 10;
      if (idx == selected) {
        display.fillRect(0, y - 1, 128, 9, SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK);
      } else {
        display.setTextColor(SSD1306_WHITE);
      }
      display.setCursor(2, y);
      String ssid = WiFi.SSID(idx);
      if (ssid.length() > 10) ssid = ssid.substring(0, 9) + ".";
      display.print(ssid);

      int32_t rssi = WiFi.RSSI(idx);
      display.setCursor(82, y);
      display.print(rssi);
      display.print("dBm");
    }
    display.setTextColor(SSD1306_WHITE);
    display.display();
    delay(20);
  }

  String targetSSID = WiFi.SSID(selected);
  bool isEncrypted = (WiFi.encryptionType(selected) != ENC_TYPE_NONE);

  String password = "";
  if (isEncrypted) {
    password = showVirtualKeyboard(targetSSID);
  }

  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("Connecting to:");
  display.setCursor(0, 24);
  display.println(targetSSID);
  display.display();

  WiFi.begin(targetSSID.c_str(), password.c_str());
  unsigned long startConnect = millis();
  bool connected = false;
  int dots = 0;

  while (millis() - startConnect < 20000) {
    if (WiFi.status() == WL_CONNECTED) {
      connected = true;
      break;
    }
    if (digitalRead(OK_BUTTON) == LOW) {
      WiFi.disconnect();
      waitForRelease();
      return;
    }
    dots = (dots + 1) % 4;
    display.fillRect(0, 42, 128, 12, SSD1306_BLACK);
    display.setCursor(0, 42);
    display.print("Authenticating");
    for (int d = 0; d < dots; d++) display.print(".");
    display.display();
    delay(400);
  }

  if (!connected) {
    display.clearDisplay();
    display.setCursor(10, 15);
    display.println("Failed to Connect!");
    display.setCursor(10, 30);
    display.println("Check Password/Signal");
    display.setCursor(10, 48);
    display.println("Press OK...");
    display.display();
    waitForRelease();
    while (digitalRead(OK_BUTTON) != LOW) delay(10);
    waitForRelease();
    WiFi.disconnect();
    return;
  }

  display.clearDisplay();
  display.setCursor(10, 15);
  display.println("Connected!");
  display.setCursor(10, 32);
  display.println("Starting Repeater...");
  display.display();
  delay(1000);

  WiFi.mode(WIFI_AP_STA);
  #if LWIP_FEATURES && !LWIP_IPV6
  auto& server = WiFi.softAPDhcpServer();
  server.setDns(WiFi.dnsIP(0));
  #endif

  WiFi.softAPConfig(
    IPAddress(172, 217, 28, 254),
    IPAddress(172, 217, 28, 254),
    IPAddress(255, 255, 255, 0)
  );
  WiFi.softAP("node", "password");

  #if LWIP_FEATURES && !LWIP_IPV6
  err_t ret = ip_napt_init(NAPT, NAPT_PORT);
  if (ret == ERR_OK) {
    ip_napt_enable_no(SOFTAP_IF, 1);
  }
  #endif

  waitForRelease();

  while (true) {
    if (digitalRead(OK_BUTTON) == LOW) {
      waitForRelease();
      break;
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(2, 0);
    display.print("REPEATER ACTIVE [ON]");
    display.drawFastHLine(0, 10, 128, SSD1306_WHITE);

    display.setCursor(2, 13);
    display.print("STA:");
    display.print(targetSSID.substring(0, 8));
    display.print(" ");
    display.print(WiFi.RSSI());
    display.print("dBm");

    display.setCursor(2, 24);
    display.print("Hotspot: node");

    display.setCursor(2, 35);
    display.print("Pass:    password");

    display.setCursor(2, 46);
    display.print("Clients: ");
    display.print(WiFi.softAPgetStationNum());
    display.print(" connected");

    display.setCursor(2, 56);
    display.print("Press OK to Stop");

    display.display();
    delay(200);
  }

  WiFi.softAPdisconnect(true);
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  display.clearDisplay();
  display.setCursor(15, 25);
  display.println("Repeater Stopped");
  display.display();
  delay(1000);
}

// ===================== SETTINGS SUBMENU =====================
void settingsMenu() {
  const char* settingsOptions[] = {
    "1. Brightness",
    "2. Use as Repeater",
    "3. Back to Menu"
  };
  const int totalSettings = 3;
  int selectedOption = 0;

  waitForRelease();

  while (true) {
    if (digitalRead(UP_BUTTON) == LOW) {
      selectedOption = (selectedOption - 1 + totalSettings) % totalSettings;
      delay(180);
    }
    if (digitalRead(DOWN_BUTTON) == LOW) {
      selectedOption = (selectedOption + 1) % totalSettings;
      delay(180);
    }
    if (digitalRead(OK_BUTTON) == LOW) {
      waitForRelease();
      if (selectedOption == 0) {
        brightnessMenu();
        waitForRelease();
      } else if (selectedOption == 1) {
        wifiRepeaterMenu();
        waitForRelease();
      } else {
        break;
      }
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(2, 2);
    display.println("SETTINGS:");
    display.drawFastHLine(0, 12, 128, SSD1306_WHITE);

    for (int i = 0; i < totalSettings; i++) {
      int y = 18 + i * 14;
      if (i == selectedOption) {
        display.fillRect(0, y - 2, 128, 12, SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK);
      } else {
        display.setTextColor(SSD1306_WHITE);
      }
      display.setCursor(4, y);
      display.print(settingsOptions[i]);
    }
    display.setTextColor(SSD1306_WHITE);
    display.display();
    delay(20);
  }

  inGame = false;
}
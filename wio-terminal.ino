#include <sstream>
#include <vector>

#include "TFT_eSPI.h"
#include "Seeed_FS.h" //Including SD card library
#include "RawImage.h"  //Including image processing library
TFT_eSPI tft;

// while this flag is true, wait for button press and stay.
bool nop_flag = false;
void sleep() {
  nop_flag = true;
}
bool sleeping() {
  return nop_flag;
}

bool left_flag = false;
void button_handler_left() {
  left_flag = true;
  nop_flag = false;
}

bool right_flag = false;
void button_handler_right() {
  right_flag = true;
  nop_flag = false;
}

using ImageBase = uint16_t;
using Image = RawImage<ImageBase>;
Image* img;

char const* imagefile = "1012-rgb565.bmp";

int32_t pos_for_log() {
  int32_t const step{10};
  int32_t static pos{};

  pos += step;
  return pos;
}
void log(char const* msg) {
  tft.drawString(msg, 10, pos_for_log());
}
void log(int32_t val) {
  std::stringstream ss;
  ss << "val: " << val;
  tft.drawString(ss.str().c_str(), 10, pos_for_log());
}

void setup() {
  //Initialise LCD screen
  tft.begin();
  tft.setRotation(3);
  tft.setTextColor(TFT_DARKGREEN, TFT_WHITE);
  tft.setTextDatum(MC_DATUM);
  log("init...");

  log("button init...");
  pinMode(BUTTON_1, INPUT); //left button
  pinMode(BUTTON_3, INPUT); //right button
  attachInterrupt(digitalPinToInterrupt(BUTTON_1), button_handler_left, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_3), button_handler_right, FALLING);

  log("SD init...");
  //Initialise SD card
  if (!SD.begin(SDCARD_SS_PIN, SDCARD_SPI, 16000000)) {
    log("SD init failed");
    tft.fillScreen(TFT_RED);
    tft.drawString("no SD Card", tft.width() / 2, tft.height() / 2);
    while (1);
  }

  log("image init...");
  img = newImage<ImageBase>(imagefile, 240, 240);
  if (!img) {
    log("image init failed");
    tft.fillScreen(TFT_RED);
    tft.drawString("image init failed", tft.width() / 2, tft.height() / 2);
    while (1);
  }

  log("setup finished...");
}

bool mode = false;

std::vector<uint32_t> colors = {TFT_RED, TFT_GREEN, TFT_BLUE};

unsigned long interval = 1000;

void loop() {
  if (sleeping()) {
    delay(interval);
    return;
  }

  if (left_flag) {
    left_flag = false;
    mode = !mode;
  }

  if (mode) {
    img->draw();
    sleep();
    delay(interval);
  } else {
    size_t static c{};
    tft.fillScreen(colors[c++ % colors.size()]);
    delay(interval);
  }
}

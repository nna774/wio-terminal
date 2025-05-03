#pragma once
#include<stdint.h>
#include<SD/Seeed_SD.h>

/*
  USAGE:
    // when use 8bit color.
    Raw8 * img8 = newImage<uint8_t>("path to sd card image.", 240, 360);

    // when use 16bit color.
    Raw16 * img16 = newImage<uint16_t>("path to sd card image.", 240, 360);

    // do some drawing.
    // img8->draw();

    // remember release it
    img8->release();
    img16->release();
*/

/*
  Original RawImage.h is from Seeed Studio Wio Terminal example.
    - https://files.seeedstudio.com/wiki/Wio-Terminal/res/RawImage.h
    - https://wiki.seeedstudio.com/Wio-Terminal-LCD-Loading-Image/
  But it has some bugs. So nana@nna774.net rewrote the code based on the reference implementation.
*/

extern TFT_eSPI tft;

template<class type>
struct RawImage {
  type* ptr() { return reinterpret_cast<type*>(val_); }
  type get(int16_t x, int16_t y) { return this->ptr()[y * width() + x]; }
  void draw(size_t x = 0, size_t y = 0) { tft.pushImage(x, y, width(), height(), ptr()); }
  void release() { delete[] val_; }
  int16_t width() { return width_; }
  int16_t height() { return height_; }

  RawImage(uint8_t* val, int16_t width, int16_t height) : val_{val}, width_{width}, height_{height} {}
private:
  uint8_t* val_;
  int16_t width_;
  int16_t height_;
};

typedef RawImage<uint8_t>  Raw8;
typedef RawImage<uint16_t> Raw16;

template<class type>
RawImage<type> * newImage(const char * path, int16_t width, int16_t height) {
  typedef RawImage<type> raw;
  File f = SD.open(path, FILE_READ);
  if (!f) {
    return nullptr;
  }
  int32_t size = f.size();
  uint8_t* buf = new uint8_t[size];
   if (!buf) {
    return nullptr;
  }
  f.read(buf, size);
  f.close();

  return new raw(buf, width, height);
}

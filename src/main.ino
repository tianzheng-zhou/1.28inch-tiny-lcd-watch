#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"
#include <Arduino.h>
#include <string>
#include <math.h>


#include "figures/figure1.h"
#include "figures/figure2.h"

hw_timer_t *timer = NULL;
volatile uint32_t ms = 50000000;

#define FIG_1 myBitmap2

#define TFT_CS 10
#define TFT_DC 8
#define TFT_RST 7

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                    \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif

Adafruit_GC9A01A tft = Adafruit_GC9A01A(TFT_CS, TFT_DC, TFT_RST);


void setup()
{
  Serial.begin(115200);
  // 初始化定时器
  timer = timerBegin(0, 80, true); // Timer 0, 80分频（1MHz），true表示递增计数

  // 附加中断处理函数
  timerAttachInterrupt(timer, &onTimer, true);

  // 设置报警值为1000000（即1毫秒）
  timerAlarmWrite(timer, 1000, true);

  // 启动定时器
  timerAlarmEnable(timer);

  tft.begin();
  tft.setRotation(0); // Set the rotation if needed

  tft.fillScreen(GC9A01A_BLACK); // Clear the screen

  tft.drawRGBBitmap(0,0,FIG_1,240,240);
  show_dial();
}

void loop()
{
  if(ms%1000==0){
    tick();
  }

  if (ms == 86400000)
  {
    ms = 0;
  }
}

void tick(){
  erase_second_ball_niddle(4,90,GC9A01A_BLACK);
  erase_minute_niddle(80,4,GC9A01A_BLACK);
  erase_hour_niddle(50,6,GC9A01A_BLACK);

  show_RTS_from_second(73, 70, 2, GC9A01A_PINK, GC9A01A_BLACK);
  show_second_ball_niddle(4,90,GC9A01A_RED);
  show_minute_niddle(80,4,GC9A01A_BLUE);
  show_hour_niddle(50,6,GC9A01A_GREEN);

  tft.fillCircle(120,120,4,GC9A01A_WHITE);

}

void show_second(int x, int y, int size, short color, short back_color){
  /*
  这个函数用来显示计时器的读秒数

  x,y:指针位置
  size:文字大小
  color:文字颜色
  back_color:背景颜色
  */

  tft.setTextSize(size); // Set text size

  tft.setTextColor(color); // Set text color

  tft.setCursor(x, y); // Set the cursor position

  std::string time_str;
  time_str = std::to_string(ms / 1000);

  static char time_charlist[50];

  strcpy(time_charlist, time_str.c_str());


  for (int i = 0; i < time_str.length(); i++)
  {
    tft.fillRect(x + i * (size * 6), y, size * 5, size * 7, back_color);
  }


  tft.println(time_charlist); // Print text


}

void show_RTS_from_second(int x, int y, int size, short color, short back_color){
  /*
  这个函数用来显示RTS时间（从秒获取）
  会清除背景

  x,y:指针位置
  size:文字大小
  color:文字颜色
  back_color:背景颜色
  */
  tft.setTextSize(size);   // Set text size
  tft.setTextColor(color); // Set text color
  tft.setCursor(x, y);     // Set the cursor position

  int total_second = ms / 1000;

  int hours = total_second / 3600;
  std::string hours_str;
  if (hours / 10 == 0)
  {
    hours_str = (std::string) "0" + std::to_string(hours);
  }
  else
  {
    hours_str = std::to_string(hours);
  }

  int minutes = (total_second % 3600) / 60;
  std::string minutes_str;
  if (minutes / 10 == 0)
  {
    minutes_str = (std::string) "0" + std::to_string(minutes);
  }
  else
  {
    minutes_str = std::to_string(minutes);
  }

  int seconds = total_second % 60;
  std::string seconds_str;
  if (seconds / 10 == 0)
  {
    seconds_str = (std::string) "0" + std::to_string(seconds);
  }
  else
  {
    seconds_str = std::to_string(seconds);
  }

  std::string rts = hours_str + (std::string) ":" + minutes_str + (std::string) ":" + seconds_str;


  char rts_char[10];

  strcpy(rts_char, rts.c_str());

    for (int i = 0; i < rts.length(); i++)
    {
      choose_bitmap_rect_tofill(x+i*size*6,y,size*5,size*7);
      //tft.fillRect(x + i * (size * 6), y, size * 5, size * 7, back_color);
    }
    // tft.fillScreen(back_color);
  

  tft.println(rts_char); // Print text


}

void rotation_fill_rect_withcolor(int angle, int w, int h, int right_respect_to_center, int color)
    { 
      /*
      绘制一个旋转过后的矩形（从中心点看）
      朝向表盘正右方为0度
      正的度数表示顺时针旋转

      angle:旋转的角度
      w:表示矩形的宽度（从中心点的角度看）
      h:表示高度
      right_respect_to_center:表示矩形中心点和屏幕中心点的距离
      */


      float angle_rad = angle * DEG_TO_RAD; // 30度转换为弧度
      
      int cx = 120 - sin(angle_rad)*right_respect_to_center; // 中心x坐标

      int cy = 120 + cos(angle_rad)*right_respect_to_center; // 中心y坐标

      int width = w;
      int height = h;
     

      // 计算旋转后的顶点

      float cosA = cos(angle_rad);
      float sinA = sin(angle_rad);

      int x0 = cx + (-width / 2) * cosA - (-height / 2) * sinA;
      int y0 = cy + (-width / 2) * sinA + (-height / 2) * cosA;

      int x1 = cx + (width / 2) * cosA - (-height / 2) * sinA;
      int y1 = cy + (width / 2) * sinA + (-height / 2) * cosA;

      int x2 = cx + (width / 2) * cosA - (height / 2) * sinA;
      int y2 = cy + (width / 2) * sinA + (height / 2) * cosA;

      int x3 = cx + (-width / 2) * cosA - (height / 2) * sinA;
      int y3 = cy + (-width / 2) * sinA + (height / 2) * cosA;

      // 填充两个三角形

      //choose_bitmap_triangle_tofill(x0, y0, x1, y1, x2, y2);
      //choose_bitmap_triangle_tofill(x0, y0, x2, y2, x3, y3)
      tft.fillTriangle(x0, y0, x1, y1, x2, y2, color);
      tft.fillTriangle(x0, y0, x2, y2, x3, y3, color);
    }

void rotation_fill_rect_withbitmap(int angle, int w, int h, int right_respect_to_center){
        float angle_rad = angle * DEG_TO_RAD; // 30度转换为弧度
      
      int cx = 120 - sin(angle_rad)*right_respect_to_center; // 中心x坐标

      int cy = 120 + cos(angle_rad)*right_respect_to_center; // 中心y坐标

      int width = w;
      int height = h;
     

      // 计算旋转后的顶点

      float cosA = cos(angle_rad);
      float sinA = sin(angle_rad);

      int x0 = cx + (-width / 2) * cosA - (-height / 2) * sinA;
      int y0 = cy + (-width / 2) * sinA + (-height / 2) * cosA;

      int x1 = cx + (width / 2) * cosA - (-height / 2) * sinA;
      int y1 = cy + (width / 2) * sinA + (-height / 2) * cosA;

      int x2 = cx + (width / 2) * cosA - (height / 2) * sinA;
      int y2 = cy + (width / 2) * sinA + (height / 2) * cosA;

      int x3 = cx + (-width / 2) * cosA - (height / 2) * sinA;
      int y3 = cy + (-width / 2) * sinA + (height / 2) * cosA;

      // 填充两个三角形

      choose_bitmap_triangle_tofill(x0, y0, x1, y1, x2, y2);
      choose_bitmap_triangle_tofill(x0, y0, x2, y2, x3, y3);
      //tft.fillTriangle(x0, y0, x1, y1, x2, y2, color);
      //tft.fillTriangle(x0, y0, x2, y2, x3, y3, color);
}

void show_dial(){
  //用于绘制表盘
  for (int i = 0; i < 4; i++)
  {
    tft.setRotation(i);
    tft.fillRect(117, 0, 6, 20, GC9A01A_YELLOW);
    rotation_fill_rect_withcolor(30,4,24,120,GC9A01A_ORANGE);
    rotation_fill_rect_withcolor(60,4,24,120,GC9A01A_ORANGE);
  }
  ;
  tft.setRotation(0);
  }

void show_second_ball_niddle(int r, int distance,int color){
  int second = (ms/1000)%60;
  double rad = second*6*DEG_TO_RAD;
  int x=120+distance*sin(rad);
  int y=120-distance*cos(rad);
  tft.fillCircle(x,y,r,color);

}

void erase_second_ball_niddle(int r, int distance, int background_color){
  int second = (ms/1000-1)%60;

    double rad = second*6*DEG_TO_RAD;
    int x=120+distance*sin(rad);
    int y=120-distance*cos(rad);
    //tft.fillCircle(x,y,r,background_color);
    choose_bitmap_circle_tofill(x,y,r);

}

void show_minute_niddle(int len, int w, int color){
  int minutes = ((ms/1000) % 3600) / 60;
  rotation_fill_rect_withcolor(minutes*6+180,w,len,len/2,color);
}

void erase_minute_niddle(int len, int w, int color){
  int minutes = ((ms/1000) % 3600) / 60 - 1;
  rotation_fill_rect_withbitmap(minutes*6+180,w,len,len/2);
}

void show_hour_niddle(int len, int w, int color){
  int hour = (ms/1000) / 3600;
  int minutes = ((ms/1000) % 3600) / 60;
  rotation_fill_rect_withcolor(hour*30+minutes/2+180,w,len,len/2,color);
}

void erase_hour_niddle(int len, int w, int color){
  int hour = (ms/1000)/ 3600;
  int minutes = ((ms/1000) % 3600) / 60 - 1;
  rotation_fill_rect_withbitmap(hour*30+minutes/2+180,w,len,len/2);
}

void choose_bitmap_rect_tofill(int x, int y, int w, int h){
  for(int i=0;i<w;i++){
    for(int j = 0;j<h;j++){
      tft.drawPixel(x+i,y+j,FIG_1[(y+j)*240+x+i]);
    }
  }
}

void choose_bitmap_circle_tofill(int x0, int y0, int r){
  int h = 2 * r + 1;
  int x=x0;
  int y=y0-r;
  choose_bitmap_line_toshow(x, y, x, y + h - 1);
  _bitmap_fillCircleHelper(x0, y0, r, 3, 0);
  
}

void _bitmap_fillCircleHelper(int16_t x0, int16_t y0, int16_t r,uint8_t corners, int16_t delta) {

  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;
  int16_t px = x;
  int16_t py = y;

  delta++; // Avoid some +1's in the loop

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    // These checks avoid double-drawing certain lines, important
    // for the SSD1306 library which has an INVERT drawing mode.
    if (x < (y + 1)) {
      if (corners & 1)
        _fast_vline(x0 + x, y0 - y, 2 * y + delta);
      if (corners & 2)
        _fast_vline(x0 - x, y0 - y, 2 * y + delta);
    }
    if (y != py) {
      if (corners & 1)
        _fast_vline(x0 + py, y0 - px, 2 * px + delta);
      if (corners & 2)
        _fast_vline(x0 - py, y0 - px, 2 * px + delta);
      py = y;
    }
    px = x;
  }
}

void _fast_vline(int16_t x, int16_t y, int16_t h){
  choose_bitmap_line_toshow(x, y, x, y + h - 1);
}

void choose_bitmap_line_toshow(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
  short temp;
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    temp=x0;
    x0=y0;
    y0=temp;

    temp=x1;
    x1=y1;
    y1=temp;

  }

  if (x0 > x1) {
    temp=x0;
    x0=x1;
    x1=temp;

    temp=y0;
    y0=y1;
    y1=temp;

  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0 <= x1; x0++) {
    if (steep) {
      tft.writePixel(y0, x0, FIG_1[x0*240+y0]);
    } else {
      tft.writePixel(x0, y0, FIG_1[y0*240+x0]);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void choose_bitmap_triangle_tofill(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t x2, int16_t y2){

  int16_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    _swap_int16_t(y0, y1);
    _swap_int16_t(x0, x1);
  }
  if (y1 > y2) {
    _swap_int16_t(y2, y1);
    _swap_int16_t(x2, x1);
  }
  if (y0 > y1) {
    _swap_int16_t(y0, y1);
    _swap_int16_t(x0, x1);
  }


  if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if (x1 < a)
      a = x1;
    else if (x1 > b)
      b = x1;
    if (x2 < a)
      a = x2;
    else if (x2 > b)
      b = x2;
    _bitmap_drawFastHLine(a, y0, b - a + 1);

    return;
  }

  int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0,
          dx12 = x2 - x1, dy12 = y2 - y1;
  int32_t sa = 0, sb = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if (y1 == y2)
    last = y1; // Include y1 scanline
  else
    last = y1 - 1; // Skip it

  for (y = y0; y <= last; y++) {
    a = x0 + sa / dy01;
    b = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if (a > b)
      _swap_int16_t(a, b);
    _bitmap_drawFastHLine(a, y, b - a + 1);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = (int32_t)dx12 * (y - y1);
  sb = (int32_t)dx02 * (y - y0);
  for (; y <= y2; y++) {
    a = x1 + sa / dy12;
    b = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if (a > b)
      _swap_int16_t(a, b);
    _bitmap_drawFastHLine(a, y, b - a + 1);
  }

}

void _bitmap_drawFastHLine(int16_t x, int16_t y, int16_t w) {
  choose_bitmap_line_toshow(x, y, x + w - 1, y);
}

void IRAM_ATTR onTimer()
{
  ms++;
}

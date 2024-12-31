#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"
#include <Arduino.h>
#include <string>
#include <math.h>
#include "figure1.h"

hw_timer_t *timer = NULL;
volatile uint32_t ms = 50000000;


#define TFT_CS 10
#define TFT_DC 8
#define TFT_RST 7

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

  show_dial();
  tft.drawBitmap(0,0,gImage_figure1,239,240,32767);
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


  tft.fillCircle(120,120,4,GC9A01A_WHITE);

  show_RTS_from_second(73, 70, 2, GC9A01A_PINK, GC9A01A_BLACK);
  show_second_ball_niddle(4,90,GC9A01A_RED);
  show_minute_niddle(80,4,GC9A01A_BLUE);
  show_hour_niddle(50,6,GC9A01A_GREEN);
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
      tft.fillRect(x + i * (size * 6), y, size * 5, size * 7, back_color);
    }
    // tft.fillScreen(back_color);
  

  tft.println(rts_char); // Print text


}

void rotation_fill_rect(int angle, int w, int h, int right_respect_to_center, int color)
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

      tft.fillTriangle(x0, y0, x1, y1, x2, y2, color);
      tft.fillTriangle(x0, y0, x2, y2, x3, y3, color);
    }

void show_dial(){
  //用于绘制表盘
  for (int i = 0; i < 4; i++)
  {
    tft.setRotation(i);
    tft.fillRect(117, 0, 6, 20, GC9A01A_YELLOW);
    rotation_fill_rect(30,4,24,120,GC9A01A_ORANGE);
    rotation_fill_rect(60,4,24,120,GC9A01A_ORANGE);
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
    tft.fillCircle(x,y,r,background_color);

}

void show_minute_niddle(int len, int w, int color){
  int minutes = ((ms/1000) % 3600) / 60;
  rotation_fill_rect(minutes*6+180,w,len,len/2,color);
}

void erase_minute_niddle(int len, int w, int color){
  int minutes = ((ms/1000) % 3600) / 60 - 1;
  rotation_fill_rect(minutes*6+180,w,len,len/2,color);
}

void show_hour_niddle(int len, int w, int color){
  int hour = (ms/1000) / 3600;
  int minutes = ((ms/1000) % 3600) / 60;
  rotation_fill_rect(hour*30+minutes/2+180,w,len,len/2,color);
}

void erase_hour_niddle(int len, int w, int color){
  int hour = (ms/1000)/ 3600;
  int minutes = ((ms/1000) % 3600) / 60 - 1;
  rotation_fill_rect(hour*30+minutes/2+180,w,len,len/2,color);
}


void IRAM_ATTR onTimer()
{
  ms++;
}

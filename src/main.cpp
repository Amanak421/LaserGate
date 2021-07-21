#include <Arduino.h>
//#include <Encoder.h>
#include "display.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>

Display my_oled;


/*#define FOTO_PIN_START 8  //někdy A8
#define FOTO_PIN_STOP 9 //někdy A9

#define HIGH_PIN 22

int foto_value_start = true;
int foto_value_stop = true;

bool last_foto_start = false;
unsigned long last_time_start;

bool last_foto_stop = true;
unsigned long last_time_stop;

bool now_timer = false;

#define FOTO_WAIT 50

int timer_minute;
int timer_sec;
int timer_setina;

int _timer_minute;
int _timer_sec;
int _timer_setina;

float _speed;

#define TIMER_MARK 1000
unsigned long last_time_timer;

//Encoder myEnc(2, 3);          // Založení třídy rotačního enkoderu
long oldPosition  = -999;
int current_direction;
int last_direction;
#define TIME_MARK_ENC 500
unsigned long last_time_enc;

const int UP = 1;
const int DOWN = -1;

bool tlacitko;
bool tlacitko2;
bool tlacitko3;

int MinulyStav1;
int MinulyStav2;
int MinulyStav3;
unsigned long CasZmeny1;
unsigned long CasZmeny2;
unsigned long CasZmeny3;
const int BUTTONPIN = 4;
const int BUTTONPIN2 = 40;
const int BUTTONPIN3 = 41;
const int PRODLEVA = 50;

//#define DEBUG           // Odpoznámkováním tohoto řádku se spustí debug




#define TIME_MARK_TIMER 10          // časová značka --> později smazat
unsigned long last_time_timer_;



bool _run_timer = false;
bool _stop_timer = false;
bool _timer_running = false;
//unsigned long timer_start;
unsigned long timer_stop;

unsigned long current_time = 0;
unsigned long _time = 0;

int distance_meters;    // Proměná, do které se nahraje hodnota z potenciometru
float distance_centimeters;
float distance;*/

//Adafruit_SSD1306 my_display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); //připojení displeje

void setup() {
  Serial.begin(9600);

  /*pinMode(HIGH_PIN, OUTPUT);
  digitalWrite(HIGH_PIN, HIGH);

  pinMode(BUTTONPIN, INPUT_PULLUP);


  pinMode(FOTO_PIN_START, INPUT);
  pinMode(FOTO_PIN_STOP, INPUT);*/

  my_oled.begin();
  my_oled.topbar();
  my_oled.pageHome();

  /*my_display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  my_display.display();
  my_display.clearDisplay();

  my_display.setTextSize(1);  
  my_display.setCursor(0,19);
  my_display.setTextColor(WHITE, BLACK);
  my_display.println("Stopky se spusti");
  my_display.setCursor(0,29);
  my_display.println("po projeti prvnim");
  my_display.setCursor(0,39);
  my_display.println("laserem.");
  my_display.setCursor(2, 54);
  my_display.print("Nast. vzdalen.: ");
  my_display.print("25");
  my_display.print(" m");
  my_display.display();*/
  

  bool _run_timer = false;
  bool _stop_timer = false;

  //distance = 1;


}

void loop() {


  my_oled.updateEncoder();

  /*topbar();
  encoder();
  page_switch();*/

  //run_timer_();
  //_timer();


  //Serial.println(button_enc());
  /*
  Serial.print("1:      ");
  Serial.println(analogRead(FOTO_PIN_START));
  Serial.print("2:      ");
  Serial.println(analogRead(FOTO_PIN_STOP));
  */
  
}

/*void _timer(){

  if(_timer_running == true){

  timer_setina = (current_time / 100);
  timer_setina = timer_setina - ((timer_sec * 10) + (timer_minute * 600));

    if(timer_setina >= 10){
      timer_sec++;
      timer_setina = 0;
      }
      if(timer_sec >= 60){
        timer_minute++;
        timer_sec = 0;
        timer_setina = 0;
        }
        if(timer_minute >= 60){
          //Konec časomiry
          }

   #ifdef DEBUG
   Serial.print("Setiny: ");
   Serial.println(timer_setina);
   Serial.print("Vteriny: ");
   Serial.println(timer_sec);
   Serial.print("Minuty: ");
   Serial.println(timer_minute);
   #endif
  }
   
}*/

/*void run_timer_(){

  if(foto_start() == 1 && actual_page == PAGE_HOME){
    _run_timer = true;
    actual_page = PAGE_TIMER;
    }

  if(foto_stop() == 0 && _timer_running == true){
    now_timer = true;  
     }
  
  if(foto_stop() == 1 && now_timer == true && _timer_running == true){
    _stop_timer = true;
    actual_page = PAGE_SPEED_RESULTS;
    }

  // if(foto_stop() == 1 && actual_page == PAGE_TIMER){
  //  _stop_timer = true;
  // actual_page = PAGE_SPEED_RESULTS;
  //  }
  
  if(_run_timer == true){
    
    timer_start = millis();
    _timer_running = true;
    _run_timer = false;
    
    }

  if(_stop_timer == true){

    timer_stop = millis();
    _timer_running = false; 
    _stop_timer = false;
    
    }

  current_time = millis() - timer_start;  
}*/



//display.drawBitmap(0, 0, myBitmap, 128, 64, WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)

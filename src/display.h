#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>

#include <RotaryEncoder.h>

#include <ESP32Encoder.h>

#define PAGE_HOME 1
#define PAGE_MENU 2
#define PAGE_SET_DISTANCE 3
#define PAGE_SET_MOD 4
#define PAGE_COMMING_SOON 5
#define PAGE_SPEED_RESULTS 6
#define PAGE_TIMER 7

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

class Display{

    private:

        #define TIME_MARK_DISPLAY 200
        unsigned long last_time_display;

        bool first_run_set_distance = true; // Proměná, aby část kódu proběhla jen jednou
        bool first_run_set_mod = true;
        bool first_run_home = true;
        bool first_run_menu = true;
        bool first_run_comming_soon = true;
        bool first_run_speed_results = true;
        bool first_run_timer = true;

        const int ERROR_CABLE = 1;      //nastavení konstant erorů
        const int ERROR_FRONT_FOTOREZISTOR = 2;
        const int ERROR_BACK_FOTOREZISTOR = 3;
        int actual_error = 0;

        int actual_page = 1;

        

        bool bluetooth_active = false;       //hodnota aktulního stavu bluetoothu, true --> aktivní, flase --> neaktivní

        const int MOD_SPEED = 1;            //Konstanty módů, které se dají nastavit
        const int MOD_ACCELERATION = 2;
        int current_mod = 0;                //aktuální mód

        int last_reading = 0;
        int _cursor = 1;                    //aktuální pozice kurzoru podle hodnoty z potenciometru
        int last_cursor = 1;                //poslední pozice kurzoru --> používá se k vymazání proužku displeje, kde se pohybuje kurzor

        int max_cursor = 4;

        int distance = 0;
        int distance_meters = 0;
        int distance_centimeters = 0;

        int timer_minute;
        int timer_sec;
        int timer_setina;

        int _timer_minute;
        int _timer_sec;
        int _timer_setina;

        int foto_value_start = true;
        int foto_value_stop = true;

        bool last_foto_start = false;
        unsigned long last_time_start;

        bool last_foto_stop = true;
        unsigned long last_time_stop;

        bool now_timer = false;

        unsigned long current_time = 0;
        unsigned long _time = 0;

        bool _run_timer = false;
        bool _stop_timer = false;
        bool _timer_running = false;
        unsigned long timer_start;
        unsigned long timer_stop;

        float _speed;

        const int ENCODER_READ_PAUSE = 250;     //pauza mezi změnami enkoderu
        unsigned long last_encoder_read;

        int pos = 0;

        bool button_enc();




    public:

        Display();
        void begin();
        
        void pageHome();
        void topbar();
        void pageMenu();
        void pageSetDistance();
        void pageSetMod();
        void pageCommingSoon();
        void pageSwitch();
        void pageTimer();
        void pageSpeedResults();

        void clearMenu();
        void clearBluePart();
        void clearYellowPart();


    void encoderBegin();
    void buttonBegin();

    bool updateEncoder();

    struct Button {     //struktura talačítka
    const uint8_t PIN;
    uint32_t numberKeyPresses;
    bool pressed;
    };


};



#endif
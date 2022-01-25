#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


Adafruit_SSD1306 oled_display(128, 64, &Wire, -1);


const int PAGE_HOME = 0x01;     //PAGES
const int PAGE_MENU = 0x02;
const int PAGE_SET_DISTANCE = 0x03;
const int PAGE_SET_MOD = 0x04;
const int PAGE_COMMING_SOON = 0x05;
const int PAGE_SPEED_RESULTS = 0x06;
const int PAGE_TIMER = 0x07;
const int PAGE_CALIBRATION = 0x0B;

const int NONE = 0x08;
const int SPEED = 0x09;     //MODS
const int ACCELERATION = 0x0A;

bool first_run_set_distance = true; // FIRST RUN FOR PAGES
bool first_run_set_mod = true;
bool first_run_home = true;
bool first_run_menu = true;
bool first_run_comming_soon = true;
bool first_run_speed_results = true;
bool first_run_timer = true;
bool first_run_calibration = true;

int actual_page = 0x01;         //for showing actual page
int last_page = 0;              //optimalization for showing pages repeatetly

int current_mod = SPEED;

bool bluetooth_active = false;

int actual_error = 0;

double distance = 2;

int max_cursor = 4;
int last_cursor = 0;
int cursor = 1;

unsigned int lastDebounceTime = 0;      //BUTTON
const unsigned int DEBOUNCE_DELAY = 100;
int lastFlickerableState = LOW;
int lastSteadyState = LOW;
int currentState = LOW;
bool pressed = false;

const int BUTTON_PIN = 14;

int counter = 0;    //ROTARY ENCODER
int aState = 0;
int aLastState = 0; 

const int OUTPUTA =  18;     //encoder pins
const int OUTPUTB =  5;


bool event_update = false;
bool topbar_update = true;

double speed_time = 0;
double speed = 0;
unsigned long start_timer = 0;
unsigned long timer_end = 0;
bool timer_run = false;
bool timer_stop = false;
bool timer_running = false;
bool now_timer = false;

int timer_min = 0;
int timer_sec = 0;
int timer_setina = 0;

const int TIMER_COM = 100;
unsigned long timer_com_time = 0;

int current_time = 0;

const int FOTO_START = 2;
int FOTO_START_VALUE = 150;
int foto_value_start = 0;

const int FOTO_STOP = 15;
int FOTO_STOP_VALUE = 100;
int foto_value_stop = 0;


bool com_enabled = false;

bool buttonEnc();

void pageHome();
void pageMenu();
    void clearMenu();
void pageSetDistance();
void pageSetMod();
void pageSoon();
void pageTimer();
void pageSpeedResults();

void topbar();

void begin();

int actualPage();
int lastPage();
void updatePage();
void updateButton();
void updateEncoder();
void pageCalibration();

void clearBluePart();

bool fotoStart();
bool fotoStop();

void timer();
void runTimer();

void readCommand();
void reset();


void setup(){
    Serial.begin(115200);

    pinMode(OUTPUTA, INPUT);
    pinMode(OUTPUTB, INPUT);

    oled_display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    oled_display.display();
    oled_display.clearDisplay();

    pinMode(BUTTON_PIN, INPUT_PULLUP);

    pinMode(FOTO_START, INPUT);
    pinMode(FOTO_STOP, INPUT);

    pageCalibration();

    Serial.println("INIT DONE");
    Serial.println("C-I-DONE");


}

void loop(){

    readCommand();

    updateButton();
    updateEncoder();
    updatePage();
    //current_time = millis();
    timer();
    runTimer();
    /*Serial.println(number);
    number++;
    delay(250);*/
    
}



void pageHome(){

    if(first_run_home == true){           // clear display in first run
        clearBluePart();
        first_run_home = false;
    }

    oled_display.setTextSize(1);  
    oled_display.setCursor(0,19);
    oled_display.setTextColor(WHITE, BLACK);
    oled_display.println("Stopky se spusti");
    oled_display.setCursor(0,29);
    oled_display.println("po projeti prvnim");
    oled_display.setCursor(0,39);
    oled_display.println("laserem.");
    oled_display.setCursor(2, 54);
    oled_display.print("Nast. vzdalen.: ");
    oled_display.print(distance, 1);
    oled_display.print("m");
    oled_display.display();

    if(buttonEnc() == 1){
        actual_page = PAGE_MENU;
        cursor = 1;
        first_run_home = true;
    }
}

void pageMenu(){

    max_cursor = 4;

    if(first_run_menu == true){           // Při prvním spuštění vymaže modrou část
        clearBluePart();
        first_run_menu = false;
    }

    if(cursor != last_cursor){     // Při pozice kurzoru smaže proužek displeje, kde se kurzor pohybuje
        clearMenu();
        last_cursor = cursor;
    }

    oled_display.setTextSize(1);         // Vypíše jednotlivé položky menu
    oled_display.setCursor(12, 18);
    oled_display.setTextColor(WHITE, BLACK);
    oled_display.println("Hlavni obrazovka");
    oled_display.setCursor(12, 30);
    oled_display.println("Nastavit vzdalenost");
    oled_display.setCursor(12, 42);
    oled_display.println("Zmena modu");
    oled_display.setCursor(12, 54);
    oled_display.println("Zmena nast. zrychl.");

    switch(cursor){                // Zobrazí kurzor u aktuální položky, která je vybraná
        
        case 1: oled_display.setCursor(2, 18);
                oled_display.println(">");
                if(buttonEnc() == 1){
                    actual_page = PAGE_HOME;
                    first_run_menu = true;
                }
                break;

        case 2: oled_display.setCursor(2, 30);
                oled_display.println(">");
                if(buttonEnc() == 1){
                    actual_page = PAGE_SET_DISTANCE;
                    cursor = 1;
                    first_run_menu = true;
                }
                break;

        case 3: oled_display.setCursor(2, 42);
                oled_display.println(">");
                if(buttonEnc() == 1){
                    actual_page = PAGE_SET_MOD;
                    cursor = 1;
                    first_run_menu = true;
                }
                break;

        case 4: oled_display.setCursor(2, 54);
                oled_display.println(">");
                if(buttonEnc() == 1){
                    actual_page = PAGE_COMMING_SOON;
                    first_run_menu = true;
                }
                break;
        
        }

    oled_display.display();      // show changes on display

}

void clearMenu(){
  oled_display.setTextSize(1);
  oled_display.setTextColor(WHITE, BLACK);
  oled_display.setCursor(2, 18);
  oled_display.println(" ");
  oled_display.setCursor(2, 30);
  oled_display.println(" ");
  oled_display.setCursor(2, 42);
  oled_display.println(" ");
  oled_display.setCursor(2, 54);
  oled_display.println(" ");
  oled_display.display();
}

void pageSetDistance(){
    max_cursor = 100;

    if(first_run_set_distance == true){
        clearBluePart();
        first_run_set_distance = false;
        distance = 0;
        cursor = 0;
        }

    distance = (float)cursor / (float)10;
    
    oled_display.setTextColor(WHITE, BLACK);
    oled_display.setTextSize(1);
    oled_display.setCursor(2, 20);
    oled_display.println("Vzdal. mezi lasery:");
    oled_display.setTextSize(2);
    oled_display.setCursor(32, 35);
    oled_display.print("  ");
    oled_display.setCursor(32, 35);
    oled_display.print(distance, 1);
    oled_display.setCursor(92, 35);
    oled_display.println("m");
    oled_display.display();


    if(buttonEnc() == 1){
        actual_page = PAGE_HOME;
        first_run_set_distance = true;   
        }
}

void pageSetMod(){
    if(first_run_set_distance == true){
        clearBluePart();
        first_run_set_distance = false;    
    }

    max_cursor = 3;

    if(cursor != last_cursor){     // Při pozice kurzoru smaže proužek displeje, kde se kurzor pohybuje
            oled_display.setTextSize(1);
            oled_display.setTextColor(WHITE, BLACK);
            oled_display.setCursor(2, 30);
            oled_display.println(" ");
            oled_display.setCursor(2, 42);
            oled_display.println(" ");
            oled_display.setCursor(2, 54);
            oled_display.println(" ");
            oled_display.display();
        last_cursor = cursor;
    }

    oled_display.setTextColor(WHITE, BLACK);
    oled_display.setTextSize(1);
    oled_display.setCursor(2, 20);
    oled_display.println("Zvolte mod:");
    oled_display.setCursor(12, 30);
    oled_display.println("Mereni rychlosti");
    oled_display.setCursor(12, 42);
    oled_display.println("Mereni zrychleni");
    oled_display.drawLine(0, 51, 128, 51, WHITE);
    oled_display.setCursor(12, 54);
    oled_display.println("Zpet");
    oled_display.display();

    

    switch(cursor){              // Zobrazí kurzor u aktuální položky, která je vybraná
        
        case 1: oled_display.setCursor(2, 30);
                oled_display.println("-");
                if(buttonEnc() == 1){
                    current_mod = SPEED;
                    actual_page = PAGE_MENU;
                    cursor = 1;
                    first_run_set_distance = true;     
                    topbar_update = true;      
                }
                break;

        case 2: oled_display.setCursor(2, 42);
                oled_display.println("-");
                if(buttonEnc() == 1){
                    current_mod = SPEED;
                    actual_page = PAGE_COMMING_SOON;
                    cursor = 1;
                    first_run_set_distance = true;  
                    topbar_update = true;          
                }
                break;

        case 3: oled_display.setCursor(2, 54);
                oled_display.println("-");
                if(buttonEnc() == 1){
                    actual_page = PAGE_MENU;
                    cursor = 1;
                    first_run_set_distance = true;           
                }
                break;
        
        }

    oled_display.display();
}

void pageSoon(){
    if(first_run_comming_soon == true){
        clearBluePart();
        first_run_comming_soon = false;    
    }  
  
    oled_display.setTextSize(1);
    oled_display.setTextColor(WHITE, BLACK);
    oled_display.setCursor(2, 30);
    oled_display.println("Tato funkce jeste");
    oled_display.setCursor(2, 40);
    oled_display.println("neni dostupna...");      
    oled_display.display();

    if(buttonEnc() == 1){
        actual_page = PAGE_MENU;
        first_run_comming_soon = true;           
    }
}

void pageTimer(){

    if(first_run_timer == true){           // Při prvním spuštění vymaže modrou část
        oled_display.clearDisplay();
        first_run_timer = false;
    }

    oled_display.setTextSize(1);
    oled_display.setTextColor(WHITE, BLACK);
    oled_display.setCursor(2, 20);
    oled_display.println("Cas:");
    oled_display.setTextSize(2);
    oled_display.setCursor(23, 35);
    oled_display.print(timer_min);
    oled_display.print(":");
    oled_display.print(timer_sec);
    oled_display.print(".");
    oled_display.println(timer_setina);
    oled_display.display();

}

void pageSpeedResults(){

    if(first_run_speed_results == true){           // Při prvním spuštění vymaže modrou část
        oled_display.clearDisplay();
        first_run_speed_results = false;
    }

    Serial.print("START ");
    Serial.println(start_timer);
    Serial.print("end ");
    Serial.println(timer_end);

    speed_time = timer_end - start_timer;
    Serial.println(speed_time);
    speed_time /= (double)1000;

    speed = distance / speed_time;
    //round(_speed);
    
    oled_display.setTextSize(1);
    oled_display.setTextColor(WHITE, BLACK);
    oled_display.setCursor(2, 20);
    oled_display.print("Vzdalenost:");
    oled_display.setCursor(2, 32);
    oled_display.print("Cas:");
    oled_display.setCursor(2, 44);
    oled_display.print("Rychlost:");
    oled_display.setCursor(70, 20);
    oled_display.print(distance, 1);
    oled_display.println("m");
    oled_display.setCursor(70, 32);
    oled_display.print(speed_time, 1);
    oled_display.println("s");
    oled_display.setCursor(70, 44);
    oled_display.print(speed, 1);
    oled_display.println("m/s");
    oled_display.display();

    if(com_enabled){
        Serial.print("C-R-");
        Serial.print(distance, 1);
        Serial.print(":");
        Serial.print(speed_time, 1);
        Serial.print("/");
        Serial.println(speed, 1);
    }    

    if(buttonEnc() == 1){
        
        actual_page = PAGE_HOME;
        speed_time = 0;
        speed = 0;
        timer_setina = 0;
        timer_sec = 0;
        timer_min = 0;

        first_run_set_distance = true; // Proměná, aby část kódu proběhla jen jednou
        first_run_set_mod = true;
        first_run_home = true;
        first_run_menu = true;
        first_run_comming_soon = true;
        first_run_speed_results = true;
        first_run_timer = true;

        timer_run = false;
        timer_stop = false;
        timer_running = false;
        start_timer = 0;
        timer_end = 0;

        oled_display.clearDisplay();
        
    }

}

void clearBluePart(){         // Smaže modrou část displeje
  
  //oled_display.clearDisplay();
  oled_display.fillRect(0, 16, 128, 64, BLACK);
  oled_display.display();
  
}

void pageCalibration(){
    if(first_run_calibration){
        clearBluePart();
        first_run_calibration = false;
    }

    oled_display.setTextSize(1);  
    oled_display.setCursor(0,19);
    oled_display.setTextColor(WHITE, BLACK);
    oled_display.println("Zamirte lasery ");
    oled_display.setCursor(0,29);
    oled_display.println("na senzory");
    oled_display.setCursor(0,39);
    oled_display.println("a zmacknete tlacitko.");
    oled_display.display();

    while(buttonEnc() != true){
        updateButton();
    }

    FOTO_START_VALUE = analogRead(FOTO_START) + 80;
    FOTO_STOP_VALUE = analogRead(FOTO_STOP) + 80;

    oled_display.clearDisplay();
    oled_display.setTextSize(1);  
    oled_display.setCursor(0,19);
    oled_display.setTextColor(WHITE, BLACK);
    oled_display.println("Kalibrace dokoncena");
    oled_display.setCursor(0,29);
    oled_display.print("START :");
    oled_display.println(FOTO_START_VALUE);
    oled_display.setCursor(0,39);
    oled_display.print("STOP :");
    oled_display.println(FOTO_STOP_VALUE);
    oled_display.display();

    while(buttonEnc() != true){
        updateButton();
    }


}

void updatePage(){

    if(actual_page == PAGE_HOME && (last_page != PAGE_HOME || event_update == true)){
        last_page = PAGE_HOME;
        pageHome();
        Serial.println("PAGE HOME");
    }else if(actual_page == PAGE_MENU && (last_page != PAGE_MENU || event_update == true)){
        last_page = PAGE_MENU;
        pageMenu();
        Serial.println("PAGE MENU");
    }else if(actual_page == PAGE_SET_DISTANCE && (last_page != PAGE_SET_DISTANCE || event_update == true)){
        last_page = PAGE_SET_DISTANCE;
        pageSetDistance();
        Serial.println("PAGE SET DISTANCE");
    }else if(actual_page == PAGE_SET_MOD && (last_page != PAGE_SET_MOD || event_update == true)){
        last_page = PAGE_SET_MOD;
        pageSetMod();
        Serial.println("PAGE SET MOD");
    }else if(actual_page == PAGE_COMMING_SOON && (last_page != PAGE_COMMING_SOON || event_update == true)){
        last_page = PAGE_COMMING_SOON;
        pageSoon();
        //Serial.println("PAGE COMMING SOON");
    }else if(actual_page == PAGE_TIMER && (last_page != PAGE_COMMING_SOON || event_update == true)){
        last_page = PAGE_TIMER;
        pageTimer();
        //Serial.println("PAGE TIMER");
    }else if(actual_page == PAGE_SPEED_RESULTS && (last_page != PAGE_SPEED_RESULTS || event_update == true)){
        last_page = PAGE_SPEED_RESULTS;
        pageSpeedResults();
        //Serial.println("PAGE SPEED RESULTS");
    }

    event_update = false;
    topbar();

}

void topbar(){      // Zobrazí horní lištu

    if(topbar_update){

        oled_display.setTextSize(1);
        oled_display.setTextColor(WHITE, BLACK);
        oled_display.setCursor(74, 5);
        oled_display.drawLine(56, 3, 56, 12, WHITE);
        oled_display.drawLine(69, 3, 69, 12, WHITE);

        switch(actual_error){ // Zobrazí zda je zařízení přiravené nebo zobrazí jaký eror nastal
            
            case 0:oled_display.println("Pripraven");
                oled_display.display();
                break;

            case 1:oled_display.println("ERROR 1");
                oled_display.display();
                break;
            
            case 2:oled_display.println("ERROR 2");
                oled_display.display();
                break;
            
            case 3:oled_display.println("ERROR 3");
                oled_display.display();
                break;
            }

        if(bluetooth_active == 1){        // Zobrazí zda je bluetooth aktivní
            oled_display.setCursor(61, 5);
            oled_display.setTextColor(WHITE, BLACK);
            oled_display.setTextSize(1);
            oled_display.println("B");
            oled_display.display();   
            }

        switch(current_mod){              // Zobrazí jaký mód je vybrán
            
            case SPEED:oled_display.setCursor(0, 5);
                oled_display.setTextColor(WHITE, BLACK);
                oled_display.setTextSize(1);
                oled_display.println("Rychlost");
                oled_display.display();
                break;

            case ACCELERATION:oled_display.setCursor(0, 5);
                oled_display.setTextColor(WHITE, BLACK);
                oled_display.setTextSize(1);
                oled_display.println("Zrychleni");
                oled_display.display();
                break;
            
            }
    }

    topbar_update = false;
  
}

bool buttonEnc(){
  if(pressed){
    pressed = false;
    return true;
  }else{
    return false;
  }
}

void updateButton(){
  
  currentState = digitalRead(BUTTON_PIN);

  if (currentState != lastFlickerableState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
    // save the the last flickerable state
    lastFlickerableState = currentState;
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (lastSteadyState == HIGH && currentState == LOW){
      Serial.println("The button is pressed");
      pressed = true;
      event_update = true;
      //return true;
    }
    else if (lastSteadyState == LOW && currentState == HIGH)
      Serial.println("The button is released");
      //return false;

    // save the the last steady state
    lastSteadyState = currentState;
  }


}

void updateEncoder(){

  aState = digitalRead(OUTPUTA); // Reads the "current" state of the outputA
   // If the previous and the current state of the outputA are different, that means a Pulse has occured
   if (aState != aLastState){     
     // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
     if (digitalRead(OUTPUTB) != aState) { 
       counter ++;
       event_update = true;
     } else {
       counter --;
       event_update = true;
     }
     //Serial.print("Position: ");

        if(counter > max_cursor){
            //Serial.println("ZERO COUNTER");
            counter = 1;
        }else if(counter <= 0){
            counter = max_cursor;
        }
        cursor = counter;
        //Serial.println(counter);
     
   } 
   aLastState = aState; // Updates the previous state of the outputA with the current state

}

bool fotoStart(){
    foto_value_start = analogRead(FOTO_START);

    if(foto_value_start > 170){


        return true;
        
    }
    /*else if(foto_value_start > 170){
        
        return false;
        
    }*/
    return false;
}

bool fotoStop(){
    foto_value_stop = analogRead(FOTO_STOP);

    if(foto_value_stop > 200){

        
        return true;
    }/*else if(foto_value_stop < 100){


        return false;
        
    }*/
    return false;
}

void timer(){
    if(timer_running == true){

        timer_setina = (current_time / 100);
        timer_setina = timer_setina - ((timer_sec * 10) + (timer_min * 600));

            if(timer_setina >= 10){
            timer_sec++;
            timer_setina = 0;
            }
            if(timer_sec >= 60){
                timer_min++;
                timer_sec = 0;
                timer_setina = 0;
                }
                if(timer_min >= 60){
                //Konec časomiry
                }

        
        if(millis() - timer_com_time >= TIMER_COM && com_enabled == true){
            Serial.print("C-T-");
            Serial.print(timer_min);
            Serial.print(":");
            Serial.print(timer_sec);
            Serial.print(".");
            Serial.println(timer_setina);
            timer_com_time = millis();
        }
        

        #ifdef DEBUG
            Serial.print("Setiny: ");
            Serial.println(timer_setina);
            Serial.print("Vteriny: ");
            Serial.println(timer_sec);
            Serial.print("Minuty: ");
            Serial.println(timer_min);
        #endif
    }
}

void runTimer(){

  if(fotoStart() == true && actual_page == PAGE_HOME){
      Serial.println("TIMER START");
    timer_run = true;
    actual_page = PAGE_TIMER;
    if(com_enabled)     Serial.println("C-C-START");
    }

  if(fotoStop() == 0 && timer_running == true){
    now_timer = true;  
     }
  
  if(fotoStop() == 1 && now_timer == true && timer_running == true){
    Serial.println("TIMER END");
    if(com_enabled)     Serial.println("C-C-STOP");
    timer_stop = true;
    actual_page = PAGE_SPEED_RESULTS;
    }

  /*if(foto_stop() == 1 && actual_page == PAGE_TIMER){
    _stop_timer = true;
    actual_page = PAGE_SPEED_RESULTS;
    }*/
  
  if(timer_run == true){
    
    start_timer = millis();
    timer_running = true;
    timer_run = false;
    
    }

  if(timer_stop == true){

    timer_end = millis();
    timer_running = false; 
    timer_stop = false;
    
    }

  current_time = millis() - start_timer;  
}

void readCommand(){
    if(Serial.available()){
        String command = Serial.readString();
        if(command == "C-G-RESET"){
            reset();
        }else if(command == "C-CONNECT"){
            com_enabled = true;
            Serial.println("C-CONNECTED");
            reset();
        }else if(command == "C-ALL"){
            Serial.println("DOSTAL JSEM ALL");
            Serial.print("C-A-D-");
            Serial.print(distance);
            Serial.print("-Z-");
            Serial.print(FOTO_START_VALUE);
            Serial.print("-K-");
            Serial.println(FOTO_STOP_VALUE);

        }
    }

}

void reset(){
    actual_page = PAGE_HOME;
    speed_time = 0;
    speed = 0;
    timer_setina = 0;
    timer_sec = 0;
    timer_min = 0;

    first_run_set_distance = true; // Proměná, aby část kódu proběhla jen jednou
    first_run_set_mod = true;
    first_run_home = true;
    first_run_menu = true;
    first_run_comming_soon = true;
    first_run_speed_results = true;
    first_run_timer = true;

    timer_run = false;
    timer_stop = false;
    timer_running = false;
    start_timer = 0;
    timer_end = 0;

    oled_display.clearDisplay();
}
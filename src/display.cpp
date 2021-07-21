
#include "display.h"

Adafruit_SSD1306 *oled_display;

//ESP32Encoder encoder;

Display::Button encoder_button = {26, 0, false};

const int DEBOUNCE_TIME = 400;
unsigned long debounceTimer;
bool last_state = true;

void IRAM_ATTR isr() {
                                      
    if (last_state == 1 && millis() - debounceTimer > DEBOUNCE_TIME) // neni-li nastaven priznak         
    {                                          // tlacitko stisknuto a uplynul-li vetsi cas  
                                               // od zmeny stavu tlacitka nez je mez (50ms)
      last_state = 0;                             // nastav priznak tlacitko stisknuto
      encoder_button.numberKeyPresses +=1;
      encoder_button.pressed = true;       // zmen hodnotu vystupu LED
    }

    debounceTimer = millis();                       // zapamatuj si posledni cas, kdy bylo nestisknuto
    last_state = 1;                            // nuluj priznak, tlacitko stisknuto

}

#define PIN_IN1 25
#define PIN_IN2 33

RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);

ICACHE_RAM_ATTR void checkPosition()
{
  encoder.tick(); // just call tick() to check the state.
}


//Adafruit_SSD1306 my_display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); //připojení displeje

Display::Display(){

    //Serial.begin(9600);

    


}

void Display::begin(){
  oled_display = new Adafruit_SSD1306(128, 64, &Wire, -1);    //Připojení displeje
  oled_display->begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled_display->display();
  oled_display->clearDisplay();

  actual_error = 0;
  bluetooth_active = false;
  current_mod = MOD_SPEED;
  _cursor = 0;

  encoderBegin();
  buttonBegin();

  Serial.println("DISPLAY INIT DONE");
}

void Display::encoderBegin(){
  /*  // Enable the weak pull down resistors

	//ESP32Encoder::useInternalWeakPullResistors=DOWN;
	// Enable the weak pull up resistors
	ESP32Encoder::useInternalWeakPullResistors=UP;

	// use pin 19 and 18 for the first encoder
	//encoder.attachHalfQuad(33, 25);
  encoder.attachSingleEdge(25, 33);
	// use pin 17 and 16 for the second encoder
		
	// set starting count value after attaching
	encoder.setCount(1);*/
  pinMode(PIN_IN1, INPUT);
  pinMode(PIN_IN2, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_IN1), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_IN2), checkPosition, CHANGE);
}

void Display::buttonBegin(){
    pinMode(encoder_button.PIN, INPUT_PULLUP);
    attachInterrupt(encoder_button.PIN, isr, FALLING);
    Serial.print("TLACITKO PRIPOJENO");
}

bool Display::updateEncoder(){
    /*if(millis() - last_encoder_read > ENCODER_READ_PAUSE){
        int reading = encoder.getCount();
        Serial.println(reading);
        //Serial.print("act_cursor: ");
        //Serial.println(act_cursor);

      if(reading > last_reading){
        _cursor += 1;
      }else if(reading < last_reading){
        _cursor -= 1;
      }

        //if(act_cursor > max_cursor){
        //    act_cursor = 1;
        //    encoder.setCount(1);
        //}else if(act_cursor < 1){
        //    act_cursor = max_cursor;
        //    encoder.setCount(max_cursor);
        //}

        if(_cursor != last_cursor){
            Serial.println("Encoder count = " + String(_cursor));
            last_cursor = _cursor;
            return true;
        }

      last_reading = reading;
      last_encoder_read = millis();
      return false;
    }
    return false;*/
  
  //encoder.tick();

  int newPos = encoder.getPosition();
  if (pos != newPos) {

    int last_cursor = _cursor;

    Serial.print("pos:");
    Serial.print(newPos);
    Serial.print(" dir:");
    Serial.println((int)(encoder.getDirection()));

    if(newPos >= pos + 2){
      pos = newPos;
      _cursor++;
      Serial.print("CURSOR u : ");
      Serial.println(_cursor);
    }else if(newPos <= pos - 2){
      pos = newPos;
      _cursor--;
      Serial.print("CURSOR d : ");
      Serial.println(_cursor);
    }


    /*if(_cursor > max_cursor){
      _cursor = 1;
    }else if(_cursor < 1){
      _cursor = 0;
    }*/

    /*Serial.print("CURSOR: ");
    Serial.println(_cursor);*/
    


  }

  return false;

}

void Display::pageHome(){      //Zobrazí text 

  /*if(first_run_home == true){           // Při prvním spuštění vymaže modrou část
    clearBluePart();
    first_run_home = false;
    }*/

  oled_display->setTextSize(1);  
  oled_display->setCursor(0,19);
  oled_display->setTextColor(WHITE, BLACK);
  oled_display->println("Stopky se spusti");
  oled_display->setCursor(0,29);
  oled_display->println("po projeti prvnim");
  oled_display->setCursor(0,39);
  oled_display->println("laserem.");
  oled_display->setCursor(2, 54);
  oled_display->print("Nast. vzdalen.: ");
  oled_display->print(distance);
  oled_display->print("m");
  oled_display->display();

  /*if(button_enc() == 1){
    actual_page = PAGE_MENU;
    _cursor = 1;
    first_run_home = true;
  }*/
  
}

void Display::topbar(){      // Zobrazí horní lištu
  
  oled_display->setTextSize(1);
  oled_display->setTextColor(WHITE, BLACK);
  oled_display->setCursor(74, 5);
  oled_display->drawLine(56, 3, 56, 12, WHITE);
  oled_display->drawLine(69, 3, 69, 12, WHITE);

  switch(actual_error){ // Zobrazí zda je zařízení přiravené nebo zobrazí jaký eror nastal
    
    case 0:oled_display->println("Pripraven");
           oled_display->display();
           break;

    case 1:oled_display->println("ERROR 1");
           oled_display->display();
           break;
    
    case 2:oled_display->println("ERROR 2");
           oled_display->display();
           break;
    
    case 3:oled_display->println("ERROR 3");
           oled_display->display();
           break;
    }

  if(bluetooth_active == 1){        // Zobrazí zda je bluetooth aktivní
    oled_display->setCursor(61, 5);
    oled_display->setTextColor(WHITE, BLACK);
    oled_display->setTextSize(1);
    oled_display->println("B");
    oled_display->display();   
    }

  switch(current_mod){              // Zobrazí jaký mód je vybrán
    
    case 1:oled_display->setCursor(0, 5);
           oled_display->setTextColor(WHITE, BLACK);
           oled_display->setTextSize(1);
           oled_display->println("Rychlost");
           oled_display->display();
           break;

    case 2:oled_display->setCursor(0, 5);
           oled_display->setTextColor(WHITE, BLACK);
           oled_display->setTextSize(1);
           oled_display->println("Zrychleni");
           oled_display->display();
           break;
    
    }
  
}

void Display::pageMenu(){                      // Zobrazí obrazovku menu

#ifdef DEBUG
  Serial.println(_cursor);
  Serial.println(last_cursor);
#endif

  if(first_run_menu == true){           // Při prvním spuštění vymaže modrou část
    oled_display->clearDisplay();
    first_run_menu = false;
  }

  if(_cursor != last_cursor){     // Při pozice kurzoru smaže proužek displeje, kde se kurzor pohybuje
       clearMenu();
       last_cursor = _cursor;
  }

  oled_display->setTextSize(1);         // Vypíše jednotlivé položky menu
  oled_display->setCursor(12, 18);
  oled_display->setTextColor(WHITE, BLACK);
  oled_display->println("Hlavni obrazovka");
  oled_display->setCursor(12, 30);
  oled_display->println("Nastavit vzdalenost");
  oled_display->setCursor(12, 42);
  oled_display->println("Zmena modu");
  oled_display->setCursor(12, 54);
  oled_display->println("Zmena nast. zrychl.");

  switch(_cursor){                // Zobrazí kurzor u aktuální položky, která je vybraná
    
    case 1: oled_display->setCursor(2, 18);
            oled_display->println(">");
            if(button_enc() == 1){
              actual_page = PAGE_HOME;
              first_run_menu = true;
              }
            break;

    case 2: oled_display->setCursor(2, 30);
            oled_display->println(">");
            if(button_enc() == 1){
              actual_page = PAGE_SET_DISTANCE;
              _cursor = 1;
              first_run_menu = true;
              }
            break;

    case 3: oled_display->setCursor(2, 42);
            oled_display->println(">");
            if(button_enc() == 1){
              actual_page = PAGE_SET_MOD;
              _cursor = 1;
              first_run_menu = true;
              }
            break;

    case 4: oled_display->setCursor(2, 54);
            oled_display->println(">");
            if(button_enc() == 1){
              actual_page = PAGE_COMMING_SOON;
              first_run_menu = true;
              }
            break;

    case 5: _cursor = 1;
            break;

    case 0: _cursor = 4;
             break;
    
    }

  oled_display->display();      // Zobrazí změny na displeji
  
}

void Display::clearMenu(){        // Smaže proužek displeje, kde se kurzor pohybuje
  
  oled_display->setTextSize(1);
  oled_display->setTextColor(WHITE, BLACK);
  oled_display->setCursor(2, 18);
  oled_display->println(" ");
  oled_display->setCursor(2, 30);
  oled_display->println(" ");
  oled_display->setCursor(2, 42);
  oled_display->println(" ");
  oled_display->setCursor(2, 54);
  oled_display->println(" ");
  oled_display->display();
}

void Display::clearBluePart(){         // Smaže modrou část displeje
  
  oled_display->clearDisplay();
  oled_display->display();
  
}

void Display::clearYellowPart(){
  
  oled_display->fillRect(0, 0, 128, 15, BLACK);
  oled_display->display();
  
}

void Display::pageSetDistance(){

  #ifdef DEBUG
    Serial.println(first_run_set_distance);
  #endif

  if(first_run_set_distance == true){
    clearBluePart();
    first_run_set_distance = false;
    distance_meters = 0;
    distance_centimeters = 0;
    distance = 0;
    }
  
  oled_display->setTextColor(WHITE, BLACK);
  oled_display->setTextSize(1);
  oled_display->setCursor(2, 20);
  oled_display->println("Vzdal. mezi lasery:");
  oled_display->setTextSize(2);
  oled_display->setCursor(32, 35);
  oled_display->print(distance);
  oled_display->setCursor(92, 35);
  oled_display->println("m");
  oled_display->display();

  if(distance > 10){

    distance = 10;
    
    oled_display->setTextSize(1);
    oled_display->setCursor(2, 55);
    oled_display->println("Maximalni vzdalenost!");
    //clear_blue_part();
    
    }

    else if(distance < 0){
      
      distance = 0;
      
      oled_display->setTextSize(1);
      oled_display->setCursor(2, 55);
      oled_display->println("Minimalni vzdalenost!");
      //clear_blue_part();
      
      }

      else{

        oled_display->setTextSize(1);
        oled_display->setCursor(2, 55);
        oled_display->println("                     ");
        
        }

  if(button_enc() == 1){
    actual_page = PAGE_HOME;
    first_run_set_distance = true;        
    }
  
  }

void Display::pageSetMod(){       // Menu na nastavení módu průjezdových bran
  
  if(first_run_set_distance == true){
    clearBluePart();
    first_run_set_distance = false;    
    }

  oled_display->setTextColor(WHITE, BLACK);
  oled_display->setTextSize(1);
  oled_display->setCursor(2, 20);
  oled_display->println("Zvolte mod:");
  oled_display->setCursor(12, 30);
  oled_display->println("Mereni rychlosti");
  oled_display->setCursor(12, 42);
  oled_display->println("Mereni zrychleni");
  oled_display->drawLine(0, 51, 128, 51, WHITE);
  oled_display->setCursor(12, 54);
  oled_display->println("Zpet");
  oled_display->display();

  if(_cursor != last_cursor){     // Při pozice kurzoru smaže proužek displeje, kde se kurzor pohybuje
        oled_display->setTextSize(1);
        oled_display->setTextColor(WHITE, BLACK);
        oled_display->setCursor(2, 30);
        oled_display->println(" ");
        oled_display->setCursor(2, 42);
        oled_display->println(" ");
        oled_display->setCursor(2, 54);
        oled_display->println(" ");
        oled_display->display();
       last_cursor = _cursor;
  }

  switch(_cursor){              // Zobrazí kurzor u aktuální položky, která je vybraná
    
    case 1: oled_display->setCursor(2, 30);
            oled_display->println(">");
            if(button_enc() == 1){
              current_mod = MOD_SPEED;
              actual_page = PAGE_MENU;
              _cursor = 1;
              first_run_set_distance = true;           
              }
            break;

    case 2: oled_display->setCursor(2, 42);
            oled_display->println(">");
            if(button_enc() == 1){
              current_mod = MOD_ACCELERATION;
              actual_page = PAGE_MENU;
              _cursor = 1;
              first_run_set_distance = true;           
              }
            break;

    case 3: oled_display->setCursor(2, 54);
            oled_display->println(">");
            if(button_enc() == 1){
              actual_page = PAGE_MENU;
              _cursor = 1;
              first_run_set_distance = true;           
              }
            break;

    case 4: _cursor = 1;
            break;

    case 0: _cursor = 3;
            break;
    
    }
  
}

void Display::pageCommingSoon(){          //Zobrazí obrazovku s textem 

  if(first_run_comming_soon == true){
    clearBluePart();
    first_run_comming_soon = false;    
    }  
  
  oled_display->setTextSize(1);
  oled_display->setTextColor(WHITE, BLACK);
  oled_display->setCursor(2, 30);
  oled_display->println("Tato funkce jeste");
  oled_display->setCursor(2, 40);
  oled_display->println("neni dostupna...");      
  oled_display->display();

  if(button_enc() == 1){
    actual_page = PAGE_MENU;
    first_run_comming_soon = true;           
    }
  
}

void Display::pageSwitch(){
  
  switch(actual_page){
    
    case PAGE_HOME: pageHome();
                    break;

    case PAGE_MENU: pageMenu();
                    break;

    case PAGE_SET_DISTANCE: pageSetDistance();
                            break;

    case PAGE_SET_MOD:  pageSetMod();
                        break;

    case PAGE_COMMING_SOON: pageCommingSoon();
                            break;

    case PAGE_SPEED_RESULTS:  pageSpeedResults();
                        break;

    case PAGE_TIMER:  pageTimer();
                      break;
    }
  
}

void Display::pageTimer(){

  if(first_run_timer == true){           // Při prvním spuštění vymaže modrou část
    oled_display->clearDisplay();
    first_run_timer = false;
  }

  oled_display->setTextSize(1);
  oled_display->setTextColor(WHITE, BLACK);
  oled_display->setCursor(2, 20);
  oled_display->println("Cas:");
  oled_display->setTextSize(2);
  oled_display->setCursor(23, 35);
  oled_display->print(timer_minute);
  oled_display->print(":");
  oled_display->print(timer_sec);
  oled_display->print(".");
  oled_display->println(timer_setina);
  oled_display->display();

}

void Display::pageSpeedResults(){

  if(first_run_speed_results == true){           // Při prvním spuštění vymaže modrou část
    oled_display->clearDisplay();
    first_run_speed_results = false;
  }

  _time = timer_stop - timer_start;
  _time /= 1000;

  _speed = distance / _time;
  //round(_speed);
  
  oled_display->setTextSize(1);
  oled_display->setTextColor(WHITE, BLACK);
  oled_display->setCursor(2, 20);
  oled_display->print("Vzdalenost:");
  oled_display->setCursor(2, 32);
  oled_display->print("Cas:");
  oled_display->setCursor(2, 44);
  oled_display->print("Rychlost:");
  oled_display->setCursor(70, 20);
  oled_display->print(distance);
  oled_display->println("m");
  oled_display->setCursor(70, 32);
  oled_display->print(_time);
  oled_display->println("s");
  oled_display->setCursor(70, 44);
  oled_display->print(_speed);
  oled_display->println("m/s");
  oled_display->display();

  if(button_enc() == 1){
    
    actual_page = PAGE_HOME;
    _time = 0;
    _speed = 0;
    timer_setina = 0;
    timer_sec = 0;
    timer_minute = 0;
    current_time = 0;

    first_run_set_distance = true; // Proměná, aby část kódu proběhla jen jednou
    first_run_set_mod = true;
    first_run_home = true;
    first_run_menu = true;
    first_run_comming_soon = true;
    first_run_speed_results = true;
    first_run_timer = true;

    _run_timer = false;
    _stop_timer = false;
    _timer_running = false;
    timer_start = 0;
    timer_stop = 0;

    first_run_set_distance = true; // Proměná, aby část kódu proběhla jen jednou
    first_run_set_mod = true;
    first_run_home = true;
    first_run_menu = true;
    first_run_comming_soon = true;
    first_run_speed_results = true;
    first_run_timer = true;
    oled_display->clearDisplay();
    
    }
  
}

bool Display::button_enc(){
  return false;
}
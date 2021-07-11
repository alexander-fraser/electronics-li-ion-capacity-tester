// Li-ion Battery Capacity Tester
// 2021-06-19
// 
// This program is used in conjunction with a microcontroller, a constant-current 
// load and an OLED display to test li-ion batteries. Specifically, it is used to
// determine a battery's mAh capacity. 
//
// At startup, the user is prompted to set the constant current level for the
// test. The microcontroller uses PWM to pass the associated voltage to the 
// op amp in the circuit, which controls the current load on the battery through
// a MOSFET. The microcontroller stops discharging the battery once the
// voltage cutoff is reached. The code monitors the amount of time that it takes 
// for the battery to deplete to the voltage cutoff and then calculates and displays 
// the battery capacity. 


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(-1);

const int screen_address = 0x3C;
const int pin_set_current = 5;
const int pin_read_voltage = A0;
const int pin_read_current = A1;
const int pin_button_select = 4;
const int pin_button_enter = 2;

const int set_current_array[3] = {50, 100, 250};
const int set_current_value = 0;     // Current value here is a default (just in case).
float capacity = 0.0;                 
float load_resistance = 100.0;        // Resistance (in Ohms) of the load resistor used to measure current.
float start_voltage = 0.0;
unsigned long time_start = 0;
unsigned long time_previous = 0;
unsigned long time_now = 0;


void setup(){                
  Serial.begin(9600);
  initialize_oled();
  initialize_pins();
  display_splash_screen();
  //select_current();
  select_start();
}


void loop(){
  // Start discharging the battery at the specified current.
  analogWrite(pin_set_current, set_current_value);    

  // Monitor the voltage, current, and time to accumulate the battery capacity.
  float battery_voltage = analogRead(pin_read_voltage) * (5.0 / 1023.0);   
  float test_current = analogRead(pin_read_current) * (5.0 / 1023.0) / load_resistance;
  time_now = millis();
  unsigned long sample_duration = time_now - time_previous;     
  time_previous = time_now;
  capacity = capacity + (test_current * 1000.0 * (sample_duration / 3600000.0));

  // Send the output to the OLED display.
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Voltage:  " + String(battery_voltage) + " V");
  display.println("Current:  " + String(test_current * 1000.0) + " mA");
  display.println("Duration: " + String(time_now / 60000.0) + " min");
  display.setTextColor(BLACK, WHITE);
  display.println("Capacity: " + String(capacity) + " mAh");
  display.display();
  
  delay(5000);   
}


void initialize_oled(){
  // Initialize the OLED with the I2C address.
  display.begin(SSD1306_SWITCHCAPVCC, screen_address);  
  display.clearDisplay();
}

void initialize_pins(){
  // Initialize the pins.
  pinMode(pin_button_select, INPUT_PULLUP);
  pinMode(pin_button_enter, INPUT_PULLUP);
  pinMode(pin_read_voltage, INPUT);
  pinMode(pin_read_current, INPUT);
  pinMode(pin_set_current, OUTPUT);
}

void display_splash_screen(){
  // Display splash screen.
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.setTextColor(WHITE);
  display.println("");
  display.println("   Li-ion Battery    ");
  display.println("   Capacity Tester   ");
  display.display();
  delay(5000);  
}

void select_current(){

  // 
  while (digitalRead(pin_button_enter) == HIGH) {
    
    // Send the output to the OLED display.
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,0);
    display.setTextColor(BLACK, WHITE);
    display.println("Select Test Current");
    display.setTextColor(WHITE);
    display.println("50mA");
    display.println("100mA");
    display.println("250mA");
    display.display();
  
  }
  
}

void select_start(){
  // Request user to start test.
    
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.setTextColor(WHITE);
  display.println("   Connect battery   ");
  display.println("      and press      ");
  display.println("        ENTER        ");
  display.println("    to start test    ");
  display.display();

  while (digitalRead(pin_button_enter) == HIGH) {
    // Do nothing and wait for ENTER button to be pressed.
    int button_state = digitalRead(pin_button_enter);
    Serial.println(button_state);
  }

  time_start = millis();
}

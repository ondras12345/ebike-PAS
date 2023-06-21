/*
ATtiny core:
- http://drazzy.com/package_drazzy.com_index.json
- https://github.com/SpenceKonde/ATTinyCore
Board: ATtiny25/45/85 (no bootloader)
Chip: ATtiny85
Clock Source: 1MHz (internal)
Timer1 Clock: CPU (CPU frequency)
LTO: Enabled
millis()/micros(): Enabled
Save EEPROM: EEPROM Retained
B.O.D. Level: B.O.D. Enabled (4.3V)

Burn Bootloader first to set fuses, than Upload Using Programmer.
*/

// https://github.com/PaulStoffregen/TimerOne
// you need to get it from Git (commit
// 756a4b3cf4bd77c8f53483ede2ea0e245409c624 needs to be applied)
#include <TimerOne.h>

#define pin_start 3  // active low
#define pin_stop 4  // active low
#define pin_output 1

#define start_hold_time 2000  // ms
// 10Hz --> 0.1s:
#define output_period 100000  // us


bool running = false;
unsigned long start_prev_millis = 0;
bool prev_pin_start = true;  // active low


void setup()
{
    pinMode(pin_start, INPUT);
    pinMode(pin_stop, INPUT);
    pinMode(pin_output, OUTPUT);

    /* If the start button is held at the time of startup, wait for it to be
    released (in case it was shorted or something). */
    delay(500);
    while(!digitalRead(pin_start));

    Timer1.initialize(output_period / 2);
    Timer1.attachInterrupt(timer1_interrupt);
    Timer1.stop();
    digitalWrite(pin_output, LOW);
}


void loop()
{
    if (!digitalRead(pin_stop) && running)
    {
        running = false;
        Timer1.stop();
        digitalWrite(pin_output, LOW);
    }


    if (running) return;

    // Treat pin_start as HIGH (not pressed) if pin_stop is LOW
    bool curr_pin_start = digitalRead(pin_start) || !digitalRead(pin_stop);

    if (!curr_pin_start && prev_pin_start)
        start_prev_millis = millis();

    if (!curr_pin_start && (millis() - start_prev_millis) >= start_hold_time)
    {
        running = true;
        Timer1.start();
    }

    prev_pin_start = curr_pin_start;
}


void timer1_interrupt()
{
    static bool output_state = false;
    output_state = !output_state;
    digitalWrite(pin_output, output_state);
}

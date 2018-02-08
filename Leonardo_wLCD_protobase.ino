/******************************************************************************

    Leonardo_wLCD_protobase.ino
        basis for multiple quick test things
        debugout on usbserial interface: 115200baud

    hardware:
        Board:
            Arduino compatible (with serial port)
            Arduino MICRO
            LED on pin 13
        Connections:
            D4 --> push button to GND
            D5 --> push button to GND
            D6 --> push button to GND
            D7 --> push button to GND
            D8 / A8 --> poti 5V-GND
            D9 --> LED green
            D10 --> LED blue
            D11 --> LED white

            SCK --> Clock input of ?
            MOSI --> Data input of ?

            D2 I2C SDA --> Data for ?
            D3 I2C SCL --> Clock for ?

            D0 RX --> Serial receive for DMX
            D1 TX --> Serial transmit for DMX
            D12 --> DMX direction (Send/Receive)


    libraries used:
        ~ slight_DebugMenu
        ~ slight_Button
        ~ slight_FaderLin
            MIT, cc by sa, s-light - stefan krueger,
            http://s-light.eu
        ~ DMXSerial
			Copyright (c) 2005-2012 by Matthias Hertel,
			http://www.mathertel.de

    written by stefan krueger (s-light),
        github@s-light.eu, http://s-light.eu, https://github.com/s-light/

    changelog / history
        check git commit messages

    TO DO:
        ~ xx


******************************************************************************/
/******************************************************************************
    The MIT License (MIT)

    Copyright (c) 2018 Stefan Krüger

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
******************************************************************************/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Includes
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// use "file.h" for files in same directory as .ino
// #include "file.h"
// use <file.h> for files in library directory
// #include <file.h>

#include <SPI.h>

#include <LiquidCrystal.h>


#include <slight_DebugMenu.h>
#include <slight_FaderLin.h>
#include <slight_ButtonInput.h>

#include <DMXSerial.h>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Info
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void sketchinfo_print(Print &out) {
    out.println();
    //             "|~~~~~~~~~|~~~~~~~~~|~~~..~~~|~~~~~~~~~|~~~~~~~~~|"
    out.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
    out.println(F("|                       ^ ^                      |"));
    out.println(F("|                      (0,0)                     |"));
    out.println(F("|                      ( _ )                     |"));
    out.println(F("|                       \" \"                      |"));
    out.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
    out.println(F("| Leonardo_wLCD_protobase.ino"));
    out.println(F("|   branch: 'DMXMonitor' - receive DMX and show on lcd"));
    out.println(F("|"));
    out.println(F("| This Sketch has a debug-menu:"));
    out.println(F("| send '?'+Return for help"));
    out.println(F("|"));
    out.println(F("| dream on & have fun :-)"));
    out.println(F("|"));
    out.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
    out.println(F("|"));
    //out.println(F("| Version: Nov 11 2013  20:35:04"));
    out.print(F("| version: "));
    out.print(F(__DATE__));
    out.print(F("  "));
    out.print(F(__TIME__));
    out.println();
    out.println(F("|"));
    out.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
    out.println();

    //out.println(__DATE__); Nov 11 2013
    //out.println(__TIME__); 20:35:04
}


// Serial.print to Flash: Notepad++ Replace RegEx
//     Find what:        Serial.print(.*)\("(.*)"\);
//     Replace with:    Serial.print\1\(F\("\2"\)\);



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// definitions (global)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Debug Output

Print &DebugOut = Serial;
Stream &DebugIn = Serial;
// attention: in setup_DebugOut 'Serial' is hardcoded used for initialisation


bool infoled_state = 0;
const byte infoled_pin = 13;

unsigned long debugOut_LiveSign_TimeStamp_LastAction = 0;
const uint16_t debugOut_LiveSign_UpdateInterval = 1000; //ms

bool debugOut_LiveSign_Serial_Enabled = 0;
bool debugOut_LiveSign_LED_Enabled = 1;

bool dmx_serial_out_enabled = true;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Menu

// slight_DebugMenu(Stream &in_ref, Print &out_ref, uint8_t input_length_new);
slight_DebugMenu myDebugMenu(Serial, Serial, 15);


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// LCD

LiquidCrystal lcd(A5, A4, A3, A2, A1, A0);

uint32_t lcd_update_timestamp_last = 0;
const uint16_t lcd_update_interval = 500; //ms

struct cursor_pos_t {
    uint8_t x;
    uint8_t y;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// analog in

const uint8_t analog_pin = 8;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// button

// enum button_names {
//     button
// };

const uint16_t button_duration_Debounce      =   10;
const uint16_t button_duration_HoldingDown   = 1000;
const uint16_t button_duration_ClickSingle   =   40;
const uint16_t button_duration_ClickLong     = 1000;
const uint16_t button_duration_ClickDouble   =  200;

const uint8_t button_1_BLACK = 0;
const uint8_t button_2_GREEN = 1;
const uint8_t button_3_YELLOW = 2;
const uint8_t button_4_RED = 3;


const uint8_t buttons_COUNT = 4;
slight_ButtonInput buttons[buttons_COUNT] = {
    slight_ButtonInput(
        button_1_BLACK,
        4,
        button_getInput,
        button_onEvent,
        button_duration_Debounce,
        button_duration_HoldingDown,
        button_duration_ClickSingle,
        button_duration_ClickLong,
        button_duration_ClickDouble
    ),
    slight_ButtonInput(
        button_2_GREEN,
        5,
        button_getInput,
        button_onEvent,
        button_duration_Debounce,
        button_duration_HoldingDown,
        button_duration_ClickSingle,
        button_duration_ClickLong,
        button_duration_ClickDouble
    ),
    slight_ButtonInput(
        button_3_YELLOW,
        6,
        button_getInput,
        button_onEvent,
        button_duration_Debounce,
        button_duration_HoldingDown,
        button_duration_ClickSingle,
        button_duration_ClickLong,
        button_duration_ClickDouble
    ),
    slight_ButtonInput(
        button_4_RED,
        7,
        button_getInput,
        button_onEvent,
        button_duration_Debounce,
        button_duration_HoldingDown,
        button_duration_ClickSingle,
        button_duration_ClickLong,
        button_duration_ClickDouble
    )
};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// DMX

const uint8_t dmx_pin_direction = 12;

// timeout in milliseconds
const uint32_t dmx_valid_timeout = 1000;

bool dmx_valid = false;

uint16_t dmx_start_channel = 1;
uint8_t dmx_value = 0;
uint8_t dmx_values_old[] = {0, 0, 0, 0};

enum display_modes {
    dm_uint8,
    dm_int8,
    dm_uint16,
    dm_int16,
    dm_uint32,
    dm_int32,
};

display_modes display_mode = dm_uint8;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// other things..












//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// functions
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// DebugOut

// freeRam found at
// http://forum.arduino.cc/index.php?topic=183790.msg1362282#msg1362282
// posted by mrburnette
int freeRam () {
    extern int __heap_start, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void setup_DebugOut(Print &out) {
    // for ATmega32U4 devices:
    #if defined (__AVR_ATmega32U4__)
        // wait for arduino IDE to release all serial ports after upload.
        delay(1000);
    #endif

    Serial.begin(115200);

    // for ATmega32U4 devices:
    #if defined (__AVR_ATmega32U4__)
        // Wait for Serial Connection to be Opend from Host or
        // timeout after 6second
        uint32_t timeStamp_Start = millis();
        while( (! Serial) && ( (millis() - timeStamp_Start) < 1000 ) ) {
            // nothing to do
        }
    #endif


    out.println();

    out.print(F("# Free RAM = "));
    out.println(freeRam());
}

void handle_debugout() {
    if (
        (millis() - debugOut_LiveSign_TimeStamp_LastAction) >
        debugOut_LiveSign_UpdateInterval
    ) {
        debugOut_LiveSign_TimeStamp_LastAction = millis();

        if ( debugOut_LiveSign_Serial_Enabled ) {
            DebugOut.print(millis());
            DebugOut.print(F("ms;"));
            DebugOut.print(F("  free RAM = "));
            DebugOut.print(freeRam());
            // DebugOut.print(F("; bat votlage: "));
            // DebugOut.print(bat_voltage/100.0);
            // DebugOut.print(F("V"));
            DebugOut.println();
        }

        if ( debugOut_LiveSign_LED_Enabled ) {
            infoled_state = ! infoled_state;
            if (infoled_state) {
                //set LED to HIGH
                digitalWrite(infoled_pin, HIGH);
            } else {
                //set LED to LOW
                digitalWrite(infoled_pin, LOW);
            }
        }

    }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Menu System

void setup_DebugMenu(Print &out) {
    out.print(F("# Free RAM = "));
    out.println(freeRam());

    out.println(F("setup DebugMenu:")); {
        // myDebugMenu.set_user_EOC_char(';');
        myDebugMenu.set_callback(handleMenu_Main);
        myDebugMenu.begin();
    }
    out.println(F("\t finished."));
}

// Main Menu
void handleMenu_Main(slight_DebugMenu *pInstance) {
    Print &out = pInstance->get_stream_out_ref();
    char *command = pInstance->get_command_current_pointer();
    // out.print("command: '");
    // out.print(command);
    // out.println("'");
    switch (command[0]) {
        case 'h':
        case 'H':
        case '?': {
            // help
            out.println(F("__________________________________________"));
            out.println();
            out.println(F("Help for Commands:"));
            out.println();
            out.println(F("\t '?': this help"));
            out.println(F("\t 'i': sketch info"));
            out.println(F("\t 'y': toggle DebugOut livesign print"));
            out.println(F("\t 'Y': toggle DebugOut livesign LED"));
            out.println(F("\t 'x': tests"));
            out.println(F("\t 'X': test snprintf"));
            out.println();
            out.println(F("\t 'A': Show 'HelloWorld' "));
            // out.println(F("\t 'a': toggle sequencer"));
            // out.println(F("\t 's': set channel 's1:65535'"));
            // out.println(F("\t 'f': DemoFadeTo(ID, value) 'f1:65535'"));
            out.println();
            out.println(F("__________________________________________"));
        } break;
        case 'i': {
            sketchinfo_print(out);
        } break;
        case 'y': {
            out.println(F("\t toggle DebugOut livesign Serial:"));
            debugOut_LiveSign_Serial_Enabled = !debugOut_LiveSign_Serial_Enabled;
            out.print(F("\t debugOut_LiveSign_Serial_Enabled:"));
            out.println(debugOut_LiveSign_Serial_Enabled);
        } break;
        case 'Y': {
            out.println(F("\t toggle DebugOut livesign LED:"));
            debugOut_LiveSign_LED_Enabled = !debugOut_LiveSign_LED_Enabled;
            out.print(F("\t debugOut_LiveSign_LED_Enabled:"));
            out.println(debugOut_LiveSign_LED_Enabled);
        } break;
        case 'x': {
            // get state
            out.println(F("__________"));
            out.println(F("Tests:"));

            out.println(F("nothing to do."));

            // uint16_t wTest = 65535;
            uint16_t wTest = atoi(&command[1]);
            out.print(F("wTest: "));
            out.print(wTest);
            out.println();

            out.print(F("1: "));
            out.print((byte)wTest);
            out.println();

            out.print(F("2: "));
            out.print((byte)(wTest>>8));
            out.println();

            out.println();

            out.println(F("__________"));
        } break;
        case 'X': {
            // get state
            out.println(F("__________"));
            out.println(F("test snprintf:"));
            // out.print(F("result:"));

            int16_t values[] = {
                 1,  50,  300,  5000,  11333,
                -1, -50, -300, -5000, -11333,
            };

            char line[100];
            int16_t value = 0;
            for (size_t i = 0; i < (sizeof(values)/sizeof(int16_t)); i++) {
                value = values[i];
                    snprintf(line, sizeof(line),
                    "u  3: '%3u' = '% 3u' = '%+3u'",
                    value, value, value
                );
                out.println(line);
                snprintf(line, sizeof(line),
                    "d  3: '%3d' = '% 3d' = '%+3d'",
                    value, value, value
                );
                out.println(line);
            }
            for (size_t i = 0; i < (sizeof(values)/sizeof(int16_t)); i++) {
                value = values[i];
                    snprintf(line, sizeof(line),
                    "u  4: '%4u' = '% 4u' = '%+4u'",
                    value, value, value
                );
                out.println(line);
                snprintf(line, sizeof(line),
                    "d  4: '%4d' = '% 4d' = '%+4d'",
                    value, value, value
                );
                out.println(line);
            }
            for (size_t i = 0; i < (sizeof(values)/sizeof(int16_t)); i++) {
                value = values[i];
                    snprintf(line, sizeof(line),
                    "u  5: '%5u' = '% 5u' = '%+5u'",
                    value, value, value
                );
                out.println(line);
                snprintf(line, sizeof(line),
                    "d  5: '%5d' = '% 5d' = '%+5d'",
                    value, value, value
                );
                out.println(line);
            }
            for (size_t i = 0; i < (sizeof(values)/sizeof(int16_t)); i++) {
                value = values[i];
                    snprintf(line, sizeof(line),
                    "u  6: '%6u' = '% 6u' = '%+6u'",
                    value, value, value
                );
                out.println(line);
                snprintf(line, sizeof(line),
                    "d  6: '%6d' = '% 6d' = '%+6d'",
                    value, value, value
                );
                out.println(line);
            }
            for (size_t i = 0; i < (sizeof(values)/sizeof(int16_t)); i++) {
                value = values[i];
                    snprintf(line, sizeof(line),
                    "u  7: '%7u' = '% 7u' = '%+7u'",
                    value, value, value
                );
                out.println(line);
                snprintf(line, sizeof(line),
                    "d  7: '%7d' = '% 7d' = '%+7d'",
                    value, value, value
                );
                out.println(line);
            }



            out.println(F("__________________________________________"));
        } break;
        //---------------------------------------------------------------------
        case 'A': {
            out.println(F("\t Hello World! :-)"));
        } break;
        case 'd': {
            out.println(F("\t toggle dmx serial out:"));
            dmx_serial_out_enabled = !dmx_serial_out_enabled;
            out.print(F("\t dmx_serial_out_enabled:"));
            out.println(dmx_serial_out_enabled);
        } break;
        case 's': {
            out.print(F("\t set channel "));
            // convert part of string to int
            // (up to first char that is not a number)
            // uint8_t command_offset = 1;
            // uint8_t ch = atoi(&command[command_offset]);
            // // convert single character to int representation
            // // uint8_t id = &command[1] - '0';
            // command_offset = 3;
            // if (ch > 9) {
            //     command_offset = command_offset +1;
            // }
            // out.print(ch);
            // out.print(F(" : "));
            // uint16_t value = atoi(&command[command_offset]);
            // out.print(value);
            // out.println();
            //
            // if (output_enabled) {
            //     tlc.setChannel(ch, value);
            //     tlc.write();
            // }
        } break;
        // case 'f': {
        //     out.print(F("\t DemoFadeTo "));
        //     // convert part of string to int
        //     // (up to first char that is not a number)
        //     uint8_t id = atoi(&command[1]);
        //     // convert single character to int representation
        //     // uint8_t id = &command[1] - '0';
        //     out.print(id);
        //     out.print(F(" : "));
        //     uint16_t value = atoi(&command[3]);
        //     out.print(value);
        //     out.println();
        //     //demo_fadeTo(id, value);
        //     tlc.setChannel()
        //     out.println(F("\t demo for parsing values --> finished."));
        // } break;
        //---------------------------------------------------------------------
        default: {
            if(strlen(command) > 0) {
                out.print(F("command '"));
                out.print(command);
                out.println(F("' not recognized. try again."));
            }
            pInstance->get_command_input_pointer()[0] = '?';
            pInstance->set_flag_EOC(true);
        }
    } // end switch

    // end Command Parser
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// button

void buttons_setup(Print &out) {
    out.println(F("setup button:")); {
        // out.println(F("\t set button pin"));
        // pinMode(button.getPin(), INPUT_PULLUP);
        // out.println(F("\t button begin"));
        // button.begin();
        for (size_t index = 0; index < buttons_COUNT; index++) {
            pinMode(buttons[index].getPin(), INPUT_PULLUP);
            buttons[index].begin();
        }
    }
    out.println(F("\t finished."));
}

void buttons_update() {
    for (size_t index = 0; index < buttons_COUNT; index++) {
        buttons[index].update();
    }
}

boolean button_getInput(uint8_t id, uint8_t pin) {
    // read input invert reading - button closes to GND.
    // check HWB
    // return ! (PINE & B00000100);
    return ! digitalRead(pin);
}

void button_onEvent(slight_ButtonInput *pInstance, byte bEvent) {
    // DebugOut.print(F("FRL button:"));
    // DebugOut.println((*pInstance).getID());
    //
    // DebugOut.print(F("Event: "));
    // DebugOut.print(bEvent);
    // // (*pInstance).printEvent(Serial, bEvent);
    // DebugOut.println();

    uint8_t button_id = (*pInstance).getID();

    // show event additional infos:
    switch (bEvent) {
        // case slight_ButtonInput::event_StateChanged : {
        //     DebugOut.println(F("\t state: "));
        //     (*pInstance).printlnState(Serial);
        //     DebugOut.println();
        // } break;
        case slight_ButtonInput::event_Down : {
            // DebugOut.println(F("FRL down"));
        } break;
        case slight_ButtonInput::event_HoldingDown : {
            uint32_t duration = (*pInstance).getDurationActive();
            // DebugOut.println(F("duration active: "));
            // DebugOut.println(duration);

            uint8_t count = 1;

            if (duration <= 5000) {
                count = 5;
            }
            else if (duration <= 10000) {
                count = 10;
            }
            else if (duration <= 15000) {
                count = 50;
            }

            if (button_id == button_1_BLACK) {
                dmx_start_channel -= (uint16_t)count;
                print_ch(lcd);
            }
            if (button_id == button_2_GREEN) {
                dmx_start_channel += (uint16_t)count;
                print_ch(lcd);
            }
            if (button_id == button_3_YELLOW) {
                //
            }
            if (button_id == button_4_RED) {
                //
            }

        } break;
        case slight_ButtonInput::event_Up : {
            DebugOut.println(F("up"));
            // myFaderRGB_fadeTo(500, 0, 0, 1);
        } break;
        case slight_ButtonInput::event_Click : {
            DebugOut.println(F("click"));

            if (button_id == button_1_BLACK) {
                dmx_start_channel -= (uint16_t)1;
                print_ch(lcd);
            }
            if (button_id == button_2_GREEN) {
                dmx_start_channel += (uint16_t)1;
                print_ch(lcd);
            }
            if (button_id == button_3_YELLOW) {
                previous_display_mode();
            }
            if (button_id == button_4_RED) {
                next_display_mode();
            }

        } break;
        case slight_ButtonInput::event_ClickLong : {
            DebugOut.println(F("click long"));
        } break;
        case slight_ButtonInput::event_ClickDouble : {
            // DebugOut.println(F("click double"));
        } break;
        case slight_ButtonInput::event_ClickTriple : {
            // DebugOut.println(F("click triple"));
        } break;
        // case slight_ButtonInput::event_ClickMulti : {
        //     DebugOut.print(F("click count: "));
        //     DebugOut.println((*pInstance).getClickCount());
        // } break;
    }  // end switch
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// LCD

void setup_LCD(Print &out) {
    out.println(F("setup button:")); {
        out.println(F("\t set 16x2"));
        // setup LCD's number of columns & rows
        lcd.begin(16, 2);

        out.println(F("\t print welcome text"));
        // set cursor (first char, first line)
        lcd.setCursor(0, 0);
        lcd.print("32U4wLCD_proto");
        lcd.setCursor(0, 1);
        lcd.print("DMXMonitor");

        // delay(500);
        // lcd.clear();
    }
    out.println(F("\t finished."));
}


// Display Layout:
//              0000000000111111
//              0123456789012345
//           0  ch:512 uint16  *
// dm_uint8  1   255 255 255 255
// dm_int8   1  -128-128-128-128
// dm_uint16 1   65535  65535
// dm_int16  1  -32768 -32768
// dm_uint32 1   4294967295
// dm_int32  1  -2147483648
char dm_uint8_fs[]  = "%4u%4u%4u%4u";
char dm_int8_fs[]   = "%4d%4d%4d%4d";
char dm_uint16_fs[] = "%6u %6u";
char dm_int16_fs[]  = "%6d %6d";
char dm_uint32_fs[] = "%11u";
char dm_int32_fs[]  = "%11d";

cursor_pos_t lp_ch_text = {0, 0};
cursor_pos_t lp_ch = {3, 0};
cursor_pos_t lp_mode = {7, 0};
cursor_pos_t lp_dmx_signal = {15, 0};
cursor_pos_t lp_values = {0, 1};


void print_ch(LiquidCrystal &lcd) {
    lcd.setCursor(lp_ch.x, lp_ch.y);
    // lcd.print(dmx_start_channel);
    // slight_DebugMenu::print_uint16_align_right(lcd, dmx_start_channel);
    char line[4];
    snprintf(line, sizeof(line), "%3d", dmx_start_channel);
    lcd.print(line);
}

void print_mode(LiquidCrystal &lcd) {
    lcd.setCursor(lp_mode.x, lp_mode.y);
    switch (display_mode) {
        case dm_uint8: {
            lcd.print("uint8 ");
        } break;
        case dm_int8: {
            lcd.print(" int8 ");
        } break;
        case dm_uint16: {
            lcd.print("uint16");
        } break;
        case dm_int16: {
            lcd.print(" int16");
        } break;
        case dm_uint32: {
            lcd.print("uint32");
        } break;
        case dm_int32: {
            lcd.print(" int32");
        } break;
    }
}

void print_DMXSignal(LiquidCrystal &lcd) {
    lcd.setCursor(lp_dmx_signal.x, lp_dmx_signal.y);
    if (dmx_valid) {
        // blink
        if ((millis() % 1000) <= 500) {
            lcd.print("*");
        } else {
            lcd.print("+");
        }
    } else {
        lcd.print("!");
    }
}



void print_DMXValues_clear(LiquidCrystal &lcd) {
    lcd.setCursor(lp_values.x, lp_values.y);
    lcd.print("                ");
}

void print_DMXValues(LiquidCrystal &lcd) {
    lcd.setCursor(lp_values.x, lp_values.y);
    char line[17];
    switch (display_mode) {
        case dm_uint8: {
            snprintf(line, sizeof(line), dm_uint8_fs,
                uint8_t(DMXSerial.read(dmx_start_channel + 0)),
                uint8_t(DMXSerial.read(dmx_start_channel + 1)),
                uint8_t(DMXSerial.read(dmx_start_channel + 2)),
                uint8_t(DMXSerial.read(dmx_start_channel + 3))
            );
        } break;
        case dm_int8: {
            snprintf(line, sizeof(line), dm_int8_fs,
                int8_t(DMXSerial.read(dmx_start_channel + 0)),
                int8_t(DMXSerial.read(dmx_start_channel + 1)),
                int8_t(DMXSerial.read(dmx_start_channel + 2)),
                int8_t(DMXSerial.read(dmx_start_channel + 3))
            );
        } break;
        case dm_uint16: {
            uint16_t value1 = 0;
            value1 |= DMXSerial.read(dmx_start_channel + 0) << 8;
            value1 |= DMXSerial.read(dmx_start_channel + 1);
            uint16_t value2 = 0;
            value2 |= DMXSerial.read(dmx_start_channel + 2) << 8;
            value2 |= DMXSerial.read(dmx_start_channel + 3);
            snprintf(line, sizeof(line), dm_uint16_fs,
                uint16_t(value1),
                uint16_t(value2)
            );
        } break;
        case dm_int16: {
            uint16_t value1 = 0;
            value1 |= DMXSerial.read(dmx_start_channel + 0) << 8;
            value1 |= DMXSerial.read(dmx_start_channel + 1);
            uint16_t value2 = 0;
            value2 |= DMXSerial.read(dmx_start_channel + 2) << 8;
            value2 |= DMXSerial.read(dmx_start_channel + 3);
            snprintf(line, sizeof(line), dm_int16_fs,
                int16_t(value1),
                int16_t(value2)
            );
        } break;
        case dm_uint32: {
            uint32_t value = 0;
            value |= uint32_t (DMXSerial.read(dmx_start_channel + 0)) << 24;
            value |= uint32_t (DMXSerial.read(dmx_start_channel + 1)) << 16;
            value |= uint32_t (DMXSerial.read(dmx_start_channel + 2)) << 8;
            value |= DMXSerial.read(dmx_start_channel + 1);
            snprintf(line, sizeof(line), dm_uint32_fs, uint32_t(value));
        } break;
        case dm_int32: {
            uint32_t value = 0;
            value |= uint32_t (DMXSerial.read(dmx_start_channel + 0)) << 24;
            value |= uint32_t (DMXSerial.read(dmx_start_channel + 1)) << 16;
            value |= uint32_t (DMXSerial.read(dmx_start_channel + 2)) << 8;
            value |= DMXSerial.read(dmx_start_channel + 1);
            snprintf(line, sizeof(line), dm_int32_fs, int32_t(value));
        } break;
    }
    lcd.print(line);
    if (dmx_serial_out_enabled) {
        char line2[17];
        snprintf(line2, sizeof(line2), "%12u:", millis());
        DebugOut.print(line2);
        DebugOut.print(line);
        DebugOut.println();
    }
}

void update_LCD(LiquidCrystal &lcd) {
    lcd.clear();

    lcd.setCursor(lp_ch_text.x, lp_ch_text.y);
    lcd.print("ch:");
    print_ch(lcd);

    print_mode(lcd);
    print_DMXSignal(lcd);

    print_DMXValues(lcd);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// DMX

void setup_DMX(Print &out) {
    out.println(F("setup DMX:"));

    // pin for direction
    pinMode(dmx_pin_direction, OUTPUT);

    // set to receive mode
    // DebugOut.println(F("\t set direction pin to Low = 'Receive' "));
    // digitalWrite(dmx_pin_direction, LOW);
    out.println(F("\t init as DMXReceiver"));
    DMXSerial.init(DMXReceiver, dmx_pin_direction);

    // set to send mode
    // DebugOut.println(F("\t set direction pin to High = 'Send' "));
    // digitalWrite(dmx_pin_direction, HIGH);
    // DebugOut.println(F("\t init as DMXController"));
    // DMXSerial.init(DMXController, dmx_pin_direction);



    // DebugOut.println(F("\t set some values"));
    // DMXSerial.write(10, 255);
    // DMXSerial.write(11, 255);
    // DMXSerial.write(12, 1);
    // read dmx values
    // DMXSerial.read(1);

    out.println(F("\t finished."));
}

void handle_DMX() {

    bool dmx_valid_new = false;
	if (DMXSerial.noDataSince() < dmx_valid_timeout) {
		dmx_valid_new = true;
	}

    // check if dmx_valid state has changed
    if (dmx_valid != dmx_valid_new) {
        dmx_valid = dmx_valid_new;
        print_mode(lcd);
        print_DMXSignal(lcd);
	}

    if (dmx_valid) {
        // check if values are new
        // this triggers really to much with no changing data...
        // if (DMXSerial.dataUpdated()) {
        //     DMXSerial.resetUpdated();
        //     print_DMXValues(lcd);
        // }
        // check if values are new
        bool flag_new = false;
        for (size_t i = 0; i < sizeof(dmx_values_old); i++) {
            uint8_t value_new = DMXSerial.read(dmx_start_channel + i);
            if (dmx_values_old[i] != value_new) {
                dmx_values_old[i] = value_new;
                flag_new = true;
            }
        }
        if (flag_new) {
            print_DMXValues(lcd);
        }
        // uint8_t dmx_value_new = DMXSerial.read(dmx_start_channel);
        // set_value(dmx_value_new);
    }
    // else {
    //     set_value(
    //         map(
    //             analogRead(analog_pin),
    //              0, 1023,
    //              0, 255
    //         )
    //     );
    // }



    // combine 16bit value
	// uiDMXValue_Pan  = DMXSerial.read(11) << 8;
	// uiDMXValue_Pan  = uiDMXValue_Pan | DMXSerial.read(12);
    // read 8bit value
	// uiDMXValue_Pan  = DMXSerial.read(12);


}


void set_value(uint8_t dmx_value_new) {
    // check if dmx_value has changed
    if (dmx_value != dmx_value_new) {
        dmx_value = dmx_value_new;
        // print_DMXValues(lcd);
        // myServo.write(
        //     map(dmx_value, 0, 255, 0, 180)
        // );
    }
}

void next_display_mode() {
    switch (display_mode) {
        case dm_uint8: {
            display_mode = dm_int8;
        } break;
        case dm_int8: {
            display_mode = dm_uint16;
        } break;
        case dm_uint16: {
            display_mode = dm_int16;
        } break;
        case dm_int16: {
            display_mode = dm_uint32;
        } break;
        case dm_uint32: {
            display_mode = dm_int32;
        } break;
        case dm_int32: {
            display_mode = dm_uint8;
        } break;
        default: {
            display_mode = dm_uint8;
        }
    }
    print_mode(lcd);
    print_DMXValues_clear(lcd);
    print_DMXValues(lcd);
}

void previous_display_mode() {
    switch (display_mode) {
        case dm_uint8: {
            display_mode = dm_int32;
        } break;
        case dm_int8: {
            display_mode = dm_uint8;
        } break;
        case dm_uint16: {
            display_mode = dm_int8;
        } break;
        case dm_int16: {
            display_mode = dm_uint16;
        } break;
        case dm_uint32: {
            display_mode = dm_int16;
        } break;
        case dm_int32: {
            display_mode = dm_uint32;
        } break;
        default: {
            display_mode = dm_uint8;
        }
    }
    print_mode(lcd);
    print_DMXValues_clear(lcd);
    print_DMXValues(lcd);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// other things..



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// setup
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setup() {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // initialise PINs

    //LiveSign
    pinMode(infoled_pin, OUTPUT);
    digitalWrite(infoled_pin, HIGH);

    // as of arduino 1.0.1 you can use INPUT_PULLUP

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    setup_DebugOut(DebugOut);

    setup_DebugMenu(DebugOut);

    sketchinfo_print(DebugOut);

    setup_LCD(DebugOut);

    buttons_setup(DebugOut);

    setup_DMX(DebugOut);

    delay(2000);
    update_LCD(lcd);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // go
    DebugOut.println(F("Loop:"));
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// main loop
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void loop() {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // menu input
    myDebugMenu.update();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // update sub parts

    buttons_update();

    handle_debugout();

    handle_DMX();

    if (
        (millis() - lcd_update_timestamp_last) > lcd_update_interval
    ) {
        lcd_update_timestamp_last =  millis();
        print_DMXSignal(lcd);
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// THE END
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

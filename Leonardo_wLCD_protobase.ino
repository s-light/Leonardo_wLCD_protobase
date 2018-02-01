/******************************************************************************

    Leonardo_wLCD_protobase.ino
        basis for multiple quick test things
        debugout on usbserial interface: 115200baud

    hardware:
        Board:
            Arduino compatible (with serial port)
            LED on pin 13
        Connections:
            D4 --> push button to GND
            D5 --> push button to GND
            D6 --> push button to GND
            D7 --> push button to GND
            D8 / A8 --> poti 5V-GND
            D9 --> LED green
            D9 --> LED blue
            D9 --> LED white

            SCK --> Clock input of ?
            MOSI --> Data input of ?

            I2C SDA --> Data for LSM303 compass
            I2C SCL --> Clock for LSM303 compass

            RX --> Serial receive for ?
            TX --> Serial transmit for ?


    libraries used:
        ~ slight_DebugMenu
        ~ slight_Button
        ~ slight_FaderLin
            MIT, cc by sa, s-light - stefan krueger,
            http://s-light.eu
        ~ LSM303 compass
            Copyright (c) 2013 Pololu Corporation
            license MIT
            https://github.com/pololu/lsm303-arduino

    written by stefan krueger (s-light),
        github@s-light.eu, http://s-light.eu, https://github.com/s-light/

    changelog / history
        check git commit messages

    TO DO:
        ~ xx


******************************************************************************/
/******************************************************************************
    The MIT License (MIT)

    Copyright (c) 2017 Stefan Krüger

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
#include <slight_ButtonInput.h>

#include <Wire.h>
#include <LSM303.h>


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
    out.println(F("|   basis for multiple quick test things"));
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

boolean infoled_state = 0;
const byte infoled_pin = 13;

unsigned long debugOut_LiveSign_TimeStamp_LastAction = 0;
const uint16_t debugOut_LiveSign_UpdateInterval = 1000; //ms

boolean debugOut_LiveSign_Serial_Enabled = 0;
boolean debugOut_LiveSign_LED_Enabled = 1;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Menu

// slight_DebugMenu(Stream &in_ref, Print &out_ref, uint8_t input_length_new);
slight_DebugMenu myDebugMenu(Serial, Serial, 15);


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// LCD

LiquidCrystal lcd(A5, A4, A3, A2, A1, A0);


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// button

slight_ButtonInput button(
    0,  // byte cbID_New
    4,  // byte cbPin_New,
    button_getInput,  // tCbfuncGetInput cbfuncGetInput_New,
    button_onEvent,  // tcbfOnEvent cbfCallbackOnEvent_New,
      30,  // const uint16_t cwDuration_Debounce_New = 30,
     500,  // const uint16_t cwDuration_HoldingDown_New = 1000,
      50,  // const uint16_t cwDuration_ClickSingle_New =   50,
     500,  // const uint16_t cwDuration_ClickLong_New =   3000,
     500   // const uint16_t cwDuration_ClickDouble_New = 1000
);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// LSM303 compass


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// other things..

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// functions
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// debug things

// freeRam found at
// http://forum.arduino.cc/index.php?topic=183790.msg1362282#msg1362282
// posted by mrburnette
int freeRam () {
    extern int __heap_start, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Menu System

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
            out.println(F("____________________________________________________________"));
            out.println();
            out.println(F("Help for Commands:"));
            out.println();
            out.println(F("\t '?': this help"));
            out.println(F("\t 'i': sketch info"));
            out.println(F("\t 'y': toggle DebugOut livesign print"));
            out.println(F("\t 'Y': toggle DebugOut livesign LED"));
            out.println(F("\t 'x': tests"));
            out.println();
            out.println(F("\t 'A': Show 'HelloWorld' "));
            // out.println(F("\t 'a': toggle sequencer"));
            // out.println(F("\t 's': set channel 's1:65535'"));
            // out.println(F("\t 'f': DemoFadeTo(ID, value) 'f1:65535'"));
            out.println();
            out.println(F("____________________________________________________________"));
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
        //---------------------------------------------------------------------
        case 'A': {
            out.println(F("\t Hello World! :-)"));
        } break;
        case 'a': {
            out.println(F("\t toggle sequencer:"));
            // if (sequencer_mode == sequencer_OFF) {
            //     /* code */
            //     sequencer_mode = sequencer_CHANNELCHECK;
            //     out.print(F("\t sequencer_mode: CHANNELCHECK\n"));
            // }
            // else {
            //     sequencer_mode = sequencer_OFF;
            //     out.print(F("\t sequencer_mode: OFF\n"));
            // }
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
// LCD

void setup_LCD(Print &out) {
    out.println(F("setup button:")); {
        out.println(F("\t set 16x2"));
        // setup LCD's number of columns & rows
        lcd.begin(16, 2);

        out.println(F("\t print welcome text"));
        // set cursor (first char, first line)
        lcd.setCursor(0, 0);
        lcd.print("hello world :-)");
        lcd.setCursor(0, 1);
        lcd.print("32U4wLCD_proto");

        // delay(500);
        // lcd.clear();
    }
    out.println(F("\t finished."));
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// button

void setup_Buttons(Print &out) {
    out.println(F("setup button:")); {
        out.println(F("\t set button pin"));
        pinMode(button.getPin(), INPUT_PULLUP);
        out.println(F("\t button begin"));
        button.begin();
    }
    out.println(F("\t finished."));
}

boolean button_getInput(uint8_t id, uint8_t pin) {
    // read input invert reading - button closes to GND.
    // check HWB
    // return ! (PINE & B00000100);
    return ! digitalRead(pin);
}

void button_onEvent(slight_ButtonInput *pInstance, byte bEvent) {
    // Serial.print(F("FRL button:"));
    // Serial.println((*pInstance).getID());
    //
    // Serial.print(F("Event: "));
    // Serial.print(bEvent);
    // // (*pInstance).printEvent(Serial, bEvent);
    // Serial.println();

    // uint8_t button_id = (*pInstance).getID();

    // show event additional infos:
    switch (bEvent) {
        // case slight_ButtonInput::event_StateChanged : {
        //     Serial.println(F("\t state: "));
        //     (*pInstance).printlnState(Serial);
        //     Serial.println();
        // } break;
        case slight_ButtonInput::event_Down : {
            // Serial.println(F("FRL down"));
        } break;
        case slight_ButtonInput::event_HoldingDown : {
            uint32_t duration = (*pInstance).getDurationActive();
            Serial.println(F("duration active: "));
            Serial.println(duration);
            if (duration <= 5000) {
                // myFaderRGB_fadeTo(1000, 65000, 65000, 65000);
            }
            else if (duration <= 10000) {
                // myFaderRGB_fadeTo(1000, 0, 65000, 65000);
            }
            else {
                // myFaderRGB_fadeTo(1000, 0, 0, 65000);
            }

        } break;
        case slight_ButtonInput::event_Up : {
            Serial.println(F("up"));
            // myFaderRGB_fadeTo(500, 0, 0, 1);
        } break;
        case slight_ButtonInput::event_Click : {
            Serial.println(F("click"));
            //
        } break;
        case slight_ButtonInput::event_ClickLong : {
            Serial.println(F("click long"));
            //
        } break;
        case slight_ButtonInput::event_ClickDouble : {
            // Serial.println(F("click double"));
            // sequencer_mode = sequencer_HORIZONTAL;
            // sequencer_interval = 1000;
            // Serial.print(F("\t sequencer_mode: HORIZONTAL\n"));
        } break;
        case slight_ButtonInput::event_ClickTriple : {
            // sequencer_mode = sequencer_SPIRAL;
            // sequencer_interval = 100;
            // Serial.print(F("\t sequencer_mode: SPIRAL\n"));
            // Serial.println(F("click triple"));
        } break;
        // case slight_ButtonInput::event_ClickMulti : {
        //     Serial.print(F("click count: "));
        //     Serial.println((*pInstance).getClickCount());
        // } break;
    }  // end switch
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// LSM303 compass




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
    // initialise serial

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

        Print &out = Serial;
        out.println();

        out.print(F("# Free RAM = "));
        out.println(freeRam());

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // print welcome

        sketchinfo_print(out);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // setup LCD

        setup_LCD(out);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // setup button

        out.print(F("# Free RAM = "));
        out.println(freeRam());

        setup_Buttons(out);


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // setup LSM303

        setup_LSM303(out);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // setup XXX1

        // out.print(F("# Free RAM = "));
        // out.println(freeRam());
        //
        // out.println(F("setup XXX1:")); {
        //
        //     out.println(F("\t sub action"));
        // }
        // out.println(F("\t finished."));

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // show serial commands

        // myDebugMenu.set_user_EOC_char(';');
        myDebugMenu.set_callback(handleMenu_Main);
        myDebugMenu.begin();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // go

        out.println(F("Loop:"));

} /** setup **/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// main loop
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void loop() {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // menu input
        myDebugMenu.update();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // update sub parts

        // myFaderRGB.update();
        button.update();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // timed things

        // if (sequencer_mode != sequencer_OFF) {
        //     if(
        //         (millis() - sequencer_timestamp_last) > sequencer_interval
        //     ) {
        //         sequencer_timestamp_last =  millis();
        //         // calculate_step();
        //     }
        // }



    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // debug output

        if (
            (millis() - debugOut_LiveSign_TimeStamp_LastAction) >
            debugOut_LiveSign_UpdateInterval
        ) {
            debugOut_LiveSign_TimeStamp_LastAction = millis();

            if ( debugOut_LiveSign_Serial_Enabled ) {
                Serial.print(millis());
                Serial.print(F("ms;"));
                Serial.print(F("  free RAM = "));
                Serial.print(freeRam());
                // Serial.print(F("; bat votlage: "));
                // Serial.print(bat_voltage/100.0);
                // Serial.print(F("V"));
                Serial.println();
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

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // other things

} /** loop **/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// THE END
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

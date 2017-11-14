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

            I2C SDA --> Data for ?
            I2C SCL --> Clock for ?

            RX --> Serial receive for ?
            TX --> Serial transmit for ?


    libraries used:
        ~ slight_DebugMenu
        ~ slight_Button
        ~ slight_FaderLin
            MIT, cc by sa, s-light - stefan krueger,
            http://s-light.eu
        ~ Adafruit_NeoPixel
			Written by Phil Burgess / Paint Your Dragon for Adafruit Industries,
			Adafruit Industries GNU LGPL,
			https://github.com/adafruit/Adafruit_NeoPixel
			https://learn.adafruit.com/adafruit-neopixel-uberguide
		~ Adafruit_DotStar
			Written by Limor Fried and Phil Burgess for Adafruit Industries,
			Adafruit Industries GNU LGPL,
			https://github.com/adafruit/Adafruit_DotStar
			https://learn.adafruit.com/adafruit-dotstar-leds
        ~ FastLED
            MIT, FastLED-Team
            https://github.com/FastLED/FastLED

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
#include <slight_FaderLin.h>
#include <slight_ButtonInput.h>

// #include <Tlc59711.h>

// #include <Adafruit_NeoPixel.h>
// #include <Adafruit_DotStar.h>

#include "FastLED.h"

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
// FaderLin

// const uint8_t myFaderRGB__channel_count = colors_per_led;
// slight_FaderLin myFaderRGB(
//     0, // byte cbID_New
//     myFaderRGB__channel_count, // byte cbChannelCount_New
//     myFaderRGB_callback_OutputChanged, // tCbfuncValuesChanged cbfuncValuesChanged_New
//     myCallback_onEvent // tCbfuncStateChanged cbfuncStateChanged_New
// );

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
// Pixel LED Strip

// Wohnzimmer: 4m 60Pixel/m + 1m 144Pixel/m = 384 Pixel
// Wohnzimmer: 1,5m 60Pixel/m = 90 Pixel
// const uint16_t myStrip_PixelCount = 5; // 60mA/Pixel

// Attetion:
//   the slight_FaderLin lib can only handle up to 255channels -
//   so max 80 Pixel!!
// but the next problem is the available memory! 50 are working.
// const uint16_t myStrip_PixelCount = 120;
// const uint16_t myStrip_PixelCount = 384;

const uint16_t myStrip_PixelCount = 144;


// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

// const uint8_t pinNeoPixelData		= 5;
// Adafruit_NeoPixel myStrip = Adafruit_NeoPixel(myStrip_PixelCount, pinNeoPixelData, NEO_GRB + NEO_KHZ800);


// use HW SPI
// Adafruit_DotStar myStrip = Adafruit_DotStar(myStrip_PixelCount);
// Adafruit_DotStar myStrip = Adafruit_DotStar(myStrip_PixelCount, DOTSTAR_RGB);

// CRGB leds[myStrip_PixelCount];
CRGBArray<myStrip_PixelCount> leds;

bool strip_off = true;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sequencer

enum sequencer_modes {
    sequencer_OFF,
    sequencer_CHANNELCHECK,
    sequencer_HORIZONTAL,
    sequencer_SPIRAL,
};

uint8_t sequencer_mode = sequencer_OFF;

uint32_t sequencer_timestamp_last = millis();
uint32_t sequencer_interval = 1000; // ms

//
uint16_t value_low = 1;
uint16_t value_high = 65000;




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
// FaderLin

// void setup_myFaderRGB(Print &out) {
//     out.println(F("setup myFaderRGB:"));
//
//     out.println(F("\t myFaderRGB.begin();"));
//     myFaderRGB.begin();
//
//     out.println(F("\t myFaderRGB welcome fade"));
//     // myFaderRGB.startFadeTo( 1000, memList_A[memList_A__Full]);
//     myFaderRGB_fadeTo(1000, 60000, 60000, 0);
//     // run fading
//     // while ( myFaderRGB.getLastEvent() == slight_FaderLin::event_fading_Finished) {
//     //     myFaderRGB.update();
//     // }
//     while ( myFaderRGB.update() == slight_FaderLin::state_Fading) {
//         // nothing to do.
//     }
//     myFaderRGB.update();
//     // myFaderRGB.startFadeTo( 1000, memList_A[memList_A__Off]);
//     myFaderRGB_fadeTo(1000, 0, 0, 1);
//     // run fading
//     // while ( myFaderRGB.getLastEvent() != slight_FaderLin::event_fading_Finished) {
//     //     myFaderRGB.update();
//     // }
//     while ( myFaderRGB.update() == slight_FaderLin::state_Fading) {
//         // nothing to do.
//     }
//     myFaderRGB.update();
//
//     out.println(F("\t finished."));
// }
//
// void myFaderRGB_callback_OutputChanged(uint8_t id, uint16_t *values, uint8_t count) {
//
//     // if (bDebugOut_myFaderRGB_Output_Enable) {
//     //     Serial.print(F("OutputValue: "));
//     //     printArray_uint16(Serial, wValues, bCount);
//     //     Serial.println();
//     // }
//
//     // for (size_t channel_index = 0; channel_index < count; channel_index++) {
//     //     tlc.setChannel(channel_index, values[channel_index]);
//     // }
//
//     if (output_enabled) {
//         tlc.setRGB(values[0], values[1], values[2]);
//         tlc.write();
//     }
//
// }
//
// void myFaderRGB_fadeTo(uint16_t duration, uint16_t r, uint16_t g, uint16_t b) {
//     uint16_t values[myFaderRGB__channel_count];
//     // init array with 0
//     values[0] = r;
//     values[1] = g;
//     values[2] = b;
//     myFaderRGB.startFadeTo(duration, values);
// }
//
// void myCallback_onEvent(slight_FaderLin *pInstance, byte event) {
//
//
//     // Serial.print(F("Instance ID:"));
//     // Serial.println((*pInstance).getID());
//     //
//     // Serial.print(F("Event: "));
//     // (*pInstance).printEvent(Serial, event);
//     // Serial.println();
//
//
//     // react on events:
//     switch (event) {
//         case slight_FaderLin::event_StateChanged : {
//             // Serial.print(F("slight_FaderLin "));
//             // Serial.print((*pInstance).getID());
//             // Serial.println(F(" : "));
//             // Serial.print(F("\t state: "));
//             // (*pInstance).printState(Serial);
//             // Serial.println();
//
//             // switch (state) {
//             //     case slight_FaderLin::state_Standby : {
//             //             //
//             //         } break;
//             //     case slight_FaderLin::state_Fading : {
//             //             //
//             //         } break;
//             //     case slight_FaderLin::state_Finished : {
//             //             //
//             //         } break;
//             // } //end switch
//
//
//         } break;
//
//         case slight_FaderLin::event_fading_Finished : {
//             // Serial.print(F("\t fading Finished."));
//         } break;
//     } //end switch
//
// }

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
            // if (sequencer_mode == sequencer_OFF) {
            //     sequencer_mode = sequencer_CHANNELCHECK;
            //     sequencer_interval = 500;
            //     Serial.print(F("\t sequencer_mode: CHANNELCHECK\n"));
            // }
            // else {
            //     sequencer_off();
            //     sequencer_mode = sequencer_OFF;
            //     Serial.print(F("\t sequencer_mode: OFF\n"));
            // }


            strip_off = !strip_off;
            if (strip_off) {
                Serial.println(F("switch on"));
                // leds.fill_solid(CRGB::White);
                // leds.fadeToBlackBy(40);
                // switch all leds on - one after the other
                for(int whiteLed = 0; whiteLed < myStrip_PixelCount; whiteLed = whiteLed + 1) {
                    leds[whiteLed] = CRGB::White;
                    FastLED.show();
                    delay(10);
                }
            }
            else {
                Serial.println(F("switch off"));
                // leds.fadeToBlackBy(40);
                leds.fill_solid(CRGB::Black);
            }
            FastLED.show();

        } break;
        case slight_ButtonInput::event_ClickLong : {
            Serial.println(F("click long"));
            // Move a single white led
            for(int whiteLed = 0; whiteLed < myStrip_PixelCount; whiteLed = whiteLed + 1) {
                // Turn our current led on to white, then show the leds
                leds[whiteLed] = CRGB::White;

                // Show the leds (only one of which is set to white, from above)
                FastLED.show();

                // Wait a little bit
                delay(10);

                // Turn our current led back to black for the next loop around
                leds[whiteLed] = CRGB::Black;
            }
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
// LED Strip

void setup_LEDStrip(Print &out) {
    out.println(F("setup LEDStrip:"));

    out.println(F("\t init lib"));
    // myStrip.begin();
    FastLED.addLeds<APA102, RGB>(leds, myStrip_PixelCount);
    // out.println(F("\t start with leds off"));
    // myStrip.show();
    out.println(F("\t finished."));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// LEDBoard

// void setup_Boards(Print &out) {
//     out.println(F("setup LEDBoards:"));
//
//     out.println(F("\t init tlc lib"));
//     tlc.beginFast();
//     out.println(F("\t start with leds off"));
//     tlc.setRGB();
//     tlc.write();
//     out.println(F("\t set leds to 1, 1, 1"));
//     tlc.setRGB(1, 1, 1);
//     tlc.write();
//
//     out.println(F("\t finished."));
// }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Sequencer

// uint8_t sequencer_current_step = 0;
// uint8_t sequencer_direction_forward = true;
//
// void sequencer_off() {
//     Serial.println("sequencer_off");
//     uint16_t values[colorchannels_per_board];
//     // init array with 0
//     memset(values, 0, colorchannels_per_board);
//
//     for (size_t ch = 0; ch < colorchannels_per_board; ch++) {
//         values[ch] = value_low;
//     }
//
//     // reset sequencer
//     sequencer_current_step = 0;
//
//     // now map values to tlc chips and write theme to output
//     map_to_allBoards(values);
// }
//
// void calculate_step__channelcheck(uint16_t values[]) {
//     // Serial.print("calculate_step__channelcheck: ");
//
//     for (size_t ch = 0; ch < colorchannels_per_board; ch++) {
//         values[ch] = value_low;
//         if (ch == sequencer_current_step) {
//             values[ch] = value_high;
//         }
//     }
//
//     // prepare next step
//     Serial.print("sequencer_current_step: ");
//     Serial.println(sequencer_current_step);
//     sequencer_current_step = sequencer_current_step + 1;
//     if (sequencer_current_step >= colorchannels_per_board) {
//         sequencer_current_step = 0;
//     }
// }
//
// void calculate_step__horizontal(uint16_t values[]) {
//     // Serial.println("calculate_step__horizontal: ");
//
//     for (size_t column = 0; column < leds_per_column; column++) {
//         for (size_t row = 0; row < leds_per_row; row++) {
//
//             uint8_t pixel = channel_position_map[column][row];
//             uint8_t ch = pixel * 3;
//
//             // set pixel to low
//             values[ch + 0] = value_low;
//             values[ch + 1] = value_low;
//             values[ch + 2] = value_low;
//
//
//             if (column == sequencer_current_step) {
//                 // set pixel to high
//                 values[ch + 0] = value_low;
//                 values[ch + 1] = value_high;
//                 values[ch + 2] = value_high;
//             }
//
//
//         }
//     }
//
//     // prepare next step
//     // Serial.print("sequencer_current_step: ");
//     // Serial.println(sequencer_current_step);
//     sequencer_current_step = sequencer_current_step + 1;
//     if (sequencer_current_step >= leds_per_column) {
//         sequencer_current_step = 0;
//     }
//
// }
//
// void calculate_step__spiral(uint16_t values[]) {
//     // Serial.println("calculate_step__spiral: ");
//
//     const uint8_t spiral_order[leds_per_column][leds_per_row] {
//         { 0,  1,  2,  3},
//         {11, 12, 13,  4},
//         {10, 15, 14,  5},
//         { 9,  8,  7,  6},
//     };
//
//     for (size_t column = 0; column < leds_per_column; column++) {
//         for (size_t row = 0; row < leds_per_row; row++) {
//
//             uint8_t pixel = channel_position_map[column][row];
//             uint8_t ch = pixel * 3;
//
//             // set pixel to low
//             values[ch + 0] = value_low;
//             values[ch + 1] = value_low;
//             values[ch + 2] = value_low;
//
//
//             if (spiral_order[column][row] == sequencer_current_step) {
//                 // set pixel to high
//                 values[ch + 0] = 20000;
//                 values[ch + 1] = value_low;
//                 values[ch + 2] = 65535;
//             }
//
//
//         }
//     }
//
//     // prepare next step
//     // Serial.print("sequencer_current_step: ");
//     // Serial.println(sequencer_current_step);
//     if (sequencer_direction_forward) {
//         // forward
//         if (sequencer_current_step >= (leds_per_column * leds_per_row)-1 ) {
//             sequencer_current_step = sequencer_current_step - 1;
//             sequencer_direction_forward = false;
//             // Serial.println("direction switch to backwards");
//         }
//         else {
//             sequencer_current_step = sequencer_current_step + 1;
//         }
//     }
//     else {
//         // backwards
//         if (sequencer_current_step == 0 ) {
//             sequencer_current_step = sequencer_current_step + 1;
//             sequencer_direction_forward = true;
//             // Serial.println("direction switch to forward");
//         }
//         else {
//             sequencer_current_step = sequencer_current_step - 1;
//         }
//     }
//     // Serial.print("next step: ");
//     // Serial.println(sequencer_current_step);
//
// }
//
// void calculate_step() {
//     Serial.print("calculate_step: ");
//     uint16_t values[colorchannels_per_board];
//     // init array with 0
//     memset(values, 0, colorchannels_per_board);
//
//     // deside what sequencer we want to run
//
//     switch (sequencer_mode) {
//         case sequencer_OFF: {
//             1;
//         } break;
//         case sequencer_CHANNELCHECK: {
//             calculate_step__channelcheck(values);
//         } break;
//         case sequencer_HORIZONTAL: {
//             calculate_step__horizontal(values);
//         } break;
//         case sequencer_SPIRAL: {
//             calculate_step__spiral(values);
//         } break;
//     }
//
//     // debug out print array:
//     // Serial.print("values: ");
//     // slight_DebugMenu::print_uint16_array(
//     //     Serial,
//     //     values,
//     //     colorchannels_per_board
//     // );
//     // Serial.println();
//
//     // now map values to tlc chips and write theme to output
//     map_to_allBoards(values);
// }
//
// void map_to_allBoards(uint16_t values[]) {
//     if (output_enabled) {
//         // set all channels (mapping)
//         for (
//             size_t channel_index = 0;
//             channel_index < colorchannels_per_board;
//             channel_index++
//         ) {
//             // uint8_t mapped_channel = mapping_single_board[i];
//             // Serial.print("mapping: ");
//             // Serial.print(i);
//             // Serial.print("-->");
//             // Serial.print(mapped_channel);
//             // Serial.println();
//             for (size_t board_index = 0; board_index < boards_count; board_index++) {
//                 tlc.setChannel(
//                     channel_index + (tlc_channels_per_board * board_index),
//                     values[channel_index]
//                 );
//             }
//         }
//         // write data to chips
//         tlc.write();
//     }
// }
//



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
    // setup LEDBoard

        setup_LEDStrip(out);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // setup LEDBoard

        // setup_Boards(out);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // setup Fader

    // out.print(F("# Free RAM = "));
    // out.println(freeRam());

    // setup_myFaderRGB(out);

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

        if (sequencer_mode != sequencer_OFF) {
            if(
                (millis() - sequencer_timestamp_last) > sequencer_interval
            ) {
                sequencer_timestamp_last =  millis();
                // calculate_step();
            }
        }



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

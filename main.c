/************** ECE2049 DEMO CODE ******************/
/**************  20 August 2016   ******************/
/***************************************************/
#define RST 1
#define C3_ 523/4
#define D3_ 587/4
#define E3_ 659/4
#define F3_ 698/4
#define G3_ 784/4
#define A3_ 440/2
#define B3f 466/2
#define B3_ 494/2
#define C4_ 523/2
#define D4_ 587/2
#define E4_ 659/2
#define F4_ 698/2
#define G4_ 784/2
#define A4_ 440
#define B4f 466
#define B4_ 494
#define C5_ 523
#define D5_ 587
#define E5_ 659
#define F5_ 698
#define G5_ 784




#include <msp430.h>

/* Peripherals.c and .h are where the functions that implement
 * the LEDs and keypad, etc are. It is often useful to organize
 * your code by putting like functions together in files.
 * You include the header associated with that file(s)
 * into the main file of your project. */
#include "peripherals.h"

// Function Prototypes
void countDown();

// Declare globals here
char currKey = 0;
// Main
void main(void)

{
    WDTCTL = WDTPW | WDTHOLD;       // Stop watchdog timer

    // Useful code starts here
    initLeds();

    configDisplay();
    configKeypad();

    // *** Intro Screen ***
    Graphics_clearDisplay(&g_sContext); // Clear the display

    // Write some text to the display
    Graphics_drawStringCentered(&g_sContext, "MSP430 Hero", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "Press '*' to begin", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
    while (getKey() != '*');
    countDown();

    playTone(D4_,4);
    playTone(D3_,4);
    playTone(D4_,8);
    while (1) {
        playTone(A4_,16);
        playTone(D4_,16);
        playTone(D5_,16);
        playTone(A4_,16);
        playTone(C5_,4);
    //    playTone(RST,4);
        playTone(C5_,8);
        playTone(C5_,4);
        playTone(B4_,16);
        playTone(C5_,16);
        playTone(B4_,16);
        playTone(A4_,16);
        playTone(B4_,16);

        playTone(F4_,8);
        playTone(D4_,8);
        playTone(A4_,4);
        playTone(A4_,4);
        playTone(A4_,8);
        playTone(A4_,8);

        playTone(F4_,16);
        playTone(A4_,16);
        playTone(D4_,16);
        playTone(F4_,16);
        playTone(G4_,4);
        playTone(G4_,4);
        playTone(G4_,8);
        playTone(F4_,8);

        playTone(A4_,16);
        playTone(G4_,16);
        playTone(F4_,8);
        playTone(D4_,4);
        playTone(D4_,4);
        playTone(D4_,8);
    }


    while (1)    // Forever loop
    {
        // Check if any keys have been pressed on the 3x4 keypad
        currKey = getKey();



    }  // end while (1)
}




/**
 * Write a function to configure the 4 lab board buttons. See the Lab Board schematics,
 * Homework 2 and Lecture6- 8 for information on digital IO and the buttons.
 */



/**
 *
 * Write a function that returns the state of the lab board buttons with 1=pressed and 0=not pressed.
 * For example if S1 alone is pressed then the function should return 00000001b = 0x01. If only S3 is
 * pressed then the function should return 00000100b = 0x04. If both S1 and S3 are pressed then the
 * function should return 00000101b = 0x05, etc. Remember that the buttons are not on contiguous I/O
 * port pins. You will have to check each individually.
 */

unsigned char getBoardButtons() {
    unsigned char buttons = 0b00000000; //standardized in C++14, may not work
    //    if (getS1) {
    //        buttons |= 0b00000001;
    //    }
    //    if (getS2){
    //        buttons |= 0b00000010;
    //    }
    //    if (getS3){
    //        buttons |= 0b00000100;
    //    }
    //    if (getS4){
    //        buttons |= 0b00001000;
    //    }
    return buttons;


}

/**
 * Write a complete C function to configure and light the 2 user LEDs on the MSP430F5529 Launchpad board
 * based on the char argument passed. If BIT0 of the argument = 1, LED1 is lit and if BIT0=0 then LED1
 * is off. Similarly, if BIT1 of the argument = 1, LED2 is lit and if BIT1=0 then LED2 is off. Again, see
 * the MSP430F5529 Launchpad User's Guide (Useful Links), HW#2 and Lecture 8 for information on the user
 * LEDs and functions configuring and the using other LEDs.
 *
 */

void setLEDs(unsigned char ledconfig) {

    //    if (ledconfig & 0b00000001) {
    //        setLED1on();
    //    }
    //    if (ledconfig & 0b00000010) {
    //        setLED2on();
    //    }
    //    if (ledconfig & 0b00000100) {
    //        setLED3on();
    //    }
    //    if (ledconfig & 0b00001000) {
    //        setLED4on();
    //    }


}

/**
 * In order to play a song, you will need to find a way to give each note of your song both pitch and
 * duration. You will also need to find a way to map notes to LEDs such that the same note always light
 * the same LED. With only 4 multi-colored LEDs the same LED will need to correspond to more than one
 * note but that is the case in Guitar Hero, too. What data structures will you use to store pitch,
 * duration and the corresponding LED?
 *
 * pitch as unsigned char to get 255 pitches
 *
 * duration should be again an unsigned char, but used inversely, so a value of 4 would be a quarter note
 *
 * What length songs will you eventually want to play? Given how
 * you choose to save your notes, etc., how much memory will that require?
 *
 * Assuming to chose a song that has 3 tracks,
 *          one with 8th notes repeating every other measure
 *          one with 16th notes repeating every measure
 *          one with 4th notes repeating every 4 measures
 *
 *          and each note takes two bytes
 *
 * this would take up 2*(8*2 + 16 + 4*4) = 64 + 32 = 96 bytes to create a 3 track loop
 */

void playNote (char note) {
    if (note == 'A') {

    }
    if (note == 'B') {

    }
    if (note == 'C') {

    }
    if (note == 'D') {

    }
    if (note == 'E') {

    }
    if (note == 'F') {

    }
    if (note == 'G') {

    }
}

void countDown() {
    Graphics_clearDisplay(&g_sContext); // Clear the display
    Graphics_drawStringCentered(&g_sContext, "3", 1, 48, 15,OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
    swDelay(1);
    Graphics_drawStringCentered(&g_sContext, "2", 1, 48, 15,OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
    swDelay(1);
    Graphics_drawStringCentered(&g_sContext, "1", 1, 48, 15,OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
    swDelay(1);

}

void humiliatePlayer(){
	Graphics_clearDisplay(&g_sContext); // Clear the display
        Graphics_drawStringCentered(&g_sContext, "You Lose!", AUTO_STRING_LENGTH, 48, 15,OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, "You'll Never Be An", AUTO_STRING_LENGTH, 48, 25,OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, "MSP430 HERO!", AUTO_STRING_LENGTH, 48, 35,OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, "Press *", AUTO_STRING_LENGTH, 48, 45,OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, "To Reset", AUTO_STRING_LENGTH, 48, 55,OPAQUE_TEXT);
        Graphics_flushBuffer(&g_sContext);
	//TODO Play some sad music
        while (getKey() != '*');
}

void congratulatePlayer(){
	Graphics_clearDisplay(&g_sContext); // Clear the display
        Graphics_drawStringCentered(&g_sContext, "You Are An", AUTO_STRING_LENGTH, 48, 15,OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, "MSP430 HERO!", AUTO_STRING_LENGTH, 48, 25,OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, "Press *", AUTO_STRING_LENGTH, 48, 35,OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, "To Reset", AUTO_STRING_LENGTH, 48, 45,OPAQUE_TEXT);
        Graphics_flushBuffer(&g_sContext);
        //TODO play some fanfare
	while (getKey() != '*');
	
}


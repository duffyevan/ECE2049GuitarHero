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

#define MAX_MISSED_NOTES 5
#define MAX_LOOPS 2

#include <msp430.h>

/* Peripherals.c and .h are where the functions that implement
 * the LEDs and keypad, etc are. It is often useful to organize
 * your code by putting like functions together in files.
 * You include the header associated with that file(s)
 * into the main file of your project. */
#include "peripherals.h"

// Function Prototypes
void countDown();
void kickstartSong(int noteIndex);

//Structures
typedef struct tone {
    int frequency;
    int duration;
} Tone;

// Declare globals here
char currKey = 0; // keeps track of the current keypad key
char currentNoteIndex = 0; // keeps track of the current location in the song array
unsigned char missedNotes = 0; // keeps track of the number of notes the player has missed, end the game when theyve missed too many
unsigned char currentLEDs = 0; // keeps track of the current state of the LEDs
unsigned char loopCounter = 0;

Tone song[] = {{D4_,4},{D3_,4},{D4_,8},{A4_,16},{D4_,16},{D5_,16},{A4_,16},{C5_,4},
{C5_,8},{C5_,4},{B4_,16},{C5_,16},{B4_,16},{A4_,16},{B4_,16},{F4_,8},{D4_,8},{A4_,4},
{A4_,4},{A4_,8},{A4_,8},{F4_,16},{A4_,16},{D4_,16},{F4_,16},{G4_,4},{G4_,4},{G4_,8},
{F4_,8},{A4_,16},{G4_,16},{F4_,8},{D4_,4},{D4_,4},{D4_,8}}; // this is the song!


//ISRs
#pragma vector=TIMER2_A0_VECTOR
__interrupt void Timer_A2_ISR(void){
    BuzzerOff(); // stop the previous tone
    currentNoteIndex++; // step to the next tone
    if (currentNoteIndex == 35){ // if we reach the end, loop to the beginning (after the intro)
        currentNoteIndex = 3; // loop the song
	loopCounter++;
    }
    timerA2InterruptDisable(); // dont play the next note till the software tells you to
    // set the leds to a led corrisponding to a range of notes
    if (song[currentNoteIndex].frequency > C5_){
    	setLeds(0x08);
    	currentLEDs = 0x08;
    }
    else if (song[currentNoteIndex].frequency > E4_){
    	setLeds(0x04);
    	currentLEDs = 0x04;
    }
    else if (song[currentNoteIndex].frequency > G3_){
    	setLeds(0x02);
    	currentLEDs = 0x02;
    }
    else {
    	setLeds(0x01);
    	currentLEDs = 0x01; // hang on to the led state for later...
    }
    
    unsigned int cntr = 0;
    while (++cntr < 0xA0FF && getButtons() != currentLEDs);

    if (cntr == 0xA0FF){
	missedNotes++;
    }
    playHWTone(song[currentNoteIndex].frequency, song[currentNoteIndex].duration); // kickstart the next tone of the song
}


// Main
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;       // Stop watchdog timer
    _BIS_SR(GIE); // global interrupt enable


    // Useful code starts here
    initLeds(); // init the ports for the LEDs
    configureButtons(); // init the ports for the push buttons

    configDisplay(); // init the graphics lib
    configKeypad(); // init the ports for the keypad

    while (1){
    	// *** Intro Screen ***
    	Graphics_clearDisplay(&g_sContext); // Clear the display

    	// Write some text to the display
    	Graphics_drawStringCentered(&g_sContext, "MSP430 Hero", AUTO_STRING_LENGTH, 48, 15, OPAQUE_TEXT);
    	Graphics_drawStringCentered(&g_sContext, "Press '*' to begin", AUTO_STRING_LENGTH, 48, 25, OPAQUE_TEXT);
    	Graphics_flushBuffer(&g_sContext);
    	while (getKey() != '*');
    	countDown(); // play the 3,2,1 animation
    	Graphics_clearDisplay(&g_sContext); // Clear the display
    	Graphics_drawStringCentered(&g_sContext, "Auckland", AUTO_STRING_LENGTH, 48, 15, OPAQUE_TEXT); // print the name of the song while the song is playing
    	Graphics_flushBuffer(&g_sContext);

    	setupTimerA2(); // setup timer control register
    	kickstartSong(0); // start the song from the beginning


    	while (1)    // Forever loop
    	{
    	    
    	    if (missedNotes >= MAX_MISSED_NOTES){ // if the player misses too many notes
    	    	timerA2InterruptDisable(); // stop the timer interrupts so the next note doesnt play
    	    	BuzzerOff(); // stop the current tone
    		
		humiliatePlayer();

    	    	missedNotes = 0; // reset the missed notes for the next game
    	    	currentNoteIndex = 0; // put the index of the song at the beginnig
    	    	loopCounter = 0;
		break; // break out and return to the menu
    	    }

	    if (loopCounter > MAX_LOOPS){
		timerA2InterruptDisable(); // stop the timer interrupts so the next note doesnt play
    	    	BuzzerOff(); // stop the current tone
    		
		congratulatePlayer();	
	    	
    	    	missedNotes = 0; // reset the missed notes for the next game
    	    	currentNoteIndex = 0; // put the index of the song at the beginnig
    	    	loopCounter = 0;
		break; // break out and return to the menu
	    }
   	}  // end while (1)
    }
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

// unsigned char getBoardButtons() {
//     unsigned char buttons = 0b00000000; //standardized in C++14, may not work
//     
//     if (getS1) {
//         buttons |= 0b00000001;
//     }
//     if (getS2){
//         buttons |= 0b00000010;
//     }
//     if (getS3){
//         buttons |= 0b00000100;
//     }
//     if (getS4){
//         buttons |= 0b00001000;
//     }
//     return buttons;
// 
// 
// }

/**
 * Write a complete C function to configure and light the 2 user LEDs on the MSP430F5529 Launchpad board
 * based on the char argument passed. If BIT0 of the argument = 1, LED1 is lit and if BIT0=0 then LED1
 * is off. Similarly, if BIT1 of the argument = 1, LED2 is lit and if BIT1=0 then LED2 is off. Again, see
 * the MSP430F5529 Launchpad User's Guide (Useful Links), HW#2 and Lecture 8 for information on the user
 * LEDs and functions configuring and the using other LEDs.
 *
 */

//void setLEDs(unsigned char ledconfig) {
//
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


//}

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
        Graphics_drawStringCentered(&g_sContext, "Press #", AUTO_STRING_LENGTH, 48, 45,OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, "To Reset", AUTO_STRING_LENGTH, 48, 55,OPAQUE_TEXT);
        Graphics_flushBuffer(&g_sContext);
        playTone(B3_,4);
        playTone(RST,4);
        playTone(B3_,4);
        playTone(B4_,4);
	while (getKey() != '#');
}

void congratulatePlayer(){
	Graphics_clearDisplay(&g_sContext); // Clear the display
        Graphics_drawStringCentered(&g_sContext, "You Are An", AUTO_STRING_LENGTH, 48, 15,OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, "MSP430 HERO!", AUTO_STRING_LENGTH, 48, 25,OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, "Press #", AUTO_STRING_LENGTH, 48, 35,OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, "To Reset", AUTO_STRING_LENGTH, 48, 45,OPAQUE_TEXT);
        Graphics_flushBuffer(&g_sContext);
        //TODO play some fanfare
        playTone(D4_,4);
        playTone(D3_,4);
        playTone(D4_,8);
	while (getKey() != '#');
	
}

void kickstartSong(int noteIndex){
    currentNoteIndex = noteIndex;
    playHWTone(song[currentNoteIndex].frequency, song[currentNoteIndex].duration); // kickstart the song, the rest is handled by interrupts
}


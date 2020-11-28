// Physics 4BL Group 2 Project 4
// Written by Luke Bender 11-18-20

#include <LiquidCrystal.h>

# define CHARS 0
# define NUMBERS 1
# define CHARS_AND_NUMBERS 2

// MODIFY THESE BETWEEN TESTS ===================
    // Do 5 tests for every STIMULI_LENGTH, TEST_TYPE combination
    const int STIMULI_LENGTH = 1; // 1, 4, 7, 10
    const int TEST_TYPE = CHARS; // CHARS, NUMBERS, CHARS_AND_NUMBERS
// ==============================================

const int WORKING_SET = 4;
const int COMPARE_SET = 10;

// CONSTANTS
const int RS = 12, EN = 11, D4 = 5, D5 = 4, D6 = 3, D7 = 2;

const int LCD_HEIGHT = 2;
const int LCD_WIDTH = 16;

const int YES_BUTTON = 8;
const int NO_BUTTON = 9;

const int BUTTON_DELAY = 300;

const float LOWER_MATCH_CHANCE = 0.4;
const float UPPER_MATCH_CHANCE = 0.6;

const int LOWER_DELAY_TIME = 500;
const int UPPER_DELAY_TIME = 2000;

// GLOBALS
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
char memorize_strings[WORKING_SET][STIMULI_LENGTH + 1];
char compare_strings[COMPARE_SET][STIMULI_LENGTH + 1];

// FUNCTIONS
void fill_set(char set[][STIMULI_LENGTH + 1], const int SET_LENGTH);
void clear_set(char set[][STIMULI_LENGTH + 1], const int SET_LENGTH);
void fill_random(char set[][STIMULI_LENGTH + 1], const int SET_LENGTH, int num_random);

void setup()
{
    Serial.begin(9600);

    lcd.begin(LCD_WIDTH, LCD_HEIGHT);

    pinMode(YES_BUTTON, INPUT);
    pinMode(NO_BUTTON, INPUT);

    randomSeed(analogRead(A0));

    lcd.setCursor(0, 0);
    lcd.print("OPEN MONITOR    ");
    lcd.setCursor(0, 1);
    lcd.print("BEFORE TESTING  ");
    delay(3000);
}

void loop() // Perform tests repeatedly
{
    // Indicate Starting
    lcd.setCursor(0, 0);
    lcd.print("Starting...     ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
    delay(1000);

    lcd.setCursor(0, 0);
    lcd.print("Filling lists...");
    lcd.setCursor(0, 1);
    lcd.print("                ");

    // Populate sets
    fill_set(memorize_strings, WORKING_SET);
    fill_set(compare_strings, COMPARE_SET);

    lcd.setCursor(0, 0);
    lcd.print("Filling matches ");
    lcd.setCursor(0, 1);
    lcd.print("...             ");

    // Populate matches with certain chances
    int num_matching = random((int)(LOWER_MATCH_CHANCE * COMPARE_SET), (int)(UPPER_MATCH_CHANCE * COMPARE_SET));
    fill_random(compare_strings, COMPARE_SET, num_matching);

    // Show all stimuli
    for(int i = 0; i != WORKING_SET; i++)
    {
        // Print stimuli
        lcd.setCursor(0, 0);
        lcd.print("Stimulus ");
        lcd.print(i + 1);
        lcd.print(":      ");
        lcd.setCursor(0, 1);
        lcd.print(memorize_strings[i]);

        // Wait for confirmation
        while(digitalRead(YES_BUTTON) == LOW && digitalRead(NO_BUTTON) == LOW) {;}
        delay(BUTTON_DELAY);
    }

    lcd.setCursor(0, 0);
    lcd.print("Finished stimuli");
    lcd.setCursor(0, 1);
    lcd.print("Press when ready");   

    // Wait for confirmation
    while(digitalRead(YES_BUTTON) == LOW && digitalRead(NO_BUTTON) == LOW) {;}
    delay(BUTTON_DELAY);

    // Loop through all comparisons
    for(int i = 0; i != COMPARE_SET; i++)
    {
        delay(random(LOWER_DELAY_TIME, UPPER_DELAY_TIME));

        lcd.setCursor(0, 0);
        lcd.print("Recognize?      ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);

        int is_match = 0;

        // Check if matches one of stimuli in working set
        for(int j = 0; j != WORKING_SET; j++)
        {
            is_match = 1;
            for(int k = 0; k != STIMULI_LENGTH; k++)
            {
                if(memorize_strings[j][k] != compare_strings[i][k])
                    is_match = 0;
            }
            if(is_match)
                break;
        }

        // STARTED TIME
        long start_time = millis();
        lcd.print(compare_strings[i]);

        if(is_match) // This is in the stimuli set
        {
            // Wait for user to respond
            while(digitalRead(YES_BUTTON) == LOW && digitalRead(NO_BUTTON) == LOW) {;}

            if(digitalRead(YES_BUTTON) == HIGH) // Correct
            {
                // Calculate time
                long end_time = millis();
                long time_delta = end_time - start_time;

                // Output
                Serial.print(compare_strings[i]);
                Serial.print(",");
                Serial.print(time_delta);
                Serial.print(",");
                Serial.println(1);
            }
            else if (digitalRead(NO_BUTTON) == HIGH) // Wrong
            {
                // Calculate time
                long end_time = millis();
                long time_delta = end_time - start_time;

                // Output
                Serial.print(compare_strings[i]);
                Serial.print(",");
                Serial.print(time_delta);
                Serial.print(",");
                Serial.println(0);
            }
        }
        else
        {
            // Wait for user to respond
            while(digitalRead(YES_BUTTON) == LOW && digitalRead(NO_BUTTON) == LOW) {;}
            if(digitalRead(YES_BUTTON) == HIGH) // Wrong
            {
                // Calculate time
                long end_time = millis();
                long time_delta = end_time - start_time;

                // Output
                Serial.print(compare_strings[i]);
                Serial.print(",");
                Serial.print(time_delta);
                Serial.print(",");
                Serial.println(0);
            }
            else if (digitalRead(NO_BUTTON) == HIGH) // Correct
            {
                // Calculate time
                long end_time = millis();
                long time_delta = end_time - start_time;

                // Output
                Serial.print(compare_strings[i]);
                Serial.print(",");
                Serial.print(time_delta);
                Serial.print(",");
                Serial.println(1);
            }
        }

        // Clean screen
        lcd.setCursor(0, 0);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
    }

    // Give time to release button
    delay(BUTTON_DELAY);

    // Clear sets for population again
    clear_set(memorize_strings, WORKING_SET);
    clear_set(compare_strings, COMPARE_SET);

    // Don't start another test until ready
    lcd.setCursor(0, 0);
    lcd.print("Finished test   ");
    lcd.setCursor(0, 1);
    lcd.print("Push to run more");

    // Wait for confirmation
    while(digitalRead(YES_BUTTON) == LOW && digitalRead(NO_BUTTON) == LOW) {;}
    delay(BUTTON_DELAY);
}

void fill_set(char set[][STIMULI_LENGTH + 1], const int SET_LENGTH)
{
    for(int i = 0; i != SET_LENGTH; i++)
    {
        for(int j = 0; j != STIMULI_LENGTH; j++)
        {
            switch(TEST_TYPE)
            {
                case(CHARS): // pick random lowercase letter
                    set[i][j] = 97 + random(0, 26); //(rand() % 26);
                    break;
                case(NUMBERS): // pick random digit
                    set[i][j] = 48 + random(0, 10); //(rand() % 10);
                    break;
                case(CHARS_AND_NUMBERS): // pick random lowercase letter or digit
                    int index = random(0, 36); //(rand() % 36);
                    if(index > 9)
                        set[i][j] = index + 87;
                    else
                        set[i][j] = index + 48;
                    break;
            }
        }
        set[i][STIMULI_LENGTH] = '\0'; // add terminating byte for printing
    }
}

void clear_set(char set[][STIMULI_LENGTH + 1], const int SET_LENGTH)
{
    for(int i = 0; i != SET_LENGTH; i++)
    {
        for(int j = 0; j != STIMULI_LENGTH; j++)
        {
            set[i][j] = '\0';
        }
        set[i][STIMULI_LENGTH] = '\0';
    }    
}

void fill_random(char set[][STIMULI_LENGTH + 1], const int SET_LENGTH, int num_random)
{
    // Keep track of which elements have been replaced already
    int *replaced = (int*)malloc(sizeof(int) * num_random);
    for(int i = 0; i != num_random; i++)
    {
        replaced[i] = -1;
    }

    // Pick which elements to replace
    for(int i = 0; i != num_random; i++)
    {
        int compare_index;
        int working_index;
        int original = 1;
        while(1)
        {
            original = 1;
            compare_index = random(0, COMPARE_SET);
            for(int j = 0; j != num_random; j++)
            {
                if(compare_index == replaced[j])
                    original = 0;
            }
            if(original)
                break;
        }

        working_index = random(0, WORKING_SET);
        for(int j = 0; j != STIMULI_LENGTH; j++)
        {
            compare_strings[compare_index][j] = memorize_strings[working_index][j];
        }
        compare_strings[compare_index][STIMULI_LENGTH] = '\0';
        replaced[i] = compare_index;
    }
    free(replaced);
}

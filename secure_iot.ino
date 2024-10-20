#include <Keypad.h> //Handles keypad input.
#include <Wire.h> //Manages I2C communication, used by the LCD.
#include <LiquidCrystal_I2C.h> //Controls the LCD using I2C protocol.
LiquidCrystal_I2C lcd(0x27, 16, 2); //Initializes the LCD with I2C address 0x27, 16 columns, and 2 rows.

#define passwordsize 5 //Defines the size of the password as 5 (4 digits + 1 null terminator).

unsigned int Masterpass_hash; // Store the hashed password
char Data[passwordsize]; //Array to store the user-entered password.
byte data_count = 0; //Tracks the number of entered characters.
char customKey; //Stores the current key pressed.

bool state = HIGH; //Tracks the current state of the relay (HIGH means off, LOW means on).

const byte ROWS = 4;  //Defines a 4x4 keypad with numbers and some special characters.
const byte COLS = 4;  //four columns
//Maps the rows and columns of the keypad to specific Arduino pins.
char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = { 12, 11, 10, 9 };  //connect to the row pinouts of the keypad.
byte colPins[COLS] = { 8, 7, 6, 5 };  //connect to the column pinouts of the keypad.

//Creates an instance of the Keypad class to manage input.
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Function to hash the password using enhanced XOR hashing
unsigned int hashPassword(char* password) {
  unsigned int hash = 7; // Start with a prime number
  for (int i = 0; i < passwordsize - 1; i++) {
    hash ^= (password[i] << i);  // XOR character shifted by its position
  }
  return hash;
}

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // Store the hashed version of the master password
  char Masterpass[passwordsize] = "0303"; // Set the master password
  Masterpass_hash = hashPassword(Masterpass);
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:"); //Displays "Enter Password:" on the LCD.
  customKey = customKeypad.getKey(); //Waits for a key press and stores the pressed key in customKey.

  //If a key is pressed, it is displayed on the LCD, stored in the Data array, and data_count is incremented.
  if (customKey) {
    lcd.setCursor(data_count, 1);
    lcd.print("");  // Display '' for each entered character for privacy
    Data[data_count] = customKey;
    data_count++;
  }

  // When 4 characters are entered (data_count == passwordsize - 1):
  if (data_count == passwordsize - 1) {
    lcd.clear();
    
    // Hash the entered password and compare with the hashed master password
    unsigned int entered_hash = hashPassword(Data);
    if (entered_hash == Masterpass_hash) {
      state = !state;  // Toggle the state
      digitalWrite(LED_BUILTIN, state);
      lcd.setCursor(0, 0);
      lcd.print("Password Matched");
      if (state == LOW) {
        lcd.setCursor(4, 1);
        lcd.print("Load ON");
      } else {
        lcd.setCursor(4, 1);
        lcd.print("Load OFF");
      }
      delay(3000);
      lcd.clear();
    } else {
      lcd.setCursor(0, 0);
      lcd.print("Wrong Password");
      delay(3000);
      lcd.clear();
    }

    // Reset data_count for the next input and clear the Data array.
    data_count = 0;
    memset(Data, 0, sizeof(Data)); // Clear the Data array to avoid residual input.
  }
}

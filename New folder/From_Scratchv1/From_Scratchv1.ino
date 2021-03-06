

/*
  SD card basic file example
*/
#include <SPI.h>
#include <SD.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <String.h>


#define SD_CS 4
#define RFID_SS 10
#define RFID_RST 9
#define SD_POWER 7

MFRC522 rfid(RFID_SS, RFID_RST);
LiquidCrystal_I2C lcd(0x27, 20, 4);

void CommsInit();
void LCDInit();
void RFIDInit();
void CreateFile();
void ReadUID();
void WriteMaster(); // Writes UID to Master File in SD card
void WriteOrdinary(); // Writes UID to Ordinary File in SD card
boolean IsMaster =  false; // Returns true if Scanned Tag's UID is found in Master File
boolean IsOrdinary = false; //Set to true if file is not from Master file
boolean CheckMatch();
char test;



File myFile;
byte readCard[4]; // Array to store UID of a Single Tag temporarily

unsigned long DataBucket; // Temporarily Stores Scanned UID into a single variable

void setup() {

  CommsInit(); // Initializes Serial and SPI communication Protocals
  LCDInit();
  CreateFile(); // Creates files in SD Card

}

void loop() {
  // nothing happens after setup finishes.
  CheckMatch();
  ReadUID();
}
void CreateFile() {
  pinMode(SD_POWER, OUTPUT);
  digitalWrite(SD_POWER, LOW);
  Serial.println("Initializing SD card...");
  lcd.print("SD card init...");
  if (!SD.begin(SD_CS)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  if (!SD.exists("Master.txt")) {
    Serial.println("Creating Master.txt...");
    myFile = SD.open("Master.txt", FILE_WRITE);
    myFile.close();
    delay(200);
    Serial.println("Master.txt File Created.");
  }
  if (!SD.exists("Ordinary.txt")) {
    Serial.println("Creating Ordinary.txt...");
    myFile = SD.open("Ordinary.txt", FILE_WRITE);
    myFile.close();
    delay(200);
    Serial.println("Ordinary.txt File Created.");
  }
}

void RFIDInit() {//Creates an Instance of RFID Object called rfid and initializes it
  //Starting RFID and stopping SD
  digitalWrite(SD_CS, HIGH);
  digitalWrite(SD_POWER, HIGH);
  pinMode(RFID_SS, OUTPUT);
  digitalWrite(RFID_SS, LOW);
  rfid.PCD_Init();
  rfid.PCD_SetAntennaGain(rfid.RxGain_max);
}
void CommsInit() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  SPI.begin();
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}
void ReadUID() {
  //Stopping SD Card in readiness for RFID initialization
  RFIDInit();
  // Getting ready for Reading Tags/Cards
  if ( ! rfid.PICC_IsNewCardPresent()) { //If a new Access Card is placed to RFID reader continue
    return 0;
  }
  if ( ! rfid.PICC_ReadCardSerial()) {   //Since a Tag is placed, get Serial and continue
    return 0;
  }
  // Assuming Tags have 4 byte UID, others may have 7 (Reminder)
  Serial.println(F("Scanned Access Tag Unique ID:"));

  for (int i = 0; i < 4; i++) {
    readCard[i] = rfid.uid.uidByte[i];
    Serial.print(readCard[i], HEX);
  }
 Serial.println("");
//
//  DataBucket = rfid.uid.uidByte[0] << 24;
//  DataBucket += rfid.uid.uidByte[1] << 16;
//  DataBucket += rfid.uid.uidByte[2] << 8;
//  DataBucket += rfid.uid.uidByte[3];

  //long _DataBucket = (long)DataBucket;
 
   
  //Invoke CardType Func to check the Access CardType
  // cardType();
  //  if (IsMaster) {
  //    WriteMaster();
  //  } else if(IsOrdinary) {
  //    //TO DO Disambiguate
  //    WriteOrdinary();
  //  }else{
  //    //Reject
  //  }
  rfid.PICC_HaltA(); // Stop reading
  return 1;
}
void WriteMaster() {
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, LOW);
  pinMode(SD_POWER, OUTPUT);
  digitalWrite(SD_POWER, LOW);
  Serial.println("Initializing SD card...");

  if (!SD.begin(SD_CS)) {
    SD.begin(SD_CS);
    Serial.println("initialization done.");
  }
  myFile = SD.open("Master.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to Master.txt...");

    //     myFile.print(readCard[0]);
    //     myFile.print(readCard[1]);
    //     myFile.print(readCard[2]);
    //     myFile.print(readCard[3]);

    DataBucket = rfid.uid.uidByte[0] << 24;
    DataBucket += rfid.uid.uidByte[1] << 16;
    DataBucket += rfid.uid.uidByte[2] << 8;
    DataBucket += rfid.uid.uidByte[3];

    myFile.print(DataBucket);
    myFile.print(",");
    // close the file:
    myFile.close();
    //GrepMaster();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening Master.txt");
  }
}

void WriteOrdinary() {
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, LOW);
  pinMode(SD_POWER, OUTPUT);
  digitalWrite(SD_POWER, LOW);
  Serial.println("Initializing SD card...");

  if (!SD.begin(SD_CS)) {
    SD.begin(SD_CS);
    Serial.println("initialization done.");
  }
  myFile = SD.open("Ordinary.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to Ordinary.txt...");

    /*
      We can use thios code to store the variable we want to write to SD Card

      This will generate a String which will be dumped in to the Text file and later on used
      to perform a search query
    */


    myFile.print(readCard[0]);
    myFile.print(readCard[1]);
    myFile.print(readCard[2]);
    myFile.print(readCard[3]);

    myFile.print(",");

    test = myFile.print(readCard[0]);
    test += myFile.print(readCard[1]);
    test += myFile.print(readCard[2]);
    test += myFile.print(readCard[3]);
    myFile.print(",");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening Ordinary.txt");
  }
}
void LCDInit () {
  lcd.init();                      // initialize the lcd
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();

  lcd.setCursor(1, 0);
  lcd.print("Marafique Lift");
  lcd.setCursor(-1, 1);
  lcd.print("System  Booting!");
  lcd.setCursor(-3, 2);
  lcd.print("Please Wait...");
  lcd.setCursor(-2, 3);
  lcd.print("KASP3R TECH!");
}


void GrepMaster() {

  uint16_t RFArray[] = {DataBucket};
  //uint16_t RFArray[]={12454};

  uint16_t SDArray[] = {myFile.read()};
  //uint16_t SDArray[]={1,3,54,6,54,2,12454};

  uint16_t SDval;
  uint16_t RFval;
 
  RFval = RFArray[0];
  for (uint8_t i = 0; i < sizeof(SDArray); i++) {
    SDval = SDArray[i];
    if (RFval != SDval) {
      Serial.println("Another Function");
      return 0;
    }
    else
    {
      ;
    }
  }
}

  void Authorize() {
    //Actuate Relay
  }
  boolean CheckMatch() {
  int bufferposition =0;
  File myFile;
  uint16_t buffer_size = 8;//uint8_t buffer_size = 8;
  char Buffer[buffer_size];
  boolean SDfound =1;
  String buffer;
  pinMode (7, OUTPUT);
  digitalWrite(7, LOW);
myFile = SD.open("Master.txt");

  if (!myFile) {
    Serial.print("The text file cannot be opened");
    while(1);
  }

  while (myFile.available()) {
    buffer = myFile.readStringUntil(',');
    if (buffer == test) { //if (buffer == *&readfile) {
    Serial.println("Match");
    }
    else {
     ;
    }
  }

  myFile.close();
}



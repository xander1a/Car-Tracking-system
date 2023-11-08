////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
//GSM Module TX is connected to Arduino D8
#define SIM800_TX_PIN 11  //7
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//float lati[6] = {12, 11, 10, 9, 8, 7};// latitude
//float longi[6] = {12, 11, 10, 9, 8, 7}; // longitude
            String landom_latitude ;
          String landom_longtite;
//GSM Module RX is connected to Arduino D9
#define SIM800_RX_PIN 12//5
LiquidCrystal_I2C lcd(0x27,20,4); 
SoftwareSerial mySerial(SIM800_TX_PIN, SIM800_RX_PIN); //Create software serial object to communicate with GSM Module
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int device_1 = A1; // attach pin D13 of Arduino to Device-1
int device_2 = A2; // attach pin D13 of Arduino to Device-2
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// defines variables
int _timeout;
int index = 0;
String number = "";
String message = "";
//String mynbr = "+250725139004";
String mynbr = "+250782555091";
char incomingByte;
String incomingData;
int  buzzer1=A0;
int button1=A3;
int button_state=0;
bool atCommand = true;
String _buffer;

////////////////////////////////////////////////////////////////////////////////
#define EXE_INTERVAL_1 100
#define EXE_INTERVAL_2 150
#define EXE_INTERVAL_3 200
#define EXE_INTERVAL_4 250
#define EXE_INTERVAL_5 300
//#define EXE_INTERVAL_2 4000

unsigned long lastExecutedMillis_1 = 0; // vairable to save the last executed time for code block 1
unsigned long lastExecutedMillis_2 = 0;
unsigned long lastExecutedMillis_3 = 0;
unsigned long lastExecutedMillis_4 = 0;
unsigned long lastExecutedMillis_5 = 0;

///////////////////////////////////////////////////////////////////////////
   

      ///////////////////////////////////////////////////////////////////////////
const byte numRows= 4;          //number of rows on the keypad
const byte numCols= 4;          //number of columns on the keypad
 
char keymap[numRows][numCols]= 
{
{'1', '2', '3', 'A'}, 
{'4', '5', '6', 'B'}, 
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};
 
char keypressed;                 //Where the keys are stored it changes very often
char code[]= {'1','2','3','4'};  //The default code, you can change it or make it a 'n' digits one
 
char check1[sizeof(code)];  //Where the new key is stored
char check2[sizeof(code)];  //Where the new key is stored again so it's compared to the previous one
 
short a=0,i=0,s=0,j=0;          //Variables used later
 
byte rowPins[numRows] = {9, 8, 7, 6}; //Rows 0 to 3 //if you modify your pins you should modify this too
byte colPins[numCols]= {5, 4, 3,2}; //Columns 0 to 3
 
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);
///////////////////////////////////////////////////////////////////////////////////
int l;

void setup()
{
  lcd.init();                    // initialize the lcd
  lcd.backlight();
  lcd.setCursor(4, 0);
  lcd.print("Wel Come");
  lcd.setCursor(1, 1);
  lcd.print("Enter Password");
  Serial.print("welcome to system");
  /////////////////////////////////////////////////////////////////////////////
  
  Serial.begin(9600); // Serial Communication for Serial Monitor is starting with 9600 of baudrate speed
  mySerial.begin(9600); // Serial Communication for GSM Module is starting with 9600 of baudrate speed
  pinMode(device_1, OUTPUT); //Sets the device_1 as an OUTPUT
  pinMode(buzzer1, OUTPUT);
  pinMode(button1, INPUT);
  pinMode(device_2, OUTPUT); //Sets the device_2 as an OUTPUT
  //digitalWrite(device_1, LOW); //Sets the device_1 in to OFF state at the beginning
  //digitalWrite(device_2, LOW); //Sets the device_2 in to OFF state at the beginning

  // Check if you're currently connected to SIM800L
  while (!mySerial.available()) {
    mySerial.println("AT");
    delay(1000);
    Serial.println("connecting....");
  }

  Serial.println("Connected..");

  mySerial.println("AT+CMGF=1");  //Set SMS Text Mode
  delay(1000);
  mySerial.println("AT+CNMI=1,2,0,0,0");  //procedure, how to receive messages from the network
  delay(1000);
  mySerial.println("AT+CMGL=\"REC UNREAD\""); // Read unread messages

  Serial.println("Ready to received Commands..");
  digitalWrite(buzzer1,HIGH);
  delay(500);
   digitalWrite(buzzer1,LOW);
   //delay(7000);
   //callNumber();
}

void loop()
{
  if (mySerial.available()) {
    delay(100);

    // Serial buffer
    while (mySerial.available()) {
      incomingByte = mySerial.read();
      incomingData += incomingByte;
    }

    delay(10);
    if (atCommand == false) {
      receivedMessage(incomingData);
    } else {
      atCommand = false;
    }

    //delete messages to save memory
    if (incomingData.indexOf("OK") == -1) {
      mySerial.println("AT+CMGDA=\"DEL ALL\"");
      delay(1000);
      atCommand = true;
    }

    incomingData = "";
  }
  button_state=analogRead(A3);
  //Serial.println("button:");
  //Serial.println(button_state);
  if(button_state==1023){
    cordon();
    if(l==0){
      
       digitalWrite(buzzer1,HIGH);
      Reply("Latitude",landom_latitude,"Longitude",landom_longtite);
       l=1; 
       //Serial.println("HIGH:");
    }
    else{
      l=0;
      }
    }
    else{
       //Serial.println("LOW:");
            digitalWrite(buzzer1,LOW);
      
      }
 //button();
 kora();
 //keypadd();

 
}
void keypadd(){
 // cordon();
   keypressed = myKeypad.getKey();               //Constantly waiting for a key to be pressed
    if(keypressed == '*'){                      // * to open the lock
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Enter code");            //Message to show
           ReadCode();                          //Getting code function
                  if(a==sizeof(code))
            digitalWrite(device_1,HIGH);
                  else{
                          Wrong();
                  lcd.clear();               
                  lcd.print("Wrong code"); 
                  digitalWrite(device_2,LOW); //Message to print when the code is wrong
                  digitalWrite(buzzer1,HIGH);
                  delay(1000); 
                    digitalWrite(device_1,LOW);
          lcd.setCursor(0,1);
                  lcd.print("Press * to Unlock");
                  }
            delay(2000);
            lcd.clear();
            lcd.print("Standby");             //Return to standby mode it's the message do display when waiting
        }
 
     if(keypressed == '#'){                  //To change the code it calls the changecode function
      ChangeCode();
      lcd.clear();
      lcd.print("Standby");                 //When done it returns to standby mode
    lcd.setCursor(0,1);
      lcd.print("Press * to Unlock");
     }
 
    
   if(keypressed == 'B'){                  //To change the code it calls the changecode function
      
      lcd.clear();
   // digitalWrite(Lock, LOW);
      lcd.print("Locked: Press * to ");
      lcd.setCursor(0,1);
                  lcd.print("to unlock!!"); 
     }    
  }
  /////////////////////////////////////////////////////////////////////////////////
  void ReadCode(){                  //Getting code sequence
       i=0;                      //All variables set to 0
       a=0;
       j=0; 
                                     
     while(keypressed != 'A'){                                     //The user press A to confirm the code otherwise he can keep typing
           keypressed = myKeypad.getKey();                         
             if(keypressed != NO_KEY && keypressed != 'A' ){       //If the char typed isn't A and neither "nothing"
              lcd.setCursor(j,1);                                  //This to write "*" on the lcd whenever a key is pressed it's position is controlled by j
              lcd.print("*");
              j++;
            if(keypressed == code[i]&& i<sizeof(code)){            //if the char typed is correct a and i increments to verify the next caracter
                 a++;                                              
                 i++;
                 }
            else
                a--;                                               //if the character typed is wrong a decrements and cannot equal the size of code []
            }
            }
    keypressed = NO_KEY;
 
}
 
void ChangeCode(){                      //Change code sequence
      lcd.clear();
      lcd.print("Changing code");
      delay(1000);
      lcd.clear();
      lcd.print("Enter old code");
      ReadCode();                      //verify the old code first so you can change it
      
            if(a==sizeof(code)){      //again verifying the a value
            lcd.clear();
            lcd.print("Changing code");
            GetNewCode1();            //Get the new code
            GetNewCode2();            //Get the new code again to confirm it
            s=0;
              for(i=0 ; i<sizeof(code) ; i++){     //Compare codes in array 1 and array 2 from two previous functions
              if(check1[i]==check2[i])
              s++;                                //again this how we verifiy, increment s whenever codes are matching
              }
                  if(s==sizeof(code)){            //Correct is always the size of the array
                  
                   for(i=0 ; i<sizeof(code) ; i++){
                  code[i]=check2[i];         //the code array now receives the new code
                  EEPROM.put(i, code[i]);        //And stores it in the EEPROM
                  
                  }
                  lcd.clear();
                  lcd.print("Code Changed");
                  delay(2000);
                  }
                  else{                         //In case the new codes aren't matching
                  lcd.clear();
                  lcd.print("Codes are not");
                  lcd.setCursor(0,1);
                  lcd.print("matching !!");
                  delay(2000);
                  }
          }
          else{                     //In case the old code is wrong you can't change it
          lcd.clear();
          lcd.print("Wrong");
          delay(2000);
          }
}
 
void GetNewCode1(){                      
  i=0;
  j=0;
  lcd.clear();
  lcd.print("Enter new code");   //tell the user to enter the new code and press A
  lcd.setCursor(0,1);
  lcd.print("and press A");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("and press A");     //Press A keep showing while the top row print ***
             
         while(keypressed != 'A'){            //A to confirm and quits the loop
             keypressed = myKeypad.getKey();
               if(keypressed != NO_KEY && keypressed != 'A' ){
                lcd.setCursor(j,0);
                lcd.print("*");               //On the new code you can show * as I did or change it to keypressed to show the keys
                check1[i]=keypressed;     //Store caracters in the array
                i++;
                j++;                    
                }
                }
keypressed = NO_KEY;
}
 
void GetNewCode2(){                         //This is exactly like the GetNewCode1 function but this time the code is stored in another array
  i=0;
  j=0;
  
  lcd.clear();
  lcd.print("Confirm code");
  lcd.setCursor(0,1);
  lcd.print("and press A");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("and press A");
 
         while(keypressed != 'A'){
             keypressed = myKeypad.getKey();
               if(keypressed != NO_KEY && keypressed != 'A' ){
                lcd.setCursor(j,0);
                lcd.print("*");
                check2[i]=keypressed;
                i++;
                j++;                    
                }
                }
keypressed = NO_KEY;
}
 
void OpenDoor(){             //Lock opening function open for 3s
  lcd.clear();
  lcd.print("Unlocked:Welcome");
  digitalWrite(device_1,HIGH);
  }
////////////////////////////////////////////////////////////////////////////////
void receivedMessage(String inputString) {

  //Get The number of the sender
  index = inputString.indexOf('"') + 1;
  inputString = inputString.substring(index);
  index = inputString.indexOf('"');
  number = inputString.substring(0, index);
  Serial.println("Number: " + number);
  //Get The Message of the sender
  index = inputString.indexOf("\n") + 1;
  message = inputString.substring(index);
  message.trim();
  Serial.println("Message: " + message);
  message.toUpperCase(); // uppercase the message received

  delay(50);// Added delay between two readings
  kora();
}
////////////////////////////////////////////////////////////////////////////////////
 void kora(){
  if(mynbr== number){
  //turn Device 1 ON
  if (message.indexOf("START") > -1) {
     // button();
     keypadd();
    // digitalWrite(device_1, HIGH);
       digitalWrite(device_2, HIGH);
         //digitalWrite(device_1, LOW);
      
  }

  //turn OFF system
  else if (message.indexOf("STOP") > -1) {
    digitalWrite(device_2, LOW);
    digitalWrite(device_1, LOW);
    Serial.println("Command: Device is Off.");
   
  }

  else if (message.indexOf("DATA") > -1) {
    cordon();
    Reply("Latitude",landom_latitude,"Longitude",landom_longtite);
    Serial.println("Sending data");
  }
}
else {
 // Serial.println("Incoming nbr does not mush");
  }
 }
  String _readSerial() {
  _timeout = 0;
  while  (!mySerial.available() && _timeout < 12000  )
  {
    delay(13);
    _timeout++;
  }
  if (mySerial.available()) {
    return mySerial.readString();
  }
}
  
 /////////////////////////////////////////////////////////////////////////////////////////////////////// 
  void callNumber() {
  mySerial.print (F("ATD"));
  mySerial.print (number);
  mySerial.print (F(";\r\n"));
  _buffer = _readSerial();
  Serial.println(_buffer);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void Reply(String text, String latitude,String text1, String longitude)
{
  
    mySerial.print("AT+CMGF=1\r");
    delay(1000);
    mySerial.print("AT+CMGS=\""+number+"\"\r");
    delay(1000);
    mySerial.print(text);
    mySerial.print(latitude);
     mySerial.print("\n");
    mySerial.print(text1);
   mySerial.print(longitude);
    delay(100);
    mySerial.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
    delay(1000);
    Serial.println("SMS Sent Successfully.");
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////


void cordon(){
   unsigned long currentMillis = millis();

  if (currentMillis - lastExecutedMillis_1 >= EXE_INTERVAL_1  ) {
    lastExecutedMillis_1 = currentMillis; // save the last executed time
           landom_latitude= "-1.97871206004455" ;
           landom_longtite= "30.1055967831963";
                      Serial.println(landom_latitude);
          Serial.println(landom_longtite);

        
    }

  else    if (currentMillis - lastExecutedMillis_2 >= EXE_INTERVAL_2  ) {
    lastExecutedMillis_2 = currentMillis; // save the last executed time
       landom_latitude= "-1.97871206004454" ;
       landom_longtite= "30.1055967831965";
           Serial.println(landom_latitude);
          Serial.println(landom_longtite);
        
    }

  else    if (currentMillis - lastExecutedMillis_3 >= EXE_INTERVAL_3  ) {
    lastExecutedMillis_3 = currentMillis; // save the last executed time
           landom_latitude= "-1.97871206004442" ;
           landom_longtite= "30.1055967831964";
           Serial.println(landom_latitude);
          Serial.println(landom_longtite);

        
    }

   else   if (currentMillis - lastExecutedMillis_4 >= EXE_INTERVAL_4  ) {
    lastExecutedMillis_4 = currentMillis; // save the last executed time
       
      landom_latitude= "-1.97871206004434" ;
       landom_longtite= "30.1055967831973";
           Serial.println(landom_latitude);
          Serial.println(landom_longtite);

        
    }
   else   if (currentMillis - lastExecutedMillis_5 >= EXE_INTERVAL_5  ) {
    lastExecutedMillis_5 = currentMillis; // save the last executed time
       landom_latitude= "-1.97871206004454" ;
       landom_longtite= "30.1055967831965";
          Serial.println(landom_latitude);
          Serial.println(landom_longtite);
        
    }

  }
  void Wrong(){
    
     //The ReadCode function assign a value to a (it's correct when it has the size of the code array)
                   cordon();
                  Reply("Wrog password  longtude:",landom_latitude,"Longitude",landom_longtite);
                  Serial.print("Wrong Password");
                  //Open lock function if code is correct
    
    }
  ///////////////////////////////////////////////////////////////////////////////////////////////////

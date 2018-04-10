/*
Project: Worst Alarm Clock Ever
Author: Carl Shields (of modifications to original project)
Based on "Alarm Clock" by Tiziano Bianchettin
Original project found here:
https://create.arduino.cc/projecthub/Tittiamo/alarm-clock-f61bad?ref=search&ref_id=alarm
*/
//************libraries**************//
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal.h>

//************LCD and RTC***********//

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

RTC_DS1307 RTC;

//************Button****************//
int P1 = 6; // Button SET MENU'
int P2 = 8; // Button +
int P3 = 7; // Button -
int P4 = 9; // SWITCH Alarm

//**************Alarm***************//
#define LED 13
#define buzzer 10

//************Variables*************//
int hourupg;
int minupg;
int yearupg;
int monthupg;
int dayupg;
int menu = 0;
int setAlarmMode = 0;

uint8_t alarmHours = 0, alarmMinutes = 0;  // Holds the current alarm time

bool alarmDismissedWhileSounding = false;

bool isJokeMode = false;
bool oneCheckForJoke = false;

int mode5Counter = 0;

void setup()
{
	lcd.clear();
	lcd.begin(16, 2);

	pinMode(P1,INPUT_PULLUP); // https://www.arduino.cc/en/Tutorial/InputPullupSerial
	pinMode(P2,INPUT_PULLUP);
	pinMode(P3,INPUT_PULLUP);
	pinMode(P4,INPUT_PULLUP);
	pinMode(LED,OUTPUT);
	pinMode(buzzer, OUTPUT); // Set buzzer as an output
	Serial.begin(9600);
	Wire.begin();
	RTC.begin();

	if (! RTC.isrunning()) {
		Serial.println("RTC is NOT running!");
		// Set the date and time at compile time
		RTC.adjust(DateTime(__DATE__, __TIME__));
	}
	//RTC.adjust(DateTime(__DATE__, __TIME__)); //removing "//" to adjust the time
	// The default display shows the date and time
	int menu = 0;
}
 
void loop()
{
// Uncomment to print the current time from RTC to serial. 
//  serialPrintNowTime();

// check if you press the SET button and increase the menu index
  if(digitalRead(P1)== LOW) 
  {
   menu = menu + 1;
  }
  
  if((digitalRead(P2)== LOW)&&(digitalRead(P3)== LOW))
  {
    DisplaySetHourAlarm();
    DisplaySetMinuteAlarm();
    printSetAlarmConfirmation();
  }
  
// in which subroutine should we go?
  if (menu == 0)
    {
     DisplayDateTime(); // void DisplayDateTime
     Alarm(); // Alarm control
	}
  if (menu == 1)
    {
    DisplaySetHour();
    }
  if (menu == 2)
    {
    DisplaySetMinute();
    }
  if (menu == 3)
    {
    DisplaySetYear();
    }
  if (menu == 4)
    {
    DisplaySetMonth();
    }
  if (menu == 5)
    {
    DisplaySetDay();
    }
  if (menu == 6)
    {
    StoreAgg(); 
    delay(500);
    menu=0;
    }
    delay(100);
}

void printSetAlarmConfirmation()
{
	lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("ALARM SET TO");
    lcd.setCursor(5,1);
    lcd.print(alarmHours, DEC);
    lcd.print(":");
    
    if (alarmMinutes < 10) {
    	lcd.print(0);
    	lcd.print(alarmMinutes, DEC);
    } else {
		lcd.print(alarmMinutes, DEC);
    }
    
    delay(1000);
    lcd.clear();
    
    // Automatically turn alarm on after setting it. 
    setAlarmMode = 1;
}

void DisplayDateTime ()
{
// We show the current date and time
  DateTime now = RTC.now();

  lcd.setCursor(0, 0);
  
  if (now.hour() <= 9)
  {
    lcd.print("0");
  }
  lcd.print(now.hour(), DEC);
  hourupg=now.hour();
  lcd.print(":");
  if (now.minute() <= 9)
  {
    lcd.print("0");
  }
  lcd.print(now.minute(), DEC);
  minupg=now.minute();

  lcd.setCursor(6, 0);
  
  if (now.day() <= 9)
  {
    lcd.print("0");
  }
  lcd.print(now.day(), DEC);
  dayupg=now.day();
  lcd.print("/");
  if (now.month() <= 9)
  {
    lcd.print("0");
  }
  lcd.print(now.month(), DEC);
  monthupg=now.month();
  lcd.print("/");
  lcd.print(now.year(), DEC);
  yearupg=now.year();
}

void DisplaySetHour()
{
// time setting
  lcd.clear();
  DateTime now = RTC.now();
  if(digitalRead(P2)==LOW)
  {
    if(hourupg == 23)
    {
      hourupg = 0;
    }
    else
    {
      hourupg = hourupg + 1;
    }
  }
   if(digitalRead(P3) == LOW)
  {
    if(hourupg == 0)
    {
      hourupg = 23;
    }
    else
    {
      hourupg = hourupg - 1;
    }
  }
  lcd.setCursor(0,0);
  lcd.print("Set Hour:");
  lcd.setCursor(0,1);
  lcd.print(hourupg,DEC);
  delay(200);
}

void DisplaySetMinute()
{
// Setting the minutes
  lcd.clear();
  if(digitalRead(P2) == LOW)
  {
    if (minupg == 59)
    {
      minupg = 0;
    }
    else
    {
      minupg = minupg + 1;
    }
  }
   if(digitalRead(P3) == LOW)
  {
    if (minupg == 0)
    {
      minupg = 59;
    }
    else
    {
      minupg = minupg - 1;
    }
  }
  lcd.setCursor(0,0);
  lcd.print("Set Minutes:");
  lcd.setCursor(0,1);
  lcd.print(minupg,DEC);
  delay(200);
}
  
void DisplaySetYear()
{
// setting the year
  lcd.clear();
  if(digitalRead(P2) == LOW)
  {    
    yearupg = yearupg + 1;
  }
   if(digitalRead(P3) == LOW)
  {
    yearupg = yearupg - 1;
  }
  lcd.setCursor(0,0);
  lcd.print("Set Year:");
  lcd.setCursor(0,1);
  lcd.print(yearupg,DEC);
  delay(200);
}

void DisplaySetMonth()
{
// Setting the month
  lcd.clear();
  if(digitalRead(P2) == LOW)
  {
    if (monthupg == 12)
    {
      monthupg = 1;
    }
    else
    {
      monthupg = monthupg + 1;
    }
  }
   if(digitalRead(P3) == LOW)
  {
    if (monthupg == 1)
    {
      monthupg = 12;
    }
    else
    {
      monthupg = monthupg - 1;
    }
  }
  lcd.setCursor(0,0);
  lcd.print("Set Month:");
  lcd.setCursor(0,1);
  lcd.print(monthupg,DEC);
  delay(200);
}

void DisplaySetDay()
{
// Setting the day
  lcd.clear();
  if(digitalRead(P2) == LOW)
  {
    if (dayupg == 31)
    {
      dayupg = 1;
    }
    else
    {
      dayupg = dayupg + 1;
    }
  }
   if(digitalRead(P3) == LOW)
  {
    if (dayupg == 1)
    {
      dayupg = 31;
    }
    else
    {
      dayupg = dayupg - 1;
    }
  }
  lcd.setCursor(0,0);
  lcd.print("Set Day:");
  lcd.setCursor(0,1);
  lcd.print(dayupg,DEC);
  delay(200);
}

void StoreAgg()
{
// Variable saving
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("SAVING IN");
  lcd.setCursor(0,1);
  lcd.print("PROGRESS");
  RTC.adjust(DateTime(yearupg,monthupg,dayupg,hourupg,minupg,0));
  delay(200);
  lcd.clear();
}

void DisplaySetHourAlarm()// Setting the alarm minutes
{
  while(digitalRead(P1) == HIGH){

  lcd.clear();

  if(digitalRead(P2) == LOW)
  {
    if(alarmHours == 23)
    {
      alarmHours = 0;
    }
    else
    {
      alarmHours = alarmHours + 1;
    }
  }
   if(digitalRead(P3) == LOW)
  {
    if(alarmHours == 0)
    {
      alarmHours = 23;
    }
    else
    {
      alarmHours = alarmHours - 1;
    }
  }
  lcd.setCursor(0,0);
  lcd.print("Set HOUR Alarm:");
  lcd.setCursor(0,1);
  lcd.print(alarmHours,DEC);
  delay(200);
 }
 delay(200);
}

void DisplaySetMinuteAlarm()// Setting the alarm minutes
 {
  while(digitalRead(P1) == HIGH){ 

  lcd.clear();
  if(digitalRead(P2) == LOW)
  {
    if (alarmMinutes == 59)
    {
      alarmMinutes = 0;
    }
    else
    {
      alarmMinutes = alarmMinutes + 1;
    }
  }
   if(digitalRead(P3) == LOW)
  {
    if (alarmMinutes == 0)
    {
      alarmMinutes = 59;
    }
    else
    {
      alarmMinutes = alarmMinutes - 1;
    }
  }
  lcd.setCursor(0,0);
  lcd.print("Set MIN. Alarm:");
  lcd.setCursor(0,1);
  lcd.print(alarmMinutes,DEC);
  delay(200);
 }
 delay(200);
}

void printAlarmOn(){
  lcd.setCursor(0,2);
  lcd.print("Alarm: ");

  if (alarmHours <= 9)
  {
    lcd.print("0");
  }
  lcd.print(alarmHours, DEC);
  
  lcd.print(":");
  if (alarmMinutes <= 9)
  {
    lcd.print("0");
  }
  lcd.print(alarmMinutes, DEC); 
}

void printAlarmOff() {
  lcd.setCursor(0, 2);
  lcd.print("Alarm: Off  ");  
}

void Alarm() {
	// Joke mode cannot be exited once active unless the joke mode itself allows it
	if((digitalRead(P4) == LOW) && (isJokeMode == false))
	{
		setAlarmMode = setAlarmMode + 1;
	}
  
	if (setAlarmMode == 0)
	{
		isJokeMode = false; // reset joke mode flag
		oneCheckForJoke = false; // reset the check for joke mode
		printAlarmOff();
		noTone (buzzer);
		digitalWrite(LED,LOW);
	}
     
	if (setAlarmMode == 1)
	{
		if(oneCheckForJoke == false) // check if this is initial run of mode 1
		{
			oneCheckForJoke = true;
			jokeModeSelector(); // decide it it's time to have fun
		}

		printAlarmOn();

		DateTime now = RTC.now();

		if ( now.hour() == alarmHours && now.minute() == alarmMinutes && alarmDismissedWhileSounding == false)
		{
			digitalWrite(LED,HIGH);
			tone(buzzer,880); //play the note "A5" (LA5)
			delay (300);
			tone(buzzer,698); //play the note "F6" (FA5)
			Serial.println("Alarm sounding");
		} else if ( now.hour() == alarmHours && now.minute() == alarmMinutes && alarmDismissedWhileSounding == true)
		{
			noTone (buzzer);
			digitalWrite(LED,LOW);
			Serial.println("Alarm dismissed");	
		} else 
		{
			noTone (buzzer);
			digitalWrite(LED,LOW);
			alarmDismissedWhileSounding = false;
		}
	} 
    
	if (setAlarmMode == 2) // if alarm is dismissed while sounding, keep it on, if not turn it off. 
	{
		DateTime now = RTC.now();

		if (now.hour() == alarmHours && now.minute() == alarmMinutes) // in this case we assume alarm was just dismissed
		{															  // while sounding and flag as such. 
			alarmDismissedWhileSounding = true;
			Serial.println("Dismissed changed to true");
		}
		
		if (alarmDismissedWhileSounding == true)
		{
			setAlarmMode = 1;
		} else
		{
			setAlarmMode = 0;
		}
	}

	// modes 3 and up are joke modes

	if (setAlarmMode == 3) // Saved by lack of Flamethrower
	{
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("No Flamethrower");
		lcd.setCursor(0, 1);
		lcd.print("You have one?");
		delay(500);
		if((digitalRead(P4) == LOW) && (isJokeMode == true)) // this joke allows exit any time
		{
			lcd.clear();
			lcd.setCursor(0, 0);
			lcd.print("No?");
			lcd.setCursor(0, 1);
			lcd.print("FINE U NO FUN :(");
			if((digitalRead(P4) == LOW) && (isJokeMode == true))
			{
				delay(2000);
				lcd.clear();
				setAlarmMode = 0; // mode 0 resets joke mode flags
			}
		}
	}
	
	if (setAlarmMode == 4) // Insults
	{
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("You No Show");
		lcd.setCursor(0, 1);
		lcd.print("Face Tomorrow");
		delay(2000);
		
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("No Alarm For You");
		lcd.setCursor(0, 1);
		lcd.print("Ugly Face");
		delay(2000);
		
		if((digitalRead(P4) == LOW) && (isJokeMode == true)) // this joke allows exit any time
		{
			lcd.clear();
			lcd.setCursor(0, 0);
			lcd.print("Your Face");
			lcd.setCursor(0, 1);
			lcd.print("Do What U Want");
			delay(2000);
			lcd.clear();
			setAlarmMode = 0; // mode 0 resets joke mode flags
		}
	}

	if (setAlarmMode == 5) // Unending noise, betrayal. 
	{
		lcd.clear();
		
		while (mode5Counter < 300000) // 300000/300 = 1000 5 mins / alarm buzzer tone delay = 1000 cycles for 5 mins
		{
			lcd.setCursor(0, 0);
			lcd.print("This No End");
			lcd.setCursor(0, 1);
			lcd.print("Five Minutes");
		
			digitalWrite(LED,HIGH);
			tone(buzzer,880); //play the note "A5" (LA5)
			delay (300);
			tone(buzzer,698); //play the note "F6" (FA5)
			
			mode5Counter++;
		}
		
		digitalWrite(LED, LOW);
		noTone(buzzer);
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("I'm Done");
		lcd.setCursor(0, 1);
		lcd.print("For Now");
		
		if ((digitalRead(P4) == LOW) && (isJokeMode = true))
		{
			mode5Counter = 0;
			lcd.clear();
			
			while (mode5Counter < 300000)
			{
				lcd.setCursor(0, 0);
				lcd.print("I Lied");
				lcd.setCursor(0, 1);
				lcd.print("Round Two! :)");
				digitalWrite(LED,HIGH);
				tone(buzzer,880); //play the note "A5" (LA5)
				delay (300);
//				tone(buzzer,698); //play the note "F6" (FA5)
				mode5Counter++;
			}
			lcd.clear();
			setAlarmMode = 0;
		}
	}

	delay(200);
}

void serialPrintNowTime() {
	DateTime now = RTC.now();
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
}

// Joke modes setAlarmMode 3+, cannot be entered with P4. 

void jokeModeSelector() 
{
	randomSeed(analogRead(A0));
	
	if ((random(100) % 2) == 0) // if random number is even
	{
		isJokeMode = true;
		Serial.println("Joke True");
	} else // if odd
	{
		isJokeMode = false;
		Serial.println("Joke False");
	}
	
	int randomNumber = random(3, 6); // random number from 3 to 5 inclusive to determine which joke to use.
//	randomNumber = 3;
//	isJokeMode = true;
	if (isJokeMode == true)
	{
		switch (randomNumber)
		{
			case 3:
			{
				Serial.println("Joke 1");
				setAlarmMode = 3;
			}
			break;
			case 4:
			{
				Serial.println("Joke 2");
				setAlarmMode = 4;
			}
			break;
			case 5:
			{
				Serial.println("Joke 3");
				setAlarmMode = 5;
			}
			break;
		}
	}
}

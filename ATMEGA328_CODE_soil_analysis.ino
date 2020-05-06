#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#define relay 4
//LiquidCrystal lcd(7,5,10,11,12,13);
LiquidCrystal lcd(13,12,11,10,9,8);
SoftwareSerial swSer(7,6);
int v0,v1,v2,v3,v4;
char ch;
void setup()
{
  Serial.begin(9600);
  swSer.begin(9600);
  pinMode(relay,OUTPUT);
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("T:   ");
  lcd.setCursor(0,1);
  lcd.print("H:   ");
  }

void loop(){
  do{
     if(swSer.available()>0){
         ch = swSer.read();delay(10);
         Serial.write(ch);
     }
   }while(ch!='Z');
    ch=swSer.read();v0=(int)ch;delay(10);
    ch=swSer.read();v1=(int)ch;delay(10);
    ch=swSer.read();v2=(int)ch;delay(10);
    ch=swSer.read();v3=(int)ch;delay(10);
    ch=swSer.read();v4=(int)ch;delay(10);
  lcd.setCursor(0,0);
  lcd.print("T:");
  lcd.print(v1);
  lcd.print("  ");
  lcd.setCursor(10,0);
  lcd.print("WL:");
  lcd.print(v4);
  lcd.print(" ");
  lcd.setCursor(0,1);
  lcd.print("H:");
  lcd.print(v0);  
  lcd.print("  ");
  lcd.print(v3);  
  lcd.print(" ");
  lcd.setCursor(11,1);
  if(v2==1){
    lcd.print("P:ON ");
    digitalWrite(relay,HIGH);    
  }
  else{
    lcd.print("P:OFF");
    digitalWrite(relay,LOW);    
  }
  
  
}

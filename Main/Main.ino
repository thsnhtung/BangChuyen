#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

LiquidCrystal_I2C lcd(0x20, 16, 2);
const byte rows = 4; // numberOfRow
const byte columns = 4; //numberOfColumn

// key pad map
char keys[rows][columns] =
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'},
};


byte rowPins[rows] = {5, 4, 3, 2};    // key pad pinout
byte columnPins[columns] = {6, 7, 8, 9};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, columnPins, rows, columns);


void LCD_INIT()
{
  lcd.init();
  lcd.backlight();
  lcd.setCursor(5, 0);
  lcd.print("Xin chao") ;
}

char Press_keypad()
{
  int state = 0 ;
  char key = '0' ;
  char temp = keypad.getKey();
  while (temp == 0)
    temp = keypad.getKey();
  key = temp;
  return key ;
}

volatile int Product[3]  ;
volatile int Select[3] ;
void Product_Init(int A = 5 , int B = 5 , int C = 5)
{
  Product[0] = A ;
  Product[1] = B ;
  Product[2] = C ;
}

void DisplayProduct()
{
  lcd.clear() ;
  lcd.setCursor(4, 0);
  lcd.print("San pham") ;
  lcd.setCursor(3, 1);
  lcd.print("A:") ;
  lcd.setCursor(5, 1);
  lcd.print(Product[0]) ;
  lcd.setCursor(7, 1);
  lcd.print("B:") ;
  lcd.setCursor(9, 1);
  lcd.print(Product[1]) ;
  lcd.setCursor(11, 1);
  lcd.print("C:") ;
  lcd.setCursor(13, 1);
  lcd.print(Product[2]) ;
}


int GetState()
{
  char key = Press_keypad() ;
  if (key == '#')
    return -1 ;  // POP
  else if ( key == '*')
    return 1 ; // PUSH
  else
    return 0 ; // ERROR
}

int SelectType(char type)
{
  char temp ;
  int result = 0 ;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("So luong ") ;
  lcd.print(type) ;
  lcd.print(" :") ;
  lcd.setCursor(2, 1);
  while (1)
  {
    temp = Press_keypad();
    if (temp == 'D')
    {
      lcd.print(temp) ;
      delay(300) ;
      return result ;
    }
    if (temp >= '0' and temp <= '9')
    {
      lcd.print(temp) ;
      result = (temp - 48) + result * 10;
      delay(200) ;
    }
    else
    {
      lcd.clear();
      lcd.setCursor(1, 1);
      lcd.print("Error. Nhap lai") ;
      delay(600) ;
      return SelectType(type) ;
    }
  }
  delay(100) ;
  return result ;
}


void Selection()
{
  Select[0] = SelectType('A') ;
  Select[1] = SelectType('B') ;
  Select[2] = SelectType('C') ;
}

bool CanPop(int State)
{
  if (State == -1)   // POP
  {
    lcd.clear();
    lcd.print("Nhap san pham") ;
    delay(500) ;
    Selection() ;
    for (int i = 0 ; i < 3 ; i ++)
    {
      if (Product[i] < Select[i])
        return false ;
    }
    return true ;
  }
  else
    return false ;
}

void SendUart(bool Enable)
{
  if (Enable == true)
  {
    for (int i = 0 ; i < 3 ; i ++)
      Serial.println(Select[i]) ;
    Serial.println("Confirm") ; 
  }
  else
  {
    return ;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);//baudrate 9600
  LCD_INIT() ;
  delay(1000) ;
  Product_Init() ;
  DisplayProduct() ;
}


void loop()
{
  DisplayProduct() ;
  int State = GetState() ;
  SendUart(CanPop(State)) ; 
}

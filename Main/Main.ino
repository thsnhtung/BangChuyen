#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#define DEFAULT_VALUE 5         // Gia tri san pham MAX
LiquidCrystal_I2C lcd(0x27, 16, 2);
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


char buffer[160] ;      // Buffer Mem
volatile int buffer_point = 0 ;


byte rowPins[rows] = {52, 50, 48, 46};    // key pad pinout
byte columnPins[columns] = {53, 51, 49, 47};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, columnPins, rows, columns);



volatile int Product[3]  ;                // So luong san pham
volatile int Select[3] ;                  // So luong lay ra
const int Trig = 22 ;                   // Cac chan cua sensor
const int Echo = 23 ;
const int Trig1 = 24 ;
const int Echo1 = 25 ;
const int Trig2 = 26 ;
const int Echo2 = 27 ;
const int Dir = 28 ;
const int Step = 29 ;
const int Dir1 = 30 ;
const int Step1 = 31 ;
const int Dir2 = 32 ;
const int Step2 = 33 ;


void LCD_INIT() ;
char Press_keypad() ;                        //Lay nut nhan
void Product_Init(int A = DEFAULT_VALUE , int B = DEFAULT_VALUE , int C = DEFAULT_VALUE)  ;              // Khoi tao
void DisplayProduct(int ProductThreshold);           // In ra so luong san pham
int GetState()    ;                      // Lay trang thai 1 la them vao -1 la lay ra
int SelectType(char type)  ;             // Lua chon loai san pham lay ra
bool Selection()   ;                       // Lay loai san pham
bool CanPop(int State);          // Kiem tra xem so luong san pham co du de lay hay khong
void ReCalculate(bool Enable) ;                // Tinh lai so luong san pham
void DisplayBill(int sum)  ;                          // tinh bill
void Push(int State)   ;                                   // Check xem co phai lay ra hay khong
float CountDistance(int trig , int echo);        // Do khoang cach tu ultrasonic
void Run(int Speed);        // stepper motor
bool isEmpty();
bool Check(bool Enable);
void LCD_Count();
void MainMotor()
{
  
}


//***************************************************************************//
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);//baudrate 9600
  LCD_INIT() ;
  delay(500) ;
  Product_Init() ;
  DisplayProduct(2) ;
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT) ;
  pinMode(Trig1, OUTPUT);
  pinMode(Echo1, INPUT) ;
  pinMode(Trig2, OUTPUT);
  pinMode(Echo2, INPUT) ;
  pinMode(Dir, OUTPUT);
  pinMode(Step, OUTPUT);
  pinMode(Dir1, OUTPUT);
  pinMode(Step1, OUTPUT);
  pinMode(Dir2, OUTPUT);
  pinMode(Step2, OUTPUT);
}

//****************************LOOP*******************************//


void loop()
{
  while (1)
  {
    DisplayProduct(2) ;
    int State = GetState() ;
    Check(CanPop(State)) ; 
    Push(State) ;
    MainMotor() ; 
  }
}


//*****************************************************************************************************************************************************************************************************************//


float CountDistance(int trig , int echo)        // Do khoang cach tu ultrasonic
{
  digitalWrite(trig, LOW) ;
  delayMicroseconds(2) ;
  digitalWrite(trig, HIGH) ;
  delayMicroseconds(5) ;
  digitalWrite(trig, LOW) ;
  return pulseIn(echo, HIGH) / 2 / 29.412;
}


void Run(int Speed)        // stepper motor
{
  bool Dir = true ;
  if (Speed < 0)
  {
    Speed = -Speed ;
    Dir = false ;
  }
  digitalWrite(Dir, int(Dir) );
  digitalWrite(Dir1, int(Dir) );
  digitalWrite(Dir2, int(Dir) );
  for (int x = 0; x < 200; x++)
  {
    digitalWrite(Step, HIGH);
    digitalWrite(Step1, HIGH);
    digitalWrite(Step2, HIGH);
    delayMicroseconds(1500 - Speed);
    digitalWrite(Step, LOW);
    digitalWrite(Step1, LOW);
    digitalWrite(Step2, LOW);
    delayMicroseconds(1500 - Speed);
  }
}


bool isEmpty()                                      // kiem tra xem da chuyen di het san pham hay chua
{
  for (int i = 0 ; i < 3 ; i ++)
  {
    if (Select[i] > 0)
      return false ;
  }
  return true ;
}

bool Check(bool Enable)                                        // check va count
{
  if (Enable == false)
    return false ; 
  ReCalculate(Enable) ; 
  int dist = 0  , dist1 = 0 , dist2 = 0 ;
  int Distance = 0 , Distance1 = 0 , Distance2 = 0 ;
  int i = 0 ;
  int sum = Select[0] + Select[1] * 2 + Select[2] * 1.5;
  LCD_Count() ; 
  while (i < 50000)
  {
    Run(1000) ;
    Distance  = CountDistance(Trig, Echo);
    Distance1 = CountDistance(Trig1, Echo1);
    Distance2 = CountDistance(Trig2, Echo2);
    if (Distance < 8)
    {
      dist = 1 ;
    }
    else if (Distance >= 8 && dist == 1)
    {
      Select[0]--;
      dist = 0 ;
    }
    if (Distance1 < 8)
    {
      dist1 = 1 ;
    }
    else if (Distance1 >= 8 && dist1 == 1)
    {
      Select[1]--;
      dist1 = 0 ;
    }
    if (Distance2 < 8)
    {
      dist2 = 1 ;
    }
    else if (Distance2 >= 8 && dist2 == 1)
    {
      Select[2]--;
      dist2 = 0 ;
    }
    LCD_Count() ; 
    if (isEmpty() == true)
    {
      DisplayBill(sum) ; 
      return true ;
    }
    i++ ;
  }
}

void LCD_Count()
{

  lcd.clear() ;
  lcd.setCursor(4, 0);
  lcd.print("Dem san pham") ;
  lcd.setCursor(3, 1);
  lcd.print("A:") ;
  lcd.setCursor(5, 1);
  lcd.print(Select[0]) ;
  lcd.setCursor(7, 1);
  lcd.print("B:") ;
  lcd.setCursor(9, 1);
  lcd.print(Select[1]) ;
  lcd.setCursor(11, 1);
  lcd.print("C:") ;
  lcd.setCursor(13, 1);
  lcd.print(Select[2]) ;

}

void LCD_INIT()
{
  lcd.init();
  lcd.backlight();
  lcd.setCursor(5, 0);
  lcd.print("Xin chao") ;
}


char Press_keypad()                         //Lay nut nhan
{
  char key = '0' ;
  char temp = keypad.getKey();
  while (temp == 0)
    temp = keypad.getKey();
  key = temp;
  return key ;
}


void Product_Init(int A = DEFAULT_VALUE , int B = DEFAULT_VALUE , int C = DEFAULT_VALUE)                // Khoi tao
{
  Product[0] = A ;
  Product[1] = B ;
  Product[2] = C ;
}


void DisplayProduct(int ProductThreshold)           // In ra so luong san pham
{
  bool isLessThan = false ;
  for (int i = 0 ; i < 3 ; i ++)
  {
    if (Product[i] < ProductThreshold)
      isLessThan = true ;
  }
  if (isLessThan == false)
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
  else
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Please refill") ;
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
}


int GetState()                          // Lay trang thai 1 la them vao -1 la lay ra
{
  char key = Press_keypad() ;
  if (key == '#')
    return -1 ;  // POP
  else if ( key == '*')
    return 1 ; // PUSH
  else
    return 0 ; // ERROR
}


int SelectType(char type)               // Lua chon loai san pham lay ra
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
    if (temp == '#')
    {
      lcd.print(temp) ;
      delay(100) ;
      return result ;
    }
    if (temp == '*')
    {
      lcd.clear() ;
      lcd.print("Huy") ;
      delay(300) ;
      return -1 ;
    }
    if (temp >= '0' and temp <= '9')
    {
      lcd.print(temp) ;
      result = (temp - 48) + result * 10;
      delay(100) ;
    }
    else
    {
      lcd.clear();
      lcd.setCursor(1, 1);
      lcd.print("Error. Nhap lai") ;
      delay(300) ;
      return SelectType(type) ;
    }
  }
  delay(100) ;
  return result ;
}


bool Selection()                          // Lay loai san pham
{
  Select[0] = SelectType('A') ;
  if (Select[0] == -1)
    return false ;
  Select[1] = SelectType('B') ;
  if (Select[1] == -1)
    return false ;
  Select[2] = SelectType('C') ;
  if (Select[2] == -1)
    return false ;
  return true ;
}


bool CanPop(int State)          // Kiem tra xem so luong san pham co du de lay hay khong
{
  if (State == -1)   // POP
  {
    lcd.clear();
    lcd.print("Lay ra san pham") ;
    delay(500) ;
    if (Selection() == false)
    {
      return false ;
    }
    for (int i = 0 ; i < 3 ; i ++)
    {
      if (Product[i] < Select[i])
      {
        lcd.clear();
        lcd.print("Vuot qua so  ") ;
        lcd.setCursor(0, 1);
        lcd.print("luong hien co ") ;
        delay(1500) ;
        return false ;
      }
    }
    return true ;
  }
  else
    return false ;
}


void ReCalculate(bool Enable)                 // Tinh lai so luong san pham
{
  for (int i = 0 ; i < 3; i++)
  {
    Product[i] = Product[i] - Select[i] ;
  }

}





void DisplayBill(int sum)                            // tinh bill
{
  lcd.clear() ;
  lcd.print("BILL");
  lcd.setCursor(0, 1) ;
  lcd.print("Total: ") ;
  lcd.print(sum) ;
  lcd.print("$") ;
  int key = Press_keypad();
  while (key != '#')
    key = Press_keypad();
  lcd.clear() ;
  lcd.print("Complete");
  delay(300) ;
  return ;
}



void Push(int State)                                      // Check xem co phai lay ra hay khong
{
  if (State == 1)
  {
    char  type ;
    lcd.clear();
    lcd.print("Them san pham: ");
    lcd.setCursor(0, 1) ;
    lcd.print("Nhap loai: ");
    while (1)
    {
      type = Press_keypad() ;
      if (type == 'A')
      {
        lcd.clear();
        lcd.print("Them loai A...") ;
        Product_Init(DEFAULT_VALUE, Product[1], Product[2]) ;
        delay(300) ;
        return ;
      }
      else if (type == 'B')
      {
        lcd.clear();
        lcd.print("Them loai B...") ;
        Product_Init(Product[0], DEFAULT_VALUE, Product[2]) ;
        delay(300) ;
        return ;
      }
      else if (type == 'C')
      {
        lcd.clear();
        lcd.print("Them loai C...") ;
        Product_Init(Product[0],  Product[1], DEFAULT_VALUE) ;
        delay(300) ;
        return ;
      }
      else if (type == '#')
      {
        lcd.clear();
        lcd.print("Huy") ;
        delay(300) ;
        return ;
      }
      else
      {
        lcd.clear();
        lcd.print("Nhap sai ") ;
        lcd.setCursor(0, 1);
        lcd.print("Hay nhap lai") ;
        delay(300) ;
        lcd.clear();
        lcd.print("Nhap loai: ");
        continue;
      }
    }
    ReCalculate(true);
  }
  else
    return ;
}




//*****************************************************************************************************************************************************************************************************************//

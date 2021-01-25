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


byte rowPins[rows] = {5, 4, 3, 2};    // key pad pinout
byte columnPins[columns] = {6, 7, 8, 9};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, columnPins, rows, columns);



volatile int Product[3]  ;                // So luong san pham
volatile int Select[3] ;                  // So luong lay ra


void LCD_INIT() ;
char Press_keypad() ;                        //Lay nut nhan
void Product_Init(int A = DEFAULT_VALUE , int B = DEFAULT_VALUE , int C = DEFAULT_VALUE)  ;              // Khoi tao
void DisplayProduct(int ProductThreshold);           // In ra so luong san pham
int GetState()    ;                      // Lay trang thai 1 la them vao -1 la lay ra
int SelectType(char type)  ;             // Lua chon loai san pham lay ra
bool Selection()   ;                       // Lay loai san pham
bool CanPop(int State);          // Kiem tra xem so luong san pham co du de lay hay khong
void ReCalculate(bool Enable) ;                // Tinh lai so luong san pham
void ReceiveUART();                            // inifite
bool SendUart() ;                             // Gui  uart-- wait for ok mess
void WaitingForRespone() ;                       // phan hoi tu mega-- nhan ma tin ok nen dem du so luong tu sensor
void DisplayBill()  ;                          // tinh bill
void Communicate(bool Enable)  ;                       // giao tiep vs mega
void Push(int State)   ;                                   // Check xem co phai lay ra hay khong


//***************************************************************************//
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);//baudrate 9600
  LCD_INIT() ;
  delay(500) ;
  Product_Init() ;
  DisplayProduct(2) ;
}

//****************************LOOP*******************************//


void loop()
{
  while (1)
  {
    DisplayProduct(2) ;
    int State = GetState() ;
    Communicate(CanPop(State)) ;
    Push(State) ;
  }
}


//*****************************************************************************************************************************************************************************************************************//

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


void ReceiveUART()                            // inifite
{
  memset(buffer, '\0', 160);
  buffer_point = 0 ;
  while (Serial.available() == 0 );
  while (Serial.available() != 0 )
  {
    buffer[buffer_point] = Serial.read() ;
    buffer_point ++ ;
    delay(200) ;
  }
}


bool SendUart()                              // Gui  uart-- wait for ok mess
{
  for (int i = 0 ; i < 3 ; i ++)
  {
    Serial.print(Select[i]) ;
    ReceiveUART() ;
    if (buffer_point == 0)
    {
      // Loi UART
      // sang den
      return false;
    }
    memset(buffer, '\0', 160);
    buffer_point = 0 ;
  }
  return true ;
}


void WaitingForRespone()                        // phan hoi tu mega-- nhan ma tin ok nen dem du so luong tu sensor
{
  ReceiveUART();
  if (strstr(buffer, "OK") != NULL)
  {
    ReCalculate(true);
    DisplayBill() ;
  }
  else
  {
    pinMode(A0, OUTPUT) ;
    digitalWrite(A0, HIGH) ;
    //Sang den bao hieu .
  }
  memset(buffer, '\0', 160);
  buffer_point = 0 ;
}


void DisplayBill()                            // tinh bill
{
  int sum = Select[0] + Select[1] * 2 + Select[2] * 1.5  ;
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


void Communicate(bool Enable)                         // giao tiep vs mega
{
  if (Enable == true)
  {
    lcd.clear() ;
    lcd.print("Processing...");
    SendUart() ;
    WaitingForRespone() ;
  }
  else
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

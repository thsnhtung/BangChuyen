
volatile int Transfer[3] ;
char received_data;

char buffer[160] ;
volatile int buffer_point = 0 ;
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

float CountDistance(int trig , int echo);        // Do khoang cach tu ultrasonic
void ReceiveUART()    ;                        // infinite
void Active();                                 // Cho nhan gia tri cua uno
void Run(int Speed);        // stepper motor
bool isEmpty();                                      // kiem tra xem da chuyen di het san pham hay chua
bool Check();                                     // check va count
void MainMotor();

void setup() {
  Serial.begin(9600) ;
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


void loop()
{
  Active() ;
  if (Check())
  {
    Serial.print("OK") ;
    MainMotor();
  }
}


float CountDistance(int trig , int echo)        // Do khoang cach tu ultrasonic
{
  digitalWrite(trig, LOW) ;
  delayMicroseconds(2) ;
  digitalWrite(trig, HIGH) ;
  delayMicroseconds(5) ;
  digitalWrite(trig, LOW) ;
  return pulseIn(echo, HIGH) / 2 / 29.412;
}


void ReceiveUART()                            // infinite
{
  memset(buffer, '\0', 160);
  buffer_point = 0 ;
  while (Serial.available() == 0)
  {
  }
  while (Serial.available() != 0 )
  {
    buffer[buffer_point] = Serial.read() ;
    buffer_point ++ ;
    delay(200) ;
  }
}


void Active()                                 // Cho nhan gia tri cua uno
{
  int i = 0 ;
  while (i < 3)
  {
    ReceiveUART() ;
    if (buffer_point > 0)
    {
      Serial.print("OK") ;
      sscanf(buffer, "%d" , &Transfer[i] );
      memset(buffer, '\0', 160);
      buffer_point = 0 ;
      i++ ;
    }
  }
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

void MainMotor()
{
  //xxxxxxxxxxxxxxxxx
}


bool isEmpty()                                      // kiem tra xem da chuyen di het san pham hay chua
{
  for (int i = 0 ; i < 3 ; i ++)
  {
    if (Transfer[i] > 0)
      return false ;
  }
  return true ;
}


bool Check()                                        // check va count
{
  int dist = 0  , dist1 = 0 , dist2 = 0 ;
  int Distance = 0 , Distance1 = 0 , Distance2 = 0 ;
  int i = 0 ;
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
      Transfer[0]--;
      dist = 0 ;
    }
    if (Distance1 < 8)
    {
      dist1 = 1 ;
    }
    else if (Distance1 >= 8 && dist1 == 1)
    {
      Transfer[1]--;
      dist1 = 0 ;
    }
    if (Distance2 < 8)
    {
      dist2 = 1 ;
    }
    else if (Distance2 >= 8 && dist2 == 1)
    {
      Transfer[2]--;
      dist2 = 0 ;
    }

    if (isEmpty() == true)
    {
      return true ;
    }
    i++ ;
  }
}

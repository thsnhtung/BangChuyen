
const int Dir = 28 ;
const int Step = 29 ;
void setup() {
  pinMode(Dir, OUTPUT);
  pinMode(Step, OUTPUT);
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
  for (int x = 0; x < 200; x++)
  {
    digitalWrite(Step, HIGH);
    delayMicroseconds(1500 - Speed);
    digitalWrite(Step, LOW);
    delayMicroseconds(1500 - Speed);
  }
}

void loop() {
  Run(1000);
}


#define analogPin 36
#define a 0
#define b 1
#define z 2
#define y 3
#define Button1 22
#define Signal 23
#define Error 21
#define Pulse 20

unsigned  long start;
unsigned long finish;
unsigned  int task;
unsigned int tyme;

unsigned int val;
unsigned int counter=0;
char c=0;
char pock;
unsigned int reading;
unsigned int lastReading;
unsigned int haldCycle;
float frequency;
char button;
char error_code;
unsigned int dato[4];
unsigned int avg;


unsigned int strt;
unsigned int fnsh;
unsigned int toime;

unsigned int beginning;
unsigned int ending;


void setup() {
  // put your setup code here, to run once:
   Serial.begin(9600);
     //   adc1_config_width(width);
      //  adc1_config_channel_atten(channel, atten);
      pinMode(Button1, INPUT);
      pinMode(Pulse, INPUT);
      pinMode(Signal, OUTPUT);
        pinMode(Error, OUTPUT);
      pinMode(Button1, INPUT_PULLDOWN);
   pinMode(Pulse, INPUT_PULLDOWN);
}

void loop() {

start=micros();
 counter++;
 if(counter>600) {counter=0;} // Clear counter after 5 seconds (0.2 Hz)
 Serial.print("Execution time:");
 Serial.print(task,DEC);
 Serial.print('\n');


//Printing what needs to be printed





 
//Reading potentiometer
    if(((counter+a)%5)==0) // 24 Hz cycle
    {
       val = analogRead(analogPin); 
        Serial.print("Analog read:");
        Serial.print(val,DEC);
        Serial.print('\n');
    }

//Computing average value
    if(((counter+b)%5)==0) // 24 Hz cycle
    {
      if(c>=4) {c=0;}
   dato[c]=val;
   c++;
   
   avg=dato[0]+dato[1]+dato[2]+dato[3];
   avg=avg>>2;
   Serial.print("Average value:");
        Serial.print(avg,DEC);
        Serial.print('\n');
        
    }

//Reading pushbutton
    if(((counter+z)%24)==0) // 5 Hz cycle
    {
       button= digitalRead(Button1);
       if(button==1) { digitalWrite(Signal,HIGH);}
       else { digitalWrite(Signal,LOW);}
        Serial.print("Button state:");
        Serial.print(button,DEC);
        Serial.print('\n');
    }
 //Do nothing thousand times
    if(((counter+z)%12)==0) // 10 Hz cycle
    {
       for(char i=0; i++; i<250)
       {
          for(char j=0; i++; i<4)
          {
          __asm__ __volatile__ ("nop");
          }
       }
    }
// perform check
      if(((counter+y)%40)==0) // 3 Hz cycle
      {
      if(avg > 2047){error_code = 1;}
      else{ error_code = 0;}
      }

 // vusualise check result
      if(((counter+y)%40)==0) // 3 Hz cycle
      {
      if(error_code==1){digitalWrite(Error,HIGH);}
      else{digitalWrite(Error,LOW);}
      }


//evaluation of frequency
 if(((counter)%120)==0) // 1 Hz cycle
 {
  pock=0;
  strt=micros();
    while(toime<3000)
    {
      reading=digitalRead(Pulse);
       if (reading!= lastReading && pock!=1) 
       {
       beginning=micros();
       pock=1;
       }
        else if (reading!= lastReading && pock==1) 
        {
        ending=micros();
        haldCycle=ending-beginning;
        frequency=500000/haldCycle;
        goto next;
        }
        fnsh=micros();
        toime=fnsh-strt;
        lastReading=reading;
    }

 }
 
 next:
 
 finish=micros();
 task=finish-start;
// tyme=8333-task;
 //delayMicroseconds(tyme);
delay(1000);
}

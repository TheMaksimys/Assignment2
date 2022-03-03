
#define analogPin 36
#define a 0
#define b 1

unsigned  long start;
unsigned long finish;
unsigned  int task;
unsigned int val;
char counter=0;
char c=0;
char i;
unsigned int dato[4];
unsigned int avg;


void setup() {
  // put your setup code here, to run once:
   Serial.begin(9600);
     //   adc1_config_width(width);
      //  adc1_config_channel_atten(channel, atten);

   
}

void loop() {

//Reading potentiometer
    if(((counter+a)%5)==0) // 24 Hz cycle
    {
       val = analogRead(analogPin); 
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
        Serial.print(avg,DEC);
        Serial.print('\n');
        
    }




start=micros();
  delay(1000);
 finish=micros();
 task=finish-start;
 task=task;
 counter++;
Serial.print(task,DEC);
 Serial.print('\n');

}

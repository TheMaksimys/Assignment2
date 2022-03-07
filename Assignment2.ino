// Assignment 2 for B31DG by Maksims Latkovskis
// This code performs the CPU access administration between the several repeatable subroutines using the cyclic executive with 120 Hz slot frequency

// Defining the pins for the I/O
#define ANALOGPIN 36 // Pin for analog input ADC1_CH0
#define ERRORPIN 21 // Pin for the error indication LED
#define WATCHDOGPIN 18 // Pin for the output watchdog waveform 
#define DIGITALINPUTPIN 22 // Pin for the pushbutton input
#define SIGNALPIN 19 // Pin for the external square wave input
#define ANALOGREADPIN 17 // Pin to display the analog reading time

// Defining global variables
unsigned long functionStart; // Absolute time of the slot beggining
unsigned long functionFinish; // Absolute time of the end of all functions in the given slot
unsigned int slotCounter=0; // Counter of the slots (total 120 slots per second)
unsigned  int functionDuration; // Time it took to do all instructions since the beggining of the slot
int extraDelay; // Delay required to make the slot be 8333 microseconds

char analogAdress; // Analog readings array indexing
unsigned int analogReading; // Very last value to be stored from the ADC
unsigned int analogData[4]; // Array to store 4 last values from the ADC
unsigned int analogAverage; // Average of the last 4 values from the ACC

char buttonInput; // Pushbutton reading
char errorCode; // Error state

unsigned int measuredFreq; // Measured frequency of the square wave

//----------------------------------------------------------------------------------------------------------------------------------------------
void setup(void)
{
  Serial.begin(9600); // Starts the serial communication and sets baud rate to 9600
  pinMode(DIGITALINPUTPIN, INPUT);
  pinMode(DIGITALINPUTPIN, INPUT_PULLDOWN); // Prevents pin from being floating 
  pinMode(SIGNALPIN, INPUT);
  pinMode(SIGNALPIN, INPUT_PULLDOWN); // Prevents pin from being floating 
  pinMode(WATCHDOGPIN, OUTPUT);
  pinMode(ERRORPIN, OUTPUT);
  pinMode(ANALOGREADPIN, OUTPUT);
}
//----------------------------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------------------------------
void loop(void) // Single time slot function of the Cyclic Executive (repeating)
{
  functionBeginning: // Address line
  functionStart=micros(); // Sets the CPU running time as the starting point of the time slot
  slotCounter++; // Increments the counter since the last iteration
  if(slotCounter>599) {slotCounter=0;} // Clears Slot counter after 5 seconds (0.2 Hz)

  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // Cyclic Executive function calls (constants are added to the slotCounter value to offset subroutines to different slots, refer report for more details)
 
      // Printing data on the serial monitor at 0.2 Hz frequency
      if(slotCounter==599) {SerialOutput();}

      // Generating watchdog at 40 Hz frequency
      if(((slotCounter)%3)==0) {Watchdog();}

      // Reading potentiometer value at 24 Hz frequency
      if(((slotCounter)%5)==0) {AnalogRead();}

      // Computing average of 4 last potentiometer readings at 24 Hz frequency
      if(((slotCounter+1)%5)==0) {AnalogAverage();}

      // Reading the digital input at 5 Hz frequency
      if(((slotCounter+8)%24)==0) {DigitalInput();}
   
      // Doing nothing thousand times at 10 Hz frequency
      if(((slotCounter+7)%12)==0) {Nop1000times();}
 
      // Performing error check at 3 Hz frequency
      if(((slotCounter+2)%40)==0) {ErrorCheck();}

      // Toggling the error event signalling at 3 Hz frequency
      if(((slotCounter+3)%40)==0) {ErrorDisplay();}

      // Evaluating the square signal frequency at 1 Hz frequency
      if(((slotCounter+4)%120)==0) {FrequencyMeasure();}
 
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  
  functionFinish=micros(); // Sets the CPU running time as the finishing point of the utilised slot time (rest of the function is quicker than 0.333 miliseconds)
  functionDuration= functionFinish-functionStart; // Computes the time it took to perform all necessary functions
  extraDelay=8333-functionDuration; // Computes the time required to make the slot of 8333 useconds (120 Hz)
  if(extraDelay<0) // Checks whether the additional delay is a positive number
  {
    Serial.print("Slot overflow"); // Prints the error message for the debugging purpose
    goto functionBeginning; // Jumps to the loop beggining to start the next slot
  }
  delayMicroseconds(extraDelay); // Introduced additional delay to make the slot of 8333 seconds
}
//----------------------------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------------------------------
void DigitalInput(void) // Reads the digital state of the button 
{
  buttonInput= digitalRead(DIGITALINPUTPIN); // Reads the digital state of the button (either 0 or 1)
}
//----------------------------------------------------------------------------------------------------------------------------------------------
void ErrorCheck(void) // Checks whether the average analog value is greater than the half of the maximum possible analog reading (?>(4096/2)-1)
{
  if(analogAverage>2047) {errorCode = 1;} // Setting the error flag 
  else{errorCode=0;} // Clearing the error flag
}
//----------------------------------------------------------------------------------------------------------------------------------------------
void ErrorDisplay(void) // Toggles the LED if the error occurs
{
  digitalWrite(ERRORPIN,errorCode); // Switched the LED either ON or OFF
}
//----------------------------------------------------------------------------------------------------------------------------------------------
void AnalogRead(void) // Reads the analog value from the ANALOGPIN pin
{
  digitalWrite(ANALOGREADPIN, HIGH); // Indicate the start of the ADC
  analogReading = analogRead(ANALOGPIN);
  digitalWrite(ANALOGREADPIN, LOW); // Indicate the end of the ADC
}
//----------------------------------------------------------------------------------------------------------------------------------------------
void AnalogAverage(void) // Computes average of last four analog readings
{ 
  // This section logs the most recent analog reading to the longest-ever updated cell (there are 4 cells in total)
  if(analogAdress>=4) {analogAdress=0;} // Making sure only valid array cells are accessed
  analogData[analogAdress]=analogReading; // Loggin the last analog reading into the array cell
  analogAdress++; // Icrementing array adress

  // This section computes the average of last four readings
  analogAverage=analogData[0]+analogData[1]+analogData[2]+analogData[3]; // Sums all values in the array
  analogAverage=analogAverage>>2; // Divides the sum by four to obtain average
}
//----------------------------------------------------------------------------------------------------------------------------------------------
void Watchdog(void) // Generates the 50us pulse
{
  digitalWrite(WATCHDOGPIN,HIGH);
  delayMicroseconds(50);
  digitalWrite(WATCHDOGPIN,LOW);
}
//----------------------------------------------------------------------------------------------------------------------------------------------
void Nop1000times(void) // Sets CPU in no-operation state for 1000 clock cycles (approx. 1ms); two for loops to simplify counter register access
{
  for(char i=0; i++; i<4)
  {
    for(char j=0; i++; i<250)
    {
      __asm__ __volatile__ ("nop"); // "volatile" is needed to prevent optimisation by the compiler
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------------------------------
void FrequencyMeasure(void) // Function to measure the frequency of the 50% duty cycle square wave
{
  // Parameters to prevent function from occupying more than 3 miliseconds of the CPU time
  unsigned long measureStart; // Absolute time of the measuring start
  unsigned long measureFinish; // Absolute time of the measuring finish
  unsigned int measureTime=0; // Time it took since the function start to measure the frequency

  // Parameters to measure the half-cycle duration of the input square signal (in microseconds)
  unsigned long stateStart;  // Absolute time of the half-cycle start
  unsigned long stateFinish; // Absolute time of the half-cycle finish
  unsigned int stateTime; // Half-cycle duration of the input square signal
  char firstTrigger=0; // Trigger to distinguish between the first and the second detected edges
  char currentReading; // Current signal pin state
  char previousReading; // Previous signal pin state

  measureStart=micros(); // Beggining of the measurement
  previousReading=digitalRead(SIGNALPIN); // Performs the very first reading in the sequence (crucial line, do-not remove)
  while(measureTime<3000) // Prevents function from occupying more than 3 miliseconds of the CPU time
  {
    currentReading=digitalRead(SIGNALPIN); // Reads the digital signal pin input (0 or 1)
    if ((currentReading!= previousReading) & (firstTrigger!=1)) // If the first edge was detected
    {
      stateStart=micros(); // Start counting the half-cycle duration
      firstTrigger=1; // assign trigger to 1 to allow the second edge detection processing    
    }  
    else if ((currentReading!= previousReading) & (firstTrigger==1)) // If the second edge was detected
    {
      stateFinish=micros(); // Finish counting the half-cycle duration
      stateTime=stateFinish-stateStart; // Calculate the half-cycle duration
      measuredFreq=500000/stateTime; // Compute frequency in Hz [f= 1/T = 2/(T/2)
      return; // Terminate this subroutine 
    }      
    measureFinish=micros(); // End of the measurement
    measureTime=measureFinish-measureStart; // Computing measurement time
    previousReading=currentReading; // Updating the previous reading state
  }
  measuredFreq=0; // In case of faulty measurement output zero
}
//----------------------------------------------------------------------------------------------------------------------------------------------
void SerialOutput(void) // Printing the data on the serial monitor
{
  Serial.print("Average value: "); // Printing average of 4 last analog readings
  Serial.print(analogAverage,DEC);
  Serial.print('\n');
  
  Serial.print("Button state: "); // Printing the button state
  Serial.print(buttonInput,DEC);
  Serial.print('\n');
  
  Serial.print("Frequency: "); // Printing measured frequency of the 50% square signal
  Serial.print(measuredFreq,DEC);
  Serial.print(" Hz");
  Serial.print('\n');
}
//----------------------------------------------------------------------------------------------------------------------------------------------

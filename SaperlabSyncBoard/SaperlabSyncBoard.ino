// Audio synchronizing for Saper lab
// Stephen Zhang 12/13/2021

#define pin_audio PIN_C6
#define pin_TTL PIN_B5
#define pin_readyLED PIN_B4
#define pin_swtich PIN_D7
#define pin_LED PIN_D6

// Flags
bool sysready = false;
bool pulse = false;
bool readyled = false;

// buzz
unsigned int buzzfreq = 2000;

// Time
unsigned int cycle = 31333;
unsigned int pulse_width = 2000;
unsigned int stepsize = 100;

unsigned long t0, t1;

void setup(){
  // Pins
  pinMode(pin_audio, OUTPUT); // Set buzzer - pin 9 as an output
  pinMode(pin_TTL, OUTPUT);
  pinMode(pin_readyLED, OUTPUT);
  pinMode(pin_LED, OUTPUT);
  pinMode(pin_swtich, INPUT_PULLUP); 

  // flags
  pulse = false;
  readyled = false;
  sysready = true;
  
  // Outputs
  digitalWrite(pin_audio, LOW);
  digitalWrite(pin_TTL, LOW);
  digitalWrite(pin_readyLED, LOW);
  digitalWrite(pin_LED, LOW);

  // t0
  t0 = micros();
}

void loop(){
  // Time
  t1 = micros();
  
  // Get input (HIGH = ready, LOW = engaged)
  sysready = digitalRead(pin_swtich);

  if (!sysready && readyled){
    // engaged but light on
    readyled = false;
    digitalWrite(pin_readyLED, LOW);
  }
  else if (sysready && !readyled){
    // ready but light off
    readyled = true;
    digitalWrite(pin_readyLED, HIGH);
  }
  
  // Dealing with pulses
  if (((t1 - t0) > cycle) && !sysready && !pulse){
    // New cycle, reset
    t0 = t1;

    // buzzer
    tone(pin_audio, buzzfreq);

    // TTL
    digitalWrite(pin_TTL, HIGH);
    digitalWrite(pin_LED, HIGH);

    // flag
    pulse = true;
  }
  if (((t1 - t0) > pulse_width) && !sysready && pulse){
    // within cycle, turn off
    noTone(pin_audio);  

    // TTL
    digitalWrite(pin_TTL, LOW);
    digitalWrite(pin_LED, LOW);

    // flag
    pulse = false;
  }
  if (sysready && pulse){
    // no longer engaged
    noTone(pin_audio);  

    // TTL
    digitalWrite(pin_TTL, LOW);
    digitalWrite(pin_LED, LOW);

    // flag
    pulse = false;
  }
  
  delayMicroseconds(stepsize);
  
}

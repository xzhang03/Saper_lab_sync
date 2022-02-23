// Audio synchronizing for Saper lab
// Stephen Zhang 12/13/2021

#define debugmode false // Debug

#define pin_audio PIN_C6
#define pin_TTL PIN_B5
#define pin_readyLED PIN_B4
#define pin_swtich PIN_D7
#define pin_LED PIN_D6
#define pin_LED_blue PIN_B6 // blue LED, currently unused

// Flags
bool sysready = false; // Flagged true during setup. Flag false to start pulsing
bool pulse = false;
bool readyled = false;
bool useswitch = true; // Used to disable switch while the USB connection is going

// buzz
unsigned int buzzfreq = 2000;

// Time
unsigned int cycle = 33333;
unsigned int pulse_width = 2000;
unsigned int stepsize = 100;

unsigned long t0, t1;

// Serial communication
byte m, n;

long pos = 255;

void setup(){
  Serial.begin(19200);
  
  // Pins
  pinMode(pin_audio, OUTPUT); // Set buzzer - pin 9 as an output
  pinMode(pin_TTL, OUTPUT);
  pinMode(pin_readyLED, OUTPUT);
  pinMode(pin_LED, OUTPUT);
  pinMode(pin_LED_blue, OUTPUT);
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
  digitalWrite(pin_LED_blue, LOW);

  // t0
  t0 = micros();
}

void loop(){
  // Time
  t1 = micros();
  
  if (Serial.available() >= 2){
    // Read 2 bytes
    m = Serial.read();
    n = Serial.read();
    
    // Parse serial
    parseserial();
  }
  
  // Get input (HIGH = ready, LOW = engaged)
  if (useswitch){
    sysready = digitalRead(pin_swtich);
  }
  
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

  // Pulsing
  pulsefunction();
  
  delayMicroseconds(stepsize);
  
}

// Parse serial
void parseserial(){
  if (debugmode){
    m = m - '0';
    n = n - '0';
  }

  // m table
  // ========== Operational ==========
  // 2: new cycle time (1000000/n, n in Hz)
  // 1: start pulse
  // 0: stop pulse
  // 5: Quad encoder position (255)
  
  
  switch (m){

    case 0:
      // End pulsing
      sysready = true;
      useswitch = true;
      if (debugmode){
        Serial.println("Pulse end.");
      }
      break;
      
    case 1:
      // Start pulsing
      sysready = false;
      useswitch = false; // Disable switch
      if (debugmode){
        Serial.println("Pulse start.");
      }
      break;

    case 2:
      // Set frequency
      cycle = 1000000 / n;

      break;

    case 5:

//      Serial.println(pos);      
      Serial.write((byte *) &pos, 4);
      break;
  }
}

void pulsefunction(){
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
}

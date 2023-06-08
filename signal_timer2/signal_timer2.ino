int interrupt = 0;

void setupTimer2() {
  noInterrupts();
  // Clear registers
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;

  // 1 Hz (16000000/((124+1)*256))
  OCR2A = 124;
  // CTC
  TCCR2A |= (1 << WGM21);
  // Prescaler 256
  TCCR2B |= (1 << CS22);
  // Output Compare Match A Interrupt Enable
  TIMSK2 |= (1 << OCIE2A);
  interrupts();
}

void setup() {
  Serial.begin(9600);
  pinMode(9, OUTPUT);
  //pinMode(ledPin, OUTPUT);
  pinMode(2, INPUT);
  setupTimer2();
}

void loop(){
  
}

ISR(TIMER2_COMPA_vect) {
  //digitalWrite(ledPin, digitalRead(ledPin) ^ 1);
  //Serial.println("seconde");
  //gestionPoulailler();
  //heure++;
  interrupt++;
  if (interrupt == 2000) {
    Serial.println("seconde");
    interrupt = 0;
  }
  
}

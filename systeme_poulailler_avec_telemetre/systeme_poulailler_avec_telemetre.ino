#include <Servo.h>
#include "Ultrasonic.h"

Servo servo;
Ultrasonic ultrasonic(7);

#define ledPin 13

int interrupt = 0;
int distanceLimite = 15;
int heure = 0;
int angle = 10;

int ouvertureEte = 6;
int fermetureEte = 22;

int ouvertureHiver = 8;
int fermetureHiver = 20;

char hiver = true; // true => heures d'hiver ; false => heures d'été
char porteOuverte = false;
char jour  = false;

int i = 0;

void ouverturePorte() {
  // scan from 0 to 180 degrees
  digitalWrite(9, HIGH);
  porteOuverte = true;
  
  for(angle = 10; angle < 180; angle++)  
  {                                  
    servo.write(angle);               
    //delay(15);                   
  } 
  //Serial.println("INFO : Ouverture terminée");
}

void fermeturePorte() {
  digitalWrite(9, LOW);
  porteOuverte = false;
  
  for(angle = 120; angle > 10; angle--)    
  {                                
    servo.write(angle);           
    //delay(15);       
  }
  //Serial.println("INFO : Fermeture terminée");
}

void afficherHeure() { // Simple affichage de l'heure
    Serial.print("Il est ");
    Serial.print(heure);
    Serial.println(" heure");
}


void gestionPorte(int heureOuverture, int heureFermeture) {
  /* 
   *  Prend en argument l'heure d'ouverture et l'heure de fermeture.
   *  Compare l'heure aux heures consignes. Ouvre et ferme en conséquence
   */
  if (heure == heureOuverture) { // Heure d'ouverture de la porte
    jour = true;
    digitalWrite(9, HIGH);
    porteOuverte = true;
    Serial.println("Ouverture de la porte");
  }
  else if (heure == heureFermeture) { ;// Heure de femeture de la porte
    jour = false;
    digitalWrite(9, LOW);
    porteOuverte = false;
    Serial.println("Fermeture de la porte");
  }
}


void gestionPoulailler() { // Appelée à chaque interruption
  if (heure == 25) { // Journée écoulée, remise de à 0 de l'heure
    heure = 0;
    afficherHeure();
  } 
    
  else {
    afficherHeure();

    if (hiver) {
      gestionPorte(ouvertureHiver, fermetureHiver);
    }
    else {
      gestionPorte(ouvertureEte, fermetureEte);
    }
  
  }
}


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
  pinMode(ledPin, OUTPUT);
  pinMode(2, INPUT);
  setupTimer2();
  servo.attach(8);
}

void loop() {
  
  /*
   * Pour changer de saison il faut "appuyer" un bouton simulée: 
   * la broche 2 devra recevoir un signal HAUT pendant DEUX SECONDES.
   */
  int btn = digitalRead(2);
  long distance;
  
  if (btn > 0 && i == 4) { // si le bouton est "appuyé" depuis 2 secondes...
    hiver = !hiver; // changement de saison
    Serial.println("ATTENTION : Changement de saison !");
    i=0;
  }
  else if (btn > 0) { // On compte le temps pendant lequel le bouton est "appuyé"
    i++;
  }
  else { // Si le bouton est relâché, on repart de 0
    i=0;
  }

  /*
   * Mesure de la distance pour s'assurer qu'aucun obstacle n'est devant la porte
   */
  distance = ultrasonic.MeasureInCentimeters();
  if (distance < distanceLimite && !porteOuverte && !jour)
  {
    Serial.println("ATTENTION : Obstacle détecté !");
    ouverturePorte();
  }
  else if (distance > distanceLimite && porteOuverte && !jour)
  {
    fermeturePorte();
  }
 

  
  delay(500); 
}

ISR(TIMER2_COMPA_vect) {
  interrupt++;
  if (interrupt == 2000) { // Interruptions à 2kHz donc interrupt=2000 <=> 1 seconde écoulée
    digitalWrite(ledPin, digitalRead(ledPin) ^ 1);
    gestionPoulailler();
    heure++;
    interrupt = 0;
  }
}

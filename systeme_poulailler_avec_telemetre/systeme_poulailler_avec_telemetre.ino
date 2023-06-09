#include <Servo.h>
#include <Ultrasonic.h>

/*
 * LED témoin : BROCHE 9
 * commande moteur : BROCHE 8 
 * Capteur ultrason : BROCHE 7 
 * Bouton : BROCHE 2
 */

Servo servo;
Ultrasonic ultrasonic(7);

#define ledPin 13

int interrupt = 0;
int distanceLimite = 15;
int heure = 0;
int angle = 10;

int ouvertureDefaut = 8;
int fermetureDefaut = 18;

int ouverturePerso = 0;
int fermeturePerso = 0;
int compteurChoix = 0; // A chaque nouveau choix d'horaire le compte s'incrémente

//char hiver = true; // true => heures d'hiver ; false => heures d'été
char heurePerso = false; // Si les heures d'ouverture/fermeture ont été changées, la variable passe à true
char choixSurOuverture = true; // true => heure perso. sélection ouverture ; false => selection heure fermeture
char porteOuverte = false;
char jour  = false;
char obstacle = false; // Si un obtacle est détecté (15 cm)

int i = 0;
int compteurBoucle = 0; // S'incrémente à chaque boucle loop

 int definirHeure(){
  /* 
   *  Chaque bouton poussoir est associé à un chiffre.
   *  Pour fixer une heure, il faut activer des boutons poussoir de tels sorte 
   *  que la somme des chiffres qui leurs sont associés soit égale à l'heure 
   *  que l'ont désire fixer.
   *  
   * bouton 1 => BROCHE 3
   * bouton 2 => BROCHE 4
   * bouton 3 => BROCHE 5
   * bouton 4 => BROCHE 6 
   * bouton 5 => BROCHE A0
   * bouton 6 => BROCHE A3
   * bouton 7 => BROCHE A4
   * bouton 8 => BROCHE A5
   */
   
  int heure = 0;

  if (digitalRead(3) > 0) { //Bouton 1
     heure ++;
  }
  if (digitalRead(4) > 0) { // Bouton 2
    heure += 2;
  }
  if (digitalRead(5) > 0) { // Bouton 3
    heure += 3;
  }
  if (digitalRead(6) > 0) { // Bouton 4
    heure += 4;
  }
  if (digitalRead(A0) > 0) { // Bouton 5
    heure += 5;
  }
  if (digitalRead(A3) > 0) { // Bouton 6
    heure += 6;
  }
  if (digitalRead(A4) > 0) { // Bouton 7
    heure += 7;
  }
  if (digitalRead(A5) > 0) { // Bouton 8
    heure += 8;
  }

  return heure;
 }


void ouverturePorte() {
  Serial.println("Ouverture de la porte");
  digitalWrite(9, HIGH);
  porteOuverte = true;

  // scan from 0 to 180 degrees
  for(angle = 10; angle < 180; angle++)  
  {                                  
    servo.write(angle);                              
  } 

}

void fermeturePorte() {
  Serial.println("Fermeture de la porte");
  digitalWrite(9, LOW);
  porteOuverte = false;
  
  for(angle = 180; angle > 10; angle--)    
  {                                
    servo.write(angle);           
    delay(5);
    detecterObstacle();

    /*
     * Si un obstacle est détecté pendant la fermeture,
     * on arrête la fermeture et on ouvre la porte
     */
    if (obstacle) { // Obstacle détecté
      for(angle; angle < 180; angle++) { // Ouverture de la porte 
        servo.write(angle); 
      }
      digitalWrite(9, HIGH); // Allumage de la LED temoin
      porteOuverte = true;
      break;      
    }
  }
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
    ouverturePorte();
  }
  else if (heure == heureFermeture) { ;// Heure de femeture de la porte
    jour = false;
    fermeturePorte();
  }
}


void gestionPoulailler() { // Appelée à chaque interruption
  if (heure == 25) { // Journée écoulée, remise de à 0 de l'heure
    heure = 0;
    afficherHeure();
  } 
    
  else {
    afficherHeure();

    if (compteurChoix > 1) { // Compteur choix > 1 => Une heure d'ouverture ET de fermeture définis
      gestionPorte(ouverturePerso, fermeturePerso);
    }
    else {
      gestionPorte(ouvertureDefaut, fermetureDefaut);
    }
  
  }
}

void detecterObstacle() {
  int distance;
    distance = ultrasonic.MeasureInCentimeters();
    
    if (distance < distanceLimite) // Si un obstacle est détecté
    {
      Serial.println("ATTENTION : Obstacle détecté !");
      //ouverturePorte();
      obstacle = true;
    }
    else
    {
      //fermeturePorte();
      obstacle = false;
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

  // Entrée interrupteur 8 voies
  pinMode(A0, INPUT);
  pinMode(A3, INPUT);  
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
}

void loop() {
  
  /*
   * Pour changer de saison il faut "appuyer" un bouton simulée: 
   * la broche 2 devra recevoir un signal HAUT pendant DEUX SECONDES.
   */
  int btn = digitalRead(2);
  long distance;
  

  // Personnalisation des horaires d'ouverture/fermeture avec le bouton
  if (digitalRead(2) == 1 && compteurBoucle < 10) {
    Serial.println("Appui bouton non pris en compte");
  }
  else if (digitalRead(2) == 1) { // Si le bouton est pressé...
    Serial.println("Bouton pressé...");
    while(digitalRead(2) == 1); // On attend que le bouton repasse à l'état bas une fois car avant les données sont souvent erronées
    int nouvelleHeure = definirHeure();
    Serial.println("ATTENTION : attendez 5 secondes avant de presser le bouton à nouveau !"); // Car le bouton ne pas pas à l'état bas directement après le relachement

    if (nouvelleHeure == 0) {
      Serial.println("Vous ne pouvez pas régler sur 0.");
    }
    else if (choixSurOuverture) {
      ouverturePerso = nouvelleHeure;
      choixSurOuverture = !choixSurOuverture;
      Serial.print("Nouvelle heure d'ouverture : ");
      Serial.print(ouverturePerso);
      Serial.println(" heure");
      compteurBoucle = 0;
      compteurChoix++;
    }
    else if (!choixSurOuverture) {
      fermeturePerso = nouvelleHeure;
      choixSurOuverture = !choixSurOuverture;
      Serial.print("Nouvelle heure de fermture : ");
      Serial.print(fermeturePerso);
      Serial.println(" heure");
      compteurBoucle = 0;
      compteurChoix++;
    }
  }


  // Mesure de la distance pour s'assurer qu'aucun obstacle n'est devant la porte
  detecterObstacle();
  if (obstacle && !porteOuverte && !jour)
  {
    ouverturePorte();
  }
  else if (!obstacle && porteOuverte && !jour)
  {
    fermeturePorte();
  }

  compteurBoucle++;
  //Serial.println(compteurBoucle);
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

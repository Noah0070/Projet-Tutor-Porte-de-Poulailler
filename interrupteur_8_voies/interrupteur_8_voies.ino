/*
 * 1 => BROCHE 3
 * 2 => BROCHE 4
 * 3 => BROCHE 5
 * 4 => BROCHE 6 
 * 5 => BROCHE A0
 * 6 => BROCHE A3
 * 7 => BROCHE A4
 * 8 => BROCHE A5
 */


 int definirHeure(){
  /* 
   *  Chaque bouton poussoir est associé à un chiffre.
   *  Pour fixer une heure, il faut activer des boutons poussoir de tels sorte 
   *  que la somme des chiffres qui leurs sont associés soit égale à l'heure 
   *  que l'ont désire fixer.
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

void setup() {
  pinMode(2, INPUT); // Bouton
  pinMode(A0, INPUT);
  pinMode(A3, INPUT);  
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  Serial.begin(9600);
}

void loop() {
  if (digitalRead(2) == 1) { // Si le bouton est pressé...
    Serial.println("Bouton pressé...");
    while(digitalRead(2) == 1); // On attend que le bouton repasse à l'état bas une fois car avant les données sont souvent erronées
    Serial.println(definirHeure());
    Serial.println("ATTENTION : attendez 5 secondes avant de presser le bouton à nouveau !"); // Car le bouton ne pas pas à l'état bas directement après le relachement
  }
  
  delay(200);
}

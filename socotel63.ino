#include <DFRobot_DF1101S.h>
#include <SoftwareSerial.h>

const int trigPin = 13;
const int echoPin = 12;
const int socotelPin = 5;
const int ledPin = LED_BUILTIN;
const int pinImpulsion = 2;
const int pinInterrupteur = 4;
const int solenoidPin1 = 6;  // Broche IN1 du L293D
const int solenoidPin2 = 7;  // Broche IN2 du L293D

long duration;
int distance;
volatile int nombreImpulsions = 0;
volatile bool cadranMonte = false;
unsigned long lastImpulsionTime = 0;

bool solenoidMoving = false;
bool combineRaccrochePrecedemment = false;

SoftwareSerial df1101sSerial(11, 10);
DFRobot_DF1101S df1101s;


void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(socotelPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(pinImpulsion, INPUT_PULLUP);
  pinMode(pinInterrupteur, INPUT_PULLUP);
  pinMode(solenoidPin1, OUTPUT);
  pinMode(solenoidPin2, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(pinImpulsion), detecterImpulsion, RISING);

  df1101sSerial.begin(115200);
  while (!df1101s.begin(df1101sSerial)) {
    Serial.println("Init failed, please check the wire connection!");
    delay(1000);
  }

  df1101s.setVol(10);
  Serial.print("VOL:");
  Serial.println(df1101s.getVol());
  delay(2000);
  df1101s.setPlayMode(df1101s.ALLCYCLE);
  Serial.print("PlayMode:");
  Serial.println(df1101s.getPlayMode());
  df1101s.setPrompt(true);
}

void loop() {
  verifierCombine();
  delay(500);
}

void verifierCombine() {
  bool phoneHangedUp = digitalRead(socotelPin) == HIGH;


  if (phoneHangedUp) {
    if (combineRaccrochePrecedemment) {
      df1101s.switchFunction(df1101s.MUSIC);
      df1101s.pause();
      Serial.println("Combiné raccroché : Mise en pause de la lecture audio");
    }
    combineRaccrochePrecedemment = false;

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.017;
    Serial.print("Distance: ");
    Serial.println(distance);

    if (distance < 20 && !solenoidMoving) {
      moveSolenoid();
    }
  } else {
    if (!combineRaccrochePrecedemment) {
      df1101s.playSpecFile("INTROINTRO.MP3");
      Serial.println("Combiné décroché : Lecture du fichier audio");
 
    }
    combineRaccrochePrecedemment = true;
    numeroCadran(); 

  }
}


void numeroCadran() {
  int etatInterrupteur = digitalRead(pinInterrupteur);


  if (etatInterrupteur == LOW) {
    if (!cadranMonte) {
      cadranMonte = true;
      nombreImpulsions = 0;
      lastImpulsionTime = millis();
      Serial.println("Interrupteur ouvert : cadran montant. Comptage des impulsions...");
    }
  } else {
    if (cadranMonte) {
      cadranMonte = false;
      Serial.println("Interrupteur ouvert : cadran descendant. Affichage du chiffre composé...");
      int chiffreCompose = convertirEnChiffre(nombreImpulsions);
      Serial.print("Chiffre composé : ");
      Serial.println(chiffreCompose);
    
      if (chiffreCompose < 4) {
        cadranPlay();
        combineRaccrochePrecedemment = false;
        return;

      } else {
        cadranRecord();
        combineRaccrochePrecedemment = false;
        return;
      }
    }
  }
}

void cadranPlay() {
  df1101s.switchFunction(df1101s.MUSIC);
  delay(500);

  // Générer un nombre aléatoire entre 1 et 10
  int fichierAleatoire = random(1, 11);

  // Jouer le fichier audio spécifique généré aléatoirement
  df1101s.playSpecFile(fichierAleatoire);

  unsigned long debutLecture = millis();
  while (millis() - debutLecture < 10000) {
    if (digitalRead(socotelPin) == HIGH) {
      Serial.println("Combiné raccroché : Lecture en pause");
      df1101s.pause();
      delay(500);
      combineRaccrochePrecedemment = true;
      return; // Sortir de la fonction si le combiné est raccroché
    }
  }
  
  // Si le combiné n'est pas raccroché, on termine normalement la lecture
  df1101s.switchFunction(df1101s.MUSIC);
  Serial.println("Lecture du dernier enregistrement...");
  Serial.println("Fin de la lecture audio.");
}



void cadranRecord() {
  df1101s.switchFunction(df1101s.RECORD);
  delay(2000);
  df1101s.start();
  Serial.println("Début de l'enregistrement...");
  
  unsigned long debutEnregistrement = millis();
  while (millis() - debutEnregistrement < 5000) {
    if (digitalRead(socotelPin) == HIGH) {
      df1101s.switchFunction(df1101s.MUSIC);
      delay(500);
      df1101s.playSpecFile(100);
      Serial.println("Combiné raccroché : Enregistrement en pause");
      df1101s.delCurFile();
      delay(500);
      df1101s.playSpecFile(100);
      df1101s.switchFunction(df1101s.MUSIC);

      combineRaccrochePrecedemment = false;
      return;
    }
  }

  String nomFichierEnregistrement = df1101s.saveRec();
  Serial.print("Enregistrement sauvegardé sous le nom : ");
  Serial.println(nomFichierEnregistrement);

  df1101s.switchFunction(df1101s.MUSIC);
  delay(2000);
  df1101s.playSpecFile(999);
  Serial.println("Lecture du dernier enregistrement...");
  delay(5000);

}


void detecterImpulsion() {
  unsigned long currentTime = millis();
  if (cadranMonte && currentTime - lastImpulsionTime >= 66) {
    nombreImpulsions++;
    Serial.println("Impulsion détectée");
    lastImpulsionTime = currentTime;
  }
}

void moveSolenoid() {
  Serial.println("Sonnerie");

  // Inverser les courants pendant 5 secondes
  for (int i = 0; i < 5; i++) {
    // Inverser la polarité
    digitalWrite(solenoidPin1, HIGH);
    digitalWrite(solenoidPin2, LOW);
    delay(100);  // Attendre une seconde

    // Inverser à nouveau la polarité
    digitalWrite(solenoidPin1, LOW);
    digitalWrite(solenoidPin2, HIGH);
    delay(100);  // Attendre une seconde
  }
}

int convertirEnChiffre(int nombreImpulsions) {
  int chiffre = nombreImpulsions % 10;
  int chiffreReel = chiffre;
  
  if (chiffreReel == 10) {
    chiffreReel = 0;
  }

  return chiffreReel;
}

// Code réalisé par Victor Simonet Alves Santos avec la collaboration du Lycée Déodat de Séverac 
// Mail: victorcaue@hotmail.fr
// Deuxième version officielle du code arduino pour la serre proximars
// Modifications:Toutes les annexes et broches sont fonctionnelles, prêt a l'emploi
// Détails: Rangé et commenté, nécessite un module de récupération d'heure en temps réel

// Blibliothèques
  #include <Wire.h>
  #include <rgb_lcd.h>
  #include <DHT.h>
  #include <Adafruit_Sensor.h>
  #include <Adafruit_TSL2561_U.h>
  #include <SoftwareSerial.h>
  #include <TimeLib.h> 
// Broches
  // Digital
    #define boutonPin 2 // Bouton sur la broche 2
    #define bulleurPin 3 // Bulleur sur la broche 3
    #define arrosagePin 6 // Arrosage sur la broche 2 
    #define eclairagePin 7 // Bouton sur la broche 2
    #define chauffagePin 8 // Bouton sur la broche 2
    #define ventilateurPin 9 // Bouton sur la broche 2
    SoftwareSerial BTserial(4, 5); // RX sur 4, TX sur 5 pour le module Bluetooth
  //Analog
    DHT dht(A0, DHT22);
  //I2C
    Adafruit_TSL2561_Unified tsl(0x29);
    rgb_lcd lcd;
// Variables
  unsigned long ActivationBouton = 0;
  unsigned long ActivationHumidite = 0;
  unsigned long BasculeAuto = 0;
  unsigned long MAJ = 0;
  unsigned long IntervalleMAJ = 10; // Intervalle de mise à jour des capteurs (10 millisecondes)
  unsigned long Inactivite = 10000; // 10 secondes d'inactivité
  unsigned long IntervalleBasculeAuto = 3000; // 3 secondes d'intervalle pour la bascule automatique
  unsigned long InactiviteMin = 21600000; // Durée minimale d'inactivité après l'arrosage (6 heures)
  unsigned long ActivationArrosage = 300000; // 5 minutes en millisecondes
  unsigned long Desactivation = 0;

  bool ActivationEnCours = false;

  int displayMode = 0; // Mode d'affichage initial : 0 pour température, 1 pour humidité air, 2 pour humidité sol, 3 pour luminosité
  int etatPrecedentBouton = LOW; // Variable pour stocker l'état précédent du bouton
  
  byte DEGREE_SYMBOL = 0; // Symbole personnalisé représentant le symbole de degré Celsius (°).
  byte degree[8] = {
      0b00000,
      0b00010,
      0b00101,
      0b00010,
      0b00000,
      0b00000,
      0b00000,
      0b00000
  };
void setup() {
  // initialisations
    Serial.begin(9600); // Initialiser la connexion série
    BTserial.begin(9600); //Initialiser la communication série pour le module Bluetooth 
    lcd.begin(16, 2); //Initialiser l'écran LCD - 2 lignes avec 16 caractères chacune
    lcd.createChar(DEGREE_SYMBOL, degree); // Enregistrer le nouveau symbole "°" sur l'écran LCD
    dht.begin(); //Initialiser le capteur DHT (Humidité de l'air et température
    setTime(0, 0, 0, 1, 1, 2024); //Définir l'heure à minuit le 1er janvier 2024
    if (!tsl.begin()) { //Initialiser le capteur TSL (luminosité) ou renvoyer un message d'échec
      Serial.println("Capteur TSL non trouvé. Vérifiez les connexions !");
    }
    tsl.enableAutoRange(true);  // Activer la plage automatique pour adapter la gamme de mesure à la luminosité
    tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS); // Choisir un temps d'intégration court pour des mesures rapides
    
  // Entrées/Sorties
    pinMode(boutonPin, INPUT); // Définir la broche du bouton comme une entrée
    pinMode(bulleurPin, OUTPUT); // Définir la broche du bulleur comme une sortie
    pinMode(arrosagePin, OUTPUT); // Définir la broche de l'arrosage comme une sortie
    pinMode(eclairagePin, OUTPUT); // Définir la broche de l'éclairage comme une sortie
    pinMode(chauffagePin, OUTPUT); // Définir la broche du chauffage comme une sortie
    pinMode(ventilateurPin, OUTPUT); // Définir la broche du ventilateur comme une sortie
}

void loop() {
  // Etat du bouton
    int etatBouton = digitalRead(boutonPin); // Lecture de l'état actuel du bouton
  //Temps actuel
    unsigned long maintenant = millis(); // Temps actuel en millisecondes
    
  // Activation du bouton pour l'intervalle d'affichage automatique
    if (etatBouton == HIGH && etatPrecedentBouton == LOW) {
        displayMode = (displayMode + 1) % 4; // Changer de mode d'affichage à chaque appui sur le bouton  
        ActivationBouton = maintenant; // Mettre à jour le moment de la dernière activation
    }
    etatPrecedentBouton = etatBouton;

    unsigned long tempsEcoule = maintenant - ActivationBouton;  // Calculer le temps écoulé depuis la dernière activation du bouton
    if (tempsEcoule >= Inactivite) { // Vérifier si le bouton a été inactif pendant 10 secondes
      if (maintenant - BasculeAuto >= IntervalleBasculeAuto) { //vérifier si 3 secondes se sont écoulées depuis la dernière bascule automatique          
        displayMode = (displayMode + 1) % 4; // Changer de mode d'affichage          
        BasculeAuto = maintenant; // Mettre à jour le temps de la dernière bascule automatique
      }
    }
  // Contrôle des composants et affichage LCD/Bluetooth via les capteurs
    if (maintenant - MAJ >= IntervalleMAJ) {
      float humiditeSol = lireHumiditeSol();
      float luminosite = lireLuminosite();
      float temperature = dht.readTemperature();
      MAJ = maintenant;
    
    // Contrôle des composants
      // Contrôle du bulleur et de l'arrosage en fonction de l'humidité du sol
        if (humiditeSol < 80 && !ActivationEnCours) {
          activerBulleur(255); // Activer le bulleur et l'arrosage
          activerArrosage(255);
          ActivationHumidite = maintenant;
          ActivationEnCours = true;
        }
        if (ActivationEnCours && maintenant - ActivationHumidite >= ActivationArrosage) {
          activerBulleur(0); // Désactiver le bulleur et l'arrosage après 5 minutes
          activerArrosage(0);
          Desactivation = maintenant;
          ActivationEnCours = false;
        }
        if (!ActivationEnCours && maintenant - Desactivation >= InactiviteMin) {
        ActivationHumidite = 0; // Réinitialiser les variables pour permettre une nouvelle activation
        Desactivation = 0;
        } 

      // Contrôle de l'éclairage en fonction de la luminosité et de l'heure
        int heure = hour(); // problème dans la récupération de l'heure, il est nécéssaire d'ajouter un module RTC (DS3231) pour récupérer une heure fidèle

        if (/*heure >= 8 && heure < 20 && */luminosite < 100) {
            digitalWrite(eclairagePin, HIGH);
        } else {
            digitalWrite(eclairagePin, LOW);
        }

      // Contrôle du chauffage
        if (temperature < 20) {
            digitalWrite(chauffagePin, HIGH);
        } else {
            digitalWrite(chauffagePin, LOW);
        }

      // Contrôle du ventilateur
        if (temperature > 25) {
            digitalWrite(ventilateurPin, HIGH);
        } else {
            digitalWrite(ventilateurPin, LOW);
        }

    // Affichage LCD et Bluetooth
      switch (displayMode) {
        case 0: // Afficher la température
            afficherTemperature();
            BTserial.println("temperature=" + String(dht.readTemperature()));
            break;
        case 1: // Afficher l'humidité de l'air
            afficherHumiditeAir();
            BTserial.println("humiditeair=" + String(dht.readHumidity()));
            break;
        case 2: // Afficher l'humidité du sol
            afficherHumiditeSol();
            BTserial.println("humiditesol=" + String(lireHumiditeSol()));
            break;
        case 3: // Afficher la luminosité
            afficherLuminosite();
            BTserial.println("luminosite=" + String(lireLuminosite()));
            break;
      }
    }
}

void afficherTemperature() {
    float temperature = dht.readTemperature();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temperature:");
    lcd.setCursor(0, 1);
    lcd.print(temperature);
    lcd.write(DEGREE_SYMBOL);
    lcd.print("C");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println("C");

    // Définir les plages de température et les couleurs correspondantes
    const float coldest = 18.0;  // Température la plus froide (bleu)
    const float hottest = 30.0;  // Température la plus chaude (rouge)

    // Calculer la couleur en fonction de la température
    int red = map(temperature, coldest, hottest, 0, 255);
    int green = 0;  // Pas de vert pour cet exemple
    int blue = map(temperature, coldest, hottest, 255, 0);

    // Limiter les valeurs RVB dans la plage valide (0-255)
    red = constrain(red, 0, 255);
    blue = constrain(blue, 0, 255);

    // Appliquer la couleur à l'écran LCD
    lcd.setRGB(red, green, blue);
}

void afficherHumiditeAir() {
    float humidite = dht.readHumidity();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Humidite Air:");
    lcd.setCursor(0, 1);
    lcd.print(humidite);
    lcd.print("%");
    Serial.print("Humidite Air: ");
    Serial.print(humidite);
    Serial.println("%");
}

void afficherHumiditeSol() {
    float humiditeSol = lireHumiditeSol();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Humidite Sol:");
    lcd.setCursor(0, 1);
    lcd.print(humiditeSol);
    lcd.print("%");
    Serial.print("Humidite Sol: ");
    Serial.print(humiditeSol);
    Serial.println("%");
}
float lireHumiditeSol() {
    int valeurCapteur = analogRead(A1);
    float humiditeSol = map(valeurCapteur, 0, 725, 0, 100);
    return humiditeSol;
}

void afficherLuminosite() {
    float luminosite = lireLuminosite();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Luminosite:");
    lcd.setCursor(0, 1);
    lcd.print(luminosite);
    lcd.print(" lux");
    Serial.print("Luminosite: ");
    Serial.print(luminosite);
    Serial.println(" lux");
}
float lireLuminosite() {
    sensors_event_t event;
    tsl.getEvent(&event);
    if (event.light) {
        return event.light;
    } else {
        return -1.0; // Valeur invalide en cas d'échec de lecture
    }
}

void activerBulleur(int VitesseBulleur) {
    analogWrite(bulleurPin, VitesseBulleur);
}

void activerArrosage(int VitesseArrosage) {
    digitalWrite(arrosagePin, VitesseArrosage);
}

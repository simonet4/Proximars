// Code réalisé par Victor Simonet Alves Santos avec la collaboration du Lycée Déodat de Séverac et Stéphan Jude Hamilton.
// Mail: victorcaue@hotmail.fr
// Cinquième version officielle du code arduino pour la serre proximars.
// Modifications: Ajout du buzzer selon la qualité de l'air.
// Détails: Rangé et commenté, compatible arduino uno et méga, Lcd RGB de préférence.

// Blibliothèques
  #include <Wire.h>
  #include <rgb_lcd.h>
  #include <DHT.h>
  #include <Adafruit_Sensor.h>
  #include <Adafruit_TSL2561_U.h>
  #include <SoftwareSerial.h>
  #include <Adafruit_NeoPixel.h>
  #include <Adafruit_SGP30.h>

// Broches sur l'arduino
  // Digital
    #define boutonPin A3 // Bouton sur la broche A4
    #define bulleurPin 2 // Bulleur sur la broche 2
    #define arrosagePin 3 // Arrosage sur la broche 3
    #define chauffagePin 4 // chauffage sur la broche 4
    #define ventilateurPin 5 // ventilateur sur la broche 5
    #define buzzerPin 6 // Buzzer sur la broche 6
    #define led1 11 // Broche de données 11 pour la première partie du ruban
    #define led2 12 // Broche de données 12 pour la deuxième partie du ruban
    #define led3 13 // Broche de données 13 pour la troisième partie du ruban
    #define NUM_LEDS 30 // Nombre de LED à 30 dans chaque partie de ruban
    SoftwareSerial BTserial(7, 8); // RX sur 7, TX sur 8 pour le module Bluetooth, soit la broche 7
  //Analog
    DHT dht(A0, DHT22); // Capteur de température et humidité DHT sur la broche A0
  //I2C
    Adafruit_TSL2561_Unified tsl(0x29); // Capteur de luminosité I2C
    rgb_lcd lcd; // Afficheur LCD I2C
  // SGP30
    Adafruit_SGP30 sgp; // Capteur de qualité de l'air I2C
// Variables
  unsigned long ActivationBouton = 0;
  unsigned long ActivationHumidite = 0;
  unsigned long BasculeAutoBouton = 0;
  unsigned long dernierEnvoiBluetooth = 0;
  unsigned long MAJ = 0;
  unsigned long Desactivation = 0;
  unsigned long IntervalleMAJ = 10; // Intervalle de mise à jour des capteurs (10 millisecondes)
  unsigned long intervalleEnvoiBluetooth = 300; // Intervalle d'envoi en millisecondes
  unsigned long Inactivite = 10000; // 10 secondes d'inactivité
  unsigned long IntervalleBasculeAutoBouton = 3000; // 3 secondes d'intervalle pour la bascule automatique
  unsigned long InactiviteMin = 21600000; // Durée minimale d'inactivité après l'arrosage (6 heures)
  unsigned long ActivationArrosage = 300000; // 5 minutes en millisecondes

  bool ActivationEnCours = false;

  int typeDonneeEnCours = 0; // 0: température, 1: humidité de l'air, 2: humidité du sol, 3: luminosité
  int displayMode = 0; // Mode d'affichage initial : 0 pour température, 1 pour humidité air, 2 pour humidité sol, 3 pour luminosité
  int etatPrecedentBouton = LOW; // Variable pour stocker l'état précédent du bouton
//autres 
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
  Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(NUM_LEDS, led1, NEO_GRB + NEO_KHZ800);
  Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(NUM_LEDS, led2, NEO_GRB + NEO_KHZ800);
  Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(NUM_LEDS, led3, NEO_GRB + NEO_KHZ800);

void setup() {
  // initialisations
    Serial.begin(9600); // Initialiser la connexion série
    BTserial.begin(9600); //Initialiser la communication série pour le module Bluetooth 
    lcd.begin(16, 2); //Initialiser l'écran LCD - 2 lignes avec 16 caractères chacune
    lcd.print("Hello, world!"); // Afficher du texte sur l'écran LCD
    lcd.createChar(DEGREE_SYMBOL, degree); // Enregistrer le nouveau symbole "°" sur l'écran LCD
    dht.begin(); //Initialiser le capteur DHT (Humidité de l'air et température
    if (!tsl.begin()) { //Initialiser le capteur TSL (luminosité) ou renvoyer un message d'échec
      Serial.println("Capteur TSL non trouvé. Vérifiez les connexions !");
    }
    if (! sgp.begin()){
    Serial.println("Sensor not found :(");
    while (1);
    }
    tsl.enableAutoRange(true);  // Activer la plage automatique pour adapter la gamme de mesure à la luminosité
    tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS); // Choisir un temps d'intégration court pour des mesures rapides
    strip1.begin();  // Initialise la première partie du ruban de LED
    strip2.begin();  // Initialise la deuxième partie du ruban de LED
    strip3.begin();  // Initialise la troisième partie du ruban de LED
  // Entrées/Sorties
    pinMode(boutonPin, INPUT); // Définir la broche du bouton comme une entrée
    pinMode(bulleurPin, OUTPUT); // Définir la broche du bulleur comme une sortie
    pinMode(arrosagePin, OUTPUT); // Définir la broche de l'arrosage comme une sortie
    pinMode(chauffagePin, OUTPUT); // Définir la broche du chauffage comme une sortie
    pinMode(ventilateurPin, OUTPUT); // Définir la broche du ventilateur comme une sortie
  //Leds
    // Initialise toutes les LED à "éteintes" (noir) pour chaque partie du ruban
      strip1.show();
      strip2.show();
      strip3.show();
}

void loop() {
  unsigned long maintenant = millis(); // Temps actuel en millisecondes
  //qualité de l'air et alarme 
  if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    return;
  }
  Serial.print("TVOC "); Serial.print(sgp.TVOC); Serial.print(" ppb\t");
  Serial.print("eCO2 "); Serial.print(sgp.eCO2); Serial.println(" ppm");

  if (sgp.eCO2 >= 1500 || sgp.TVOC >= 500) { // Conditions pour activer le buzzer
    tone(buzzerPin, 3000, 500); // Activer le buzzer si les seuils sont dépassés
  }

  // Etat du bouton
    int etatBouton = digitalRead(boutonPin); // Lecture de l'état actuel du bouton
  //Temps actuel
    if (BTserial.available()) {
        int hour = BTserial.read(); // Convertir le caractère en entier
        Serial.print("Heure reçue : ");
        Serial.println(hour);

        // Contrôler les LED en fonction de l'heure reçue
        if (hour >= 8 && hour <= 20) { // Si l'heure est entre 8h et 20h
            // Contrôlez les LED en fonction de la luminosité
            float luminosite = lireLuminosite();
            if (luminosite < 500) {
                // Allumer les LED en violet
                allumerLED(strip1, 128, 0, 128);
                allumerLED(strip2, 128, 0, 128);
                allumerLEDfe(strip3, 128, 0, 128);
            } else {
                // Éteindre les LED
                eteindreLED(strip1);
                eteindreLED(strip2);
                eteindreLED(strip3);
            }
        } else {
            // Éteindre les LED si en dehors de l'intervalle d'heures
            eteindreLED(strip1);
            eteindreLED(strip2);
            eteindreLED(strip3);
        }
    }

  // Activation du bouton pour l'intervalle d'affichage automatique
    if (etatBouton == HIGH) {
        displayMode = (displayMode + 1) % 5; // Changer de mode d'affichage à chaque appui sur le bouton  
        ActivationBouton = maintenant; // Mettre à jour le moment de la dernière activation
    }
    etatPrecedentBouton = etatBouton;

    unsigned long tempsEcoule = maintenant - ActivationBouton;  // Calculer le temps écoulé depuis la dernière activation du bouton
    if (tempsEcoule >= Inactivite) { // Vérifier si le bouton a été inactif pendant 10 secondes
      if (maintenant - BasculeAutoBouton >= IntervalleBasculeAutoBouton) { //vérifier si 3 secondes se sont écoulées depuis la dernière bascule automatique          
        displayMode = (displayMode + 1) % 4; // Changer de mode d'affichage          
        BasculeAutoBouton = maintenant; // Mettre à jour le temps de la dernière bascule automatique
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
        if (humiditeSol < 60 && !ActivationEnCours) {
          activerBulleur(HIGH); // Activer le bulleur et l'arrosage
          activerArrosage(HIGH);
          ActivationHumidite = maintenant;
          ActivationEnCours = true;
        }
        if (ActivationEnCours && maintenant - ActivationHumidite >= ActivationArrosage) {
          activerBulleur(LOW); // Désactiver le bulleur et l'arrosage après 5 minutes
          activerArrosage(LOW);
          Desactivation = maintenant;
          ActivationEnCours = false;
        }
        if (!ActivationEnCours && maintenant - Desactivation >= InactiviteMin) {
        ActivationHumidite = 0; // Réinitialiser les variables pour permettre une nouvelle activation
        Desactivation = 0;
        } 
          
      // Contrôle du chauffage
        if (temperature < 5) {
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
       if (maintenant - dernierEnvoiBluetooth >= intervalleEnvoiBluetooth) {
        // Envoyer le type de donnée actuel via Bluetooth
        envoyerDonneeBluetooth(typeDonneeEnCours);     
        // Passer au type de donnée suivant
        typeDonneeEnCours = (typeDonneeEnCours + 1) % 4; // 4 types de données au total    
        dernierEnvoiBluetooth = maintenant; // Mettre à jour le dernier moment d'envoi
    }

      switch (displayMode) {
        case 0: // Afficher la température
            afficherTemperature();           
            break;
        case 1: // Afficher l'humidité de l'air
            afficherHumiditeAir();           
            break;
        case 2: // Afficher l'humidité du sol
            afficherHumiditeSol();          
            break;
        case 3: // Afficher la luminosité
            afficherLuminosite();
            break;
      } 
    }
}


void envoyerDonneeBluetooth(int typeDonnee) {
    float valeur = 0;
    String libelle = "";

    // Sélectionner le type de donnée à envoyer en fonction du compteur
    switch (typeDonnee) {
        case 0: // température
            valeur = dht.readTemperature();
            libelle = "T ";
            break;
        case 1: // humidité de l'air
            valeur = dht.readHumidity();
            libelle = "Ha ";
            break;
        case 2: // humidité du sol
            valeur = lireHumiditeSol();
            libelle = "Hs ";
            break;
        case 3: // luminosité
            valeur = lireLuminosite();
            libelle = "L ";
            break;
        case 4: // Qualité de l'air
            valeur = sgp.eCO2
            libelle = "Q"
        default:
            break;
    }

    // Construire le message à envoyer
    String message = libelle + String(valeur);

    // Envoyer le message via Bluetooth
    BTserial.println(message);

    // Afficher le message envoyé sur la console série (pour le débogage)
    Serial.print("Sending Bluetooth data: ");
    Serial.println(message);
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
    const float hottest = 35.0;  // Température la plus chaude (rouge)

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

void allumerLED(Adafruit_NeoPixel &strip, uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, strip.Color(r, g, b));
    }
    strip.show();
}

// Fonction pour éteindre toutes les LED
void eteindreLED(Adafruit_NeoPixel &strip) {
    for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
}

void afficherHumiditeAir() {
    float humiditeair = dht.readHumidity();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Humidite Air:");
    lcd.setCursor(0, 1);
    lcd.print(humiditeair);
    lcd.print("%");
    Serial.print("Humidite Air: ");
    Serial.print(humiditeair);
    Serial.println("%");
}

void afficherHumiditeSol() {
    float humiditesol = lireHumiditeSol();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Humidite Sol:");
    lcd.setCursor(0, 1);
    lcd.print(humiditesol);
    lcd.print("%");
    Serial.print("Humidite Sol: ");
    Serial.print(humiditesol);
    Serial.println("%");
}
float lireHumiditeSol() {
    int valeurCapteur = analogRead(A2);
    float humiditeSol = map(valeurCapteur, 0, 700, 0, 100);
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
    digitalWrite(bulleurPin, VitesseBulleur);
}

void activerArrosage(int VitesseArrosage) { 
    digitalWrite(arrosagePin, VitesseArrosage);
}

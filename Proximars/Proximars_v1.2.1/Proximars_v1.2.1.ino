// Code réalisé par Victor Simonet Alves Santos avec la collaboration du Lycée Déodat de Séverac 
// mail: victorcaue@hotmail.fr
// Première version officielle du code arduino pour la serre Proximars

#include <Wire.h>
#include <rgb_lcd.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <SoftwareSerial.h>

// Déclaration des objets et des variables globales
rgb_lcd lcd;
DHT dht(A0, DHT22);
Adafruit_TSL2561_Unified tsl(0x29);
SoftwareSerial BTserial(4, 5); // RX sur 4, TX sur 5 pour le module Bluetooth
#define bouton 2 // Bouton sur la broche 2
#define bulleurPin 3 // Broche pour le contrôle du bulleur
#define arrosagePin 6 // Broche pour le contrôle de l'arrosage
#define eclairagePin 7 // Broche pour le contrôle de l'éclairage
#define chauffagePin 8 // Broche pour le contrôle du chauffage
#define ventilateurPin 9 // Broche pour le contrôle du ventilateur

unsigned long derniereActivation = 0;
unsigned long derniereBasculeAuto = 0;
unsigned long intervalleInactivite = 10000; // 10 secondes d'inactivité
unsigned long intervalleBasculeAuto = 3000; // 3 secondes d'intervalle pour la bascule automatique

int displayMode = 0; // Mode d'affichage initial : 0 pour température, 1 pour humidité air, 2 pour humidité sol, 3 pour luminosité

byte DEGREE_SYMBOL = 0;
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
    Serial.begin(9600); // Initialiser la connexion série
    BTserial.begin(9600);   // Initialiser la communication série pour le module Bluetooth
    pinMode(bouton, INPUT); // Définir la broche du bouton comme une entrée
    pinMode(bulleurPin, OUTPUT); // Définir la broche du bulleur comme une sortie
    pinMode(arrosagePin, OUTPUT); // Définir la broche de l'arrosage comme une sortie
    pinMode(eclairagePin, OUTPUT); // Définir la broche de l'éclairage comme une sortie
    pinMode(chauffagePin, OUTPUT); // Définir la broche du chauffage comme une sortie
    pinMode(ventilateurPin, OUTPUT); // Définir la broche du ventilateur comme une sortie
    lcd.begin(16, 2);    // Initialiser l'écran LCD - 2 lignes avec 16 caractères chacune
    lcd.createChar(DEGREE_SYMBOL, degree); // Enregistrer le symbole "°" sur l'écran LCD
    dht.begin(); // Initialiser le capteur DHT
    if (!tsl.begin()) {
        Serial.println("Capteur TSL2561 non trouvé. Vérifiez les connexions !");
        while (1);
    }
    tsl.enableAutoRange(true);  // Activer la plage automatique pour adapter la gamme de mesure à la luminosité
    tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS); // Choisir un temps d'intégration court pour des mesures rapides
}

void loop() {
    unsigned long maintenant = millis(); // Temps actuel en millisecondes

    // Lecture de l'état actuel du bouton
    int etatBouton = digitalRead(bouton);

    // Vérifier si le bouton est appuyé (transition LOW à HIGH)
    if (etatBouton == HIGH && derniereActivation == 0) {
        // Changer de mode d'affichage à chaque appui sur le bouton
        displayMode = (displayMode + 1) % 4;
        // Mettre à jour le temps de la dernière activation
        derniereActivation = maintenant;
    }

    // Réinitialiser le temps de la dernière activation si le bouton est relâché
    if (etatBouton == LOW) {
        derniereActivation = 0;
    }

    // Contrôler le bulleur en fonction de l'humidité du sol
    float humiditeSol = lireHumiditeSol();
    if (humiditeSol < 80) {
        activerBulleur(255); // Activer le bulleur à pleine vitesse
        activerArrosage(); // Activer l'arrosage pendant 5 minutes
    } else {
        activerBulleur(0); // Désactiver le bulleur si l'humidité du sol est suffisante
    }

    // Contrôler l'éclairage en fonction de la luminosité
    float luminosite = lireLuminosite();
    if (luminosite < 100) { // Ajustez cette valeur selon vos besoins
        digitalWrite(eclairagePin, HIGH); // Allumer l'éclairage si la luminosité est faible
    } else {
        digitalWrite(eclairagePin, LOW); // Éteindre l'éclairage si la luminosité est suffisante
    }

    // Contrôler le chauffage en fonction de la température
    float temperature = dht.readTemperature();
    if (temperature < 20) {
        digitalWrite(chauffagePin, HIGH); // Allumer le chauffage si la température est inférieure à 20°C
    } else {
        digitalWrite(chauffagePin, LOW); // Éteindre le chauffage si la température est suffisante
    }

    // Contrôler le ventilateur en fonction de la température
    if (temperature > 25) {
        digitalWrite(ventilateurPin, HIGH); // Allumer le ventilateur si la température est supérieure à 25°C
    } else {
        digitalWrite(ventilateurPin, LOW); // Éteindre le ventilateur si la température est suffisante
    }

    // Lire les données des capteurs et afficher/envoyer en fonction du mode d'affichage
    switch (displayMode) {
        case 0: // Afficher la température
            afficherTemperature();
            BTserial.println("temperature=" + String(temperature));
            break;
        case 1: // Afficher l'humidité de l'air
            afficherHumiditeAir();
            BTserial.println("humiditeair=" + String(dht.readHumidity()));
            break;
        case 2: // Afficher l'humidité du sol
            afficherHumiditeSol();
            BTserial.println("humiditesol=" + String(humiditeSol));
            break;
        case 3: // Afficher la luminosité
            afficherLuminosite();
            BTserial.println("luminosite=" + String(luminosite));
            break;
    }

    delay(100); // Attente courte pour éviter les rebonds du bouton
}

void activerBulleur(int vitesse) {
    analogWrite(bulleurPin, vitesse); // Contrôler le bulleur avec une certaine vitesse (0-255)
}

void activerArrosage() {
    digitalWrite(arrosagePin, HIGH); // Activer l'arrosage
    delay(300000); // 5 minutes (300000 ms)
    digitalWrite(arrosagePin, LOW); // Désactiver l'arrosage après 5 minutes
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

float lireHumiditeSol() {
    int valeurCapteur = analogRead(A1);
    float humiditeSol = map(valeurCapteur, 0, 725, 0, 100);
    return humiditeSol;
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

/*
 * MicroCoaster - Module Launch Track ESP32
 *
 * Zone de lancement par entraînement mécanique : un moteur fait tourner une
 * poulie, la poulie entraîne une courroie dentée, et un taquet solidaire de la
 * courroie accroche le train pour l'accélérer avant de le relâcher.
 *
 * Auteurs: CyberSpaceRS, Yamakajump
 * Version: 0.1.0
 */

#include <Arduino.h>          // Bibliothèque principale Arduino pour ESP32
#include <AyresWiFiManager.h> // Gestionnaire WiFi avec portail captif
#include <WebSocketsClient.h> // Client WebSocket pour communication serveur
#include <ArduinoJson.h>      // Manipulation des données JSON

// ========================================
// CONFIGURATION MATERIELLE
// ========================================

// Moteur d'entraînement de la poulie, via pont en H.
#define MOTOR_PWM_PIN 25
#define MOTOR_DIR_PIN 26
#define MOTOR_ENABLE_PIN 27

// Codeur sur l'arbre de la poulie. Donne la vitesse réelle de la courroie et
// permet de savoir où se trouve le taquet sur son parcours.
#define ENCODER_A_PIN 34
#define ENCODER_B_PIN 35

// Capteur de position de repos du taquet. Entre deux lancements, le taquet doit
// revenir exactement là, sinon il n'accrochera pas le train suivant.
#define CATCH_HOME_PIN 32

// Présence du train en zone, et sortie effective après lancement.
#define SENSOR_TRAIN_PIN 33
#define SENSOR_EXIT_PIN 36

// Signalisation d'état.
#define LED_READY_PIN 2
#define LED_FAULT_PIN 4

// ========================================
// PARAMETRES DE LANCEMENT
// ========================================

// Vitesse de courroie visée en fin d'accélération, en pourcentage du rapport
// cyclique moteur.
static const uint8_t LAUNCH_SPEED_PERCENT = 85;

// Durée de la rampe d'accélération, en millisecondes. C'est elle qui donne son
// caractère au lancement : courte, la poussée est brutale ; longue, elle est
// progressive. Trop courte, le taquet patine ou arrache.
static const uint16_t RAMP_UP_MS = 700;

// Décélération de la courroie une fois le train relâché. La courroie ne doit
// pas s'arrêter net : le taquet reviendrait en butée.
static const uint16_t RAMP_DOWN_MS = 900;

// Vitesse de retour du taquet à sa position de repos.
static const uint8_t RETURN_SPEED_PERCENT = 30;

// Écart toléré entre la vitesse demandée et la vitesse mesurée au codeur.
// Au-delà, c'est que le taquet patine sur le train.
static const uint8_t SLIP_TOLERANCE_PERCENT = 15;

// Délais de garde.
static const uint16_t LAUNCH_TIMEOUT_MS = 4000;
static const uint16_t RETURN_TIMEOUT_MS = 6000;

// ========================================
// ETATS
// ========================================

enum LaunchState {
  STATE_HOMING,    // Recherche de la position de repos du taquet
  STATE_IDLE,      // Taquet au repos, aucun train en zone
  STATE_LOADED,    // Train présent et accroché, en attente d'autorisation
  STATE_LAUNCHING, // Accélération de la courroie en cours
  STATE_RELEASED,  // Train relâché, décélération de la courroie
  STATE_RETURNING, // Retour du taquet vers sa position de repos
  STATE_FAULT      // Patinage, taquet perdu, ou délai dépassé
};

static LaunchState state = STATE_HOMING;

// ========================================
// A IMPLEMENTER
// ========================================
//
// setup()  : moteur en état sûr, interruptions sur les deux voies du codeur,
//            recherche de la position de repos du taquet, portail WiFi, puis
//            connexion WebSocket au serveur.
//
// loop()   : machine à états ci-dessus, asservissement de vitesse sur le
//            codeur, détection de patinage, et télémétrie périodique.
//
// Trois règles qui ne se discutent pas.
//
// Le lancement n'est jamais décidé localement. Le module exécute un ordre du
// contrôleur, qui seul sait si la voie en aval est libre.
//
// Un écart durable entre vitesse demandée et vitesse mesurée signifie que le
// taquet patine sur le train. On coupe : insister use la courroie et peut
// endommager l'accroche.
//
// Le taquet doit être à sa position de repos avant d'accepter un nouveau
// train. Sans cela, il accroche au mauvais endroit, ou pas du tout.

void setup() {
  Serial.begin(115200);
}

void loop() {
}

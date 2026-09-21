/*
 * MicroCoaster - Module Launch Track ESP32
 *
 * Zone de lancement : pilotage des bobines de propulsion, rampe de puissance
 * et verrouillage du train tant que la séquence n'est pas autorisée.
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

// Sorties de puissance. Chaque bobine est commandée par son propre MOSFET :
// la rampe s'obtient en les enclenchant l'une après l'autre, pas en modulant.
#define COIL_1_PIN 25
#define COIL_2_PIN 26
#define COIL_3_PIN 27
#define COIL_4_PIN 14

// Frein de maintien. Actif tant que le lancement n'est pas autorisé : c'est lui
// qui garantit qu'un train ne part pas sur un ordre perdu ou une coupure réseau.
#define HOLD_BRAKE_PIN 12

// Détection de présence en entrée de zone et de sortie effective du train.
#define SENSOR_ENTRY_PIN 34
#define SENSOR_EXIT_PIN 35

// Signalisation d'état.
#define LED_READY_PIN 2
#define LED_FAULT_PIN 4

// ========================================
// PARAMETRES DE LANCEMENT
// ========================================

// Délai entre l'enclenchement de deux bobines consécutives, en millisecondes.
// Plus il est court, plus l'accélération est forte.
static const uint16_t COIL_STEP_MS = 60;

// Durée maximale d'alimentation d'une bobine. Au-delà, on coupe : une bobine
// laissée sous tension chauffe et rien ne justifie de la maintenir si le train
// n'est pas sorti de la zone.
static const uint16_t COIL_MAX_ON_MS = 400;

// Temps au-delà duquel on considère que le train n'est jamais sorti.
static const uint16_t LAUNCH_TIMEOUT_MS = 3000;

// ========================================
// ETATS
// ========================================

enum LaunchState {
  STATE_IDLE,      // Aucun train en zone
  STATE_LOADED,    // Train présent, frein serré, en attente d'autorisation
  STATE_LAUNCHING, // Séquence de bobines en cours
  STATE_FAULT      // Anomalie : sortie non constatée, capteur incohérent
};

static LaunchState state = STATE_IDLE;

// ========================================
// A IMPLEMENTER
// ========================================
//
// setup()  : initialisation des sorties en état sûr (frein serré, bobines
//            coupées), portail WiFi, puis connexion WebSocket au serveur.
//
// loop()   : lecture des capteurs, machine à états ci-dessus, et remontée de
//            télémétrie périodique.
//
// Règle qui ne se discute pas : toute perte de liaison WebSocket ramène le
// module en STATE_LOADED avec le frein serré. Le lancement n'est jamais
// déclenché localement, il est toujours autorisé par le contrôleur.

void setup() {
  Serial.begin(115200);
}

void loop() {
}

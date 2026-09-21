<div align="center">

<img src="docs/banniere.png" alt="Launch Track, zone de lancement" width="100%">

</div>

La zone de lancement accélère le train sur quelques dizaines de centimètres, au lieu de le hisser en haut d'une montée. Un moteur entraîne une poulie, la poulie fait tourner une courroie dentée, et un taquet solidaire de cette courroie vient accrocher le train, l'emmène en accélérant, puis le relâche.

Le module pilote ce moteur, mesure la vitesse réelle de la courroie au codeur, et ramène le taquet à sa position de repos entre deux lancements.

Comme les autres modules, il se configure au premier démarrage par portail captif, puis rejoint le serveur en WebSocket.

**Version 0.1.0**

<img src="docs/sections/s01.png" alt="01 Principe" width="100%">

Tout tourne autour d'une question : le taquet est-il là où on croit, et entraîne-t-il vraiment le train ?

Un codeur sur l'arbre de la poulie répond aux deux. Il donne la vitesse de la courroie, ce qui permet de suivre la rampe d'accélération. Et il donne la position du taquet sur son parcours, ce qui permet de le ramener au repos après chaque lancement.

<img src="docs/schemas/etats.png" alt="HOMING : le taquet cherche sa position de repos au démarrage. IDLE : taquet au repos, zone libre. LOADED : train accroché, en attente d'autorisation. LAUNCHING : rampe d'accélération. RELEASED : train parti, la courroie décélère. RETURNING : retour du taquet au repos, puis cycle complet vers IDLE. FAULT : patinage, taquet introuvable ou délai dépassé, accessible depuis n'importe quel état." width="100%">

Le taquet ne revient pas en marche arrière brutale : la courroie décélère sur `RAMP_DOWN_MS`, puis repart lentement à `RETURN_SPEED_PERCENT` jusqu'au capteur de repos.

<img src="docs/sections/s02.png" alt="02 Sécurité" width="100%">

**Aucun lancement n'est décidé localement.** Le module exécute un ordre du contrôleur, qui seul sait si la voie en aval est dégagée.

**Le taquet doit être au repos avant d'accepter un train.** S'il ne l'est pas, il accrochera au mauvais endroit, ou pas du tout. C'est la raison d'être de l'état `HOMING` au démarrage.

**Deux délais bornent l'opération.** `LAUNCH_TIMEOUT_MS` déclare le défaut si la sortie n'est jamais constatée, `RETURN_TIMEOUT_MS` si le taquet ne retrouve pas sa position de repos.

**Le patinage coupe tout.** Si la vitesse mesurée au codeur s'écarte durablement de la consigne au-delà de `SLIP_TOLERANCE_PERCENT`, c'est que le taquet glisse sur le train au lieu de l'entraîner. Insister use la courroie, arrondit les dents, et finit par abîmer la pièce d'accroche du train.

<img src="docs/sections/s03.png" alt="03 Matériel" width="100%">

<img src="docs/schemas/brochage.png" alt="Sorties : GPIO 25 moteur PWM pour la vitesse de la courroie, GPIO 26 sens du moteur, GPIO 27 activation du moteur, GPIO 2 LED prêt pour l'état LOADED, GPIO 4 LED défaut pour l'état FAULT. Entrées : GPIO 34 codeur voie A pour la vitesse et la position, GPIO 35 codeur voie B pour le sens de rotation, GPIO 32 capteur de repos du taquet, GPIO 33 capteur de présence du train, GPIO 36 capteur de sortie." width="100%">

<img src="docs/sections/s04.png" alt="04 Réglages" width="100%">

<img src="docs/schemas/reglages.png" alt="LAUNCH_SPEED_PERCENT : vitesse visée en fin d'accélération. RAMP_UP_MS : caractère du lancement, court et brutal ou long et progressif. RAMP_DOWN_MS : décélération après la relâche. RETURN_SPEED_PERCENT : vitesse de retour au repos. SLIP_TOLERANCE_PERCENT : écart toléré avant de déclarer le patinage. LAUNCH_TIMEOUT_MS : délai au-delà duquel une sortie jamais constatée devient un défaut." width="100%">

Une rampe trop courte fait patiner le taquet ou force sur l'accroche. C'est le premier réglage à revoir si le lancement manque de tenue.

<img src="docs/sections/s05.png" alt="05 Mise en service" width="100%">

Copiez d'abord [`include/env.h.example`](include/env.h.example) en `include/env.h` et renseignez-le : identifiants du portail de secours, identité du module et son secret. Ce fichier n'est pas versionné, et sans lui le firmware ne compile pas.

Nécessite [PlatformIO](https://platformio.org/) dans Visual Studio Code.

```bash
pio run                  # compilation
pio run -t upload        # téléversement du firmware
pio run -t uploadfs      # téléversement du portail vers LittleFS
pio device monitor       # console série, 115200 bauds
```

1. Alimenter le module. Il crée un point d'accès WiFi.
2. S'y connecter et ouvrir `http://192.168.4.1`.
3. Renseigner le réseau de destination.
4. Le module redémarre, rejoint le réseau et s'annonce auprès du serveur.

Les identifiants restent en mémoire du module, jamais dans le dépôt.

<img src="docs/sections/s06.png" alt="06 Écosystème" width="100%">

Le brochage, les paramètres et la machine à états sont posés dans `src/main.cpp`. Restent à écrire la lecture du codeur sur interruption, l'asservissement de vitesse, la détection de patinage et la télémétrie.

Le socle commun à tous les modules est le [WiFi Manager](https://github.com/Microcoaster/MicroCoaster_WifiManager). L'autre manière de donner son énergie au train est le [Lift Hill](https://github.com/Microcoaster/Lift-Hill). Le pilotage se fait depuis la [WebApp](https://github.com/Microcoaster/MicroCoasterWebApp).

---

<sub>MicroCoaster · Auteur : Cybertrist</sub>

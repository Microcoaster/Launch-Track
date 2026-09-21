<div align="center">

<img src="docs/banniere.png" alt="Launch Track, zone de lancement" width="100%">

</div>

La zone de lancement accélère le train sur quelques dizaines de centimètres, au lieu de le hisser en haut d'une montée. Le module pilote une série de bobines de propulsion, les enclenche dans l'ordre pour construire une rampe de puissance, et ne relâche le frein de maintien qu'une fois la séquence autorisée par le contrôleur.

Comme les autres modules, il se configure au premier démarrage par portail captif, puis rejoint le serveur en WebSocket.

## Principe

Chaque bobine a son propre MOSFET. L'accélération ne vient pas d'une modulation de puissance mais du **décalage entre les enclenchements** : plus l'intervalle est court, plus la poussée est forte. Le réglage se fait dans `COIL_STEP_MS`.

```
Train détecté      frein serré, état LOADED
Ordre reçu         frein relâché, bobine 1, 2, 3, 4 en cascade
Sortie constatée   bobines coupées, retour en IDLE
Pas de sortie      bobines coupées, état FAULT
```

## Sécurité

Trois garde-fous, dans cet ordre d'importance.

**Le frein de maintien est le défaut.** Au repos, à la mise sous tension et à toute anomalie, il est serré. Le train ne part que sur une action explicite.

**Aucun lancement n'est décidé localement.** Le module exécute, il ne juge pas. C'est le contrôleur qui autorise, après avoir vérifié que la voie en aval est libre.

**Une perte de liaison ramène en état sûr.** Si le WebSocket tombe, le module repasse en `LOADED`, frein serré, et attend le retour du serveur.

À cela s'ajoutent deux limites matérielles : `COIL_MAX_ON_MS` coupe toute bobine restée alimentée trop longtemps, et `LAUNCH_TIMEOUT_MS` déclare le défaut si la sortie n'est jamais constatée.

## Matériel

| Élément | Broche | Rôle |
|:--|:--|:--|
| Bobines 1 à 4 | 25, 26, 27, 14 | Propulsion, un MOSFET par bobine |
| Frein de maintien | 12 | Actif au repos |
| Capteur d'entrée | 34 | Présence du train en zone |
| Capteur de sortie | 35 | Sortie effective |
| LED prêt | 2 | État `LOADED` |
| LED défaut | 4 | État `FAULT` |

## Compiler et téléverser

Nécessite [PlatformIO](https://platformio.org/) dans Visual Studio Code.

```bash
pio run                  # compilation
pio run -t upload        # téléversement du firmware
pio run -t uploadfs      # téléversement du portail vers LittleFS
pio device monitor       # console série, 115200 bauds
```

## Première mise en service

1. Alimenter le module. Il crée un point d'accès WiFi.
2. S'y connecter et ouvrir `http://192.168.4.1`.
3. Renseigner le réseau de destination.
4. Le module redémarre, rejoint le réseau et s'annonce auprès du serveur.

Les identifiants restent en mémoire du module, jamais dans le dépôt.

## État

Version `0.1.0`. Le squelette, le brochage et la machine à états sont posés dans `src/main.cpp`. Restent à écrire la séquence de bobines, l'asservissement du frein et la remontée de télémétrie.

---

<sub>MicroCoaster · Auteurs : CyberSpaceRS, Yamakajump</sub>

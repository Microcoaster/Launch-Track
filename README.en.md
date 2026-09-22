<div align="center">

<p>
  <a href="README.md"><img src="docs/langues/fr-off.png" alt="Lire cette page en français" width="150" /></a>
  <img src="docs/langues/en-on.png" alt="English, page shown" width="150" />
</p>

<img src="docs/en/banniere.png" alt="Launch Track, the launch section" width="100%">

</div>

The launch section accelerates the train over a few tens of centimetres, instead of hauling it to the top of a hill. A motor drives a pulley, the pulley turns a toothed belt, and a catch car fixed to that belt takes hold of the train, carries it away while accelerating, then lets go.

The module drives that motor, measures the real belt speed at the encoder, and brings the catch car back to its rest position between two launches.

Like the other modules, it is configured on first boot through a captive portal, then joins the server over WebSocket.

**Version 0.1.0**

<img src="docs/en/sections/s01.png" alt="01 How it works" width="100%">

Everything turns on one question: is the catch car where we think it is, and is it really carrying the train?

An encoder on the pulley shaft answers both. It gives the belt speed, which is what lets the acceleration ramp be followed. And it gives the catch car's position along its travel, which is what lets it be brought back to rest after every launch.

<img src="docs/en/schemas/etats.png" alt="HOMING: the catch car hunts for its rest position at start-up. IDLE: catch car at rest, zone clear. LOADED: train latched, waiting for clearance. LAUNCHING: acceleration ramp. RELEASED: train gone, the belt slows down. RETURNING: the catch car goes back to rest, then a full cycle to IDLE. FAULT: slip, catch car not found or timeout exceeded, reachable from any state." width="100%">

The catch car does not slam back in reverse: the belt slows down over `RAMP_DOWN_MS`, then creeps back at `RETURN_SPEED_PERCENT` until the rest sensor.

<img src="docs/en/sections/s02.png" alt="02 Safety" width="100%">

**No launch is decided locally.** The module carries out an order from the controller, which alone knows whether the track downstream is clear.

**The catch car has to be at rest before a train is accepted.** If it is not, it will latch in the wrong place, or not at all. That is the whole reason for the `HOMING` state at start-up.

**Two timeouts bound the operation.** `LAUNCH_TIMEOUT_MS` declares a fault if the exit is never seen, `RETURN_TIMEOUT_MS` if the catch car never finds its rest position again.

**Slip cuts everything.** If the speed measured at the encoder drifts lastingly from the command by more than `SLIP_TOLERANCE_PERCENT`, the catch car is sliding along the train instead of carrying it. Pushing on wears the belt, rounds the teeth, and ends up damaging the train's latching piece.

<img src="docs/en/sections/s03.png" alt="03 Hardware" width="100%">

<img src="docs/en/schemas/brochage.png" alt="Outputs: GPIO 25 motor PWM for belt speed, GPIO 26 motor direction, GPIO 27 motor enable, GPIO 2 ready LED for the LOADED state, GPIO 4 fault LED for the FAULT state. Inputs: GPIO 34 encoder channel A for speed and position, GPIO 35 encoder channel B for direction of rotation, GPIO 32 catch car rest sensor, GPIO 33 train presence sensor, GPIO 36 exit sensor." width="100%">

<img src="docs/en/sections/s04.png" alt="04 Settings" width="100%">

<img src="docs/en/schemas/reglages.png" alt="LAUNCH_SPEED_PERCENT: target speed at the end of the ramp. RAMP_UP_MS: the character of the launch, short and brutal or long and gradual. RAMP_DOWN_MS: deceleration after the release. RETURN_SPEED_PERCENT: return speed to rest. SLIP_TOLERANCE_PERCENT: the gap tolerated before calling it slip. LAUNCH_TIMEOUT_MS: the delay past which an exit never seen becomes a fault." width="100%">

Too short a ramp makes the catch car slip, or strains the latch. It is the first setting to revisit if the launch lacks bite.

<img src="docs/en/sections/s05.png" alt="05 Bringing it up" width="100%">

First copy [`include/env.h.example`](include/env.h.example) to `include/env.h` and fill it in: fallback portal credentials, the module identity and its secret. That file is not in git, and without it the firmware does not compile.

Requires [PlatformIO](https://platformio.org/) inside Visual Studio Code.

```bash
pio run                  # build
pio run -t upload        # upload the firmware
pio run -t uploadfs      # upload the portal to LittleFS
pio device monitor       # serial console, 115200 baud
```

1. Power the module. It creates a WiFi access point.
2. Connect to it and open `http://192.168.4.1`.
3. Enter the target network.
4. The module reboots, joins the network and announces itself to the server.

The credentials stay in the module's memory, never in the repository.

<img src="docs/en/sections/s06.png" alt="06 Ecosystem" width="100%">

The pinout, the parameters and the state machine are laid down in `src/main.cpp`. What remains to be written is reading the encoder on interrupt, the speed loop, slip detection and telemetry.

The common base for every module is the [WiFi Manager](https://github.com/Microcoaster/MicroCoaster_WifiManager). The other way of giving the train its energy is the [Lift Hill](https://github.com/Microcoaster/Lift-Hill). The driving is done from the [WebApp](https://github.com/Microcoaster/MicroCoasterWebApp).

---

<sub>MicroCoaster · Author: Cybertrist</sub>

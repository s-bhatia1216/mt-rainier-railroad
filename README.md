# Mt. Rainier Railroad — Bench 5

MAE 412 capstone project simulating a segment of the Mount Rainier Scenic Railroad. The board autonomously manages track switching, obstacle detection, and train control in response to fallen tree hazards.

---

## System Overview

```
Vector Board Arduino
      │
      ├── SoftwareSerial (pins 8/9) ──► Signal Sensor Board (ACIA)
      │                                  └── STrain / NTrain occupancy byte
      │
      └── Hardware Serial (FTDI) ──► iMac (bridge_direct.py) ──► DCC Command Station
                                                                   (Arduino Uno + motor shield)
```

The Arduino polls the signal sensor board every ~200 ms. When a train is detected on the North block (`NTrain != 0`), the switching sequence activates and DCC speed/direction commands are forwarded to the Command Station through the Python passthrough.

---

## Hardware

| Component | Purpose |
|---|---|
| Vector Board (Arduino-compatible) | Main controller |
| VL53L0X (×2, I2C addrs 0x30/0x31) | Time-of-flight obstacle detection (inner/outer tracks) |
| Servos (pins 10, 11) | Animated scenery (fallen trees) |
| Switch relays (×4) | Track switch direction + trigger |
| Track power relays (A1, A2) | Cut track power on obstacle detection |
| SoftwareSerial (pins 8, 9) | ACIA communication with signal sensor board |
| FTDI cable | DCC command passthrough to desktop |

---

## Software

### `final_hooray/final_hooray.ino` — primary flight script
- Polls ACIA for NTrain/STrain occupancy; on train arrival, sets `switchingActive = true`
- Runs three timed switch patterns (60 s → 30 s → 30 s, cycling) once switching is active
- Reads dual ToF sensors; cuts track relay power if obstacle threshold is crossed
- Drives two servos on randomized intervals to animate scenery

### `DCC/bridge_direct.py` — Python passthrough (runs on iMac)
- Bridges the FTDI serial port (Vector Board) to the DCC Command Station serial port
- Forwards raw bytes transparently; configure `COM3` / `COM4` to match your machine's port assignments

### `DCC/STrainNTrain_DCC.asm` / `.bin`
- Assembly firmware for the signal sensor board (ACIA)
- Returns a single byte per poll: `high nibble = STrain`, `low nibble = NTrain`

---

## Switch Patterns

| Pattern | SW1 | SW2 | SW3 | SW4 | Duration |
|---|---|---|---|---|---|
| 1 | LOW | LOW | LOW | HIGH | 60 s |
| 2 | HIGH | HIGH | HIGH | HIGH | 30 s |
| 3 | HIGH | LOW | HIGH | LOW | 30 s |

Patterns cycle continuously after train arrival is detected.

---

## Electrical Schematics

All schematics are in [`electrical_schematics/`](electrical_schematics/). Fusion 360 source files are in [`electrical_schematics/Fusion_Electronic_Files/`](electrical_schematics/Fusion_Electronic_Files/).

---

## Repo Structure

```
final_hooray/          ← active flight script
DCC/
  bridge_direct.py     ← iMac passthrough script
  STrainNTrain_DCC.*   ← signal sensor board firmware
  Arduino/             ← development/archived sketches
electrical_schematics/ ← PNG exports + Fusion/KiCad source files
physical_design/       ← photos of built hardware
```

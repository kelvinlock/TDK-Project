# NEMA17 Stepper‑Motor Linear Actuator Demo (5 cm Up–Down)

*Figure 1 – Typical 42 mm (NEMA17) bipolar stepper motor.* ([adafruit.com](https://www.adafruit.com/product/324))

This short guide shows how to build a lead‑screw linear actuator that moves **5 cm up and back to its starting point** using an **Arduino Uno**, a **NEMA17 stepper motor**, and the **BTS7960 43 A dual H‑bridge driver board** that appears in your Excel parts list. It covers the motor’s working principle, the high‑current driver board, wiring, motion maths, and a fully‑commented sketch (explained in Traditional Chinese).

---

## Bill of Materials

| # | Component                            | Key Spec / Link                                                    | Qty       |
| - | ------------------------------------ | ------------------------------------------------------------------ | --------- |
| 1 | NEMA17 stepper motor                 | 200 steps/rev, 1.8 °/step, ≈2 kg·cm holding torque                 | 1         |
| 2 | Lead screw & nut                     | Pitch = **0.8 cm/rev** (8 mm per rev) ‑ matches spreadsheet remark | 1         |
| 3 | BTS7960 dual H‑bridge driver         | 6 – 27 V, 43 A peak, dual‑channel                                  | 1         |
| 4 | Arduino Uno (or Nano)                | 5 V logic                                                          | 1         |
| 5 | 12 V DC power supply                 | ≥ 3 A                                                              | 1         |
| 6 | Wires, breadboard, mounting hardware | —                                                                  | as needed |

The **BTS7960 driver** is listed as *“PWM 12 V 24 V 12 A 雙路直流電機驅動模塊板 ‑ BTS7960”* in the uploaded sheet. Although marketed for brushed-DC motors, its two half-bridges can be used together to drive one bipolar stepper coil-per-bridge at up to 25 kHz of PWM. ([instructables.com](https://www.instructables.com/Motor-Driver-BTS7960-43A/))

---

## How Does a Stepper Motor Work?

*Figure 2 – Stator, rotor and windings inside a stepper motor.* ([monolithicpower.com](https://www.monolithicpower.com/en/learning/resources/stepper-motors-basics-types-uses?srsltid=AfmBOorNQ8DTLO_cXx15KKha2g8qxopaDbdsr7ykNs6yKkr6i5-H8Ryx), [media.monolithicpower.com](https://media.monolithicpower.com/wysiwyg/1_11.png))

A stepper’s rotor locks onto the moving magnetic field created by sequentially energising the stator windings. Driving **Coil A → Coil B → Coil C …** one after another makes the shaft rotate in fixed **steps** (1.8 ° here), giving open‑loop position control. ([monolithicpower.com](https://www.monolithicpower.com/en/learning/resources/stepper-motors-basics-types-uses?srsltid=AfmBOorNQ8DTLO_cXx15KKha2g8qxopaDbdsr7ykNs6yKkr6i5-H8Ryx))

*Figure 3 – Energising different coils advances the rotor in precise increments.* ([media.monolithicpower.com](https://media.monolithicpower.com/wysiwyg/2_10.png))

---

## Driver Board: BTS7960 (IBT‑2) Dual H‑Bridge

*Figure 4 – High‑current BTS7960 module with hefty heatsink.* ([m.media-amazon.com](https://m.media-amazon.com/images/I/618WxeO2FzL.__AC_SX300_SY300_QL70_FMwebp_.jpg))

* **Logic pins** (5 V‑tolerant):

  * `RPWM` / `LPWM` – PWM inputs for right/left half‑bridge
  * `R_EN` / `L_EN` – enable lines (HIGH = on)
* **Motor outputs:** `B+` / `B‑` for motor coil.

Because a bipolar stepper has two coils, **channel A controls Coil‑1 and channel B controls Coil‑2**. By driving the channels with complementary PWM, you can reproduce the full‑step waveforms shown above. However, for simplicity and higher stepping rates, many makers prefer a tiny *step/dir* driver such as **A4988** or **DRV8825**. The sketch below assumes that approach while still presenting the BTS7960 option.  ([instructables.com](https://www.instructables.com/Motor-Driver-BTS7960-43A/))

---

## Electrical Connections (Step/Dir Driver)

```
Arduino        A4988 / DRV8825       Stepper Motor
---------      ------------------    --------------
D3  (DIR)  →   DIR                  |
D2  (STEP) →   STEP                 | 4‑wire harness
5V          →  VDD                  | (Red, Green, Yellow, Blue)
GND         →  GND                  |
VMOT (12 V)  → Driver VMOT (with capacitors)
```

*If you decide to stay with the BTS7960, wire each half‑bridge to one coil and drive it with PWM waveforms instead of STEP pulses.*

---

## Motion Maths

* Lead screw pitch = **0.8 cm per revolution** (from spreadsheet)
* Motor steps = **200 steps/rev**
* **Linear travel per step**  = 0.8 cm ÷ 200 steps = **0.004 cm/step**
* To move **5 cm**: 5 cm ÷ 0.004 cm/step = **1250 steps**
  (the code computes this automatically).

---

## Arduino Sketch  — 程式碼與說明

```cpp
#define dirPin 2
#define stepPin 3
#define stepsPerRevolution 200 // 一圈200步（每步1.8°）

void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
}

void loop() {
  float distance_cm = 5.0; // 目標位移
  int steps = distance_cm / calculate_cm_per_step();

  // 上升
  digitalWrite(dirPin, HIGH);
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(800);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(800);
  }
  delay(2000);

  // 下降
  digitalWrite(dirPin, LOW);
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(800);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(800);
  }
  delay(2000);
}

float calculate_cm_per_step() {
  const float lead_cm_per_rev = 0.8; // 絲桿導程：0.8 cm/rev
  return lead_cm_per_rev / stepsPerRevolution;
}
```

### 繁體中文說明

1. `dirPin`（第 2 腳）決定馬達轉向：`HIGH` = 順時針（推平台上升），`LOW` = 逆時針（下降）。
2. `stepPin`（第 3 腳）只要偵測到 **上升沿** 就推動馬達「走一步」，因此每次迴圈都需送出 `HIGH → LOW`。
3. `delayMicroseconds(800)` 決定步頻（約 625 Hz），值越小速度越快；注意過快可能失步。
4. `calculate_cm_per_step()` 依導程 (0.8 cm) 與步數 (200) 計算每步位移 0.004 cm，並用來換算 5 cm 對應的 `steps`（1250 步）。
5. 透過「上升 → 暫停 2 秒 → 下降 → 暫停 2 秒」的 `loop()` 可反覆往返一次 5 cm。

---

## Test Procedure

1. Mount the NEMA17 onto the base plate and couple the lead‑screw.
2. Wire the Arduino, driver, and stepper as shown.
3. Upload the sketch and provide a stable 12 V supply to VMOT.
4. Observe the carriage advance ≈ 5 cm upward, pause, then return.

---

## Going Further

* Replace delays with the AccelStepper library to add acceleration ramps for smoother motion.
* Use micro‑stepping (e.g., 1/16) to get 0.00025 cm resolution.
* For heavier loads, keep the BTS7960 and drive coils with complementary PWM via `RPWM/LPWM` pins.

---

### References

* Adafruit product 324 – NEMA17 Stepper Motor ([adafruit.com](https://www.adafruit.com/product/324))
* MPS “Stepper Motors Basics” article ([monolithicpower.com](https://www.monolithicpower.com/en/learning/resources/stepper-motors-basics-types-uses?srsltid=AfmBOorNQ8DTLO_cXx15KKha2g8qxopaDbdsr7ykNs6yKkr6i5-H8Ryx))
* Instructables “Motor Driver BTS7960 43 A” guide ([instructables.com](https://www.instructables.com/Motor-Driver-BTS7960-43A/))

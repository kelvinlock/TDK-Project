String axes[2];
int axes_values[2];

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n'); // 例如輸入：X:132,Y:234

    if (data.startsWith("X:")) {
      int comma_index = data.indexOf(",");

      if (comma_index != -1) {
        axes[0] = data.substring(0, comma_index);           // "X:132"
        axes[1] = data.substring(comma_index + 1);          // "Y:234"

        for (int i = 0; i < 2; i++) {
          axes_values[i] = axes[i].substring(2).toInt();    // 取 "132"、"234"
          axes_values[i] = constrain(axes_values[i], 0, 255);
        }
      }
    }
    delay(1000); // 避免輸出過快
  }
}

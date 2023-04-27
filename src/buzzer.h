#define BUZZER D8

void setBuzzer(bool val) {
    digitalWrite(BUZZER, !val);
}

static unsigned int i = 0;
void setBuzzer(int co2) {
    setBuzzer(co2 >= 1000 && i % 2);
    i++;
}

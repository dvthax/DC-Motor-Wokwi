#include "wokwi-api.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  pin_t IN1;
  pin_t IN2;
  pin_t EN;
  pin_t OUT;
} chip_state_t;

static void motor_update(void *user_data, pin_t pin, uint32_t value);

void chip_init(void) {
  chip_state_t *chip = malloc(sizeof(chip_state_t));

  chip->IN1 = pin_init("IN1", INPUT);
  chip->IN2 = pin_init("IN2", INPUT);
  chip->EN  = pin_init("EN", INPUT);
  chip->OUT = pin_init("OUT", OUTPUT);

  const pin_watch_config_t config = {
    .edge = BOTH,
    .pin_change = motor_update,
    .user_data = chip
  };

  pin_watch(chip->IN1, &config);
  pin_watch(chip->IN2, &config);
  pin_watch(chip->EN,  &config);
}

void motor_update(void *user_data, pin_t pin, uint32_t value) {
  chip_state_t *chip = (chip_state_t*)user_data;

  int in1 = pin_read(chip->IN1);
  int in2 = pin_read(chip->IN2);
  float en = pin_read(chip->EN); // PWM (0.0 → 1.0)

  if (en <= 0) {
    pin_write(chip->OUT, 0);
    printf("⛔ STOP\n");
    return;
  }

  if (in1 == 1 && in2 == 0) {
    pin_write(chip->OUT, en);
    printf("➡️ FORWARD | speed=%.2f\n", en);
  }
  else if (in1 == 0 && in2 == 1) {
    pin_write(chip->OUT, en);
    printf("⬅️ BACKWARD | speed=%.2f\n", en);
  }
  else {
    pin_write(chip->OUT, 0);
    printf("⚠️ BRAKE\n");
  }
}

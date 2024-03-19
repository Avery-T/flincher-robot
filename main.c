#include "globals.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#define TIMEOUT 10000
#define LEFT_SENSOR 4
#define RIGHT_SENSOR 5
#define DEFAULT_SPEED 100
#define EPSILON 20
#define SCALE_DENOM 100

typedef enum STATE { DODGE, SCAN } STATE;
STATE dodge();
STATE scan();
void motor(u08 num, int8_t speed);
void sensorTest();

void main() {
  init();
  motor(0, 0);
  motor(1, 0);

  STATE state = SCAN;
  while (1) {
    switch (state) {
    case SCAN:
      state = scan();
      break;
    case DODGE:
      state = dodge();
      break;

    default:
      break;
    }
  }
  return;
}
/* the robot has two distance sensors and it spins in a circle
   scaning to see if anything aproaches it */
STATE scan() {
  uint8_t right_sensor = analog(RIGHT_SENSOR);
  uint8_t left_sensor = analog(LEFT_SENSOR);
  clear_screen();
  lcd_cursor(0, 0);
  print_string("scanning");
  while (1) {
    right_sensor = analog(RIGHT_SENSOR);
    left_sensor = analog(LEFT_SENSOR);

    if (right_sensor > 100 || left_sensor > 100)
      return DODGE;

    motor(0, 100);
    motor(1, -100);
  }
}

/*if somthing is closer a object is to the sensors the more it moves back, it
 * also tracks the incoming angle and centers itself so the obejct cant sneak up
 * on it */
STATE dodge() {
  clear_screen();
  lcd_cursor(0, 0);
  print_string("im scared");

  uint8_t right_sensor = analog(RIGHT_SENSOR);
  uint8_t left_sensor = analog(LEFT_SENSOR);
  uint32_t timer = 0;
  while (1) {
    right_sensor = analog(RIGHT_SENSOR);
    left_sensor = analog(LEFT_SENSOR);

    if (right_sensor > 100 || left_sensor > 100) {
      motor(0, -(left_sensor - 100));
      motor(1, -(right_sensor - 100));
      timer = 0;
    }

    else if (timer == TIMEOUT)
      return SCAN;

    else {
      motor(0, 0);
      motor(1, 0);
    }
    timer++;
  }
}

void sensorTest() {
  lcd_cursor(0, 0);
  uint8_t output0 = analog(5);
  uint8_t output1 = analog(4);
  while (1) {
    lcd_cursor(0, 0);
    output0 = analog(5);
    output1 = analog(4);
    print_num(output0);
    lcd_cursor(0, 1);
    print_num(output1);
    _delay_ms(100);
    clear_screen();
  }
}

void motor(u08 num, int8_t speed) {
  // u16 mapped_speed = (u16)(speed + 100) * 255 / 200;
  u16 mapped_speed = ((u16)speed * EPSILON + 127 * SCALE_DENOM) / SCALE_DENOM;
  set_servo(num, num ? -mapped_speed : mapped_speed);
}

void print_signed_num(int16_t num) {
  clear_screen();
  lcd_cursor(0, 0);
  if (num < 0) {
    print_string("-");
    lcd_cursor(1, 0);
    print_num(-1 * num);
  } else
    print_num(num);
}

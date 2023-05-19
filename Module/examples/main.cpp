/*
 * Copyright (C) 2023 CNPEM (cnpem.br)
 * Author: Guilherme Ricioli <guilherme.ricioli@lnls.br>
 */

#include "mbed.h"

#include "CounterModule.hpp"

#define COUNTERMODULE_STACK_SIZE 2048

/* Statically allocated stack */
static unsigned char stack[COUNTERMODULE_STACK_SIZE];

int main() {
  CounterModule count_mod(osPriorityNormal, COUNTERMODULE_STACK_SIZE, stack,
      "count_mod_task");

  count_mod.start();

  while(true) {
    char c;
    count_cmd_t count_cmd;
    bool is_valid_key = true;

    c = getc(stdin);
    switch(c) {
      case 'i':
      case 'I':
        count_cmd = INCREASE;
        break;
      case 'd':
      case 'D':
        count_cmd = DECREASE;
        break;
      case 'z':
      case 'Z':
        count_cmd = ZERO;
        break;
      default:
        is_valid_key = false;
    }

    if(is_valid_key) {
      /* Messages CounterModule with requested command */
      if(!count_mod.store_msg(count_cmd)) {
        printf("Couldn't store the message.\n");
      }
    } else {
      printf("Valid keys are i, d, z\n");
    }
  }

  return 0;
}

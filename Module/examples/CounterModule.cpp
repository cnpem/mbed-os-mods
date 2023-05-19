/*
 * Copyright (C) 2023 CNPEM (cnpem.br)
 * Author: Guilherme Ricioli <guilherme.ricioli@lnls.br>
 */

#include "CounterModule.hpp"

CounterModule::CounterModule(osPriority priority, uint32_t stack_size,
    unsigned char *stack_mem, const char *name) :
  Module<count_cmd_t, COUNTERMODULE_BUFF_SIZE>(priority, stack_size, stack_mem,
      name) {
    _count = 0;
  }

CounterModule::~CounterModule() {}

void CounterModule::_task() {
  while(true) {
    count_cmd_t msg = ZERO;

    /* Reads messages */
    if(_read_msg(msg)) {
      switch(msg) {
        case INCREASE:
          ++_count;
          break;
        case DECREASE:
          if(_count > 0) {
            --_count;
          }
          break;
        case ZERO:
          _count = 0;
          break;
      }
      /* Prints counter */
      printf("%lu\n", _count);
    }
  }
}

/*
 * Copyright (C) 2023 CNPEM (cnpem.br)
 * Author: Guilherme Ricioli <guilherme.ricioli@lnls.br>
 */

#ifndef COUNTERMODULE_HPP_
#define COUNTERMODULE_HPP_

#include "../Module.hpp"

#define COUNTERMODULE_BUFF_SIZE  1

enum count_cmd_t {
  INCREASE,
  DECREASE,
  ZERO
};

class CounterModule :
  public Module<count_cmd_t, COUNTERMODULE_BUFF_SIZE> {
    public:
      CounterModule(osPriority priority, uint32_t stack_size,
          unsigned char *stack_mem, const char *name);
      ~CounterModule();

    private:
      uint32_t _count;

      void _task();
  };

#endif /* COUNTERMODULE_HPP_ */

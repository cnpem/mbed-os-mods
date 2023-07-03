/*
 * Copyright (C) 2023 CNPEM (cnpem.br)
 * Author: Guilherme Ricioli <guilherme.ricioli@lnls.br>
 */

#ifndef COREMODULE_HPP_
#define COREMODULE_HPP_

#include "mbed.h"

#include "IntfModuleMessage.hpp"
#include "Module.hpp"

class CoreModule :
  public Module {
    public:
      CoreModule(
          mbed::Callback<bool(Kernel::Clock::duration_u32, IntfModuleMessage**)>
          try_get_for_cb,
          /* Module params */
          osPriority priority, uint32_t stack_size, unsigned char *stack_mem,
          const char *name);
      ~CoreModule();

    private:
      uint32_t _count;
      mbed::Callback<bool(Kernel::Clock::duration_u32, IntfModuleMessage**)>
        _try_get_for_cb;

      void _task();
  };

#endif /* COREMODULE_HPP_ */

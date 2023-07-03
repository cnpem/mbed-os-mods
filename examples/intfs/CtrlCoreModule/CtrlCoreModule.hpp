/*
 * Copyright (C) 2023 CNPEM (cnpem.br)
 * Author: Guilherme Ricioli <guilherme.ricioli@lnls.br>
 */

#ifndef CTRLCOREMODULE_HPP_
#define CTRLCOREMODULE_HPP_

#include "mbed.h"

#include "IntfModuleMessage.hpp"
#include "Module.hpp"

class CtrlCoreModule :
  public Module {
    public:
      CtrlCoreModule(
          mbed::Callback<bool(Kernel::Clock::duration_u32, IntfModuleMessage**)>
          try_get_for_cb,
          /* Module params */
          osPriority priority, uint32_t stack_size, unsigned char *stack_mem,
          const char *name);
      ~CtrlCoreModule();

    private:
      uint32_t _count;
      mbed::Callback<bool(Kernel::Clock::duration_u32, IntfModuleMessage**)>
        _try_get_for_cb;

      void _task();
  };

#endif /* CTRLCOREMODULE_HPP_ */

/*
 * Copyright (C) 2023 CNPEM (cnpem.br)
 * Author: Guilherme Ricioli <guilherme.ricioli@lnls.br>
 */

#ifndef INTFMODULE_HPP_
#define INTFMODULE_HPP_

#include "mbed.h"

#include "IntfModuleMessage.hpp"
#include "Module.hpp"

/* IntfModule is a class providing the basics for interface modules.
 */
class IntfModule :
  public Module {
    public:
      /* Besides Module's params, receives the command request/response
       * terminator, its maximum length and a callback for queueing messages.
       */
      IntfModule(const char terminator, const uint8_t max_cmd_len,
          mbed::Callback<bool(Kernel::Clock::duration_u32, IntfModuleMessage*,
            uint8_t)> try_put_for_cb,
          /* Module params */
          osPriority priority, uint32_t stack_size, unsigned char *stack_mem,
          const char *name);
      virtual ~IntfModule();

    protected:
      const char _terminator;
      const uint8_t _max_cmd_len;
      mbed::Callback<bool(Kernel::Clock::duration_u32, IntfModuleMessage*,
          uint8_t)> _try_put_for_cb;

    private:
      virtual void _task() = 0;
  };

#endif /* INTFMODULE_HPP_ */

/*
 * Copyright (C) 2021-2023 CNPEM (cnpem.br)
 * Author: Guilherme Ricioli <guilherme.ricioli@lnls.br>
 */

#ifndef MODULE_HPP_
#define MODULE_HPP_

#include "mbed.h"

#include "Message.hpp"

/// TODO: Should this be the same for all modules?
#define BUFF_TIMEOUT_MS 1

template<uint32_t BUFF_SIZE>
class Module {
  public:
    Module(module_id_t id, osPriority priority, uint32_t stack_size,
        unsigned char *stack_mem, const char *name) {
      this->id        = id;
      this->thread    = new Thread(priority, stack_size, stack_mem,
          name);
      this->occup     = new Semaphore(0);
      this->unoccup   = new Semaphore(BUFF_SIZE);
    }

    virtual ~Module() {
      delete this->thread;
      delete this->occup;
      delete this->unoccup;
    }

    bool start() {
      bool status = false;
      mbed::Callback<void()> cb = callback(this, &Module::task);
      status = (this->thread->start(cb) == osOK)? true : false;

      return status;
    }

    bool put_msg(const Message &msg) {
      bool status = false;
      std::chrono::milliseconds timeout(BUFF_TIMEOUT_MS);
      if(this->unoccup->try_acquire_for(timeout) == true) {
        this->buff.push(msg);
        status = (this->occup->release() == osOK)? true : false;
      } else {
        status = false;
      }

      return status;
    }

    bool get_msg(Message &msg) {
      bool status = false;
      this->occup->acquire();
      if(this->buff.pop(msg) == true) {
        status = (this->unoccup->release() == osOK)? true : false;
      } else {
        status = false;
      }

      return status;
    }

  private:
    module_id_t                                     id;
    rtos::Thread                                    *thread;
    rtos::Semaphore                                 *occup;
    rtos::Semaphore                                 *unoccup;
    mbed::CircularBuffer<Message, BUFF_SIZE>        buff;

    virtual void task() = 0;
};

#endif /* MODULE_HPP_ */

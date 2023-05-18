/*
 * Copyright (C) 2021-2023 CNPEM (cnpem.br)
 * Author: Guilherme Ricioli <guilherme.ricioli@lnls.br>
 */

#ifndef MODULE_HPP_
#define MODULE_HPP_

#include "mbed.h"

#define BUFF_TIMEOUT_MS std::chrono::duration<uint32_t, std::milli>(1)

template<typename T, uint32_t BUFF_SIZE>
class Module {
  public:
    Module(osPriority priority, uint32_t stack_size, unsigned char *stack_mem,
        const char *name) {
      _p_thread = new rtos::Thread(priority, stack_size, stack_mem, name);
      _p_buff_busy = new rtos::Semaphore(1);
    }

    virtual ~Module() {
      delete _p_thread;
      delete _p_buff_busy;
    }

    bool start() {
      mbed::Callback<void()> cb = callback(this, &Module::_task);

      return _p_thread->start(cb) == osOK? true : false;
    }

    bool store_msg(const T &msg) {
      bool status = false;

      if(_p_buff_busy->try_acquire_for(BUFF_TIMEOUT_MS)) {
        if(!_buff.full()) {
          _buff.push(msg);
          status = true;
        }
        _p_buff_busy->release();
      }

      return status;
    }

  protected:
    bool _read_msg(T &msg) {
      bool status = false;

      if(_p_buff_busy->try_acquire_for(BUFF_TIMEOUT_MS)) {
        if(!_buff.empty()) {
          status = _buff.pop(msg);
        }
        _p_buff_busy->release();
      }

      return status;
    }

  private:
    rtos::Thread *_p_thread;
    rtos::Semaphore *_p_buff_busy;
    mbed::CircularBuffer<T, BUFF_SIZE> _buff;

    virtual void _task() = 0;
};

#endif /* MODULE_HPP_ */

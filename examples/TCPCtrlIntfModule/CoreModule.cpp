/*
 * Copyright (C) 2023 CNPEM (cnpem.br)
 * Author: Guilherme Ricioli <guilherme.ricioli@lnls.br>
 */

#include "CoreModule.hpp"

CoreModule::CoreModule(
    mbed::Callback<bool(Kernel::Clock::duration_u32, IntfModuleMessage**)>
    try_get_for_cb,
    /* Module params */
    osPriority priority, uint32_t stack_size, unsigned char *stack_mem,
    const char *name) :
  Module(priority, stack_size, stack_mem, name), _try_get_for_cb(try_get_for_cb) {
    _count = 0;
  }

CoreModule::~CoreModule() {}

void CoreModule::_task() {
  while(true) {
    bool status;
    IntfModuleMessage *p_intf_mod_msg;

    /* Reads messages */
    status = _try_get_for_cb(rtos::Kernel::wait_for_u32_forever,
        &p_intf_mod_msg);
    assert(status);

    /* Processes command */
    if(!strcmp(p_intf_mod_msg->buff, "@INC")) {
      ++_count;
    } else if(!strcmp(p_intf_mod_msg->buff, "@DEC")) {
      if(_count > 0) {
        --_count;
      }
    } else if(!strcmp(p_intf_mod_msg->buff, "@ZERO")) {
      _count = 0;
    }

    /* Writes message response */
    memset(p_intf_mod_msg->buff, 0, sizeof(p_intf_mod_msg->buff));
    snprintf(p_intf_mod_msg->buff, sizeof(p_intf_mod_msg->buff), "%lu", _count);

    /* Signalizes that response is ready */
    p_intf_mod_msg->p_ready->release();
  }
}

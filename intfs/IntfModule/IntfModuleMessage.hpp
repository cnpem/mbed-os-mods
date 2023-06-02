/*
 * Copyright (C) 2023 CNPEM (cnpem.br)
 * Author: Guilherme Ricioli <guilherme.ricioli@lnls.br>
 */

#ifndef INTFMODULEMESSAGE_HPP_
#define INTFMODULEMESSAGE_HPP_

#include "mbed.h"

/* Unit of inter-thread communication for interface modules
 *
 * The interface module fills 'buff' with the incoming command request, queues
 * the message to the module which will process it and waits for the signaling
 * of '*p_ready' that the command processing is done and the response is filled
 * in 'buff'.
 */
struct IntfModuleMessage {
  char *buff;
  rtos::Semaphore *p_ready;

  IntfModuleMessage() {}
  IntfModuleMessage(char *buff, rtos::Semaphore *p_ready) :
    buff(buff), p_ready(p_ready) {}
};

#endif /* INTFMODULEMESSAGE_HPP_ */

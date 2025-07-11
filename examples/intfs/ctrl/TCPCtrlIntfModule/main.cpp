/*
 * Copyright (C) 2023 CNPEM (cnpem.br)
 * Author: Guilherme Ricioli <guilherme.ricioli@lnls.br>
 */

#include "mbed.h"
#include "EthernetInterface.h"

#include "CtrlCoreModule.hpp"
#include "TCPCtrlIntfModule.hpp"

#define CTRLCOREMODULE_STACK_SIZE 1024
#define TCPCTRLINTFMODULE_STACK_SIZE 1024

/* Statically allocated stacks */
static unsigned char ctrl_core_mod_stack[CTRLCOREMODULE_STACK_SIZE];
static unsigned char tcp_ctrl_intf_mod_stack[TCPCTRLINTFMODULE_STACK_SIZE];

/* A simple counter example.
 *
 * Instantiates a TCPCtrlIntfModule and a CtrlCoreModule, invoking its threads.
 * The CtrlCoreModule module is designed for processing control commands.
 *
 * The TCP port used is 3001 with 30s of timeout. The terminator char is '\r'.
 * Valid commands are:
 *   '@INC', to increment the counter;
 *   '@DEC', to decrement the counter;
 *   '@ZERO', to zero the counter.
 * All commands return counter's value.
 */
int main() {
  bool status;
  nsapi_error_t nsapi_status;
  rtos::Queue<CtrlIntfModuleMessage, 1> queue;
  EthernetInterface net;
  CtrlCoreModule ctrl_core_mod(
      callback(&queue, &rtos::Queue<CtrlIntfModuleMessage, 1>::try_get_for),
      osPriorityNormal, CTRLCOREMODULE_STACK_SIZE, ctrl_core_mod_stack,
      "ctrl_core_mod_task");
  TCPCtrlIntfModule tcp_ctrl_intf_mod(&net, 3001, 30000, '\n', 32,
      callback(&queue, &rtos::Queue<CtrlIntfModuleMessage, 1>::try_put_for),
      osPriorityNormal, TCPCTRLINTFMODULE_STACK_SIZE, tcp_ctrl_intf_mod_stack,
      "tcp_ctrl_intf_mod_task");

  nsapi_status = net.set_blocking(false);
  assert(nsapi_status == NSAPI_ERROR_OK);

  nsapi_status = net.connect();
  assert(nsapi_status == NSAPI_ERROR_OK);

  status = ctrl_core_mod.start();
  assert(status);
  status = tcp_ctrl_intf_mod.start();
  assert(status);

  while(true) {
    SocketAddress addr;

    do {
        ThisThread::sleep_for(5s);
        nsapi_status = net.get_connection_status();
        debug("[main] net.get_connection_status rc: %d\n", nsapi_status);
    } while(nsapi_status != NSAPI_STATUS_GLOBAL_UP);

    debug("[main] MAC: %s\n", net.get_mac_address());
    
    nsapi_status = net.get_ip_address(&addr);
    debug("[main] net.get_ip_address rc: %d\n", nsapi_status);
    assert(nsapi_status == NSAPI_ERROR_OK);
    debug("[main] IP: %s\n", addr.get_ip_address());

    do {
        ThisThread::sleep_for(500ms);
        nsapi_status = net.get_connection_status();
    } while(nsapi_status == NSAPI_STATUS_GLOBAL_UP);
  }

  return 0;
}

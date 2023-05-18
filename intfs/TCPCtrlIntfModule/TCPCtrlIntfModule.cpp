/*
 * Copyright (C) 2021-2023 CNPEM (cnpem.br)
 * Author: Guilherme Ricioli <guilherme.ricioli@lnls.br>
 */

#include "TCPCtrlIntfModule.hpp"

TCPCtrlIntfModule::TCPCtrlIntfModule(
    EthernetInterface *p_net, uint16_t port, int timeout,
    /* IntfModule params */
    const char terminator, const uint8_t max_cmd_len,
    mbed::Callback<bool(Kernel::Clock::duration_u32, IntfModuleMessage*,
      uint8_t)> try_put_for_cb, osPriority priority, uint32_t stack_size,
    unsigned char *stack_mem, const char *name) :
  IntfModule(terminator, max_cmd_len, try_put_for_cb, priority, stack_size,
      stack_mem, name), _p_net(p_net), _port(port), _timeout(timeout) {}

TCPCtrlIntfModule::~TCPCtrlIntfModule() {}

void TCPCtrlIntfModule::_task() {
  char buff[_max_cmd_len];
  rtos::Semaphore ready(0, 1);
  IntfModuleMessage intf_mod_msg = {
    .buff     = buff,
    .p_ready  = &ready
  };
  nsapi_error_t nsapi_status;
  TCPSocket server;

  server.set_blocking(true);

  nsapi_status = server.open(_p_net);
  debug("server.open rc: %d\n", nsapi_status);
  assert(nsapi_status == NSAPI_ERROR_OK);

  nsapi_status = server.bind(_port);
  debug("server.bind rc: %d\n", nsapi_status);
  assert(nsapi_status == NSAPI_ERROR_OK);

  nsapi_status = server.listen(1);
  debug("server.listen rc: %d\n", nsapi_status);
  assert(nsapi_status == NSAPI_ERROR_OK);

  while(true) {
    nsapi_size_or_error_t count;
    TCPSocket *client;

    /* Blocking */
    client = server.accept(&nsapi_status);
    debug("server.accept rc: %d\n", nsapi_status);
    assert(nsapi_status == NSAPI_ERROR_OK);

    /* Not blocking from now on */
    client->set_timeout(_timeout);

    while(true) {
      memset(buff, 0, _max_cmd_len);

      /* Reads chars from TCP socket */
      while(true) {
        char c;
        uint8_t len = strlen(buff);

        /* '+ 1' is accounting for the string terminator */
        assert(_max_cmd_len - (len + 1) > 0);

        count = client->recv(&c, 1);
        debug("client->recv rc: %d\n", count);
        if(count < 0) {
          goto nsapi_error;
        }

        if(c == _terminator) {
          break;
        } else {
          buff[len++] = c;
        }
      }

      /* Queues the command */
      nsapi_status = _try_put_for_cb(rtos::Kernel::wait_for_u32_forever,
        &intf_mod_msg, 0);
      assert(nsapi_status);

      /* Waits for the command to be processed and its response be filled in
       * 'buff'
       */
      ready.acquire();

      /* Writes the response to TCP socket */
      count = client->send(buff, strlen(buff));
      debug("client->send rc: %d\n", count);
      if(count < 0) {
        goto nsapi_error;
      }
      count = client->send(&_terminator, 1);
      debug("client->send rc: %d\n", count);
      if(count < 0) {
        goto nsapi_error;
      }
    }

nsapi_error:
    nsapi_status = client->close();
    debug("client->close rc: %d\n", nsapi_status);

    assert(nsapi_status == NSAPI_ERROR_OK);
  }
}
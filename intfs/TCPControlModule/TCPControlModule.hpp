/*
 * Copyright (C) 2021-2023 CNPEM (cnpem.br)
 * Author: Guilherme Ricioli <guilherme.ricioli@lnls.br>
 */

#ifndef INTFS_TCPCONTROLMODULE_HPP_
#define INTFS_TCPCONTROLMODULE_HPP_

#include "EthernetInterface.h"

#include "CommandResponse.hpp"
#include "EngineModule.hpp"
#include "Module.hpp"

using namespace common;

#define END_OF_COMMAND_TOKENS   "\r\n"

#define CLIENT_SOCKET_TIMEOUT_MS    30000

/// TODO: WHAT SIZE SHOULD IT BE?
#define BUFFS_SIZE              576
#define NUM_OF_BUFFS            2

#define TCP_CTL_MOD_BUFF_SIZE   1

namespace intfs {
    class TCPControlModule : public Module<TCP_CTL_MOD_BUFF_SIZE> {
        public:
            TCPControlModule(core::EngineModule &enginemod,
                EthernetInterface &net, osPriority priority,
                uint32_t stack_size);
            ~TCPControlModule();

        private:
            core::EngineModule &enginemod;
            EthernetInterface &net;

            void task();
    };
} // namespace intfs

#endif /* INTFS_TCPCONTROLMODULE_HPP_ */

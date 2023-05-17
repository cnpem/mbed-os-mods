/*
 * Copyright (C) 2021-2023 CNPEM (cnpem.br)
 * Author: Guilherme Ricioli <guilherme.ricioli@lnls.br>
 */

#ifndef INTFS_UARTCONTROLMODULE_HPP_
#define INTFS_UARTCONTROLMODULE_HPP_

#include "CommandResponse.hpp"
#include "EngineModule.hpp"
#include "Module.hpp"
#include "TCPControlModule.hpp"

using namespace common;

#define UART_CTL_MOD_BUFF_SIZE  1

namespace intfs {
    class UARTControlModule : public Module<UART_CTL_MOD_BUFF_SIZE> {
        public:
            UARTControlModule(core::EngineModule &enginemod,
                osPriority priority, uint32_t stack_size);
            ~UARTControlModule();

        private:
            core::EngineModule &enginemod;

            void task();
    };
} // namespace intfs

#endif /* INTFS_UARTCONTROLMODULE_HPP_ */
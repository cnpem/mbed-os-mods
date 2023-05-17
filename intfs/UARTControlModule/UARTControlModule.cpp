/*
 * Copyright (C) 2021-2023 CNPEM (cnpem.br)
 * Author: Guilherme Ricioli <guilherme.ricioli@lnls.br>
 */

#include "UARTControlModule.hpp"

#include "app_defs.h"
#include "CommandRequest.hpp"

#define USB_BAUD_RATE       115200

static unsigned char stack_mem[UART_CONTROL_MODULE_THREAD_STACK_SIZE];
    // __attribute__((section("AHBSRAM1"))) = {0};
static const char *const TASK_NAME = "UARTControlModule";

namespace intfs {
    UARTControlModule::UARTControlModule(core::EngineModule &enginemod,
        osPriority priority, uint32_t stack_size):
            Module<UART_CTL_MOD_BUFF_SIZE>(UART_CONTROL_MODULE, priority,
                stack_size, stack_mem, TASK_NAME), enginemod(enginemod) {}

    UARTControlModule::~UARTControlModule() {}

    /// TODO: Reimplement string parsing using std::string 
    void UARTControlModule::task() {
        enum BUFF_POS {
            MAIN = 0,
            AUX
        };

        CommandRequest cmd_req;
        //CommandResponse cmd_res;

        // Object to store received and response messages
        Message recv_msg;
        Message enginemod_msg(UART_CONTROL_MODULE,
            subject_t::COMMAND_REQ);

        BufferedSerial PC(USBTX, USBRX, USB_BAUD_RATE);
        // BufferedSerial PC(MBED_UART1, 115200);

        char buffs[NUM_OF_BUFFS][BUFFS_SIZE];

        bool status;

        memset(buffs[MAIN], 0, BUFFS_SIZE);
        memset(buffs[AUX], 0, BUFFS_SIZE);

		while(true) {
		    uint8_t count = 0;
		    char *tok = NULL;
			
		    // Retrieves back exceeding chars to main buffer
		    memset(buffs[MAIN], 0, BUFFS_SIZE);
		    strcpy(buffs[MAIN], buffs[AUX]);
		    memset(buffs[AUX], 0, BUFFS_SIZE);
		
		    // If main buffer doesn't have a complete message yet, reads from RX buffer
		    while(true) {
		        // Searches for END_OF_COMMAND_TOKENS
                tok = strstr(buffs[MAIN], END_OF_COMMAND_TOKENS);
		
		        // If main buffer has a complete message, stop readings
		        if(tok != NULL) {
		            break;
		        }
		
		        // Checks if main buffer has free space
		        if(strlen(buffs[MAIN]) < BUFFS_SIZE - 1) {
		            count = PC.read(buffs[AUX], BUFFS_SIZE - (strlen(buffs[MAIN]) + 1));
                    /// TODO: Temporarily echoing
                    // PC.write(buffs[AUX], sizeof buffs[AUX]);
		            strncat(buffs[MAIN], buffs[AUX], count);
		        } else {
		            /// TODO: [ERROR] MAIN BUFFER IS FULL!
		        }
		    }

		    // Saves exceeding chars on aux buffer
		    memset(buffs[AUX], 0, BUFFS_SIZE);
		    strcpy(buffs[AUX], tok + 2);//std::strlen(END_OF_COMMAND_TOKENS));
		
		    // Ignores exceeding chars on main buffer
		    *tok = '\0';

            // Parses received message
            cmd_req.unserialize(buffs[MAIN]);

            // Populates enginemod_msg content    
            enginemod_msg.content.cmd_req = cmd_req;

            status = this->enginemod.put_msg(enginemod_msg);
            assert(status == true);

            status = this->get_msg(recv_msg);
            assert(status == true);

            if(recv_msg.from == ENGINE_MODULE &&
                recv_msg.subject == subject_t::COMMAND_RES) {
                    recv_msg.content.cmd_res.serialize(buffs[MAIN]);
            }

            // Writes back the response
            PC.write(buffs[MAIN], strlen(buffs[MAIN]));

            tok = NULL;
        }
    }
} // namespace intfs
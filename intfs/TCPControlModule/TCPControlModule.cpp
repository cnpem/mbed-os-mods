/*
 * Copyright (C) 2021-2023 CNPEM (cnpem.br)
 * Author: Guilherme Ricioli <guilherme.ricioli@lnls.br>
 */

#include "TCPControlModule.hpp"

#include "app_defs.h"
#include "CommandRequest.hpp"
#include "DEBUG.h"

#define DEFAULT_TCP_PORT            3000

#define CONNECTION_STATUS_REFRESH   500ms

static char buffs[NUM_OF_BUFFS][BUFFS_SIZE]
    __attribute__((section("AHBSRAM1"))) = {{0}};
static unsigned char stack_mem[TCP_CONTROL_MODULE_THREAD_STACK_SIZE]
    __attribute__((section("AHBSRAM1"))) = {0};
static const char *const TASK_NAME = "TCPControlModule";

namespace intfs {

    TCPControlModule::TCPControlModule(core::EngineModule &enginemod,
        EthernetInterface &net, osPriority priority, uint32_t stack_size) :
            Module<TCP_CTL_MOD_BUFF_SIZE>(TCP_CONTROL_MODULE, priority,
                stack_size, stack_mem, TASK_NAME), enginemod(enginemod),
                    net(net) {}

    TCPControlModule::~TCPControlModule() {}

    /// TODO: Reimplement string parsing using std::string
    void TCPControlModule::task() {
        enum BUFF_POS {
            MAIN = 0,
            AUX
        };

        CommandRequest cmd_req;
        CommandResponse cmd_res;

        // Object to store received and response messages
        Message recv_msg;
        Message enginemod_msg(TCP_CONTROL_MODULE, subject_t::COMMAND_REQ);

        nsapi_error_t status;
        nsapi_connection_status_t conn_status;

        INFO("Task started!");

        memset(buffs[MAIN], 0, BUFFS_SIZE);
        memset(buffs[AUX], 0, BUFFS_SIZE);

        net.connect();

        do {
            ThisThread::sleep_for(CONNECTION_STATUS_REFRESH);
            conn_status = net.get_connection_status();
            // INFO("status: %d", conn_status);
        } while(conn_status != NSAPI_STATUS_GLOBAL_UP);
        INFO("Connection stablished with success");

        INFO("MAC address: %s", net.get_mac_address());

        SocketAddress addr;
        status = net.get_ip_address(&addr);
        if(status == NSAPI_ERROR_OK) {
            INFO("Allocated IP address is %s",
                addr.get_ip_address() ? addr.get_ip_address() : "None");
        } else {
            ERROR("IP address was not properly acquired");
        }

        TCPSocket socket;

        status = socket.open(&(this->net));
        assert(status == NSAPI_ERROR_OK);
        status = socket.bind(DEFAULT_TCP_PORT);
        assert(status == NSAPI_ERROR_OK);
        INFO("Socket opened and binded to port %d", DEFAULT_TCP_PORT);

        // Only one client per TCP module
        status = socket.listen(1);
        assert(status == NSAPI_ERROR_OK);
        INFO("Listening for connections");

		while(true) {
		    nsapi_size_or_error_t count = 0;
		    char *tok = NULL;

            TCPSocket *client = NULL;
            do {
                client = socket.accept(&status);
                if(status != NSAPI_ERROR_OK) {
                    ERROR("Accept status: %d", status);
                }
            } while(status != NSAPI_ERROR_OK);
            assert(status == NSAPI_ERROR_OK);
            INFO("Client socket opened");

            client->set_timeout(CLIENT_SOCKET_TIMEOUT_MS);

            while(true) {
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
                        count = client->recv(buffs[AUX], BUFFS_SIZE - (strlen(buffs[MAIN]) + 1));
                        if(count >= 0) {
                            strncat(buffs[MAIN], buffs[AUX], count);
                        } else {
                            if(count == NSAPI_ERROR_WOULD_BLOCK) {
                                ERROR("recv timed out");
                            } else {
                                ERROR("recv returned error code: %d", count);
                            }
                            goto nsapi_error;
                        }
                    } else {
                        /// TODO: [ERROR] MAIN BUFFER IS FULL!
                    }
                }

                // Saves exceeding chars on aux buffer
                memset(buffs[AUX], 0, BUFFS_SIZE);
                strcpy(buffs[AUX], tok + 2);//std::strlen(END_OF_COMMAND_TOKENS));

                // Ignores exceeding chars on main buffer
                *tok = '\0';

                INFO("Command recv: %s", buffs[MAIN]);

                // Parses received message
                cmd_req.unserialize(buffs[MAIN]);

                // Populates enginemod_msg content
                enginemod_msg.content.cmd_req = cmd_req;

                /// TODO: Assert this
                this->enginemod.put_msg(enginemod_msg);

                do {
                    /// TODO: Assert this
                    this->get_msg(recv_msg);
                } while(recv_msg.from != ENGINE_MODULE ||
                    recv_msg.subject != subject_t::COMMAND_RES);

                cmd_res = recv_msg.content.cmd_res;
                cmd_res.serialize(buffs[MAIN]);

                // Writes back the response
                count = client->send(buffs[MAIN], strlen(buffs[MAIN]));
                if(count >= 0) {
                    strncat(buffs[MAIN], buffs[AUX], count);
                } else {
                    if(count == NSAPI_ERROR_WOULD_BLOCK) {
                        ERROR("send timed out");
                    } else {
                        ERROR("send returned error code: %d", count);
                    }
                    goto nsapi_error;
                }
            }

nsapi_error:
            if(count < 0) {
                memset(buffs[MAIN], 0, BUFFS_SIZE);
		        memset(buffs[AUX], 0, BUFFS_SIZE);
            }

            // Closes client socket
            status = client->close();
            assert(status == NSAPI_ERROR_OK);
            INFO("Client socket closed");

            tok = NULL;
        }
    }
} // namespace intfs

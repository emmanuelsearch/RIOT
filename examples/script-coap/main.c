/*
 * Copyright (c) 2015-2016 Ken Bannister. All rights reserved.
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example of .js script as CoAP resource
 *
 * @author      Ken Bannister <kb2ma@runbox.com>
 * @author      Emmanuel Baccelli <emmanuel.baccelli@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include "msg.h"

#include "net/gcoap.h"
#include "kernel_types.h"
#include "shell.h"
#include "jerryscript.h"
#include "thread.h"
#include "xtimer.h"

#include "coap_resources.h"

char jsstack[THREAD_STACKSIZE_MAIN];
jerry_char_t script[JS_SCRIPT_MAX_SIZE];
uint8_t counter=0;

#define COAP_INBUF_SIZE (256U)

#define MAIN_QUEUE_SIZE (4)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern void lwm2m_init(void);
extern void lwm2m_register(void);
extern void rmp_register(void);
extern int _netif_config(int argc, char **argv);

static const shell_command_t shell_commands[] = {
    { NULL, NULL, NULL }
};

void *jsthread_handler(void *arg)
{
    /* remove warning unused parameter arg */
    (void)arg;
    
    while(1) {
        if(strlen((char*)script)) {
            jerry_run_simple(script, strlen((char*)script), JERRY_INIT_EMPTY);
            }
        else {
            if (!counter) {
                puts("(empty initial script)\n");
                counter++;
                }
        }
        xtimer_sleep(5);
    }
    return NULL;
}

int main(void)
{
    
    /* for the thread running the shell */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    lwm2m_init();
    puts("Javascript CoAP LWM2M example");

    puts("Waiting for address autoconfiguration...");
    xtimer_sleep(3);

    /* print network addresses */
    puts("Configured network interfaces:");
    _netif_config(0, NULL);

    /* register to LWM2M server */
    lwm2m_register();
    xtimer_sleep(3);
    rmp_register();

    /* launch javascript thread */
    thread_create(jsstack, sizeof(jsstack),
                    THREAD_PRIORITY_MAIN - 1,
                    THREAD_CREATE_STACKTEST,
                    jsthread_handler,
                    NULL, "js thread");

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);



    /* should never be reached */
    return 0;
}

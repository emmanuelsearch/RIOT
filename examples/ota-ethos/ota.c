/*
 * Copyright (C) 2019 Inria
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
 * @brief       Example application for demonstrating the OTA over Ethos & CoAP
 *
 * @author      Emmanuel Baccelli <emmanuel.baccelli@inria.fr>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "net/gcoap.h"
#include "od.h"
#include "fmt.h"

#include "cpu_conf.h"
#include "periph/cpuid.h"

#define ENABLE_DEBUG (0)
#include "debug.h"
static char device_id[10];

static ssize_t _riot_id_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len, void *ctx);
static ssize_t _riot_board_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len, void *ctx);


/* CoAP resources. Must be sorted by path (ASCII order). */
static const coap_resource_t _resources[] = {
    { "/riot/board", COAP_GET, _riot_board_handler, NULL },
    { "/riot/id", COAP_GET, _riot_id_handler, NULL },
};

static gcoap_listener_t _listener = {
    &_resources[0],
    sizeof(_resources) / sizeof(_resources[0]),
    NULL
};


static ssize_t _riot_id_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, void *ctx)
{
    (void)ctx;
    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
    /* write the RIOT board name in the response buffer */
    /* must be 'greater than' to account for payload marker byte */
    if (pdu->payload_len > strlen(device_id)) {
        memcpy(pdu->payload, device_id, strlen(device_id));
        return gcoap_finish(pdu, strlen(device_id), COAP_FORMAT_TEXT);
    }
    else {
        puts("gcoap_cli: msg buffer too small");
        return gcoap_response(pdu, buf, len, COAP_CODE_INTERNAL_SERVER_ERROR);
    }
}

static ssize_t _riot_board_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, void *ctx)
{
    (void)ctx;
    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
    /* write the RIOT board name in the response buffer */
    /* must be 'greater than' to account for payload marker byte */
    if (pdu->payload_len > strlen(RIOT_BOARD)) {
        memcpy(pdu->payload, RIOT_BOARD, strlen(RIOT_BOARD));
        return gcoap_finish(pdu, strlen(RIOT_BOARD), COAP_FORMAT_TEXT);
    }
    else {
        puts("gcoap_cli: msg buffer too small");
        return gcoap_response(pdu, buf, len, COAP_CODE_INTERNAL_SERVER_ERROR);
    }
}


void ota_init(void)
{
    uint8_t id[CPUID_LEN];
    char *pos = device_id;

    cpuid_get(id);
    pos += fmt_str(pos, "RIOT-");
    pos += fmt_byte_hex(pos, id[0]);
    pos += fmt_byte_hex(pos, id[1]);
    *pos = '\0';

    printf("Device ID = %s \n", device_id);

    gcoap_register_listener(&_listener);
}

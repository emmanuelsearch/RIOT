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
 * @brief       CLI support for .js script resource example
 *
 * @author      Ken Bannister <kb2ma@runbox.com>
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

#include "coap_resources.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static void _resp_handler(unsigned req_state, coap_pkt_t* pdu);
static ssize_t _stats_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len);

const char lwm2m_resources[] = "<3/0>, <9/0>, <3303/0>, <3315/0>";
const char lwm2m_server_addr[] = "2a05:d014:677:2900:9786:f713:6820:e17e";
const char lwm2m_server_port[] = "6683";
const char rmp_server_port[] = "5683";
const char device_id[10];
const char proxy[] = "http://tinyurl.com/rmp-api";
const char rmp_msg[] = "registration";

static ssize_t _riot_script_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len)
{
    ssize_t rsp_len = 0;
    unsigned code = COAP_CODE_EMPTY;

    /* read coap method type in packet */
    unsigned method_flag = coap_method2flag(coap_get_code_detail(pkt));

    switch(method_flag) {
    case COAP_GET:
        code = COAP_CODE_205;
        rsp_len = strlen((char*)script);
        break;
    case COAP_POST:
    case COAP_PUT:
    {
        /* overwrite the current script with the new received script  */
        memcpy(script, (char*)pkt->payload, pkt->payload_len);
        script[pkt->payload_len] = '\0';
        printf("new script:\"%s\"\n", script);
        code = COAP_CODE_CHANGED;
        break;
    }
    }

    return coap_reply_simple(pkt, code, buf, len,
            COAP_FORMAT_TEXT, (uint8_t*)script, rsp_len);


/*    return gcoap_response(pkt, buf, len, code); 
*/

}

/* CoAP resources */
static const coap_resource_t _resources[] = {
    { "/cli/stats", COAP_GET, _stats_handler },
    { "/riot/script", COAP_GET | COAP_PUT | COAP_POST, _riot_script_handler },
};
static gcoap_listener_t _listener = {
    (coap_resource_t *)&_resources[0],
    sizeof(_resources) / sizeof(_resources[0]),
    NULL
};

/* Counts requests sent by CLI. */
static uint16_t req_count = 0;

/*
 * Response callback.
 */
static void _resp_handler(unsigned req_state, coap_pkt_t* pdu)
{
    if (req_state == GCOAP_MEMO_TIMEOUT) {
        printf("gcoap: timeout for msg ID %02u\n", coap_get_id(pdu));
        return;
    }
    else if (req_state == GCOAP_MEMO_ERR) {
        printf("gcoap: error in response\n");
        return;
    }

    char *class_str = (coap_get_code_class(pdu) == COAP_CLASS_SUCCESS)
                            ? "Success" : "Error";
    printf("gcoap: response %s, code %1u.%02u", class_str,
                                                coap_get_code_class(pdu),
                                                coap_get_code_detail(pdu));
    
    if (pdu->payload_len) {
        if (pdu->content_type == COAP_FORMAT_TEXT
                || pdu->content_type == COAP_FORMAT_LINK
                || coap_get_code_class(pdu) == COAP_CLASS_CLIENT_FAILURE
                || coap_get_code_class(pdu) == COAP_CLASS_SERVER_FAILURE) {
            /* Expecting diagnostic payload in failure cases */
            printf(", %u bytes\n%.*s\n", pdu->payload_len, pdu->payload_len,
                                                          (char *)pdu->payload);
        }
        else {
            printf(", %u bytes\n", pdu->payload_len);
            od_hex_dump(pdu->payload, pdu->payload_len, OD_WIDTH_DEFAULT);
        }
    }
    else {
        printf(", empty payload\n");
    }
}

/*
 * Server callback for /cli/stats. Returns the count of packets sent by the
 * CLI.
 */
static ssize_t _stats_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len)
{
    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);

    size_t payload_len = fmt_u16_dec((char *)pdu->payload, req_count);

    return gcoap_finish(pdu, payload_len, COAP_FORMAT_TEXT);
}

static size_t _send(uint8_t *buf, size_t len, char *addr_str, char *port_str)
{
    ipv6_addr_t addr;
    size_t bytes_sent;
    sock_udp_ep_t remote;

    remote.family = AF_INET6;
    remote.netif  = SOCK_ADDR_ANY_NETIF;

    /* parse destination address */
    if (ipv6_addr_from_str(&addr, addr_str) == NULL) {
        puts("gcoap_cli: unable to parse destination address");
        return 0;
    }
    memcpy(&remote.addr.ipv6[0], &addr.u8[0], sizeof(addr.u8));

    /* parse port */
    remote.port = atoi(port_str);
    if (remote.port == 0) {
        puts("gcoap_cli: unable to parse destination port");
        return 0;
    }

    bytes_sent = gcoap_req_send2(buf, len, &remote, (gcoap_resp_handler_t) _resp_handler);
    if (bytes_sent > 0) {
        req_count++;
    }
    return bytes_sent;
}

void lwm2m_register(void)
{
    uint8_t buf[GCOAP_PDU_BUF_SIZE];
    coap_pkt_t pdu;
    size_t len;
    
    /* register to LWM2M server */
    gcoap_req_init(&pdu, &buf[0], GCOAP_PDU_BUF_SIZE, 2, "/rd");
    gcoap_add_qstring(&pdu, "b", "U");
    gcoap_add_qstring(&pdu, "lwm2m", "1.0");
    gcoap_add_qstring(&pdu, "lt", "10800");
    gcoap_add_qstring(&pdu, "ep", device_id);
    memcpy(pdu.payload, lwm2m_resources, strlen(lwm2m_resources));
    len = gcoap_finish(&pdu, strlen(lwm2m_resources), COAP_FORMAT_TEXT);
    
    printf("lwm2m: sending msg ID %u, %u bytes\n", coap_get_id(&pdu),
           (unsigned) len);
    printf("Resources registered: '%s'\n", lwm2m_resources);
    if (!_send(&buf[0], len, (char *)lwm2m_server_addr, (char *)lwm2m_server_port)) {
        puts("lwm2m registration: msg send failed");
    }

}

void rmp_register(void)
{
    uint8_t buf[GCOAP_PDU_BUF_SIZE + 64];
    coap_pkt_t pdu;
    size_t len;
    
    /* register to RMP server */
    gcoap_req_init(&pdu, &buf[0], GCOAP_PDU_BUF_SIZE + 64, 2, "/coap");
    /* Uncomment when proxy-uri is ready
    gcoap_add_proxy(&pdu, proxy); */
    gcoap_add_qstring(&pdu, "ep", device_id);
    memcpy(pdu.payload, rmp_msg, strlen(rmp_msg));
    len = gcoap_finish(&pdu, strlen(rmp_msg), COAP_FORMAT_TEXT);
    
    printf("rmp registration: sending msg ID %u, %u bytes\n", coap_get_id(&pdu),
           (unsigned) len);
    printf("Payload: '%s'\n", rmp_msg);
    if (!_send(&buf[0], len, (char *)lwm2m_server_addr, (char *)rmp_server_port)) {
        puts("rmp registration: msg send failed");
    }

}

void lwm2m_init(void)
{
    
    uint8_t id[CPUID_LEN];
    char *pos = (char *)device_id;
    
    cpuid_get(id);
    pos += fmt_str(pos, "RIOT-");
    pos += fmt_byte_hex(pos, id[0]);
    pos += fmt_byte_hex(pos, id[1]);
    *pos = '\0';
    
    printf("Device ID = %s \n", device_id);
    
    gcoap_register_listener(&_listener);
}

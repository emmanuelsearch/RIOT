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

#define ENABLE_DEBUG (0)
#include "debug.h"

static void _resp_handler(unsigned req_state, coap_pkt_t *pdu);

static char alive_payload[] = "Alive";
static char reset_payload[] = "reset";
static char dashboard_addr[] = "affe::1";
static char dashboard_port[] = "5683";
static char device_id[32];

extern char script[];
extern void js_restart(void);

ssize_t _blockwise_script_handler(coap_pkt_t* pkt, uint8_t *buf, size_t len)
{
    printf("_blockwise_script_handler()\n");

    uint32_t result = COAP_CODE_204;
    uint32_t blknum;
    unsigned szx;
    int res = coap_get_blockopt(pkt, COAP_OPT_BLOCK1, &blknum, &szx);
    if (res >= 0) {
        printf("blknum=%u blksize=%u more=%u\n", (unsigned)blknum, coap_szx2size(szx), res);
        size_t offset = blknum << (szx + 4);
        printf("received bytes %u-%u\n", (unsigned)offset, (unsigned)offset+pkt->payload_len);

        /* overwrite the current script with the new received script  */
        memcpy(script + offset, (char *)pkt->payload, pkt->payload_len);
        if (res) {
            result = COAP_CODE_231;
        }
        else {
            script[offset + pkt->payload_len] = '\0';
            puts("script received (blockwise):");
            puts("-----");
            puts(script);
            puts("-----");
            puts("restarting js.");
            js_restart();
        }
    }
    else {
        memcpy(script, (char *)pkt->payload, pkt->payload_len);
        script[pkt->payload_len] = '\0';
        puts("script received:");
        puts("-----");
        puts(script);
        puts("-----");
        puts("restarting js.");
        js_restart();
    }

    ssize_t reply_len = coap_build_reply(pkt, result, buf, len, 0);
    uint8_t *pkt_pos = (uint8_t*)pkt->hdr + reply_len;
    if (res >= 0) {
        pkt_pos += coap_put_option_block1(pkt_pos, 0, blknum, szx, res);
    }
    return pkt_pos - (uint8_t*)pkt->hdr;
}

static ssize_t _riot_script_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len)
{
    ssize_t rsp_len = 0;
    unsigned code = COAP_CODE_EMPTY;
    const char *js_mock_script = "script is fine!";

    /* read coap method type in packet */
    unsigned method_flag = coap_method2flag(coap_get_code_detail(pkt));

    switch (method_flag) {
        case COAP_GET:
            code = COAP_CODE_205;
            rsp_len = strlen((char *)js_mock_script);
            break;
        case COAP_POST:
        case COAP_PUT:
        {
            return _blockwise_script_handler(pkt, buf, len);
        }
    }

    return coap_reply_simple(pkt, code, buf, len,
                             COAP_FORMAT_TEXT, (uint8_t *)js_mock_script, rsp_len);


/*    return gcoap_response(pkt, buf, len, code);
 */

}

static ssize_t _os_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len)
{
    ssize_t rsp_len = 0;
    unsigned code = COAP_CODE_EMPTY;
    const char *os = "riot";

    /* read coap method type in packet */
    unsigned method_flag = coap_method2flag(coap_get_code_detail(pkt));

    switch (method_flag) {
        case COAP_GET:
            code = COAP_CODE_205;
            rsp_len = strlen((char *)os);
            break;
        case COAP_POST:
        case COAP_PUT:
            break;
    }

    return coap_reply_simple(pkt, code, buf, len,
                             COAP_FORMAT_TEXT, (uint8_t *)os, rsp_len);


/*    return gcoap_response(pkt, buf, len, code);
 */

}


static ssize_t _board_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len)
{
    ssize_t rsp_len = 0;
    unsigned code = COAP_CODE_EMPTY;
    const char *board = RIOT_BOARD;

    /* read coap method type in packet */
    unsigned method_flag = coap_method2flag(coap_get_code_detail(pkt));

    switch (method_flag) {
        case COAP_GET:
            code = COAP_CODE_205;
            rsp_len = strlen((char *)board);
            break;
        case COAP_POST:
        case COAP_PUT:
            break;
    }

    return coap_reply_simple(pkt, code, buf, len,
                             COAP_FORMAT_TEXT, (uint8_t *)board, rsp_len);


/*    return gcoap_response(pkt, buf, len, code);
 */

}

static ssize_t _name_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len)
{
    ssize_t rsp_len = 0;
    unsigned code = COAP_CODE_EMPTY;

    /* read coap method type in packet */
    unsigned method_flag = coap_method2flag(coap_get_code_detail(pkt));

    switch (method_flag) {
        case COAP_GET:
            code = COAP_CODE_205;
            rsp_len = strlen((char *)device_id);
            break;
        case COAP_POST:
        case COAP_PUT:
            break;
    }

    return coap_reply_simple(pkt, code, buf, len,
                             COAP_FORMAT_TEXT, (uint8_t *)device_id, rsp_len);


/*    return gcoap_response(pkt, buf, len, code);
 */

}

/* CoAP resources */
static const coap_resource_t _resources[] = {
    { "/board", COAP_GET | COAP_PUT | COAP_POST, _board_handler },
    { "/js", COAP_GET | COAP_PUT | COAP_POST, _riot_script_handler },
    { "/name", COAP_GET | COAP_PUT | COAP_POST, _name_handler },
    { "/os", COAP_GET | COAP_PUT | COAP_POST, _os_handler },
    
};
static gcoap_listener_t _listener = {
    (coap_resource_t *)&_resources[0],
    sizeof(_resources) / sizeof(_resources[0]),
    NULL
};


/*
 * Response callback.
 */
static void _resp_handler(unsigned req_state, coap_pkt_t *pdu)
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
        unsigned content_type = coap_get_content_type(pdu);

        if (content_type == COAP_FORMAT_TEXT
            || content_type == COAP_FORMAT_LINK
            || coap_get_code_class(pdu) == COAP_CLASS_CLIENT_FAILURE
            || coap_get_code_class(pdu) == COAP_CLASS_SERVER_FAILURE) {
            /* Expecting diagnostic payload in failure cases */
            printf(", %u bytes\n%.*s\n", pdu->payload_len, pdu->payload_len,
                   (char *)pdu->payload);
        }
        else {
            printf(", %u bytes\n", pdu->payload_len);
            //od_hex_dump(pdu->payload, pdu->payload_len, OD_WIDTH_DEFAULT);
        }
    }
    else {
        printf(", empty payload\n");
    }
}

static size_t _send(uint8_t *buf, size_t len, char *addr_str, char *port_str)
{
    ipv6_addr_t addr;
    size_t bytes_sent;
    sock_udp_ep_t remote;

    remote.family = AF_INET6;
    remote.netif = SOCK_ADDR_ANY_NETIF;

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
    return bytes_sent;
}

void register_keepalive(void)
{
    uint8_t buf[GCOAP_PDU_BUF_SIZE];
    coap_pkt_t pdu;
    size_t len;

    /* keepalive registration to dashboard */
    gcoap_req_init(&pdu, &buf[0], GCOAP_PDU_BUF_SIZE, 2, "/alive");
    memcpy(pdu.payload, alive_payload, strlen(alive_payload));
    len = gcoap_finish(&pdu, strlen(alive_payload), COAP_FORMAT_TEXT);

    printf("Dashboard keepalive: sending msg ID %u, %u bytes\n", coap_get_id(&pdu),
           (unsigned) len);
    if (!_send(&buf[0], len, dashboard_addr, dashboard_port)) {
        puts("Dashboard keepalive: msg send failed");
    }

}

void register_init(void)
{
    uint8_t buf[GCOAP_PDU_BUF_SIZE];
    coap_pkt_t pdu;
    size_t len;
    uint8_t id[CPUID_LEN];
    char *pos = device_id;

    cpuid_get(id);
    pos += fmt_str(pos, "RIOT-");
    pos += fmt_byte_hex(pos, id[0]);
    pos += fmt_byte_hex(pos, id[1]);
    pos += fmt_str(pos, " Javascript Container");
    *pos = '\0';

    printf("Device ID = %s \n", device_id);

    gcoap_register_listener(&_listener);
    
    /* initial registration to dashboard */
    gcoap_req_init(&pdu, &buf[0], GCOAP_PDU_BUF_SIZE, 2, "/alive");
    memcpy(pdu.payload, reset_payload, strlen(reset_payload));
    len = gcoap_finish(&pdu, strlen(reset_payload), COAP_FORMAT_TEXT);

    printf("Dashboard init: sending msg ID %u, %u bytes\n", coap_get_id(&pdu),
           (unsigned) len);
    if (!_send(&buf[0], len, dashboard_addr, dashboard_port)) {
        puts("Dashboard init: msg send failed");
    }

}

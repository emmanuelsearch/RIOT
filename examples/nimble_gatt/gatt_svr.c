/*
 * Copyright (C) 2018 Freie Universität Berlin
 *               2018 Codecoup
 *               2018 Inria
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
 * @brief       BLE peripheral example using NimBLE
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Andrzej Kaczmarek <andrzej.kaczmarek@codecoup.pl>
 * @author      Emmanuel Baccelli <emmanuel.baccelli@inria.fr>
 *
 * @}
 */


#include "ble_temperature.h"

static const char *manuf_name = "RIOT";
static const char *model_num = "NimBLE on RIOT";
uint16_t hrs_hrm_handle;

static int
gatt_svr_chr_access_temperature_data(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg);

static int
gatt_svr_chr_access_temperature_conf(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg);
                               
static int
gatt_svr_chr_access_temperature_peri(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg);

static int
gatt_svr_chr_access_device_info(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg);

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        /* Service: Heart-rate */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = (ble_uuid_t*)&gatt_svr_chr_temp_serv_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) { {
            /* Characteristic: Temperature data */
            .uuid = (ble_uuid_t*)&gatt_svr_chr_temp_data_uuid.u,
            .access_cb = gatt_svr_chr_access_temperature_data,
            .val_handle = &hrs_hrm_handle,
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
        }, {
            /* Characteristic: Temperature conf */
            .uuid = (ble_uuid_t*)&gatt_svr_chr_temp_conf_uuid.u,
            .access_cb = gatt_svr_chr_access_temperature_conf,
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
        }, {
            /* Characteristic: Temperature peri */
            .uuid = (ble_uuid_t*)&gatt_svr_chr_temp_peri_uuid.u,
            .access_cb = gatt_svr_chr_access_temperature_peri,
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
        }, {
            0, /* No more characteristics in this service */
        }, }
    },

    {
        /* Service: Device Information */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(GATT_DEVICE_INFO_UUID),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            /* Characteristic: * Manufacturer name */
            .uuid = BLE_UUID16_DECLARE(GATT_MANUFACTURER_NAME_UUID),
            .access_cb = gatt_svr_chr_access_device_info,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            /* Characteristic: Model number string */
            .uuid = BLE_UUID16_DECLARE(GATT_MODEL_NUMBER_UUID),
            .access_cb = gatt_svr_chr_access_device_info,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            0, /* No more characteristics in this service */
        }, }
    },

        {
            0, /* No more services */
        },
};

static int
gatt_svr_chr_access_temperature_data(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    /* DUMMY to do */
    static uint8_t dummy_temperature = 0x14;
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;
    /* uint16_t uuid; */
    int rc = os_mbuf_append(ctxt->om, &dummy_temperature, sizeof(dummy_temperature));

    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}


static int
gatt_svr_chr_access_temperature_conf(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    /* DUMMY to do */
    static uint8_t dummy_value = 0x01;
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;
    /* uint16_t uuid; */
    int rc = os_mbuf_append(ctxt->om, &dummy_value, sizeof(dummy_value));

    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}

static int
gatt_svr_chr_access_temperature_peri(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    /* DUMMY to do */
    static uint8_t dummy_value = 0x01;
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;
    /* uint16_t uuid; */
    int rc = os_mbuf_append(ctxt->om, &dummy_value, sizeof(dummy_value));

    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}


static int
gatt_svr_chr_access_device_info(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    uint16_t uuid;
    int rc;
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    uuid = ble_uuid_u16(ctxt->chr->uuid);

    if (uuid == GATT_MODEL_NUMBER_UUID) {
        rc = os_mbuf_append(ctxt->om, model_num, strlen(model_num));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    if (uuid == GATT_MANUFACTURER_NAME_UUID) {
        rc = os_mbuf_append(ctxt->om, manuf_name, strlen(manuf_name));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}

/* 
void
gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
    char buf[BLE_UUID_STR_LEN];

    switch (ctxt->op) {
    case BLE_GATT_REGISTER_OP_SVC:
       BLEHR_LOG(DEBUG, "registered service %s with handle=%d\n", 
                    ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                    ctxt->svc.handle);
        break;

    case BLE_GATT_REGISTER_OP_CHR:
        BLEHR_LOG(DEBUG, "registering characteristic %s with "
                           "def_handle=%d val_handle=%d\n",
                    ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                    ctxt->chr.def_handle,
                    ctxt->chr.val_handle);
        break;

    case BLE_GATT_REGISTER_OP_DSC:
        BLEHR_LOG(DEBUG, "registering descriptor %s with handle=%d\n",
                    ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                    ctxt->dsc.handle);
        break;

    default:
        assert(0);
        break;
    }
}
*/

int gatt_svr_init(void)
{
    int rc;

    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    return 0;
}


/* 
static int gatt_svr_chr_access_temperature(uint16_t conn_handle, uint16_t attr_handle,
                             struct ble_gatt_access_ctxt *ctxt,
                             void *arg) {
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    const ble_uuid_t *uuid;
    static uint8_t gatt_svr_temperature_static_val = 20;
    int rc;

    uuid = ctxt->chr->uuid;


    if (ble_uuid_cmp(uuid, &gatt_svr_chr_temp_serv_uuid.u) == 0) {
        switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_CHR:
            rc = os_mbuf_append(ctxt->om, &gatt_svr_temperature_static_val,
                                sizeof gatt_svr_temperature_static_val);
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

        default:
            assert(0);
            return BLE_ATT_ERR_UNLIKELY;
        }
    }


    assert(0);
    return BLE_ATT_ERR_UNLIKELY;

}

*/

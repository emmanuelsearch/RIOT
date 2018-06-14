#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "nimble_riot.h"

#include "nimble/nimble_port.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "host/ble_uuid.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#define GATT_DEVICE_INFO_UUID                   0x180A
#define GATT_MANUFACTURER_NAME_UUID             0x2A29
#define GATT_MODEL_NUMBER_UUID                  0x2A24

#define IRTEMPERATURE_SERV_UUID 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, \
0x00, 0x40, 0x51, 0x04, 0x00, 0xAA, 0x00, 0xF0

#define IRTEMPERATURE_DATA_UUID 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, \
0x00, 0x40, 0x51, 0x04, 0x01, 0xAA, 0x00, 0xF0

#define IRTEMPERATURE_CONF_UUID 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, \
0x00, 0x40, 0x51, 0x04, 0x02, 0xAA, 0x00, 0xF0

#define IRTEMPERATURE_PERI_UUID 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, \
0x00, 0x40, 0x51, 0x04, 0x03, 0xAA, 0x00, 0xF0

static const ble_uuid128_t gatt_svr_chr_temp_serv_uuid =
        BLE_UUID128_INIT(IRTEMPERATURE_SERV_UUID);

static const ble_uuid128_t gatt_svr_chr_temp_data_uuid =
        BLE_UUID128_INIT(IRTEMPERATURE_DATA_UUID);

 static const ble_uuid128_t gatt_svr_chr_temp_conf_uuid =
        BLE_UUID128_INIT(IRTEMPERATURE_CONF_UUID);

static const ble_uuid128_t gatt_svr_chr_temp_peri_uuid =
        BLE_UUID128_INIT(IRTEMPERATURE_PERI_UUID); 

extern uint16_t hrs_hrm_handle;

int gatt_svr_init(void);


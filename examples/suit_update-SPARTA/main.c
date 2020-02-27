/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       SUIT updates over CoAP example server application (using nanocoap)
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "irq.h"
#include "net/nanocoap_sock.h"
#include "xtimer.h"

/* START SPARTA SPECIFIC */
#include "bmx280_params.h"
#include "bmx280.h"
#include "fmt.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#include <errno.h>
#include "logo.h"
#include "ucg.h"
#include "tft_display.h"

/* STOP SPARTA SPECIFIC */

#include "shell.h"

#include "suit/coap.h"
#include "riotboot/slot.h"

#ifdef MODULE_PERIPH_GPIO
#include "periph/gpio.h"
#endif

#define COAP_INBUF_SIZE (256U)

/* START SPARTA SPECIFIC */

#define MAINLOOP_DELAY  (4)         /* read sensor every 4 seconds */
static char _sensor_stack[THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF];
bmx280_t dev;

ucg_t ucg;
static gpio_t pins[] = {
    [UCG_PIN_CS]  = TFT_PIN_CS,
    [UCG_PIN_CD]  = TFT_PIN_CD,
    [UCG_PIN_RST] = TFT_PIN_RESET
};

static uint32_t pins_enabled = (
    (1 << UCG_PIN_CS) +
    (1 << UCG_PIN_CD) +
    (1 << UCG_PIN_RST)
    );
/* STOP SPARTA SPECIFIC */

/* Extend stacksize of nanocoap server thread */
static char _nanocoap_server_stack[THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF];
#define NANOCOAP_SERVER_QUEUE_SIZE     (8)
static msg_t _nanocoap_server_msg_queue[NANOCOAP_SERVER_QUEUE_SIZE];

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static void *_nanocoap_server_thread(void *arg)
{
    (void)arg;

    /* nanocoap_server uses gnrc sock which uses gnrc which needs a msg queue */
    msg_init_queue(_nanocoap_server_msg_queue, NANOCOAP_SERVER_QUEUE_SIZE);

    /* initialize nanocoap server instance */
    uint8_t buf[COAP_INBUF_SIZE];
    sock_udp_ep_t local = { .port=COAP_PORT, .family=AF_INET6 };
    nanocoap_server(&local, buf, sizeof(buf));

    return NULL;
}

static void _clear_data_area(ucg_t* ucg)
{
    ucg_SetColor(ucg, 0, 0, 0, 0);
    ucg_DrawBox(ucg, 0, 66, 128, 40);
    ucg_SetColor(ucg, 0, 255, 255, 255);
}



/* assuming that first button is always BTN0 */
#if defined(MODULE_PERIPH_GPIO_IRQ) && defined(BTN0_PIN)
static void cb(void *arg)
{
    (void) arg;
  //  printf("Button pressed! Triggering suit update! \n");
  //  suit_coap_trigger((uint8_t *) SUIT_MANIFEST_RESOURCE, sizeof(SUIT_MANIFEST_RESOURCE));
}
#endif

static void tft_puts(ucg_t * ucg, char* str_pre, char* str_data, char* str_post,
              uint8_t offset_x, uint8_t offset_y, uint8_t center)
{
    char buf[32];
    strcpy(buf, str_pre);
    strcat(buf, str_data);
    strcat(buf, str_post);

    if(center) {
        uint8_t width = ucg_GetStrWidth(ucg, buf);
        offset_x = (width/2 < (offset_x)) ? (offset_x - width/2) : 0;
    }
    ucg_SetColor(ucg, 1, 0, 0, 0);
    ucg_SetColor(ucg, 0, 255, 255, 255);
    ucg_DrawString(ucg, offset_x, offset_y, 0, buf);
}

#ifdef MODULE_RIOTBOOT_SLOT
static void tft_print_int(ucg_t * ucg, int data, uint8_t offset_x,
                      uint8_t offset_y, uint8_t center)
{
    char buffer [8];
    sprintf(buffer, "%i", data);
    tft_puts(ucg, buffer, NULL, NULL, offset_x, offset_y, center);
}
#endif


static void _draw_riot_logo(ucg_t* ucg, uint16_t start_x, uint16_t start_y)
{
    for (int y = 0; y < 48 ; y++) {
        for (int x = 0; x < 96; x++) {
            uint32_t offset = (x + (y * 96)) * 3;
            ucg_SetColor(ucg, 0, logo[offset + 2], logo[offset + 1], logo[offset + 0]);
            ucg_DrawPixel(ucg, x + start_x, y + start_y);
        }
    }
}

static void _draw_riotboot(ucg_t* ucg)
{
#ifdef MODULE_RIOTBOOT_SLOT
    /* Running Slot */
    ucg_SetFontPosCenter(ucg);
    ucg_SetFont(ucg, ucg_font_profont15_mr);
    tft_print_int(ucg, riotboot_slot_current(), 117, 118, 1);

    /* Draw Frame For slot Info */
    ucg_SetColor(ucg, 1, 0, 0, 0);
    ucg_SetColor(ucg, 0, 255, 255, 255);
    ucg_DrawRFrame(ucg, 108, 108, 18, 18, 4);

    /* Draw Frame For Version Info */
    ucg_DrawRFrame(ucg, 2, 108, 104, 18, 4);
    ucg_SetFont(ucg, ucg_font_profont12_mr);
    ucg_SetFontPosTop(ucg);
    char buffer [32];
    const riotboot_hdr_t* hdr = riotboot_slot_get_hdr(riotboot_slot_current());
    sprintf(buffer, "VER: %"PRIu32"", hdr->version);
    tft_puts(ucg, buffer, NULL, NULL, 52, 112, 1);
#else
    (void) ucg;
#endif
}

static void _draw_app_name(ucg_t* ucg)
{
    ucg_SetFontPosTop(ucg);
    ucg_SetFont(ucg, ucg_font_profont12_mr);
    tft_puts(ucg, (char* ) APPLICATION_NAME, NULL, NULL, 63, 0, 1);
}

static void *_sensor_thread(void *arg)
{
    (void)arg;

   while (1) {
        /* read temperature, pressure [and humidity] values */
        int16_t temperature = bmx280_read_temperature(&dev);
        uint32_t pressure = bmx280_read_pressure(&dev);
#if defined(MODULE_BME280_SPI) || defined(MODULE_BME280_I2C)
        uint16_t humidity = bme280_read_humidity(&dev);
#endif

        /* format values for printing */
        char str_temp[8];
        size_t len = fmt_s16_dfp(str_temp, temperature, -2);
        str_temp[len] = '\0';
#if defined(MODULE_BME280_SPI) || defined(MODULE_BME280_I2C)
        char str_hum[8];
        len = fmt_s16_dfp(str_hum, humidity, -2);
        str_hum[len] = '\0';
#endif
        char str_press[10];
        len = fmt_u32_dec(str_press, pressure);
        str_press[len] = '\0';

/*
        // print values to STDIO 
        printf("Temperature [°C]: %s\n", str_temp);
        printf("   Pressure [Pa]: %" PRIu32 "\n", pressure);
#if defined(MODULE_BME280_SPI) || defined(MODULE_BME280_I2C)
        printf("  Humidity [%%rH]: %s\n", str_hum);
#endif
        puts("\n+-------------------------------------+\n");
*/

        _clear_data_area(&ucg);
        ucg_SetFont(&ucg, ucg_font_profont15_mr);
        tft_puts(&ucg,"TEMP: ", str_temp, NULL, 64, 66, 1);
        tft_puts(&ucg,"HUM: ", str_hum, NULL, 64, 78, 1);
        tft_puts(&ucg,"PRESS: ", str_press, NULL, 64, 90, 1);


        xtimer_sleep(MAINLOOP_DELAY);
    }

    return NULL;
}


int main(void)
{
    puts("RIOT SUIT update example application");

    int current_slot = riotboot_slot_current();
    if (current_slot != -1) {
        /* Sometimes, udhcp output messes up the following printfs.  That
         * confuses the test script. As a workaround, just disable interrupts
         * for a while.
         */
        irq_disable();
        printf("running from slot %d\n", current_slot);
        printf("slot start addr = %p\n", (void *)riotboot_slot_get_hdr(current_slot));
        riotboot_slot_print_hdr(current_slot);
        irq_enable();
    }
    else {
        printf("[FAILED] You're not running riotboot\n");
    }

#if defined(MODULE_PERIPH_GPIO_IRQ) && defined(BTN0_PIN)
    /* initialize a button to manually trigger an update */
    gpio_init_int(BTN0_PIN, BTN0_MODE, GPIO_FALLING, cb, NULL);
#endif

/* START SPARTA SPECIFIC */
    

    puts("+------------Initializing Sensors and Screen --------+");
    switch (bmx280_init(&dev, &bmx280_params[0])) {
        case BMX280_ERR_BUS:
            puts("[Error] Something went wrong when using the I2C bus");
            return 1;
        case BMX280_ERR_NODEV:
            puts("[Error] Unable to communicate with any BMX280 device");
            return 1;
        default:
            /* all good -> do nothing */
            break;
    }

/* initialize the display */

    /* Initialize to SPI */
    puts("Initializing to SPI.\n");
    ucg_SetPins(&ucg, pins, pins_enabled);
    ucg_SetDevice(&ucg, SPI_DEV(TFT_DEV_SPI));
    ucg_Init(&ucg, TFT_DISPLAY, TFT_DISPLAY_EXT, ucg_com_riotos_hw_spi);
    ucg_SetRotate90(&ucg);

    /* Initialize the display */
    puts("Initializing display.\n");
    ucg_SetFontMode(&ucg, UCG_FONT_MODE_TRANSPARENT);
    ucg_SetFont(&ucg, ucg_font_helvB12_tf);
    /* start drawing in a loop */
    puts("Drawing on screen.");
    /* Initial Screen Setup*/
    ucg_ClearScreen(&ucg);
    ucg_SetFontMode(&ucg, UCG_FONT_MODE_SOLID);
    _draw_riot_logo(&ucg, 16, 14);
    _draw_app_name(&ucg);
    _draw_riotboot(&ucg);
/*
    for (int y = 0; y < 48; y++) {
                    for (int x = 0; x < 96; x++) {
                        uint32_t offset = (x + (y * 96)) * 3;

                        ucg_SetColor(&ucg, 0, logo[offset + 0], logo[offset + 1], logo[offset + 2]);
                        ucg_DrawPixel(&ucg, x, y);
                    }
                }
*/ 

    puts("Initialization successful\n");
/* STOP SPARTA SPECIFIC */

    /* start suit coap updater thread */
    suit_coap_run();

    /* start nanocoap server thread */
    thread_create(_nanocoap_server_stack, sizeof(_nanocoap_server_stack),
                  THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST,
                  _nanocoap_server_thread, NULL, "nanocoap server");

   /* start sensor reading thread */
    thread_create(_sensor_stack, sizeof(_sensor_stack),
                  THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST,
                  _sensor_thread, NULL, "sensor readins");


    /* the shell contains commands that receive packets via GNRC and thus
       needs a msg queue */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    puts("Starting the shell");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}

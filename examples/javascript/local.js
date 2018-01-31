/*
 * Copyright (C) 2018 Inria Emmanuel Baccelli <emmanuel.baccelli@inria.fr>
 *               2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


//#0      ACT_SWITCH      LED(orange)
//#1      SENSE_BTN       Button(SW0)
//#2      ACT_ANY PWM(PA16)
//#3      ACT_ANY PWM(PA18)
//#4      SENSE_ANALOG    ADC0_1(PA06)
//#5      SENSE_ANALOG    ADC0_2(PA07)

saul.get_by_name = function (name) {
    var res;
    switch (name) {
        case "led":
            res = saul._find_name("LED(orange)");
            break;
        case "button":
            res = saul._find_name("Button(SW0)");
            break;
        case "buzzer":
            res = saul._find_name("PWM(PA18)");
            break;
        case "brightness":
            res = saul._find_name("ADC0_1(PA06)");
            break;
        case "sound":
            res = saul._find_name("ADC0_2(PA07)");
            break;
    }
    saul.set_methods(res);
    return res;
}

coap._register_handler = coap.register_handler;
coap.register_handler = function(path, methods, callback) {
    var _callback = function(method) {
        var res = callback(method);

        if (res == false) {
            this.handler.unregister();
        }

        return coap.code.CHANGED;
    }
    coap._register_handler(path, methods, _callback);
}

coap.request = coap.request_sync;

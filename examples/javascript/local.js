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
            res = saul._find_name("LED(red)");
            break;
//        case "brightness":
//            res = saul._find_name("isl29020");
//            break;
//        case "gyrometer":
//            res = saul._find_name("l3g4200d");
//            break;
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

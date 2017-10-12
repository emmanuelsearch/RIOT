this.ledorange = saul.get_by_name("led");

value = 0;
count = 10;

this.blink = function () {
    if (count > 0) {
        value = (value + 1) % 2;
        this.ledorange.write(value);
        t = timer.setTimeout(this.blink, 1000000);
        count = count -1;
    }
}

this.blink();
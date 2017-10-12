this.ledorange = saul.get_by_name("led");
this.button1 = saul.get_by_name("button");

value = 0;
count = 16;

this.blink = function () {
if (count > 0) {
    value = (value + 1) % 2;
    this.ledorange.write(value);
    t = timer.setTimeout(this.blink, 500000);
    count = count -1;
   }
}

this.button1.on_threshold(0, this.blink, saul.op.EQ);
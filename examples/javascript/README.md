### About

This example enables to execute arbitrary Javascript directly from the command line on the RIOT shell. The example uses [Jerryscript](https://github.com/jerryscript-project/jerryscript).

Still WIP: 

- For now it only works on Cortex M boards, and depending on the CPU, you may need to tweek line 9 in RIOT/pkg/Makefile.jerryscript

- Expect some issues with PyTerm which may interpret the first `;` as the end of the script command

- Except in the `print` instruction in your script, you may have to replace single brackets ' with \'.

### How to build


Type `make flash term`. You should then land in the RIOT shell.
Note: you may have to type `reboot` or to press `RESET` on the board after the flash.


### Running JerryScript Hello World! example

In the RIOT shell, `help` will provide the list of avilable commands.

The `script` command will run the test script code that you input in the commant line.
Some examples of scripts you can try:
```
script print ('hello');
```
```
script var x = Math.sqrt (64); var txt = \'\'; while (x>1) { txt += --x + \'\\n\';} print (txt);
```
```
script var person = { fname:\'John\', lname:\'Doe\', age:25 }; var text = \'\'; var x; for (x in person) { text += person[x] + \'\\n\'; } print (text);
```

Remark: outside of the print command, you may have to replace single brackets ' with \'.


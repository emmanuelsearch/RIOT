# Build, flash & run a .js example

plug your SAMR21 board via USB, then in the RIOT folder:

```
cd examples/javascript
BOARD=samr21-xpro make flash term
```

then use the script command in the RIOT shell to try out simple .js scripts
(following this [README](https://github.com/RIOT-OS/RIOT/tree/master/examples/javascript)).


# Demo of Javascript CoAP resource



### Prerequisites

- Hardware needed: 2 two boards and a USB hub (highly recommended: the hub should have its own power supply)
- Vagrant VM must have display capability and Firefox installed (you may need to install xauth, see this [guide](https://help.ubuntu.com/community/ServerGUI))
- Firefox must have the [Copper](https://addons.mozilla.org/en-US/firefox/addon/copper-270430/) add-on installed
- Host OS must have a Xserver to display Firefox running in Vagrant VM (see this [guide](https://coderwall.com/p/ozhfva/run-graphical-programs-within-vagrantboxes))
- In the RIOT folder, add and fetch a remote repository

```
git remote add emmanuel https://github.com/emmanuelsearch/RIOT.git
git fetch emmanuel
```


### Step 1: Build, flash & run a border router

plug a SAMR21 board via USB. In the RIOT folder: 

```
git checkout -b 2017.04-branch origin/2017.04-branch
cd examples/gnrc\_border\_router
BOARD=samr21-xpro make flash term
```

In another terminal in your VM, add a route to the IPv6 prefix used by the border router:

```
sudo ip address add 2001:db8:1::a/64 dev tap0
``` 

### Step 2: Build, flash & run a CoAP node with a .js resource

Open another terminal in your VM. Plug another SAMR21 board via USB, then navigate in the RIOT folder:

```
git checkout emmanuel/js-coap-samr21
cd examples/script-coap
make list-ttys
```

Check the serial and the port of the second SAMR21 board.

```
$ make list-ttys
/sys/bus/usb/devices/1-1: Atmel Corp. EDBG CMSIS-DAP serial: 'ATML2127031800001966', tty(s): ttyACM0
/sys/bus/usb/devices/1-2: Atmel Corp. EDBG CMSIS-DAP serial: 'ATML2127031800001921', tty(s): ttyACM1
```

Then build, flash and open a RIOT shell on the second SAMR21 board:

```
BOARD=samr21-xpro PORT=/dev/ttyACM1 SERIAL=ATML2127031800001921 make flash term
```

The RIOT shell then prints the IPv6 address configuration of the node (if needed, press the reset button on the board):

``` 
> RIOT CoAP Javascript Demo
> Waiting for address autoconfiguration...
> Configured network interfaces:
> Iface  6   HWaddr: 6a:96  Channel: 26  Page: 0  NID: 0x23
>            Long HWaddr: 79:76:61:44:ce:b0:ea:96 
>            TX-Power: 0dBm  State: IDLE  max. Retrans.: 3  CSMA Retries: 4 
>            ACK_REQ  CSMA  MTU:1280  HL:64  6LO  RTR  IPHC  
>            Source address length: 8
>            Link type: wireless
>            inet6 addr: ff02::1/128  scope: local [multicast]
>            inet6 addr: fe80::7b76:6144:ceb0:ea96/64  scope: local
>            inet6 addr: 2001:db8::7b76:6144:ceb0:ea96/64  scope: global
>            inet6 addr: ff02::1:ffb0:ea96/128  scope: local [multicast]
>            
> (empty initial script)
> 
``` 

The link-local IPv6 address of the node in the case above is ```fe80::7b76:6144:ceb0:ea96``` and its routable address is ```2001:db8::7b76:6144:ceb0:ea96``` (check yours which will be different).

### Step 3: Ping

From the RIOT shell on the border router (see Step 1) ping the link local address of the CoAP node:

```
ping6 fe80::7b76:6144:ceb0:ea96
```

From another terminal on your VM, ping the global address of the CoAP node:

```
ping6 2001:db8::7b76:6144:ceb0:ea96
```

### Step 4: PUT on CoAP resources

From your VM, open a Firefox browser:

```
ssh -X -p 2222 vagrant@localhost firefox
```
The password is ```vagrant```

Then from your Firefox browser window, go to the URL based on the global address of your CoAP node (replace with your address):

```
coap://[2001:db8::7b76:6144:ceb0:ea96]
```

The Copper CoAP interface should appear. Click on ```Discover```(top left button in the browser window).

A resource named ```script``` should appear on the left. Click on it, then on the ```Outgoing``` button in the center of the page, type in your script, for instance ```print('here we go!');``` and check the result on the RIOT shell running on the CoAP node (see Step 2):

```
> new script:"print('here we go!');"
here we go!
here we go!
...
```





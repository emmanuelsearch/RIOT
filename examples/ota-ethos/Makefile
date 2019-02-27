# name of your application
APPLICATION = gnrc_border_router

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := airfy-beacon arduino-duemilanove arduino-mega2560 \
                             arduino-uno b-l072z-lrwan1 blackpill bluepill calliope-mini \
                             cc2650-launchpad cc2650stk hifive1 maple-mini \
                             mega-xplained microbit msb-430 msb-430h \
                             nrf51dk nrf51dongle nrf6310 nucleo-f031k6 nucleo-f042k6 \
                             nucleo-f303k8 nucleo-l031k6 nucleo-f030r8 \
                             nucleo-f070rb nucleo-f072rb nucleo-f103rb \
                             nucleo-f302r8 nucleo-f334r8 nucleo-l053r8 \
                             nucleo-l073rz opencm904 saml10-xpro saml11-xpro \
                             spark-core stm32f0discovery telosb waspmote-pro \
                             weio wsn430-v1_3b wsn430-v1_4 yunjia-nrf51822 z1

# The following boards do not have an available UART
BOARD_BLACKLIST += mips-malta pic32-wifire pic32-clicker ruuvitag thingy52

# Select the boards with riotboot feature
FEATURES_REQUIRED += riotboot

# Set stack size to something (conservatively) enormous
CFLAGS += -DTHREAD_STACKSIZE_MAIN=4096 -g 
CFLAGS += -DGCOAP_PDU_BUF_SIZE=128 -DGNRC_PKTBUF_SIZE=2048

# use ethos (ethernet over serial) for network communication and stdio over
# UART, but not on native, as native has a tap interface towards the host.
ifeq (,$(filter native,$(BOARD)))
  GNRC_NETIF_NUMOF := 2
  USEMODULE += ethos

  # ethos baudrate can be configured from make command
  ETHOS_BAUDRATE ?= 115200
  CFLAGS += -DETHOS_BAUDRATE=$(ETHOS_BAUDRATE) -DUSE_ETHOS_FOR_STDIO
else
  GNRC_NETIF_NUMOF := 2
  TERMFLAGS += -z [::1]:17754
  USEMODULE += socket_zep
endif

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for 6LoWPAN border router
USEMODULE += gnrc_sixlowpan_border_router_default
# USEMODULE += gnrc_ipv6_router_default
USEMODULE += gcoap
USEMODULE += sock_util
# Add forwarding table
USEMODULE += fib
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

USEMODULE += od
USEMODULE += fmt

# include UHCP client
USEMODULE += gnrc_uhcpc

# Optionally include RPL as a routing protocol. When includede gnrc_uhcpc will
# configure the node as a RPL DODAG root when receiving a prefix.
#USEMODULE += gnrc_rpl

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1
TAP ?= tap0
IPV6_PREFIX ?= 2001:db8::/64

ifneq ($(BUILD_IN_DOCKER),1)
  all: riotboot
endif

ifeq (native,$(BOARD))
TERMDEPS += uhcpd-daemon

.PHONY: uhcpd-daemon

uhcpd-daemon: host-tools
	$(RIOTTOOLS)/uhcpd/bin/uhcpd $(TAP) $(IPV6_PREFIX) &
else
# We override the `make term` command to use ethos
TERMPROG ?= sudo sh $(RIOTTOOLS)/ethos/start_network.sh
TERMFLAGS ?= $(PORT) $(TAP) $(IPV6_PREFIX)

# We depend on the ethos host tools to run the border router, we build them
# if necessary
TERMDEPS += host-tools
endif

include $(RIOTBASE)/Makefile.include

.PHONY: host-tools

host-tools:
	$(Q)env -u CC -u CFLAGS make -C $(RIOTTOOLS)

# Set a custom channel if needed
ifneq (,$(filter cc110x,$(USEMODULE)))          # radio is cc110x sub-GHz
  DEFAULT_CHANNEL ?= 0
  CFLAGS += -DCC110X_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
else
  ifneq (,$(filter at86rf212b,$(USEMODULE)))    # radio is IEEE 802.15.4 sub-GHz
    DEFAULT_CHANNEL ?= 5
    CFLAGS += -DIEEE802154_DEFAULT_SUBGHZ_CHANNEL=$(DEFAULT_CHANNEL)
  else                                          # radio is IEEE 802.15.4 2.4 GHz
    DEFAULT_CHANNEL ?= 26
    CFLAGS += -DIEEE802154_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
  endif
endif

# Make 'flash' and 'flash-only' work without specific command.
# This is currently hacky as there is no way of specifiying a FLASHFILE
ifneq ($(BUILD_IN_DOCKER),1)
  all: riotboot/combined-slot0
endif
# openocd
ELFFILE = $(BINDIR_APP)-slot0-combined.bin
# edbg
HEXFILE = $(BINDIR_APP)-slot0-combined.bin
# murdock uses ':=' to get the flashfile variable so should also be overwritten
FLASHFILE = $(BINDIR_APP)-slot0-combined.bin

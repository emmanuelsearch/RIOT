# application name
APPLICATION = riot_jerry

# default BOARD enviroment
#BOARD ?= samr21-xpro
BOARD ?= iotlab-m3

# path to the RIOT base directory
RIOTBASE ?= $(CURDIR)/../..

# path to the application directory
APPDIR ?= $(CURDIR)

# path to the binary directory
BINDIR ?= $(CURDIR)/bin/

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Set stack size to something big
CFLAGS += -DTHREAD_STACKSIZE_MAIN=9092

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

#INCLUDES += -I$(JERRYDIR)/jerry-core/

# Add the shell and some shell commands
USEMODULE += shell
USEMODULE += shell_commands

# Add the package for Jerryscript
USEPKG += jerryscript


include $(RIOTBASE)/Makefile.include

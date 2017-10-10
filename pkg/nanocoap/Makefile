PKG_NAME=nanocoap
PKG_URL=https://github.com/kaspar030/sock
PKG_VERSION=33d475d360e7cd07a44a36b598a198a2f188ca00
PKG_LICENSE=LGPL-2.1

.PHONY: all

all: git-download
	@cp Makefile.nanocoap $(PKG_BUILDDIR)/nanocoap/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)/nanocoap

include $(RIOTBASE)/pkg/pkg.mk

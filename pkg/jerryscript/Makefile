PKG_NAME=jerryscript
PKG_URL=https://github.com/jerryscript-project/jerryscript.git
PKG_VERSION=e993ee6cdc955218a534cb397da9e95eeaf8b5c8
PKG_LICENSE=Apache-2.0

# CFLAGS += -Wno-strict-aliasing

.PHONY: all

all: git-download
#	mkdir -p $(PKG_BUILDDIR)/jerryscript/targets/riot
	@cp Makefile.jerryscript $(PKG_BUILDDIR)/Makefile
	$(MAKE) -C $(PKG_BUILDDIR)
	cp $(PKG_BUILDDIR)/riot/lib/libjerry-core.a ${BINDIR}/libjerrycore.a

include $(RIOTBASE)/pkg/pkg.mk

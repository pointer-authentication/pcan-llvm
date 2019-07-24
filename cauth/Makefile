#
# Author: Hans Liljestrand <hans.liljestrand@aalto.fi>
# Copyright: Secure Systems Group, Aalto University, https://ssg.aalto.fi
#
# This code is released under Apache 2.0 license
#

default: all

include Makefile.common

TESTS := arrays
TESTS += hack01

all: tests

$(LINARO_SHARED):
	mkdir -p $(LINARO_SHARED)

tests: $(LINARO_SHARED) $(TESTS)
	cp run_all.sh $(LINARO_SHARED)

.PHONY: $(TESTS)
$(TESTS):
	make -C $@ send2guest

clean:: $(TESTS:=.clean)
	if [ -d $(LINARO_SHARED) ];then rm -f $(LINARO_SHARED)/*.out $(LINARO_SHARED)/*.sh; fi

$(TESTS:=.clean): %.clean :
	make -C $* clean

# vim:ft=make

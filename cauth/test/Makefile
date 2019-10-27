#
# Author: Hans Liljestrand <hans.liljestrand@aalto.fi>
# Copyright: Secure Systems Group, Aalto University, https://ssg.aalto.fi
#
# This code is released under Apache 2.0 license
#


all: tests

include Makefile.common

DEST ?= build

TESTS = $(filter-out build/,$(wildcard */))

DEST_TEST = $(addprefix $(DEST)/,$(TESTS:/=.out))

$(DEST):
	mkdir -p $(DEST)

tests: $(DEST) $(DEST_TEST)
	cp run_all.sh $(DEST)

$(DEST_TEST): $(DEST)/%.out : $(DEST)
	make -C $*
	cp $*/$*.out $@

clean:: $(TESTS:=.clean)
	if [ -d $(DEST) ];then rm -f $(DEST)/*.out $(DEST)/*.sh; fi
	if [ -d $(DEST) ];then rmdir -p --ignore-fail-on-non-empty $(DEST); fi

$(TESTS:=.clean): %.clean :
	make -C $* clean

# vim:ft=make

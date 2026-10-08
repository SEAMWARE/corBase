#
# FILE            makefile
#
# AUTHOR          Ken Zangelin
#
# Copyright 2026 Seamware
# SPDX-License-Identifier: Apache-2.0
#
#
# corBase - the foundation every other library of the stack stands on: string,
# file and time helpers, the error stack, and the library log - the callback
# through which a library reaches the log of the executable it runs in.
#
# Every library in this stack is a SIBLING repo - `-I..` and `../<name>/lib<name>.a`
# is the layout, and it is part of the build contract rather than a convenience.
#
LIB_SO        = libcorBase.so
LIB           = libcorBase.a
CC            = gcc
INCLUDE       = -I..
DFLAGS        =
#
# EXTRA_CFLAGS - the hook for a caller that needs to ADD flags to this build.
# Not DFLAGS: `make DFLAGS=...` REPLACES it, and a `DFLAGS +=` here would be
# ignored along with it, so a caller adding one flag would drop every default.
#
CFLAGS        = -O2 -Wall -Wextra -Werror -fPIC -fstack-protector-strong $(DFLAGS) $(INCLUDE) -MMD -MP $(EXTRA_CFLAGS)

LIB_SOURCES   = corBaseInit.c          \
                corLibLog.c            \
                corCharCount.c         \
                corCharChecksum.c      \
                corProgName.c          \
                corStringSplit.c       \
                corStringArrayJoin.c   \
                corStringArrayLookup.c \
                corStringInArray.c     \
                corStringSort.c        \
                corTime.c              \
                corTimeIso.c           \
                corFileRead.c          \
                corBaseVersion.c       \
                corFloatTrim.c         \
                corFileSuffixExtract.c \
                CorErrorStack.c        \
                corStringReplace.c     \
                corCpuCount.c          \
                corFileReadInto.c      \
                corMemoryLimit.c       \
                corCo.c                \
                corCoLoop.c            \
                corCrc32c.c

BUILD        ?= debug

#
# Traces (COR_LIB_T) are compiled in for a debug build only - see corLibLog.h.
#
ifeq ($(BUILD),debug)
CFLAGS       += -DCOR_T_ON
endif
OBJDIR        = obj/$(BUILD)
OBJECTS       = $(LIB_SOURCES:%.c=$(OBJDIR)/%.o)
DEPS          = $(OBJECTS:.o=.d) $(OBJDIR)/corBaseTest.d

#
# corBaseTest - a smoke test of the library, built with it so it can never rot.
# It stays in obj/: it is not a tool, and nothing installs it.
#
TEST          = $(OBJDIR)/corBaseTest
TEST_LIBS     = -lrt -lm

#
# The tools - small number converters (hex/ascii/int), one source file each.
# `install` puts them in bin/.
#
TOOL_NAMES    = atoh atoi htoa htoi itoh
TOOLS         = $(TOOL_NAMES:%=$(OBJDIR)/tools/%)

#
# The archive and the shared library are built PER FLAVOUR, in $(OBJDIR), and then STAGED to the repo
# root, where every consumer links them (../corBase/libcorBase.a). Unconditionally, on every build:
# built in place, a debug archive is newer than obj/release/*.o, so `make BUILD=release` after a debug
# build found nothing to do and left the other flavour's archive in place. A plain cp, not cp -p:
# the staged file gets a new mtime, so whatever links it relinks. Copied and renamed, so a process
# that has the .so mapped keeps the old file.
#
all: $(OBJDIR)/$(LIB) $(OBJDIR)/$(LIB_SO) $(TEST) $(TOOLS)
	@cp -f $(OBJDIR)/$(LIB) $(LIB).tmp && mv -f $(LIB).tmp $(LIB)
	@cp -f $(OBJDIR)/$(LIB_SO) $(LIB_SO).tmp && mv -f $(LIB_SO).tmp $(LIB_SO)

#
# The staged files by name - `make libcorBase.a` stages the current flavour's archive.
#
$(LIB): $(OBJDIR)/$(LIB)
	@cp -f $< $@.tmp && mv -f $@.tmp $@

$(LIB_SO): $(OBJDIR)/$(LIB_SO)
	@cp -f $< $@.tmp && mv -f $@.tmp $@

#
# $(OBJDIR)/.flags - rebuild when the COMPILE LINE changes
#
# A flag change is invisible to every timestamp: the sources are older than the
# objects and make sees nothing to do, so the build silently keeps objects
# compiled with the previous flags. This records them and makes the objects
# depend on the record. The compiler is part of the line: `make pgo` passes
# CC="gcc -fprofile-use=...".
#
$(OBJDIR)/.flags: FORCE
	@mkdir -p $(OBJDIR)
	@echo '$(CC) $(CFLAGS)' | cmp -s - $@ || echo '$(CC) $(CFLAGS)' > $@

$(OBJDIR)/%.o: %.c $(OBJDIR)/.flags
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

#
# Removed first: `ar r` replaces and adds but never removes, so an object that
# is no longer built stays in the archive forever, and the next link quietly
# uses code that is not in the tree any more.
#
$(OBJDIR)/$(LIB): $(OBJECTS)
	@rm -f $@
	ar rcs $@ $(OBJECTS)

$(OBJDIR)/$(LIB_SO): $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS) -lm

$(TEST): $(OBJDIR)/corBaseTest.o $(OBJDIR)/$(LIB)
	$(CC) -o $@ $< $(OBJDIR)/$(LIB) $(TEST_LIBS)

#
# arm64-test - the smoke test built for aarch64 and run under QEMU (packages gcc-aarch64-linux-gnu and
# qemu-user). Objects AND archive under obj/arm64 - the test target alone, so nothing is staged: the
# top-level library is what every sibling links, and an aarch64 one there breaks the next x86 build of
# all of them.
#
ARM64_CC      = aarch64-linux-gnu-gcc

arm64-test:
	$(MAKE) BUILD=arm64 CC=$(ARM64_CC) obj/arm64/corBaseTest
	qemu-aarch64 -L /usr/aarch64-linux-gnu obj/arm64/corBaseTest

define TOOL_RULE
$(OBJDIR)/tools/$(1): tools/$(1)/$(1).c $(OBJDIR)/.flags
	@mkdir -p $(OBJDIR)/tools
	$(CC) $(CFLAGS) -o $$@ $$<
endef
$(foreach t,$(TOOL_NAMES),$(eval $(call TOOL_RULE,$(t))))

#
# install - the library needs nothing copied: consumers compile with `-I..` and
# link `../corBase/libcorBase.a` straight out of the checkout. Only the tools go
# to bin/.
#
install: all
	@mkdir -p bin
	cp $(TOOLS) bin/

di: all install

ci: clean install

clean:
	rm -rf obj bin $(LIB) $(LIB_SO) $(LIB).tmp $(LIB_SO).tmp *.o *.d *.gcno *.gcda

FORCE:

.PHONY: arm64-test all install di ci clean FORCE

-include $(DEPS)

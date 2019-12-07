CC = i386-elf-gcc
MAKE = make

CFLAGS += -g -m32 -Wall -fno-builtin -fno-stack-protector
ASFLAGS +=
LDFLAGS += -nostdlib -static
CPPFLAGS += -nostdinc

SRCS = $(wildcard $(SRCDIR)/*.S) $(wildcard $(SRCDIR)/*.c)

BLDDIR = ./build
IMGDIR = ./img
SRCDIR = ./src
SCRDIR = ./scripts

IMAGE = rose

# boot.o must be first
OBJS := $(BLDDIR)/boot.o
OBJS += $(filter-out $(BLDDIR)/boot.o,\
	$(patsubst $(SRCDIR)/%.S,$(BLDDIR)/%.o,$(filter %.S,$(SRCS))))
OBJS += $(patsubst $(SRCDIR)/%.c,$(BLDDIR)/%.o,$(filter %.c,$(SRCS)))
DEPS := $(patsubst $(BLDDIR)/%.o,$(BLDDIR)/%.d,$(OBJS))

all: mkdir rose

rose: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -Ttext=0x100000 -o $(IMGDIR)/$(IMAGE)
	$(SCRDIR)/makeimage.sh

$(BLDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -c -o $@ $<

$(BLDDIR)/%.o: $(SRCDIR)/%.S
	$(CC) $(CPPFLAGS) $(ASFLAGS) -MMD -c -o $@ $<

.PHONY: rose iwyu clean mkdir

iwyu:
	$(MAKE) -k 'CC=include-what-you-use \
		-Xiwyu --no_fwd_decls \
		-Xiwyu --mapping_file=.rose.imp'

clean:
	rm -f $(BLDDIR)/* $(IMGDIR)/$(IMAGE)

mkdir:
	@mkdir -p $(IMGDIR) $(BLDDIR)

-include $(DEPS)

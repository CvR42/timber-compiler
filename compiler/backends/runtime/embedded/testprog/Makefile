
#TARGETS_HOST = program-single              program-multi              hough
#TARGETS_TM   = program-single-trimedia.out program-multi-trimedia.out hough-trimedia.out

#TARGETS_HOST = benchmark-alltoall
#TARGETS_TM   = benchmark-alltoall-trimedia.out

TARGETS_HOST = benchmark-peertopeer
TARGETS_TM   = benchmark-peertopeer-trimedia.out

TARGETS = $(TARGETS_HOST) $(TARGETS_TM)

TMCC = tmcc

TCS_PATH=/usr/local/tcs
TMDRV_PATH=$(HOME)/trimedia/tmdrvsrc_1.0
ENSEMBLE_PATH=..

LOADLIBES = -lensemble-host-i386 -ltmdrv -lload
CPPFLAGS  = -I$(ENSEMBLE_PATH)
CFLAGS    = -Wall -O6 -g
LDFLAGS   = -L$(TMDRV_PATH)/linux/lib -L$(TCS_PATH)/lib/Linux -L$(ENSEMBLE_PATH)

TMCPPFLAGS  = -I$(TMDRV_PATH)/dsp/include -I$(ENSEMBLE_PATH)
TMCFLAGS    = -el -O5 -K
TMLDFLAGS   = -el -tmconfig=$(TMDRV_PATH)/dsp/lib/tmconfig.linux-i386 -host MacOS -L$(ENSEMBLE_PATH)
TMLOADLIBES = -lensemble-trimedia

TMCOMPILE.c = $(TMCC) $(TMCFLAGS) $(TMCPPFLAGS) $(TMTARGET_ARCH) -c
TMLINK.o    = $(TMCC) $(TMLDFLAGS) $(TARGET_ARCH)

# pattern rules (Pentium)

%.o : %.c
	$(COMPILE.c) $< $(OUTPUT_OPTION)

% : %.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

# pattern rules (TriMedia)

%-trimedia.o : %.c
	$(TMCOMPILE.c) $< $(OUTPUT_OPTION)

%-trimedia: %-trimedia.o
	$(TMLINK.o) $^ $(TMLOADLIBES) $(TMLDLIBS) -o $@

%-trimedia.out: %-trimedia
	cp $^ $@
	tmstrip -k _tmMsgStruct $@

# default rule

default: $(TARGETS) $(ENSEMBLE_PATH)/libensemble-host-i386.a $(ENSEMBLE_PATH)/libensemble-trimedia.a
	fix-permissions $(TARGETS_HOST)

clean :
	$(RM) *~ *.o $(TARGETS) *.s *.t *.pgm

indent :
	indent -npsl *.[ch]

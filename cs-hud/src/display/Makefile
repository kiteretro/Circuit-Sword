OBJS=display_tester.o \
  icon.o \
	display.o \
	display_tools.o \
	common/backgroundLayer.o \
	common/font.o \
	common/key.o \
	common/imageGraphics.o \
	common/imageKey.o \
	common/imageLayer.o \
	common/image.o \
	common/loadpng.o

BIN=display_tester

CFLAGS+=-Wall -g -O3 -Icommon $(shell libpng-config --cflags)
LDFLAGS+=-L/opt/vc/lib/ -lbcm_host -lm $(shell libpng-config --ldflags)

INCLUDES+=-I/opt/vc/include/ -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux

all: $(BIN)

%.o: %.c
	@rm -f $@
	$(CC) $(CFLAGS) $(INCLUDES) -g -c $< -o $@ -Wno-deprecated-declarations

$(BIN): $(OBJS)
	$(CC) -o $@ -Wl,--whole-archive $(OBJS) $(LDFLAGS) -pthread -Wl,--no-whole-archive -rdynamic

clean:
	@rm -f $(OBJS)
	@rm -f $(BIN)

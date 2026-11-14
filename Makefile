EE_BIN = build/PS2_GAME_STATION.ELF
EE_OBJS = build/main.o
EE_LIBS = -ldebug -lpad -lkernel
EE_INCS += -I$(PS2SDK)/ee/include -I$(PS2SDK)/common/include
EE_CFLAGS += -O2 -G0 -Wall

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal

build/main.o: src/main.c
	mkdir -p build
	$(EE_CC) $(EE_CFLAGS) $(EE_INCS) -c $< -o $@

clean:
	rm -rf build

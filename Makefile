# Native Q2 build: glibc + ALSA from the Shanling Q2 V1.32 sysroot (as in Groovebox).
SYSROOT ?= $(HOME)/.cache/q2/sysroot
Q2_CC ?= clang
Q2_LD ?= ld.lld
Q2_FLAGS = --target=mipsel-linux-gnu --sysroot=$(SYSROOT) -march=mips32r2 -mabi=32 -mfp64 -mnan=2008 -mabs=2008 -fno-strict-aliasing
Q2_LDFLAGS = -m elf32ltsmip -e __start --dynamic-linker /lib/ld-linux-mipsn8.so.1 --image-base=0x400000 -z noexecstack --hash-style=sysv --build-id=none -s
SRC = src/app.c src/game.c src/render.c
Q2_SRC = $(SRC) src/platform_q2.c src/audio.c src/crt-q2.c
Q2_OBJECTS = $(patsubst %.c,build/q2/%.o,$(Q2_SRC))
SPRITES = $(addprefix assets/sprites/,cat.png diamond.png martini.png ipod.png note.png headphones.png)
SOUNDS = $(addprefix assets/sounds/,select.wav match.wav combo.wav invalid.wav)
CARD = dist/.diamond-crush

all: dist/q2-diamond-crush.zip

build/sprites.inc: tools/pack_sprites.py $(SPRITES)
	@mkdir -p $(@D)
	python3 tools/pack_sprites.py $@

.PHONY: check-sysroot
check-sysroot:
	@test -f "$(SYSROOT)/usr/include/alsa/asoundlib.h" -a -f "$(SYSROOT)/lib/libasound.so.2" || (echo "Missing Q2 ALSA sysroot: $(SYSROOT). Build it from the exact Shanling Q2 stock V1.32 ZIP using DiamondBond/q2-rockbox's tools/shanlingq2/mksysroot.sh (see README)." >&2; exit 1)

build/q2/src/render.o: build/sprites.inc
build/q2/%.o: %.c src/app.h src/game.h src/audio.h | check-sysroot
	@mkdir -p $(@D)
	$(Q2_CC) $(Q2_FLAGS) -O2 -Wall -Wextra -Werror -std=gnu11 -Isrc -Ibuild -c $< -o $@

$(CARD)/q2-diamond-crush: $(Q2_OBJECTS)
	@mkdir -p $(@D)
	$(Q2_LD) $(Q2_LDFLAGS) -o $@ $^ \
		$(SYSROOT)/lib/libc.so.6 $(SYSROOT)/lib/libpthread.so.0 \
		$(SYSROOT)/lib/libasound.so.2 $(SYSROOT)/lib/libgcc_s.so.1 \
		$(SYSROOT)/lib/ld-linux-mipsn8.so.1

$(CARD)/q2app: q2app
	@mkdir -p $(@D)
	cp $< $@

$(CARD)/sounds.stamp: $(SOUNDS)
	@mkdir -p $(CARD)/sounds
	cp $(SOUNDS) $(CARD)/sounds/
	@touch $@

$(CARD)/LICENSE: LICENSE
	@mkdir -p $(@D)
	cp $< $@

dist/q2-diamond-crush.zip: $(CARD)/q2-diamond-crush $(CARD)/q2app $(CARD)/LICENSE $(CARD)/sounds.stamp
	cd dist && zip -q q2-diamond-crush.zip .diamond-crush/q2-diamond-crush .diamond-crush/q2app .diamond-crush/LICENSE .diamond-crush/sounds/*.wav

build/test: tests/test.c $(SRC) src/app.h src/game.h build/sprites.inc
	@mkdir -p $(@D)
	$(CC) -std=c11 -O2 -Wall -Wextra -Werror -Isrc -Ibuild -o $@ tests/test.c $(SRC)
check: build/test
	./build/test

clean:
	rm -rf build dist

.PHONY: all check clean

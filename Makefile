Q2_CC ?= clang
Q2_LD ?= ld.lld
Q2_FLAGS = --target=mipsel-linux-gnu -march=mips32r2 -mabi=32 -mfp64 -mnan=2008 -mabs=2008 -mno-abicalls -fno-pic -G0 -ffreestanding -fno-builtin -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables
SRC = src/app.c src/game.c src/render.c
CARD = dist/.diamond-crush
all: dist/q2-diamond-crush.zip
build/q2/%.o: %.c src/app.h src/game.h
	@mkdir -p $(@D)
	$(Q2_CC) $(Q2_FLAGS) -Oz -Wall -Wextra -Werror -std=c11 -Isrc -c $< -o $@
$(CARD)/q2-diamond-crush: $(patsubst %.c,build/q2/%.o,$(SRC) src/platform_q2.c)
	@mkdir -p $(@D)
	$(Q2_LD) -m elf32ltsmip -static -e __start -s -z noexecstack -o $@ $^
$(CARD)/q2app: q2app
	@mkdir -p $(@D)
	cp $< $@
$(CARD)/LICENSE: LICENSE
	@mkdir -p $(@D)
	cp $< $@
dist/q2-diamond-crush.zip: $(CARD)/q2-diamond-crush $(CARD)/q2app $(CARD)/LICENSE
	cd dist && zip -q q2-diamond-crush.zip .diamond-crush/q2-diamond-crush .diamond-crush/q2app .diamond-crush/LICENSE
build/test: tests/test.c $(SRC) src/app.h src/game.h
	@mkdir -p $(@D)
	$(CC) -std=c11 -O2 -Wall -Wextra -Werror -Isrc -o $@ tests/test.c $(SRC)
check: build/test
	./build/test
clean:
	rm -rf build dist
.PHONY: all check clean

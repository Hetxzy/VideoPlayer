target = app
src = $(wildcard ./src/*.c)
obj = $(patsubst ./src/%.c, ./src/%.o,$(src))
CC = arm-linux-gcc
$(target): $(obj)
	$(CC) $(obj) -o $(target)  -I ./include/ -L ./lib -l jpeg -lfreetype -I ./include/freetype2 -pthread
	sudo cp $(target) /nfs/videoplayer
%.o: %.c
	$(CC) -c $< -o $@    -I ./include/ -L ./lib -l jpeg -lfreetype -I ./include/freetype2 -pthread
.PHONY = clean
clean:
	rm $(obj) $(target) -f


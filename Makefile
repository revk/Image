image.o: image.c
	gcc -O -c -o $@ $< -DLIB -I/usr/local/include -L/usr/local/lib

update:
	git submodule update --remote --merge

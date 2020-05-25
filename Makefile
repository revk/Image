image.o: image.c
	cc -O -c -o $@ $< -DLIB

update:
	git submodule update --remote --merge

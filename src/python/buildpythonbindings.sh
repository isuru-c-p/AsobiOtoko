swig -includeall -ignoremissing -python -I../ z80.i && \
gcc -shared -o _z80.so ../z80.c ../mmu.c ../SDL_Helper.c ../io.c ../gpu.c z80_wrap.c -I../ -I /usr/include/python2.6/ -lSDL

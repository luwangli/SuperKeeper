GCC = g++
CFLAGS = -std=gnu++11 -Wall -g
SEEFLAGS = -march=native -msse2
SRC += ../SpreadSketch/hash.c ../SpreadSketch/bitmap.c ../SpreadSketch/spreadsketch.c
FILES = main.out

all:$(FILES)

main.out:main.cpp $(SRC)
	$(GCC) $(CFLAGS) $(SEEFLAGS) -o main.out main.cpp $(SRC)

clean:
	rm $(all) -f *~ *.o *.out *.txt *.csv core

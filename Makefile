default:

objects = xmt-base.o luaxmt.so

xmt-base.o: xmt-base.c xmt-base.h
	#gcc xmt-base.c -lsndfile -o xmt
	gcc xmt-base.c -c

luaxmt.so: xmt-lua.c xmt-base.o
	gcc -Wall -shared -fPIC -o luaxmt.so  \
		-llua xmt-lua.c -lsndfile xmt-base.o

testing: testing.c xmt-base.o
	gcc testing.c xmt-base.o -lsndfile \
		-o testing  

all:
	make ${objects}
clean:
	rm -rf ${objects}

default:
	make all
objects = xmt-base.o xmt-samples.o xmt-patterns.o xmt-instruments.o


xmt-base.o: xmt-base.c xmt-base.h
	gcc $(@:.o=.c) -c
xmt-samples.o: xmt-samples.c xmt-base.h
	gcc $(@:.o=.c) -c
xmt-patterns.o: xmt-patterns.c xmt-base.h
	gcc $(@:.o=.c) -c
xmt-instruments.o: xmt-instruments.c xmt-base.h
	gcc $(@:.o=.c) -c

.PHONY: all clean install uninstall

luaxmt.so: xmt-lua.c $(objects)
	gcc -Wall -shared -fPIC -o luaxmt.so  \
		-llua xmt-lua.c -lsndfile $(objects)
all:
	make ${objects} luaxmt.so
	cp luaxmt.so xmt.lua examples
clean:
	rm -rf ${objects} luaxmt.so
	rm -rf examples/*.xm
	rm -rf examples/luaxmt.so examples/xmt.lua

install: luaxmt.so 
	cp luaxmt.so xmt.lua /usr/local/lib/lua/5.1

uninstall:
	cd /usr/local/lib/lua/5.1; rm luaxmt.so xmt.lua	

default:

xmt: xmt.c
	gcc xmt.c -lsndfile -o xmt

sndfile-test: samplewrite.c
	gcc samplewrite.c -o sndfile-test -lsndfile
xmt-lua: xmt_lua.c
	gcc -Wall -shared -fPIC -o soundpipe.so  \
		-llua xmt_lua.c -lsndfile


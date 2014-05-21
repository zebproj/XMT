default:

xmt: xmt.c
	gcc xmt.c -lsndfile -o xmt

sndfile-test: samplewrite.c
	gcc samplewrite.c -o sndfile-test -lsndfile


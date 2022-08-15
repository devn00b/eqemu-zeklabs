
HCC=$(shell ccache -V 2>/dev/null)
ifneq (,$(findstring version,$(HCC)))
	CC=ccache gcc-4.9
else
	CC=gcc-4.9
endif

OUT=-o 
LINKOUT=-o 
NOLINK=-c
LINKER=gcc-4.9
MYSQL_FLAGS=$(shell mysql_config --cflags)
MYSQL_LIB=$(shell mysql_config --libs)
DFLAGS=-DEQDEBUG=5 -D LOGINSERVER
WFLAGS=-Wall -Wuninitialized -Wwrite-strings -Wcast-qual -Wno-deprecated  -Wcomment -Wcast-align
COPTS=$(WFLAGS) -O2 -march=native -g -pthread -pipe -DEQLAUNCH $(DFLAGS) $(MYSQL_FLAGS)
LINKOPTS=$(COPTS) -rdynamic -L. -lstdc++ -ldl $(MYSQL_LIB) -lEQEmuAuthCrypto -lcryptopp -lz


include makefile.common

include .depend

.depend depend:
	echo $(SF) "" | sed -e 's#.obj/#../common/#g' -e 's/\.o/.cpp/g' | \
	xargs gcc -MM $(COPTS) >.depend


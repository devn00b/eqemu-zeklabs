
all: cleanipc
	
cleanipc: cleanipc.cpp
	g++ cleanipc.cpp -o cleanipc

spell_explorer: spell_explorer.cpp
	g++ -g spell_explorer.cpp -o spell_explorer

azone: azone/azone.cpp
	make -C azone

clean:
	rm -f cleanipc
	make -C azone clean


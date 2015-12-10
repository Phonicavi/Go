CFLAGS=-Wall -Wextra

CSTATIC=-static-libgcc -static-libstdc++

brown:	brown.cpp brown.h gtp.cpp gtp.h interface.cpp
	g++ $(CFLAGS) brown.cpp gtp.cpp interface.cpp -o brown $(CSTATIC) -std=c++11

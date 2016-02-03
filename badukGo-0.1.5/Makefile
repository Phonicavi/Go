NAME=BadukGO
CXX=g++
CXXFLAGS=--std=gnu++11 -Wall -Wno-unused -Ofast -flto -ffast-math
CSTATLIB=-static-libgcc 
CSTATSTD=-static-libstdc++

CXXSRCS=*.cpp
CXXPARAM=$(CSTATLIB) $(CSTATSTD) $(CXXFLAGS)


$(NAME): $(CXXSRCS) 
	$(CXX) $(CXXSRCS) -o $(NAME) $(CXXPARAM)

NAME=hara
CXX=g++
CXXFLAGS= --std=gnu++11 -Wall -Wno-unused -Ofast -flto
DEPS=make.dep
CXXSRCS=$(wildcard *.cpp)
CSTATIC=-static-libgcc -static-libstdc++
HSRCS=$(wildcard *.h)
OBJS=$(CXXSRCS:.cpp=.o)

.PHONY: all clean

all: $(NAME) $(DEPS)

$(NAME): $(OBJS) 
	$(CXX) $(CXXFLAGS) $(CSTATIC) -o $@ $^

clean:
	rm -f *.o make.dep

include $(DEPS)

make.dep: $(CXXSRCS) $(HSRCS)
	$(CXX) -MM $(CSTATIC) $(CXXSRCS) > make.dep

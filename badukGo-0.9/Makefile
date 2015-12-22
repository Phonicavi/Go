NAME=BadukGO
CXX=g++
CXXFLAGS= --std=gnu++11 -Wall -Wno-unused -Ofast -flto -static-libgcc -static-libstdc++
DEPS=make.dep
CXXSRCS=$(wildcard *.cpp)
HSRCS=$(wildcard *.h)
OBJS=$(CXXSRCS:.cpp=.o)

.PHONY: all clean

all: $(NAME) $(DEPS)

$(NAME): $(OBJS) 
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f *.o make.dep

include $(DEPS)

make.dep: $(CXXSRCS) $(HSRCS)
	$(CXX) -MM $(CXXSRCS) > make.dep

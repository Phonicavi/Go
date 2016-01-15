NAME=BadukGO
CXX=g++
CXXFLAGS= --std=gnu++11 -Wall -Wno-unused -Ofast -flto 
CSTATLIB=-static-libgcc 
CSTATSTD=-static-libstdc++
DEPS=make.dep
CXXSRCS=$(wildcard *.cpp)
HSRCS=$(wildcard *.h)
OBJS=$(CXXSRCS:.cpp=.o)


all: $(NAME) $(DEPS)

$(NAME): $(OBJS) 
	$(CXX) $(CXXFLAGS) -o $@ $^

include $(DEPS)

make.dep: $(CXXSRCS) $(HSRCS)
	$(CXX) -MM $(CXXSRCS) > make.dep

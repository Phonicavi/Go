NAME=Baduk
CXX=g++
CXXFLAGS= --std=gnu++11 -Wall -Wno-unused -Ofast -flto

$(NAME): *.cpp
	$(CXX) *.cpp -o $(NAME) $(CXXFLAGS)

clean:
	rm -f *.o
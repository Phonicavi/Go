#ifndef SIZEH
#define SIZEH
#include <string>
#include <sstream>

const int MAXSIZE = 19;
const int MAXSIZE2 = MAXSIZE*MAXSIZE;

const char COORDINATES[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T'};

const int handicap19[][9]= {
  {61, 301, 289, 73,  175, 187, 67, 295, 181},
  {61, 301, 289, 73,  175, 187, 67, 295, 0},
  {61, 301, 289, 73,  175, 187, 181, 0, 0},
  {61, 301, 289, 73,  175, 187, 0, 0, 0},
  {61, 301, 289, 73,  181, 0, 0, 0, 0},
  {61, 301, 289, 73,  0, 0, 0, 0, 0},
  {61, 301, 289, 0, 0, 0, 0, 0, 0},
  {61, 301, 0, 0, 0, 0, 0, 0, 0}
};

const int handicap13[][9]= {
  {43, 127, 121, 49,  82, 88, 46, 124, 85},
  {43, 127, 121, 49,  82, 88, 46, 124, 0},
  {43, 127, 121, 49,  82, 88, 85, 0, 0},
  {43, 127, 121, 49,  82, 88, 0, 0, 0},
  {43, 127, 121, 49,  85, 0, 0, 0, 0},
  {43, 127, 121, 49,  0, 0, 0, 0, 0},
  {43, 127, 121, 0, 0, 0, 0, 0, 0},
  {43, 127, 0, 0, 0, 0, 0, 0, 0}
};

const int handicap9[][9] = {
  {21, 61, 57, 25,  39, 43, 23, 59, 41},
  {21, 61, 57, 25,  39, 43, 23, 59, 0},
  {21, 61, 57, 25,  39, 43, 41, 0, 0},
  {21, 61, 57, 25,  39, 43, 0, 0, 0},
  {21, 61, 57, 25,  41, 0, 0, 0, 0},
  {21, 61, 57, 25,  0, 0, 0, 0, 0},
  {21, 61, 57, 0, 0, 0, 0, 0, 0},
  {21, 61, 0, 0, 0, 0, 0, 0, 0}
};

static void coord_to_char(int coord, std::string &response, int size){
  if(coord == 0){
    response.append("pass");
  }
  else{
    int y = (coord - 1)/size + 1;
    int x = (coord - 1) % size;
    std::string auxstring;
    std::stringstream auxstream;
    auxstream << COORDINATES[x];
    auxstream >> auxstring;
    response.append(auxstring);

    auxstream.clear();
    auxstream << y;
    auxstream >> auxstring;

    response.append(auxstring);
  }
}
#endif

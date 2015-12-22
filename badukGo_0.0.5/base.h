##ifndef __BASE_H__
#define __BASE_H__ 

#include <sstream>
#include <string>
#define MAXSIZE 19
#define MAXSIZE_2 361
#define BLACK 0
#define WHITE 1

const char COORDINATES[] ={
	'A','B','C','D','E','F','G','H','J','K'
	,'L','M','N','O','P','Q','R','S','T'
};

static void 
coord2char(int _coord, std::string &response, int size){
	if (!_coord){
		response.append("PASS");
	}else{
		int y = (_coord-1)/size+1;
		int x = (_coord-1)%size;

		std::string ts;
		std::stringstream ss;
		ss << COORDINATES[x];
		ss >> ts;
		response.append(ts);
		ss.clear();
		ss << y;
		ss >> ts;
		response.append(ts);
	}
}


#endif
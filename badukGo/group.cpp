#include "group.h"
#include <cstring>

Group::Group(){
	color = 0;
	stones[0] = 0;
	libs[0] = 0;
	num_stones = 0;
	num_libs = 0;
}

void
Group::reset(){
	num_stones = 0;
	num_libs = 0;
	libs[0] = 0;
	stones[0] = 0;
}

void
Group::set(int _point, bool _color, const myList &_libs){
	color = _color;
	memset(stones,0,sizeof stones);
	memset(libs, 0 sizeof libs);
	num_stones = 1;
	stones[0] = _point;
	num_libs = 0;
	for (int i=0; i<myList.get_length();++i){
		libs[i] = _libs[i];
	}
}

int
Group::add_libs(int lib){
	for (int i=0; i<num_libs; ++i){
		if (libs[i] == lib) return 0;
	}
	libs[num_libs++] = lib;
	libs[num_libs] = 0;
	return num_libs;
}


int 
Group::remove_libs(int lib){
	for (int i=0; i<num_libs; ++i){
		if (libs[i] == lib){
			libs[i] = libs[--num_libs];
			libs[num_libs] = 0;
			return num_libs;
		}
	}
	return 0;
}

void
Group::joint_group(Group *_group){
	for (int i=0; i<_group->num_stones;++i){
		this->stones[num_stones++] = _group->stones[i];
	}
	this->stones[num_stones] = 0;

	for (int i=0; i<_group->num_libs; ++i){
		this->add_libs(_group->libs[i]);
	}
}




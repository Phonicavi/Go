#ifndef __GROUP_H__
#define __GROUP_H__ 

#include "base.h"
// #include <iostream>
#include <algorithm>

/* 一个虚基类list支持以下操作    准备改掉*/
class myList{        
public:
	virtual void add_point(int p) = 0;
	virtual void remove_point (int p) = 0;
	virtual int operator[](int i) = 0;
	virtual int get_length() = 0;
};

class Group{
private:
	bool color;  // 黑0白1
	int stones[MAXSIZE_2];
	int num_stones;
	int libs[MAXSIZE_2*2/3];
	int num_libs;
public:
	Group();
	~Group(){};
	void reset();
	void set(int, bool, const myList&);

	int get_color(){return color;}
	int get_num_stones(){return num_stones;}
	int get_the_stone(int i){return stones[i];}
	int *get_stones(){return stones;}

	int get_num_libs(){return num_libs;}
	int get_the_lib(int i){return libs[i];}
	int *get_libs(){return libs;}

	int add_libs(int lib);
	int remove_libs(int lib);

	/* 合并棋串 */ 
	void joint_group(Group *_group);



	class StoneItr{ // 用于遍历棋串中所有的棋子
	protected:
		const int *it;
	public:
		StoneItr(){it = 0;}
		StoneItr(const Group *group){it = group->get_stones();}
		void operator++(){++it;}
		int operator*() const {return *it};
		operator bool() const {return *it != 0;}
	};

	class LibItr : public StoneItr{  // 用于遍历棋串所有的气
	public:
		LibItr(const Group *group){it = group->get_libs();}
	};
};


template<const int scale> class GroupSet{
protected:
	Group *groups[scale];
	int repeat[scale];
	int length;

public:
	GroupSet(){
		length = 0;
		groups[0] = 0;
	}
	~GroupSet(){};

	bool add_group(Group *_group){
		if (!_group) return false;
		for (int i=0; i<length; ++i){
			if (groups[i] == _group){
				repeat[i]++;
				return false;
			}
		}
		repeat[length] = 1;
		groups[length++] = _group;
		return true;
	}

	Group *operator[](int i) const (return groups[i]);

	int get_repeat_num(int i) {return repeat[i]};
	int get_length(){return length;}
};

// PointList 和 PointSet的差别在于插入的时候有无查重
// 这个直接用 unordered_set解决好了

template<const int scale> class PointList : public myList{
	int points[scale];
	int length;

public:
	PointList(){
		length = 0;
		points[0] = 0;
	}
	~PointList(){};

	void reset(){
		length = 0;
		points[0] = 0;
	}

	void add_point (int _point){
		points[length++] = _point;
		points[length] = 0;
	}

	void reset_all_points(int size_2){
		for (int i=0;i<size_2;++i)
			points[i] = i+1;
		length = size_2;
	}

	void remove_point(int _point){
		for (int i=0; i<length; ++i){
			if (points[i] == _point){
				points[i] = points[--len];
				points[len] = 0;
				break;
			}
		}
	}

	int operator[](int i){return points[i];}
	int get_length(){return length;}
	/* 打乱一发，其实我觉得没有必要？ */
	void shuffle_points(){std::random_shuffle(points,points+len);}

};

template<const int scale> class PointSet : public myList{
	int points[scale];
	int length;

public:
	PointSet(){
		length = 0;
		points[0] = 0;
	}
	~PointSet(){};
	void reset(){
		length = 0;
		points[0] = 0;
	}

	void add_point(int _point){
		for (int i=0; i<length; ++i){
			if (points[i] == _point) return;
		}
		points[length++] = _point;
		points[length] = 0;
	}

	void remove_point(int _point){
		for (int i=0; i<length; ++i){
			if (points[i] == _point){
				points[i] = points[--len];
				points[len] = 0;
				break;
			}
		}
	}

	int operator[](int i){return points[i];}
	int get_length(){return length;}

	void shuffle_points(){std::random_shuffle(points,points+len);} // 将空位打乱一发


};


#endif

#include "Board.h"
#include <iostream>
#define min(a,b) (a)<(b)?(a):(b)

Board::Board(int _sz){
	size = _sz;
	size_2 = _sz*_sz;
	init();
}

void
Board::set(int _sz){
	size = _sz;
	size_2 = _sz*_sz;
	init();
}

void
Board::init(){
	// init four_side
	// from N, clockwise
	// 有就有， 没有就接着后一个，中间没有空
	for (int p=1; p<=size_2; ++p){
		int n=0;
		if (p <= (size-1)*size) four_side[p][n++] = p+size; // N
		if (p % size) four_side[p][n++] = p+1; // E
		if (p > size) four_side[p][n++] = p-size; // S
		if (p % size != 1) four_side[p][n++] = p-1;   // W
		four_side[p][n] = 0;
	}




	// init four_corner
	// from NW, clockwise
	for (int p=1; p<=size_2 ;++p){
		int n=0;
		if (p <= (size-1)*size && p%size != 1) // NW
			four_corner[p][n++] = p+size-1;
		if (p <= (size-1)*size && p%size) // NE
			four_corner[p][n++] = p+size+1;
		if (p > size && p%size) // SE
			four_corner[p][n++] = p-size+1;
		if (p > size && p%size != 1) // SW
			four_corner[p][n++] = p-size-1;
		four_corner[p][n] = 0;
	}


	// init eight around 这个是带空的，位置没有就空掉

	for (int p=1; p<=size_2; ++p) // from NW clockwise
	{
		if (p <= (size-1)*size){
			if (p % size != 1) eight_around[p][0] = p+size-1;
			eight_around[p][1] = p+size;
		}

		if (p % size){
			if (p <= (size-1)*size) eight_around[p][2] = p+size+1;
			eight_around[p][3] = p+1;
		}

		if (p>size){
			if (p%size)eight_around[p][4] = p-size+1;
			eight_around[p][5] = p-size;
		}

		if (p%size !=1){
			if (p>size) eight_around[p][6] = p-size-1;
			eight_around[p][7] = p-1;
		}
	}


	// init distance2edge

	for (int p=1; p<=size_2; ++p){
		int d1 = min((p-1)%size, (p-1)/size);
		int d2 = min(size-(p-1)%size, size-(p-1)/size);
		distance2edge[p] = min(d1,d2);
	}


	//init manhattan
	for (int p = 1; p <= size2; p++) {
    for (int dis = 1; dis <= 4; dis++) {
      int len = 0;
      for (int d = 0; d <= dis; d++) {
        if ((p-1)%size >= d && size-(p-1)/size-1 >= (dis-d) ) {
          within_manhattan[p][dis-1][len++] = p - d + (dis-d)*size;
        } else {
          within_manhattan[p][dis-1][len++] = 0;
        }
        if (d != dis) {
          if ((p-1)%size >= d && (p-1)/size >= (dis-d)) {
            within_manhattan[p][dis-1][len++] = p - d - (dis-d)*size;
          } else {
            within_manhattan[p][dis-1][len++] = 0;
          }
        }
        if (d != 0) {
          if (size-(p-1)%size-1 >= d && size-(p-1)/size-1 >= (dis-d)) {
            within_manhattan[p][dis-1][len++] = p + d + (dis-d)*size;
          } else {
            within_manhattan[p][dis-1][len++] = 0;
          }
          if (d != dis) {
            if (size-(p-1)%size-1 >= d && (p-1)/size >= (dis-d)) {
              within_manhattan[p][dis-1][len++] = p + d - (dis-d)*size;
            } else {
              within_manhattan[p][dis-1][len++] = 0;
            }
          }
        }
      }
    }
  }


}

void
Board::reset(){
	side = BLACK;
	ko_point = 0 // NULL
	// zobrist something
	num_stones_on_board[0] = num_stones_on_board[1] = 0;
	last_atari[0] = last_atari[1] = 0;
	Group *gr;
	for (int i=0; i<=size_2; ++i){
		if (gr = belonged_group[i]){
			gr.reset();
		}
		belonged_group[i] = 0;
	}
	//emptys.reset();
	emptys.reset_all_points(size_2);
}



void
Board::joint_neigh(int point, Group * neigh){ // neigh合到point的原因是point只会间neigh一口气，反之不是
	Group *baba = belonged_group[point];
	neigh->remove_libs(point);
	baba->joint_group(neigh);
	for (Group::StoneItr sit(neigh); st; ++st){
		belonged_group[*st] = baba;
	}
	neigh->reset();               // 感觉不会内存泄露，因为成员没有指针

	if (last_atari[neigh->get_color()] == neigh){
		last_atari[neigh->get_color()] = 0;
	}

}

void
Board::remove_neigh(Group *neigh){
	for (Group::StoneItr sit(neigh);st;++st){
		belonged_group[*st] = 0;
		num_stones_on_board[st->get_color()]--;
		// zobrist something
		emptys.add_point(*st);

		// neigh 被提掉之后， neigh的周围的棋串的气要加回来
		GroupSet<4> neigh_of_neigh;
		int n = get_neigh_groups(*st,neigh_of_neigh);
		for (int i=0; i<n; ++i){
			neigh_of_neigh[i].add_libs(*st);
			if (last_atari[neigh_of_neigh[i]->get_color()]  // 维护last_atari
				== neigh_of_neigh[i])
				last_atari[neigh_of_neigh[i]->get_color()] = 0;

		}

	}

	if (neigh == last_atari[neigh->get_color()]){
		last_atari[neigh->get_color()] = 0;
	}
	neigh->reset();
}

int 
Board::tackle_change(int point){
	int captured_if_one = 0;
	int num_captured_if_one = 0;

	GroupSet<4> neigh;
	Group *cur_gr;
	int num_neigh = get_neigh_groups(point,neigh);

	for (int i=0; i<num_neigh; ++i){
		cur_gr = neigh[i];
		if (cur_gr->get_color() == belonged_group[point]->get_color())
			joint_group(point,neigh);
		else{
			if (cur_gr->get_num_libs() == 1){
				if (cur_gr->get_num_stones() == 1){
					++num_captured_if_one;
					captured_if_one = cur_gr->get_the_stone(0);
				}
				remove_neigh(cur_gr);
			}else{
				cur_gr->remove_libs(point);
				if (cur_gr->get_num_libs() == 1){
					last_atari[cur_gr->get_color()] = cur_gr; // 维护last_atari
				}
			}
		}
	}


	if(num_captured_if_one == 1 && belonged_group[point]->get_the_stone() == 1){
		return captured_if_one;   // new ko-point
	}
	else{
		return 0;
	}
}

int 
Board::drop_stone(int point, bool _color){
	if (has_stone(point)) 
		std::err << "This point has been occupied.\n" << std::endl;
	PointSet<5> libs;
	cal_point_libs(point,libs);

	all_groups[point].set(point, _color, libs);
	belonged_group[point] = &all_groups[point];
	del_empty(point);
	num_stones_on_board[_color]++;

	// zobrist something
	ko_point = tackle_change(point);

	if (belonged_group[point]->get_num_libs() == 0){
		//sucide
		remove_neigh(belonged_group[point]);
		//actually remove itself
	}else if (belonged_group[point]->get_num_libs() == 1){
		last_atari[_color] = belonged_group[point];
	}

}


int 
Board::play_move(int mv){
	if (mv) drop_stone(mv,side);
	side = !side;
	// zobrist something
	return mv;
}

int
Board::play_move_in_game(int mv, int _side){
	if (side != _side){    // 感觉并不会出现这种现象
		real_history.add_point(PASS);
	}
	// zobrist somthing
	if (mv){
		if (!has_stone(mv) && is_legal(mv,_side)){
			drop_stone(mv,_side);
		}else return -1;
	}else {
		ko_point = 0;
	}
	side = !_side;
	real_history.add_point(mv);

	// zobrist something
	return mv;
}














//==============const function===================
int
Board::cal_point_libs(int point) {
	int n = 0;
	for (int i=0; four_side[point][i]; ++i){
		if (belonged_group[point][i] == 0) ++n;
	}
	return n;
}

int
Board::cal_point_libs(int point, myList &libs) {
	int a;
	for (int i=0; a = four_side[point][i]; ++i){
		if (belonged_group[a] == 0)
			libs.add_point(a);
	}
	return libs.get_length();
}

int 
Board::get_neigh_groups(int point, GroupSet<4> &neigh) {
	for (int i=0; four_side[point][i]; ++i){
		neigh.add_group(belonged_group[four_side[point][i]]);
	}
	return neigh.get_length();
}


int 
Board::get_neigh_groups(int point, bool _color, int max_libs, GroupSet<MAXSIZE_2/3> * neigh ) {
	int n = 0;
	for (int i=0; four_side[point][i]; ++i){
		Group * cur_gr = belonged_group[four_side[point][i]];
		if (cur_gr &&
			cur_gr->get_color() == _color &&
			cur_gr->get_num_libs() <= max_libs){
			if (neigh) neigh->add_group(cur_gr);
			++n;
		}

	}
	return n;
}

int
Board::get_neigh_groups(Group *_group, bool _color, int max_libs, GroupSet<MAXSIZE_2/3> &neigh) {
	for (Group::StoneItr sit(_group); st; ++st){
		get_neigh_groups(*st, _color, max_libs, &neigh);
	}
	return neigh.get_length();
}

int
Board::get_neigh_size(int point, bool _color) {
	int n = 0;
	GroupSet<4> neigh;
	int num_neigh = get_neigh_groups(point, neigh);
	for (int i=0; i<num_neigh; ++i){
		Group *gr = neigh[i];
		if (gr->get_color() == _color){
			n += gr->get_num_stones();
		} 
	}
	return n;
}


// 此处neigh应该为已经算好的
int 
Board::get_neigh_in_atari(int point, bool _color, GroupSet <4> &neigh){
	int n=0;
	for (int i=0; i< neigh.get_length(); ++i){
		if (neigh[i]->get_color == _color && neigh[i]->get_num_libs == 1)
			++n;
	}
	return n;
}

bool
Board::is_surrounded(int point, bool _color, int excul = 0){
	if (belonged_group[point] != 0) return false; // 要求必须是一个空位
	int a;
	for (int i=0; a=four_side[point][i]; ++i){
		if (a == excul) continue;
		if (belonged_group[a] == 0 || belonged_group[a] ->get_color != _color)
			return false;
	}
	return true;
}

bool 
Board::is_true_eye(int point, bool _color, int excul = 0){
	if (!is_surrounded(point,_color,excul)) return false;

	int n=0,i,c;
	for (i=0; c = four_corner[point][i]; ++i){
		if (belonged_group[c]!=0){
			if (belonged_group[c]->get_color() == _color)++n;
			
		}else if (is_surrounded(c,_color,excul)) ++n;
	}

	if (i == 4) // 说明是当中的位置
		return n>2;
	else return n==i;
}

bool 
Board::is_fake_eye(int point, bool _color, int excul = 0){
	if (!is_surrounded(point,_color,excul)) return false;
	int n = 0,c;
	for (i=0; c = four_corner[point][i]; ++i){
		if (belonged_group[c]!=0){
			if (belonged_group[c]->get_color() == _color)++n;
		}else if (is_surrounded(c,_color,excul)) ++n;
	}
	if (i == 4) return n<=2;
	else return n < i;
}






//==============public===========
void
Board::clear_board(){
	reset();
	real_history.reset();
}

void 
Board::restore_board(){  // 感觉这个很浪费时间的
	reset();
	for (int i=0; i<real_history.get_length(); ++i){
		if (real_history[i] != 0){
			drop_stone(real_history[i],side);
		}
		side = !side;
	}
}

bool
Board::is_legal(int point, bool _color){
	if (point == ko_point) return false;
	// zobrist something
	if (cal_point_libs(point) > 0) return true; // not suicide 
	GroupSet<4> neigh;
	int n =  get_neigh_groups(point,neigh);
	for(int i=0; i<n; ++i){
		if (neigh[i]->get_color() == _color && neigh[i]->get_num_libs() != 1)
			return true;
	}

	return get_neigh_in_atari(point,color,neigh) > 0;
}

int 
Board::legal_moves(int moves[]){
	int n = 0,p;
	for (int i=0; i<emptys.get_length(); ++i){
		p = emptys[i];
		if (is_legal(p,side)) move[n++] = p;
	}

	moves[n++] = 0;
	return n;
}


int 
Board::count_score(){
	int b=0,w=0,eye=0;
	for (int i=1; i<=size_2; ++i){
		if (belonged_group[i]){
			if (belonged_group[i]->get_color() == BLACK) ++b;
			else ++w;
		}else{
			if (is_surrounded(i,BLACK)) eye++;
			else if (is_surrounded(i,WHITE)) eye--;
		}
	}
	return b+eye-w-komi;
}

int
Board::list_score(int sli[]){
	for (int i=1; i<=size_2; ++i){
		if (belonged_group[i]){
			if (belonged_group[i]->get_color() == BLACK) ++sli[i];
			else --sli[i];
		}else{
			if (is_surrounded(i,BLACK)) ++sli[i];
			else if (is_surrounded(i,WHITE)) --sli[i];
		}
	}
}

int 
Board::mercy(){
	for (int i=0; i<2; ++i){
		if (num_stones_on_board[i] - num_stones_on_board[1-i] > size_2/3)
			return i;
	}
	return -1;
}




















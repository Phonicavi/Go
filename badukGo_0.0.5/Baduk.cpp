#include "Board.h"

/* 需要环环的各种补充和扩充 */


int
Board::choose_by_random(myList & _li, bool method){
	int len,mv;
	if((len = _li.get_length()) == 0) return 0;
	int choice =  rand()%len;
	if (method == WISER)
		for (int i=choice; (i+1)-len != choice; ++i)
			if (wiser_brain((mv=_li[i]),side)) return mv;
	else
		for (int i=choice; (i+1)-len != choice; ++i)
			if (fool_brain((mv=_li[i]),side)) return mv;
	return PASS;
}

bool
Board::fool_brain(int mv, bool _side){

}

bool
Board::wiser_brain(int mv, bool _side){

}

int
Board::random_play(){
	return play_move(choose_by_random(emptys,FOOL));
}

int 
Board::wiser_play(){

}


bool 
Board::gain_libs(int mv, Group* gr){
	int cur_lib = 1; /// assume
	if (gr) cur_lib = gr->get_num_libs();
	return area_libs(mv,gr->get_color(),0,cur_lib,gr) > cur_lib;
}


int 
Board::area_libs(int mv, bool _color, myList* _lib ,int lim, Group* excul){
	if (_lib) cal_point_libs(mv,*_lib);
	PointSet<MAXSIZE_2> libers;
	int n_lib = cal_point_libs(mv,libers);
	if (lim && n_lib > lim) return n_lib;

	// _lib 只存目前的气，不存提掉对方的子之后获得的气
	GroupSet<4> neigh;

	int n_neigh = get_neigh_groups(mv,neigh);
	for (int i=0; i<n_neigh; ++i){
		Group *cur_gr = neigh[i];
		if (cur_gr != excul){
			if (cur_gr->get_color() == _color){
				for (Group::LibStr lst(cur_gr);lst;++lst){
					if (*lst != mv){
						if (_lib) _lib->add_point(*lst);
						libers.add_point(*lst);
						if (lim && libers.get_length()>lim) return libers.get_length();

					}
				}
			}else if (cur_gr->get_num_libs() == 1){
				for (Group::StoneItr sit(cur_gr);sit; ++sit){
					int adj;
					for (int j = 0;adj = four_side[*sit][j]; ++j){
						if (adj == mv){
							libers.add_point(*sit);
							if (lim && libers.get_length() > lim) return libers.get_length();
						}else if (belonged_group[adj] && belonged_group[adj].get_color() ==  _color){
							for (int k=0; k<n_neigh; ++k){
								if(belonged_group[adj] == neigh[k]){
									libers.add_point(*st);
									if (lim && libers.get_length() > lim) return libers.get_length();
								}
							}
						}
					}
				}

			}
		}
	}
	return libers.get_length();
}

// trick
int get_atari_escapes(Group* gr, myList &);
int cal_create_eyes(int, bool);

int get_last_atari();
void get_nakade_list(int lap, myList & );
void get_capture_list(int lap, myList &);
void get_pattern_list(int lap, myList &);
void get_save_list(int lap, myList &);






//judgement 

bool is_self_atari(int mv, bool _side);
bool is_ladder(int mv, bool _side);
bool is_stones_around(int mv, int max_dist);
bool is_bad_atari(int mv, bool_side);
bool is_nakade_shape(int ,bool);
bool is_match_pattern(int, bool);
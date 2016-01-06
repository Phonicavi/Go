#include "string.h"

String::String()
{
	color = EMPTY;
	stones_number = 0;
	liberties_number= 0;
}


void String::set_up(int point, int c, const int *l, int num_liberties)
{
	color = c;
	stones_number = 1;
	stones[0] = point;

	liberties_number = num_liberties;
	for (int i = 0; i < num_liberties; ++i) {
		liberties[i] = l[i];
	}

}

int String::remove_liberty(int lib)
{
	for (int j = 0; j<liberties_number; j++)
	{
		if (liberties[j] == lib) {
			liberties[j] = liberties[--liberties_number];
			return liberties_number;
		}

	}
	return 0;
}

void String::clear()
{
	liberties_number = 0;
	stones_number = 0;
}


int String::add_liberty(int i)
{
	for (int j = 0; j < liberties_number; j++) {
		if (liberties[j] == i) return 0;
	}
	liberties[liberties_number++] = i;
	return liberties_number;
}

void String::merge_string(String *merged)
{
	//printf("stones_number:%d merged->stones_number:%d\n",stones_number,merged->stones_number);
	for (int i = 0; i < merged->stones_number; i++) {
		stones[stones_number++] = merged->stones[i];
	}
	for (int i = 0; i < merged->liberties_number; i++) {
		add_liberty(merged->liberties[i]);
	}
}

//int Group::erase_liberties(int lib)
//{
//	for (int j = 0; j < nlibs; j++) {
//		if (liberties[j] == lib) {
//			liberties[j] = liberties[--nlibs];

//			liberties[nlibs] = 0;
//			return nlibs;
//		}
//	}
//	return 0;
//}


//void Group::print_group() const
//{
//	std::cerr << "Color: " << color;
//	for (int i = 0; i < nsts; i++) std::cerr << " " << stones[i];
//	std::cerr << "\n";
//}
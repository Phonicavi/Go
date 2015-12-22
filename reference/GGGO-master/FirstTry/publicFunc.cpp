#include "publicFunc.h"

bool cmpLess(const uctNode *a, const uctNode *b){ return a->score < b->score; }
bool cmpMore(const uctNode* a, const uctNode *b){ return a->score > b->score; }
bool cmpMore2(const uctNode* a, const uctNode *b) { return a->play > b->play; }
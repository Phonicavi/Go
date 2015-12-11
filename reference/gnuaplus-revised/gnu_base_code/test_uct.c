#include "uct.h"
#include "board.h"
#include "gotodie.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main() {
    board_status b;
    int pos;
    int i, res = 0;
    double score;

    for (i = 1; i <= 1; i++) {
        srand((int)time(0) + i);
        clear_board(&b);
        score = simulate_game(&b, BLACK);
        // printf("%f\n", score);
        res += (score > 0);
    }
    printf("%d\n", res);

    return 0;
}

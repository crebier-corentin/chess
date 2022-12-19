#pragma once
#include "board.h"
#include "common.h"
#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MATE_VALUE 999999
#define DRAW_VALUE 0

bool is_mate_score(double score);
int ply_to_mate(double score);

double evaluate(BoardState *bs);
Move search_move(BoardState *bs, int depth);
Move search_move_abortable(SDL_atomic_t *abort_search, BoardState *bs);

#ifdef __cplusplus
}
#endif

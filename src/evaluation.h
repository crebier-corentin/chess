#pragma once
#include "board.h"
#include "common.h"
#include <SDL_atomic.h>

#ifdef __cplusplus
extern "C" {
#endif

double evaluate(BoardState *bs);
Move search_move(BoardState *bs, int depth);
Move search_move_abortable(SDL_atomic_t *abort_search, BoardState *bs);

#ifdef __cplusplus
}
#endif

#pragma once
#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

double evaluate(BoardState *bs);
Move search_move(BoardState *bs, int depth);

#ifdef __cplusplus
}
#endif

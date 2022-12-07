#pragma once

#include "move.h"
#include "piece.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void zobrist_init();

uint64_t zobrist_piece(Piece p, Pos pos);
uint64_t zobrist_black();
uint64_t zobrist_castle_right(Color c, bool king_side);
uint64_t zobrist_en_passant(int8_t file);
#ifdef __cplusplus
}
#endif

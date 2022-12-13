#pragma once

#include "move.h"
#include "piece.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// forward declaration
typedef struct BoardState BoardState;

typedef struct Pos
{
    int8_t x;
    int8_t y;
} Pos;

void pos_to_string(Pos pos, char *buffer);
Pos parse_pos(Pos pos, char *buffer);

typedef enum Promotion
{
    PROMOTION_NONE,
    PROMOTION_QUEEN,
    PROMOTION_ROOK,
    PROMOTION_BISHOP,
    PROMOTION_KNIGHT,
} Promotion;

PieceType promotion_to_piece_type(Promotion promotion);

typedef enum Castle
{
    CASTLE_NONE,
    CASTLE_KINGSIDE,
    CASTLE_QUEENSIDE,
} Castle;

typedef struct Move
{
    Pos from;
    Pos to;
    Promotion promotion;
    Castle castle;
    bool en_passant;
    double order_move_score; // used for sorting moves
} Move;

bool move_equals(Move a, Move b);

void move_to_long_notation(Move move, char buffer[6]);
Move parse_long_notation(char buffer[6]);

Move parse_algebraic_notation(BoardState *bs, char *buffer);

#ifdef __cplusplus
}
#endif

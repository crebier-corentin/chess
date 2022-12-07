#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Empty if 0
// First 3 bits store type, 4th and 5th bit stores color
typedef uint8_t Piece;

typedef enum PieceType
{
    PT_PAWN = 1,
    PT_KNIGHT,
    PT_BISHOP,
    PT_ROOK,
    PT_QUEEN,
    PT_KING
} PieceType;

typedef enum Color
{
    C_WHITE = 1,
    C_BLACK
} Color;

Piece piece_empty();
Piece create_piece(PieceType type, Color color);

Color get_color(Piece p);
PieceType get_type(Piece p);

bool is_empty(Piece p);

bool is_pawn(Piece p);
bool is_knight(Piece p);
bool is_bishop(Piece p);
bool is_rook(Piece p);
bool is_queen(Piece p);
bool is_king(Piece p);

bool is_white(Piece p);
bool is_black(Piece p);

#ifdef __cplusplus
}
#endif

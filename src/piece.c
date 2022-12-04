#include "piece.h"

Piece piece_empty()
{
    return (Piece)0;
}

Piece create_piece(PieceType type, Color color)
{
    return (Piece)((color << 3) | type);
}

Color get_color(Piece p)
{
    return (Color)(p >> 3);
}

PieceType get_type(Piece p)
{
    return (PieceType)(p & 7 /*0b111*/);
}

bool is_empty(Piece p)
{
    return p == 0;
}

bool is_pawn(Piece p)
{
    return get_type(p) == PT_PAWN;
}
bool is_knight(Piece p)
{
    return get_type(p) == PT_KNIGHT;
}
bool is_bishop(Piece p)
{
    return get_type(p) == PT_BISHOP;
}
bool is_rook(Piece p)
{
    return get_type(p) == PT_ROOK;
}
bool is_queen(Piece p)
{
    return get_type(p) == PT_QUEEN;
}
bool is_king(Piece p)
{
    return get_type(p) == PT_KING;
}

bool is_white(Piece p)
{
    return get_color(p) == C_WHITE;
}
bool is_black(Piece p)
{
    return get_color(p) == C_BLACK;
}

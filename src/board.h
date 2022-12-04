#pragma once

#include "array.h"
#include "move.h"
#include "piece.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PiecesListLengths
{
    int8_t pawn_len;
    int8_t knight_len;
    int8_t bishop_len;
    int8_t rook_len;
    int8_t queen_len;
    int8_t king_len;
} PiecesListLengths;

typedef struct PiecesList
{
    Pos list[64];
    PiecesListLengths white;
    PiecesListLengths black;
} PiecesList;

void pieces_offset(PiecesList *pl, PieceType pt, Color c, int8_t *out_index, int8_t **out_len);
void pieces_all_color_offset(PiecesList *pl, Color c, int8_t *out_index, int8_t *out_len);

#define NO_EN_PASSANT (-9)

typedef struct BoardState
{
    Piece board[8][8];
    PiecesList pieces;
    Color turn;
    bool white_king_side_castle;
    bool white_queen_side_castle;
    bool black_king_side_castle;
    bool black_queen_side_castle;
    int8_t en_passant_x; // NO_EN_PASSANT if no en passant
    int8_t en_passant_y; // NO_EN_PASSANT if no en passant
    int halfmove_clock;
    int fullmove_number;
} BoardState;

BoardState load_fen(const char *fen);
void print_board(BoardState *bs);
void print_attack_map(bool out_map[8][8]);

void set_piece(BoardState *bs, Pos pos, Piece p);
Piece get_piece(BoardState *bs, Pos pos);

void make_move(BoardState *bs, Move move);

bool is_in_check(BoardState *bs, Color color);

void generate_pseudo_moves(BoardState *bs, Color color, Array(Move) * out_moves);
void generate_piece_pseudo_moves(BoardState *bs, Pos pos, Array(Move) * out_moves);
void generate_pawn_pseudo_moves(BoardState *bs, Pos pos, Array(Move) * out_moves);
void generate_king_pseudo_moves(BoardState *bs, Pos pos, Array(Move) * out_moves);
void generate_queen_pseudo_moves(BoardState *bs, Pos pos, Array(Move) * out_moves);
void generate_rook_pseudo_moves(BoardState *bs, Pos pos, Array(Move) * out_moves);
void generate_bishop_pseudo_moves(BoardState *bs, Pos pos, Array(Move) * out_moves);
void generate_knight_pseudo_moves(BoardState *bs, Pos pos, Array(Move) * out_moves);

void generate_legal_moves(BoardState *bs, Color color, Array(Move) * out_moves);

void generate_attack_map(BoardState *bs, Color color, bool out_map[8][8]);

#ifdef __cplusplus
}
#endif

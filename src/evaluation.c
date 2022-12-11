#include "evaluation.h"
#include "array.h"
#include "piece.h"
#include <assert.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: implement endgame tables
// Taken from https://www.chessprogramming.org/Simplified_Evaluation_Function
double pawn_table[64] = {0,  0,   0,  0, 0,  0,  0,  0,   50,  50, 50, 50, 50, 50, 50, 50, 10, 10, 20, 30, 30,  20,
                         10, 10,  5,  5, 10, 25, 25, 10,  5,   5,  0,  0,  0,  20, 20, 0,  0,  0,  5,  -5, -10, 0,
                         0,  -10, -5, 5, 5,  10, 10, -20, -20, 10, 10, 5,  0,  0,  0,  0,  0,  0,  0,  0};
double knight_table[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50, -40, -20, 0,   0,   0,   0,   -20, -40, -30, 0,   10,  15,  15, 10,
    0,   -30, -30, 5,   15,  20,  20,  15,  5,   -30, -30, 0,   15,  20,  20,  15,  0,   -30, -30, 5,   10, 15,
    15,  10,  5,   -30, -40, -20, 0,   5,   5,   0,   -20, -40, -50, -40, -30, -30, -30, -30, -40, -50,
};
double bishop_table[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20, -10, 0,   0,   0,   0,   0,   0,   -10, -10, 0,   5,   10,  10, 5,
    0,   -10, -10, 5,   5,   10,  10,  5,   5,   -10, -10, 0,   10,  10,  10,  10,  0,   -10, -10, 10,  10, 10,
    10,  10,  10,  -10, -10, 5,   0,   0,   0,   0,   5,   -10, -20, -10, -10, -10, -10, -10, -10, -20,
};
double rook_table[64] = {0, 0,  0,  0,  0,  0, 0, 0, 5, 10, 10, 10, 10, 10, 10, 5, -5, 0,  0,  0, 0, 0,
                         0, -5, -5, 0,  0,  0, 0, 0, 0, -5, -5, 0,  0,  0,  0,  0, 0,  -5, -5, 0, 0, 0,
                         0, 0,  0,  -5, -5, 0, 0, 0, 0, 0,  0,  -5, 0,  0,  0,  5, 5,  0,  0,  0};
double queen_table[64] = {-20, -10, -10, -5, -5, -10, -10, -20, -10, 0,   0,   0,  0,  0,   0,   -10,
                          -10, 0,   5,   5,  5,  5,   0,   -10, -5,  0,   5,   5,  5,  5,   0,   -5,
                          0,   0,   5,   5,  5,  5,   0,   -5,  -10, 5,   5,   5,  5,  5,   0,   -10,
                          -10, 0,   5,   0,  0,  0,   0,   -10, -20, -10, -10, -5, -5, -10, -10, -20};
double king_table[64] = {-30, -40, -40, -50, -50, -40, -40, -30, -30, -40, -40, -50, -50, -40, -40, -30,
                         -30, -40, -40, -50, -50, -40, -40, -30, -30, -40, -40, -50, -50, -40, -40, -30,
                         -20, -30, -30, -40, -40, -30, -30, -20, -10, -20, -20, -20, -20, -20, -20, -10,
                         20,  20,  0,   0,   0,   0,   20,  20,  20,  30,  10,  0,   0,   10,  30,  20};

static double pieces_square_table(BoardState *bs, Color color)
{
    assert(bs != NULL);

    double score = 0;

    int y_offset = color == C_WHITE ? 0 : 7; // reverse the table for black

    int8_t index;
    int8_t *len;
    pieces_offset(&bs->pieces, PT_PAWN, color, &index, &len);
    for (int8_t i = index; i < index + *len; i++)
    {
        int x = bs->pieces.list[i].x;
        int y = abs(y_offset - bs->pieces.list[i].y);
        score += pawn_table[x + (y * 8)];
    }

    pieces_offset(&bs->pieces, PT_KNIGHT, color, &index, &len);
    for (int8_t i = index; i < index + *len; i++)
    {
        int x = bs->pieces.list[i].x;
        int y = abs(y_offset - bs->pieces.list[i].y);
        score += knight_table[x + (y * 8)];
    }

    pieces_offset(&bs->pieces, PT_BISHOP, color, &index, &len);
    for (int8_t i = index; i < index + *len; i++)
    {
        int x = bs->pieces.list[i].x;
        int y = abs(y_offset - bs->pieces.list[i].y);
        score += bishop_table[x + (y * 8)];
    }

    pieces_offset(&bs->pieces, PT_ROOK, color, &index, &len);
    for (int8_t i = index; i < index + *len; i++)
    {
        int x = bs->pieces.list[i].x;
        int y = abs(y_offset - bs->pieces.list[i].y);
        score += rook_table[x + (y * 8)];
    }

    pieces_offset(&bs->pieces, PT_QUEEN, color, &index, &len);
    for (int8_t i = index; i < index + *len; i++)
    {
        int x = bs->pieces.list[i].x;
        int y = abs(y_offset - bs->pieces.list[i].y);
        score += queen_table[x + (y * 8)];
    }

    pieces_offset(&bs->pieces, PT_KING, color, &index, &len);
    for (int8_t i = index; i < index + *len; i++)
    {
        int x = bs->pieces.list[i].x;
        int y = abs(y_offset - bs->pieces.list[i].y);
        score += king_table[x + (y * 8)];
    }

    return score;
}

static int count_doubled_and_isolated_pawns(BoardState *bs, Color c)
{
    assert(bs != NULL);

    int8_t index;
    int8_t *len;
    pieces_offset(&bs->pieces, PT_PAWN, c, &index, &len);

    int pawns_on_file[8] = {0};
    for (int8_t i = index; i < index + *len; i++)
    {
        pawns_on_file[bs->pieces.list[i].x]++;
    }

    int doubled_pawns = 0;
    for (int8_t i = 0; i < 8; i++)
    {
        if (pawns_on_file[i] > 1)
        {
            doubled_pawns += pawns_on_file[i] - 1;
        }
    }

    int isolated_pawns = 0;
    for (int8_t i = index; i < index + *len; i++)
    {
        Pos pos = bs->pieces.list[i];

        bool left = pos.x == 0 || pawns_on_file[pos.x - 1] == 0;
        bool right = pos.x == 7 || pawns_on_file[pos.x + 1] == 0;
        if (left || right)
        {
            isolated_pawns++;
        }
    }

    return doubled_pawns + isolated_pawns;
}
static int count_blocked_pawns(BoardState *bs, Color c)
{
    assert(bs != NULL);

    int direction = c == C_WHITE ? -1 : 1;

    int8_t index;
    int8_t *len;
    pieces_offset(&bs->pieces, PT_PAWN, c, &index, &len);

    int blocked_pawns = 0;
    for (int8_t i = index; i < index + *len; i++)
    {
        Pos pos = bs->pieces.list[i];
        Pos forward = (Pos){pos.x, pos.y + direction};
        if (!is_empty(get_piece(bs, forward)))
        {
            blocked_pawns++;
        }
    }

    return blocked_pawns;
}

double evaluate(BoardState *bs)
{
    assert(bs != NULL);

    double score = 0;

    // material
    score += bs->pieces.white.king_len * 200;
    score += bs->pieces.black.king_len * -200;
    score += bs->pieces.white.queen_len * 9;
    score += bs->pieces.black.queen_len * -9;
    score += bs->pieces.white.rook_len * 5;
    score += bs->pieces.black.rook_len * -5;
    score += bs->pieces.white.bishop_len * 3;
    score += bs->pieces.black.bishop_len * -3;
    score += bs->pieces.white.knight_len * 3;
    score += bs->pieces.black.knight_len * -3;
    score += bs->pieces.white.pawn_len * 1;
    score += bs->pieces.black.pawn_len * -1;

    // pawn structure
    score += ((double)count_doubled_and_isolated_pawns(bs, C_WHITE)) * -0.5;
    score += ((double)count_doubled_and_isolated_pawns(bs, C_BLACK)) * 0.5;
    score += ((double)count_blocked_pawns(bs, C_WHITE)) * -0.5;
    score += ((double)count_blocked_pawns(bs, C_BLACK)) * 0.5;

    // mobility
    Array(Move) white_moves = array_create_size(Move, 32);
    generate_legal_moves(bs, C_WHITE, &white_moves);
    Array(Move) black_moves = array_create_size(Move, 32);
    generate_legal_moves(bs, C_BLACK, &black_moves);

    score += ((double)array_len(white_moves)) * 0.1;
    score += ((double)array_len(black_moves)) * -0.1;

    array_free(white_moves);
    array_free(black_moves);

    // Piece square tables
    score += pieces_square_table(bs, C_WHITE) * 1;
    score += pieces_square_table(bs, C_WHITE) * -1;

    return score;
}

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define MATE_VALUE -999999
#define DRAW_VALUE 0

static double negamax(BoardState *bs, int depth, double alpha, double beta, Color c, Move *out_move)
{
    if (depth == 0)
    {
        return evaluate(bs) * (c == C_WHITE ? 1 : -1);
    }

    Array(Move) moves = array_create_size(Move, 32);
    generate_legal_moves(bs, bs->turn, &moves);

    // Checkmate and stalemate detection
    if (array_len(moves) == 0)
    {
        array_free(moves);

        if (is_in_check(bs, c))
        {
            return MATE_VALUE;
        }
        else
        {
            return DRAW_VALUE;
        }
    }

    double value = -INFINITY;
    for (size_t i = 0; i < array_len(moves); i++)
    {
        BoardState new_bs = *bs;
        make_move(&new_bs, moves[i]);

        double score = -negamax(&new_bs, depth - 1, -beta, -alpha, c == C_WHITE ? C_BLACK : C_WHITE, NULL);
        if (score > value)
        {
            value = score;
            if (out_move != NULL)
            {
                *out_move = moves[i];
            }
        }
        alpha = MAX(alpha, value);
        if (alpha >= beta)
        {
            break;
        }
    }

    array_free(moves);

    return value;
}

Move search_move(BoardState *bs, int depth)
{
    assert(bs != NULL);
    assert(depth > 0);

    Move best_move = {0};
    negamax(bs, depth, -INFINITY, INFINITY, bs->turn, &best_move);

    return best_move;
}

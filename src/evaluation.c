#include "evaluation.h"
#include "array.h"
#include "board.h"
#include "move.h"
#include "piece.h"
#include <assert.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <sort_r.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double piece_value[] = {0, 100, 300, 300, 500, 900, 20000};

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
uint64_t count = 0;
double evaluate(BoardState *bs)
{
    count++;
    assert(bs != NULL);

    double score = 0;

    // material
    score += bs->pieces.white.king_len * piece_value[PT_KING];
    score += bs->pieces.black.king_len * -piece_value[PT_KING];
    score += bs->pieces.white.queen_len * piece_value[PT_QUEEN];
    score += bs->pieces.black.queen_len * -piece_value[PT_QUEEN];
    score += bs->pieces.white.rook_len * piece_value[PT_ROOK];
    score += bs->pieces.black.rook_len * -piece_value[PT_ROOK];
    score += bs->pieces.white.bishop_len * piece_value[PT_BISHOP];
    score += bs->pieces.black.bishop_len * -piece_value[PT_BISHOP];
    score += bs->pieces.white.knight_len * piece_value[PT_KNIGHT];
    score += bs->pieces.black.knight_len * -piece_value[PT_KNIGHT];
    score += bs->pieces.white.pawn_len * piece_value[PT_PAWN];
    score += bs->pieces.black.pawn_len * -piece_value[PT_PAWN];

    // pawn structure
    score += ((double)count_doubled_and_isolated_pawns(bs, C_WHITE)) * -50;
    score += ((double)count_doubled_and_isolated_pawns(bs, C_BLACK)) * 50;
    score += ((double)count_blocked_pawns(bs, C_WHITE)) * -50;
    score += ((double)count_blocked_pawns(bs, C_BLACK)) * 50;

    // mobility
    /* Array(Move) white_moves = array_create_size(Move, 32);
    generate_legal_moves(bs, C_WHITE, &white_moves);
    Array(Move) black_moves = array_create_size(Move, 32);
    generate_legal_moves(bs, C_BLACK, &black_moves);

    score += ((double)array_len(white_moves)) * 10;
    score += ((double)array_len(black_moves)) * -10;

    array_free(white_moves);
    array_free(black_moves);
    */

    // Piece square tables
    score += pieces_square_table(bs, C_WHITE) * 1;
    score += pieces_square_table(bs, C_WHITE) * -1;

    return score;
}

static double evaluate_move(BoardState *bs, Move *move)
{
    assert(bs != NULL);
    assert(move != NULL);

    double score = 0;
    Piece move_piece = get_piece(bs, move->from);
    Piece capture_piece = get_piece(bs, move->to);

    if (!is_empty(capture_piece))
    {
        score += 10 * piece_value[capture_piece] - piece_value[move_piece];
    }

    switch (move->promotion)
    {
    case PROMOTION_NONE:
        break;
    case PROMOTION_QUEEN:
        score += piece_value[PT_QUEEN];
        break;
    case PROMOTION_ROOK:
        score += piece_value[PT_ROOK];
        break;
    case PROMOTION_BISHOP:
        score += piece_value[PT_BISHOP];
        break;
    case PROMOTION_KNIGHT:
        score += piece_value[PT_KNIGHT];
        break;
    }

    // TODO: check attack map

    return score;
}

static int move_sorter(const void *a_, const void *b_, void *context_)
{
    Move *a = (Move *)a_;
    Move *b = (Move *)b_;
    BoardState *bs = (BoardState *)context_;

    double a_score = evaluate_move(bs, a);
    double b_score = evaluate_move(bs, b);

    return (int)(a_score - b_score);
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
    sort_r(moves, array_len(moves), sizeof(Move), move_sorter, bs); // Order moves

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

    // printf("evalute %llu\n", count);

    return best_move;
}

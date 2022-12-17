#include "evaluation.h"
#include "array.h"
#include "board.h"
#include "move.h"
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

typedef struct NegamaxEntry
{
    uint64_t key;
    double value;
    Move move;
    int depth;
} NegamaxEntry;
static NegamaxEntry *cache = NULL;

static double evaluate_move(BoardState *bs, Move *move, Move *cache_move, bool pawns_attack_map[8][8])
{
    assert(bs != NULL);
    assert(move != NULL);

    double score = 0;
    Piece move_piece = get_piece(bs, move->from);
    Piece capture_piece = get_piece(bs, move->to);

    if (!is_empty(capture_piece))
    {
        score += 10 * piece_value[get_type(capture_piece)] - piece_value[get_type(move_piece)];
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

    // Penalize moving into a pawn attack
    if (pawns_attack_map[move->to.x][move->to.y])
    {
        score -= piece_value[get_type(move_piece)];
    }

    if (cache_move != NULL && move_equals(*move, *cache_move))
    {
        score += 1000;
    }

    return score;
}

#define SORT_NAME move
#define SORT_TYPE Move
#define SORT_CMP(x, y) ((y).order_move_score - (x).order_move_score)
#include <sort.h>

static void order_moves(BoardState *bs, Array(Move) moves, Move *cache_move)
{
    assert(bs != NULL);
    assert(moves != NULL);

    bool pawns_attack_map[8][8];
    generate_pawns_attack_map(bs, bs->turn == C_WHITE ? C_BLACK : C_WHITE, pawns_attack_map); // get color from moves ?

    for (size_t i = 0; i < array_len(moves); i++)
    {
        moves[i].order_move_score = evaluate_move(bs, &moves[i], cache_move, pawns_attack_map);
    }
    move_tim_sort(moves, array_len(moves));
}

#ifndef MAX
#define MAX(x, y) (((x) > (y) ? (x) : (y)))
#endif

#ifndef MIN
#define MIN(x, y) (((x) < (y) ? (x) : (y)))
#endif

#define MATE_VALUE -999999
#define DRAW_VALUE 0

static double negamax_captures(BoardState *bs, double alpha, double beta)
{
    double score = evaluate(bs) * (bs->turn == C_WHITE ? 1 : -1);
    if (score >= beta)
    {
        return beta;
    }
    alpha = MAX(alpha, score);

    Array(Move) all_moves = array_create_size(Move, 32);
    generate_pseudo_moves(bs, bs->turn, &all_moves);

    // Filter out non captures
    Array(Move) moves = array_create_size(Move, array_len(all_moves));
    for (size_t i = 0; i < array_len(all_moves); i++)
    {
        if (!is_empty(get_piece(bs, all_moves[i].to)))
        {
            array_push(moves, all_moves[i]);
        }
    }
    array_free(all_moves);

    Move *cache_move = NULL;
    ptrdiff_t cache_index = hmgeti(cache, bs->zobrist_hash);
    if (cache_index > 0)
    {
        cache_move = &cache[cache_index].move;
    }

    order_moves(bs, moves, cache_move);

    for (size_t i = 0; i < array_len(moves); i++)
    {
        BoardState new_bs = *bs;
        make_move(&new_bs, moves[i]);

        // Check if move was legal
        if (is_in_check(&new_bs, bs->turn))
        {
            continue;
        }

        score = -negamax_captures(&new_bs, -beta, -alpha);
        alpha = MAX(alpha, score);
        if (alpha >= beta)
        {
            break;
        }
    }

    array_free(moves);

    return alpha;
}

static double negamax(BoardState *bs, int depth, double alpha, double beta, Move *out_move)
{
    Move *cache_move = NULL;
    ptrdiff_t cache_index = hmgeti(cache, bs->zobrist_hash);
    if (cache_index > 0)
    {
        NegamaxEntry cache_entry = cache[cache_index];
        cache_move = &cache_entry.move;

        // If equal depth, can return cached move
        if (cache_entry.depth == depth)
        {
            if (out_move != NULL)
            {
                *out_move = cache_entry.move;
            }
            return cache_entry.value;
        }
    }

    Move best_move = {0};
    double value = -INFINITY;
    if (depth == 0)
    {
        value = negamax_captures(bs, alpha, beta);
    }
    else
    {
        Array(Move) moves = array_create_size(Move, 32);
        generate_pseudo_moves(bs, bs->turn, &moves);
        order_moves(bs, moves, cache_move);

        bool had_legal_move = false;
        for (size_t i = 0; i < array_len(moves); i++)
        {
            BoardState new_bs = *bs;
            make_move(&new_bs, moves[i]);

            //  Check if move was legal
            if (is_in_check(&new_bs, bs->turn))
            {
                continue;
            }
            had_legal_move = true;

            double score = -negamax(&new_bs, depth - 1, -beta, -alpha, NULL);
            if (score > value)
            {
                value = score;
                best_move = moves[i];
            }
            alpha = MAX(alpha, value);
            if (alpha >= beta)
            {
                break;
            }
        }

        array_free(moves);

        // Checkmate and stalemate detection
        if (!had_legal_move)
        {
            if (is_in_check(bs, bs->turn))
            {
                value = MATE_VALUE + -depth;
            }
            else
            {
                value = DRAW_VALUE;
            }
        }

        if (out_move != NULL)
        {
            *out_move = best_move;
        }
    }

    // Fill cache
    hmputs(cache, ((NegamaxEntry){
                      .key = bs->zobrist_hash,
                      .value = value,
                      .depth = depth,
                      .move = best_move,
                  }));

    return value;
}

Move search_move(BoardState *bs, int depth)
{
    assert(bs != NULL);
    assert(depth > 0);

    Move best_move = {0};
    for (int i = 1; i <= depth; i++)
    {
        count = 0;
        negamax(bs, i, -INFINITY, INFINITY, &best_move);

#if DEBUG_SEARCH_MOVE
        printf("DEPTH %d evaluate %llu\n", i, count);
        char buffer[6];
        move_to_long_notation(best_move, buffer);
        printf("%s\n", buffer);
#endif
    }

    return best_move;
}

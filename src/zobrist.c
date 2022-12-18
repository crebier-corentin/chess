#include "zobrist.h"
#include "piece.h"
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

static uint64_t get_64_rand()
{
    return (((uint64_t)rand() << 0) & 0x000000000000FFFFull) | (((uint64_t)rand() << 16) & 0x00000000FFFF0000ull) |
           (((uint64_t)rand() << 32) & 0x0000FFFF00000000ull) | (((uint64_t)rand() << 48) & 0xFFFF000000000000ull);
}

typedef struct ZobristTable
{
    uint64_t pieces[2][6][8][8]; // [color][piece type][x][y], -1 is substracted from color and piece type
    uint64_t castling[2][2];     // [color][side], -1 is substracted from color, 0 is king side, 1 is queen side
    uint64_t black_turn;
    uint64_t en_passant_file[8]; // [file]
} ZobristTable;

static ZobristTable zobrist_table = {0};

void zobrist_init()
{
    srand(time(NULL));
    for (int c = 0; c < 2; c++)
    {
        for (int pt = 0; pt < 6; pt++)
        {
            for (int x = 0; x < 8; x++)
            {
                for (int y = 0; y < 8; y++)
                {
                    zobrist_table.pieces[c][pt][x][y] = get_64_rand();
                }
            }
        }
    }
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            zobrist_table.castling[i][j] = get_64_rand();
        }
    }
    zobrist_table.black_turn = get_64_rand();
    for (int i = 0; i < 8; i++)
    {
        zobrist_table.en_passant_file[i] = get_64_rand();
    }
}

uint64_t zobrist_piece(Piece p, Pos pos)
{
    return zobrist_table.pieces[get_color(p) - 1][get_type(p) - 1][pos.x][pos.y];
}

uint64_t zobrist_black()
{
    return zobrist_table.black_turn;
}

uint64_t zobrist_castle_right(Color c, bool king_side)
{
    return zobrist_table.castling[c - 1][king_side ? 0 : 1];
}

uint64_t zobrist_en_passant(int8_t file)
{
    return zobrist_table.en_passant_file[file];
}

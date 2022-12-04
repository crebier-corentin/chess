#include "array.h"
#include "board.h"
#include "piece.h"
#include <SDL_mutex.h>
#include <SDL_thread.h>
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
// #include <windows.h>

#include <algorithm>
// #include <async++.h>
#include <atomic>
#include <execution>
#include <numeric>
#include <vector>
/*
int perft(BoardState bs, int depth)
{
    if (depth == 0)
    {
        return 1;
    }

    Array(Move) moves_arr = array_create(Move);
    generate_pseudo_moves(&bs, bs.turn, &moves_arr);

    if (depth <= 2)
    {
        int c = 0;
        for (size_t i = 0; i < array_len(moves_arr); i++)
        {
            BoardState bs2 = bs;
            make_move(&bs2, moves_arr[i]);
            c += perft(bs2, depth - 1);
        }
        return c;
    }

    std::vector<int> indexes;
    for (int i = 0; i < array_len(moves_arr); i++)
    {
        indexes.push_back(i);
    }

    return async::parallel_reduce(indexes, 0, [=](int acc, int i) {
        if (i >= array_len(moves_arr))
        {
            return acc;
        }

        BoardState new_bs = bs;
        make_move(&new_bs, moves_arr[i]);
        return acc + perft(new_bs, depth - 1);
    });
    array_free(moves_arr);
}*/

int perft(BoardState bs, int depth)
{
    if (depth == 0)
    {
        return 1;
    }

    Array(Move) moves_arr = array_create(Move);
    generate_pseudo_moves(&bs, bs.turn, &moves_arr);

    /*   std::vector<int> indexes;
       for (int i = 0; i < array_len(moves_arr); i++)
       {
           indexes.push_back(i);
       }

       int r = std::reduce(std::execution::par, indexes.begin(), indexes.end(), 0, [=](int acc, int i) {
           BoardState new_bs = bs;
           make_move(&new_bs, moves_arr[i]);
           return acc + perft(new_bs, depth - 1);
       });*/

    if (depth <= 4)
    {
        int c = 0;
        for (size_t i = 0; i < array_len(moves_arr); i++)
        {
            BoardState bs2 = bs;
            make_move(&bs2, moves_arr[i]);
            c += perft(bs2, depth - 1);
        }
        return c;
    }

    std::atomic_int r = 0;
    std::for_each(std::execution::par, moves_arr, moves_arr + array_len(moves_arr), [&](Move m) {
        BoardState new_bs = bs;
        make_move(&new_bs, m);
        r += +perft(new_bs, depth - 1);
    });

    array_free(moves_arr);

    return r;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    BoardState bs = load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // BoardState bs = load_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    // BoardState bs = load_fen("8/8/8/4Q3/8/8/8/8 w - - 0 1");
    // BoardState bs = load_fen("rnbqkbnr/4p3/8/8/8/8/4P3/RNBQKBNR w Qkq - 0 1");

    printf("%u\n", perft(bs, 7));

    return 0;
}

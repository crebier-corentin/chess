#include "array.h"
#include "board.h"
#include "perft.h"
#include "piece.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

typedef intptr_t ssize_t;

ssize_t getline(char **lineptr, size_t *n, FILE *stream)
{
    size_t pos;
    int c;

    if (lineptr == NULL || stream == NULL || n == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    c = getc(stream);
    if (c == EOF)
    {
        return -1;
    }

    if (*lineptr == NULL)
    {
        *lineptr = malloc(128);
        if (*lineptr == NULL)
        {
            return -1;
        }
        *n = 128;
    }

    pos = 0;
    while (c != EOF)
    {
        if (pos + 1 >= *n)
        {
            size_t new_size = *n + (*n >> 2);
            if (new_size < 128)
            {
                new_size = 128;
            }
            char *new_ptr = realloc(*lineptr, new_size);
            if (new_ptr == NULL)
            {
                return -1;
            }
            *n = new_size;
            *lineptr = new_ptr;
        }

        ((unsigned char *)(*lineptr))[pos++] = c;
        if (c == '\n')
        {
            break;
        }
        c = getc(stream);
    }

    (*lineptr)[pos] = '\0';
    return pos;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    SetConsoleOutputCP(65001); // unicode

    BoardState bs = load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    print_board(&bs);

    while (true)
    {
        char buffer[256] = {0};
        scanf("%255s", buffer);

        if (strcmp(buffer, "q") == 0 || strcmp(buffer, "quit") == 0 || strcmp(buffer, "exit") == 0)
        {
            break;
        }

        make_move(&bs, parse_algebraic_notation(&bs, buffer));
        print_board(&bs);
    }

    // printf("%llu\n", perft_thread_sched(&bs, depth));

    // divide(load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"), depth);

    // r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -
    // e5d7
    // https://www.chessprogramming.org/Perft_Results
    // https://github.com/agausmann/perftree

    //    BoardState bs = load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // BoardState bs = load_fen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    //  BoardState bs = load_fen("8/8/8/4Q3/8/8/8/8 w - - 0 1");
    //  BoardState bs = load_fen("rnbqkbnr/4p3/8/8/8/8/4P3/RNBQKBNR w Qkq - 0 1");
    /*
        BoardState bs = load_fen("r4rk1/2p1qppp/p1np1n2/2P1p1B1/2p1P1b1/P1NP1N2/R1P1QPPP/5RK1 b - - 0 12");

        Array(Move) moves = array_create(Move);
        generate_legal_moves(&bs, bs.turn, &moves);

        printf("correct\n");
        for (size_t i = 0; i < array_len(moves); i++)
        {
            char notation[6];
            move_to_long_notation(moves[i], notation);
            printf("%s\n", notation);
        }

        printf("wrong\n");
        divide(load_fen("r4rk1/2p1qppp/p1np1n2/2b1p1B1/1Pp1P1b1/P1NP1N2/R1P1QPPP/5RK1 w - - 0 12"), 2);
     */
    // divide(bs, 3);

    // printf("%lld\n", perft(bs, 2));

    /*     int depth = 1;
        if (argc >= 2)
        {
            depth = atoi(argv[1]);
        }

        if (argc == 3)
        {
            uint64_t nodes = 0;
            SDL_sem *sem = SDL_CreateSemaphore(24);
            PerftArgs args = {bs, depth, sem, &nodes};
            perft_thread((void *)&args);
            printf("%lld\n", nodes);
            SDL_DestroySemaphore(sem);
        }
        else
        {
            printf("%lld\n", perft(bs, depth));
        }
     */
    return 0;
}

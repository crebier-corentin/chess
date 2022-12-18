#include "array.h"
#include "board.h"
#include "evaluation.h"
#include "move.h"
#include "perft.h"
#include "piece.h"
#include "zobrist.h"
#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

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

void main_perft(int argc, char *argv[])
{

    char *fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    // if (argc > 1)
    // {
    //     fen = argv[1];
    // }

    int depth = 5;
    if (argc > 1)
    {
        depth = atoi(argv[1]);
    }

    bool threads = false;
    if (argc > 2)
    {
        threads = true;
    }

    BoardState bs = load_fen(fen);

    if (threads)
    {
        printf("%llu\n", perft_thread_sched(&bs, depth));
    }
    else
    {
        printf("%llu\n", perft(bs, depth));
    }
}

void main_search(int argc, char *argv[])
{
    char *fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    if (argc > 1)
    {
        fen = argv[1];
    }

    int depth = 7;
    if (argc > 1)
    {
        depth = atoi(argv[1]);
    }

    BoardState bs = load_fen(fen);

    while (true)
    {
        char input_buffer[256];
        scanf("%255s", input_buffer);

        if (strcmp(input_buffer, "quit") == 0)
        {
            break;
        }

        Move input_move = parse_algebraic_notation(&bs, input_buffer);
        make_move(&bs, input_move);
        print_board(&bs);

        Move m = search_move(&bs, depth);
        make_move(&bs, m);
        char buffer[6];
        move_to_long_notation(m, buffer);
        printf("%s\n", buffer);
    }
}

int main(int argc, char *argv[])
{
    zobrist_init();

    (void)argc;
    (void)argv;

#ifdef _WIN32
    SetConsoleOutputCP(65001); // unicode
#endif
    //  main_search(argc, argv);

    // char *fen = "2rq1rk1/pbp2pp1/1p2p2p/4P1Nn/2PP4/3B4/P2BQPPP/3RR1K1 b - - 0 20";
    char *fen = "rnbqk2r/pppp2P1/3b1n2/4p2p/7P/3N2P1/PPPP1P2/RNBQKB1R b KQkq - 0 9";

    BoardState bs = load_fen(fen);

    Move m = search_move(&bs, 8);
    char buffer[6];
    move_to_long_notation(m, buffer);
    printf("%s\n", buffer);

    return 0;
}

#include "array.h"
#include "board.h"
#include "move.h"
#include "perft.h"
#include "piece.h"
#include "zobrist.h"
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
    zobrist_init();

    (void)argc;
    (void)argv;

    SetConsoleOutputCP(65001); // unicode

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

    return 0;
}

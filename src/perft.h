#pragma once

#include "board.h"
#include "common.h"
#include <SDL.h>

size_t perft(BoardState bs, int depth);

typedef struct PerftArgs
{
    BoardState bs;
    int depth;
    SDL_sem *sem;
    uint64_t *ret;
} PerftArgs;

// args should be PerftArgs
int perft_thread_sdl(void *args);

uint64_t perft_thread_sched(BoardState *bs, int depth);

void divide(BoardState bs, int depth);

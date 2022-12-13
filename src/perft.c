#include "perft.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

#define SCHED_IMPLEMENTATION
#include <sched_lib.h>

size_t perft(BoardState bs, int depth)
{
    if (depth == 0)
    {
        return 1;
    }

    Array(Move) moves = array_create_size(Move, 32);
    generate_legal_moves(&bs, bs.turn, &moves);

    if (depth == 1)
    {
        size_t nodes = array_len(moves);
        array_free(moves);
        return nodes;
    }

    size_t nodes = 0;
    for (size_t i = 0; i < array_len(moves); i++)
    {
        BoardState new_bs = bs;
        make_move(&new_bs, moves[i]);
        nodes += perft(new_bs, depth - 1);
    }
    array_free(moves);

    return nodes;
}

typedef struct PerftThreadData
{
    SDL_Thread *thread;
    PerftArgs args;
    uint64_t ret;
} PerfThreadData;

int perft_thread_sdl(void *args_)
{
    PerftArgs *args = (PerftArgs *)args_;

    if (args->depth == 0)
    {
        *args->ret = 1;
        return 0;
    }

    Array(Move) moves = array_create(Move);
    generate_legal_moves(&args->bs, args->bs.turn, &moves);

    Array(PerfThreadData) threads_data = array_create_size(PerfThreadData, array_len(moves)); // Re-alloc causes issues

    uint64_t nodes = 0;
    for (size_t i = 0; i < array_len(moves); i++)
    {
        BoardState new_bs = args->bs;
        make_move(&new_bs, moves[i]);

        // Start a new thread if available, otherwise compute in this thread
        if (SDL_SemTryWait(args->sem) == 0)
        {
            array_push(threads_data, (PerfThreadData){0});

            size_t arr_index = array_len(threads_data) - 1;

            threads_data[arr_index].args.bs = new_bs;
            threads_data[arr_index].args.depth = args->depth - 1;
            threads_data[arr_index].args.sem = args->sem;
            threads_data[arr_index].args.ret = &threads_data[arr_index].ret;

            threads_data[arr_index].thread =
                SDL_CreateThread(perft_thread_sdl, "perft_thread", (void *)&threads_data[arr_index].args);
            assert(threads_data[arr_index].thread != NULL);
        }
        else
        {
            uint64_t ret = 0;
            perft_thread_sdl(
                (void *)&(PerftArgs){.bs = new_bs, .depth = args->depth - 1, .sem = args->sem, .ret = &ret});
            nodes += ret;
        }
    }
    array_free(moves);

    for (size_t i = 0; i < array_len(threads_data); i++)
    {
        SDL_WaitThread(threads_data[i].thread, NULL);
        SDL_SemPost(args->sem);
        nodes += threads_data[i].ret;
    }
    array_free(threads_data);

    *args->ret = nodes;

    return 0;
}

typedef struct PerftThreadSchedData
{
    BoardState bs;
    int depth;
    uint64_t ret;
} PerftThreadSchedData;
static void perft_thread_sched_task(void *args_, struct scheduler *sched, struct sched_task_partition partition,
                                    sched_uint thread_num)
{
    (void)partition;
    (void)thread_num;
    PerftThreadSchedData *args = (PerftThreadSchedData *)args_;

    if (args->depth == 0)
    {
        args->ret = 1;
        return;
    }

    Array(Move) moves = array_create_size(Move, 32);
    generate_legal_moves(&args->bs, args->bs.turn, &moves);

    if (args->depth == 1)
    {
        args->ret = array_len(moves);
        array_free(moves);
        return;
    }

    struct sched_task *tasks = malloc(sizeof(struct sched_task) * array_len(moves));
    PerftThreadSchedData *task_args = malloc(sizeof(PerftThreadSchedData) * array_len(moves));

    for (size_t i = 0; i < array_len(moves); i++)
    {
        BoardState new_bs = args->bs;
        make_move(&new_bs, moves[i]);

        task_args[i] = (PerftThreadSchedData){.bs = new_bs, .depth = args->depth - 1, .ret = 0};

        scheduler_add(sched, &tasks[i], &perft_thread_sched_task, &task_args[i], 0, 0);
    }

    uint64_t total = 0;
    for (size_t i = 0; i < array_len(moves); i++)
    {
        scheduler_join(sched, &tasks[i]);
        total += task_args[i].ret;
    }
    array_free(moves);
    free(tasks);
    free(task_args);

    args->ret = total;
}

uint64_t perft_thread_sched(BoardState *bs, int depth)
{
    sched_size sched_needed_memory;
    struct scheduler sched;
    scheduler_init(&sched, &sched_needed_memory, SCHED_DEFAULT, NULL);
    void *sched_memory = calloc(sched_needed_memory, 1);
    scheduler_start(&sched, sched_memory);

    struct sched_task task;
    PerftThreadSchedData task_args = {.bs = *bs, .depth = depth, .ret = 0};
    scheduler_add(&sched, &task, &perft_thread_sched_task, &task_args, 0, 0);
    scheduler_join(&sched, &task);

    scheduler_wait(&sched);
    scheduler_stop(&sched, true);
    free(sched_memory);

    return task_args.ret;
}

void divide(BoardState bs, int depth)
{
    Array(Move) moves = array_create_size(Move, 32);
    generate_legal_moves(&bs, bs.turn, &moves);

    uint64_t total = 0;
    for (size_t i = 0; i < array_len(moves); i++)
    {
        BoardState new_bs = bs;
        make_move(&new_bs, moves[i]);

        char notation[6];
        move_to_long_notation(moves[i], notation);

        uint64_t nodes = perft(new_bs, depth - 1);
        total += nodes;
        printf("%s %llu\n", notation, nodes);
    }

    printf("\n%llu\n", total);
}

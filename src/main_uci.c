#include "array.h"
#include "board.h"
#include "common.h"
#include "evaluation.h"
#include "move.h"
#include "zobrist.h"
#include <SDL.h>
#include <errno.h>
#include <pcre2.h>
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

        if (c == '\n')
        {
            break;
        }
        ((unsigned char *)(*lineptr))[pos++] = c;
        c = getc(stream);
    }

    (*lineptr)[pos] = '\0';
    return pos;
}

bool starts_with(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre), lenstr = strlen(str);
    return lenstr < lenpre ? false : memcmp(pre, str, lenpre) == 0;
}

BoardState parse_position_command(char *line, Array(uint64_t) * seen_positions)
{
    static pcre2_code *position_regex = NULL;
    if (position_regex == NULL)
    {
        int errornumber;
        PCRE2_SIZE erroroffset;

        position_regex =
            pcre2_compile((PCRE2_SPTR) "position (?:fen (?<fen>[^m]+)|(?<startpos>startpos)) ?(?:moves (?<moves>.+))?$",
                          PCRE2_ZERO_TERMINATED, 0, &errornumber, &erroroffset, NULL);

        if (position_regex == NULL)
        {
            PCRE2_UCHAR error_buffer[256];
            pcre2_get_error_message(errornumber, error_buffer, sizeof(error_buffer));
            fprintf(stderr, "PCRE2 compilation failed at offset %d: %s\n", (int)erroroffset, error_buffer);

            abort();
        }
    }
    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(position_regex, NULL);
    assert(match_data != NULL);

    int match_res = pcre2_match(position_regex, (PCRE2_SPTR)line, PCRE2_ZERO_TERMINATED, 0, 0, match_data, NULL);
    assert(match_res > 0);

    BoardState bs;
    if (pcre2_substring_length_byname(match_data, (PCRE2_SPTR) "startpos", NULL) == 0)
    {
        bs = load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    }
    else
    {

        PCRE2_UCHAR match_buffer[100] = {0};
        PCRE2_SIZE buffer_size = sizeof(match_buffer);
        match_res = pcre2_substring_copy_byname(match_data, (PCRE2_SPTR) "fen", match_buffer, &buffer_size);
        assert(match_res == 0);

        bs = load_fen((char *)match_buffer);
    }

    if (pcre2_substring_length_byname(match_data, (PCRE2_SPTR) "moves", NULL) == 0)
    {
        PCRE2_UCHAR *moves_buffer = NULL;
        PCRE2_SIZE moves_size;
        match_res = pcre2_substring_get_byname(match_data, (PCRE2_SPTR) "moves", &moves_buffer, &moves_size);
        assert(match_res == 0);

        char *move = strtok((char *)moves_buffer, " ");
        while (move != NULL)
        {
            array_push(*seen_positions, bs.zobrist_hash);
            make_move(&bs, parse_long_notation(&bs, move));

            move = strtok(NULL, " ");
        }

        pcre2_substring_free(moves_buffer);
    }

    pcre2_match_data_free(match_data);

    return bs;
}

BoardState bs;
Array(uint64_t) seen_positions;
SDL_atomic_t abort_search;
SDL_TimerID abort_timer_id;

static Uint32 COMMAND_EVENT;

int command_reader_task(void *_)
{
    (void)_;

    char *line = NULL;
    size_t line_cap = 0;

    while (getline(&line, &line_cap, stdin) > 0)
    {
        // copy string
        size_t len = strlen(line) + 1;
        char *line_copy = malloc(len);
        memcpy(line_copy, line, len);

        SDL_Event event = {.user = {.type = COMMAND_EVENT, .data1 = line_copy}};
        SDL_PushEvent(&event);
    }

    return 0;
}

int search_moves_task(void *_)
{
    (void)_;

    SDL_AtomicSet(&abort_search, 0);
    Move best_move = search_move_abortable(&abort_search, &bs, &seen_positions);

    char buffer[6] = {0};
    move_to_long_notation(best_move, buffer);
    printf("bestmove %s\n", buffer);
    fflush(stdout);

    SDL_RemoveTimer(abort_timer_id);

    return 0;
}

Uint32 abort_timer_task(Uint32 _, void *__)
{
    (void)_;
    (void)__;

    SDL_RemoveTimer(abort_timer_id);
    SDL_AtomicSet(&abort_search, 1);
    printf("Aborted by timer\n");

    return 0;
}

bool handle_command(char *line)
{
    if (strcmp(line, "uci") == 0)
    {
        printf("id name Coco's chess engine\n");
        printf("id author Coco\n");
        printf("uciok\n");
        fflush(stdout);
    }
    else if (strcmp(line, "isready") == 0)
    {
        printf("readyok\n");
        fflush(stdout);
    }
    else if (strcmp(line, "quit") == 0)
    {
        return true;
    }
    else if (strcmp(line, "print") == 0)
    {
        print_board(&bs);
    }
    else if (starts_with("position", line))
    {
        array_free(seen_positions);
        seen_positions = array_create(uint64_t);
        bs = parse_position_command(line, &seen_positions);
    }
    else if (starts_with("go", line))
    {
        SDL_Thread *search_thread = SDL_CreateThread(&search_moves_task, "search", NULL);
        SDL_DetachThread(search_thread);

        abort_timer_id = SDL_AddTimer(1000, &abort_timer_task, NULL);
    }
    else if (starts_with("stop", line))
    {
        SDL_AtomicSet(&abort_search, 1);
        SDL_RemoveTimer(abort_timer_id);
    }

    return false;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

#ifdef _WIN32
    SetConsoleOutputCP(65001); // unicode
#endif

    zobrist_init();

    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0)
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        abort();
    }

    bs = load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    seen_positions = array_create(uint64_t);

    COMMAND_EVENT = SDL_RegisterEvents(1);
    SDL_CreateThread(&command_reader_task, "stdin reader", NULL);

    SDL_Event event;
    while (SDL_WaitEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            break;
        }
        else if (event.type == COMMAND_EVENT)
        {
            bool quit = handle_command(event.user.data1);
            free(event.user.data1);
            if (quit)
            {
                break;
            }
        }
    }

    return 0;
}

#pragma once

#include "board.h"
#include "piece.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct PGNReader
    {
        char *str;
        size_t len;
        size_t pos;
    } PGNReader;

    PGNReader PGNReader_create_from_cstring(char *str);
    bool PGNReader_has_more(PGNReader *reader);
    void PGNReader_read(PGNReader *reader, size_t len, char *out);
    void PGNReader_peek(PGNReader *reader, size_t len, char *out);

    bool PGNReader_try_char(PGNReader *reader, char c);
    bool PGNReader_try_string(PGNReader *reader, char *str);
    bool PGNReader_try_char_range(PGNReader *reader, char range_start, char range_end, char *out);
    bool PGNReader_try_piece_type(PGNReader *reader, PieceType *out);
    bool PGNReader_try_pos(PGNReader *reader, Pos *out);

#ifdef __cplusplus
}
#endif

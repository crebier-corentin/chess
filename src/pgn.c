#include "pgn.h"

PGNReader PGNReader_create_from_cstring(char *str)
{
    assert(str != NULL);

    PGNReader reader = {0};
    reader.str = str;
    reader.len = strlen(str);
    reader.pos = 0;
    return reader;
}

bool PGNReader_has_more(PGNReader *reader)
{
    assert(reader != NULL);

    return reader->pos < reader->len;
}

void PGNReader_read(PGNReader *reader, size_t len, char *out)
{
    assert(out != NULL);
    assert(len > 0);

    for (size_t i = 0; i < len; i++)
    {
        assert(PGNReader_has_more(reader));
        out[i] = reader->str[reader->pos++];
    }
    out[len] = '\0';
}

void PGNReader_peek(PGNReader *reader, size_t len, char *out)
{
    assert(out != NULL);
    assert(len > 0);

    size_t pos = reader->pos;

    for (size_t i = 0; i < len; i++)
    {
        assert(pos < reader->len);
        out[i] = reader->str[pos];
    }
    out[len] = '\0';
}

bool PGNReader_try_char(PGNReader *reader, char c)
{
    assert(reader != NULL);

    if (PGNReader_has_more(reader) && reader->str[reader->pos] == c)
    {
        reader->pos++;
        return true;
    }
    return false;
}

bool PGNReader_try_string(PGNReader *reader, char *str)
{
    assert(reader != NULL);
    assert(str != NULL);

    size_t len = strlen(str);
    if (reader->pos + len >= reader->len)
    {
        return false;
    }

    for (size_t i = 0; i < len; i++)
    {
        if (reader->str[reader->pos + i] != str[i])
        {
            return false;
        }
    }
    reader->pos += len;
    return true;
}

bool PGNReader_try_char_range(PGNReader *reader, char range_start, char range_end, char *out)
{
    assert(reader != NULL);
    assert(range_start <= range_end);
    assert(out != NULL);

    if (!PGNReader_has_more(reader))
    {
        return false;
    }

    char c = reader->str[reader->pos];
    if (c >= range_start && c <= range_end)
    {
        reader->pos++;
        *out = c;
        return true;
    }

    return false;
}

bool PGNReader_try_piece_type(PGNReader *reader, PieceType *out)
{
    assert(reader != NULL);
    assert(out != NULL);

    if (!PGNReader_has_more(reader))
    {
        return false;
    }

    char c = reader->str[reader->pos];
    switch (c)
    {
    case 'K':
        *out = PT_KING;
        reader->pos++;
        return true;
    case 'Q':
        *out = PT_QUEEN;
        reader->pos++;
        return true;
    case 'R':
        *out = PT_ROOK;
        reader->pos++;
        return true;
    case 'B':
        *out = PT_BISHOP;
        reader->pos++;
        return true;
    case 'N':
        *out = PT_KNIGHT;
        reader->pos++;
        return true;
    }

    return false;
}

bool PGNReader_try_pos(PGNReader *reader, Pos *out)
{
    assert(reader != NULL);
    assert(out != NULL);

    // Rank
    if (!PGNReader_has_more(reader))
    {
        return false;
    }

    char c = reader->str[reader->pos];
    if (c >= 'a' && c <= 'h')
    {
        out->x = c - 'a';
        reader->pos++;
    }
    else
    {
        return false;
    }

    // File
    if (!PGNReader_has_more(reader))
    {
        return false;
    }

    c = reader->str[reader->pos];
    if (c >= '1' && c <= '8')
    {
        out->y = 7 - (c - '1');
        reader->pos++;
        return true;
    }

    return false;
}

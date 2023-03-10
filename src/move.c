
#include "move.h"
#include "board.h"
#include "piece.h"
#include <assert.h>
#include <pcre2.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void pos_to_string(Pos pos, char *buffer)
{
    assert(buffer != NULL);

    buffer[0] = 'a' + pos.x;
    buffer[1] = '1' + (7 - pos.y);
    buffer[2] = '\0';
}
Pos parse_pos(char *buffer)
{
    assert(buffer != NULL);
    assert(buffer[0] >= 'a' && buffer[0] <= 'h');
    assert(buffer[1] >= '1' && buffer[1] <= '8');

    Pos pos = {0};
    pos.x = buffer[0] - 'a';
    pos.y = 7 - (buffer[1] - '1');
    return pos;
}

PieceType promotion_to_piece_type(Promotion promotion)
{
    assert(promotion != PROMOTION_NONE);

    switch (promotion)
    {
    case PROMOTION_QUEEN:
        return PT_QUEEN;
    case PROMOTION_ROOK:
        return PT_ROOK;
    case PROMOTION_BISHOP:
        return PT_BISHOP;
    case PROMOTION_KNIGHT:
        return PT_KNIGHT;
    default:
        assert(false);
        return 0;
    }
}

#define MOVE_EN_PASSANT_MASK (1 << 5)
#define MOVE_PROMOTION_MASK ((1 << 2) | (1 << 3) | (1 << 4))
#define MOVE_CASTLE_MASK ((1 << 1) | 1)

Move move_create(Pos from, Pos to, Promotion promotion, Castle castle, bool en_passant)
{
    return (Move){.from = from, .to = to, .special = (en_passant << 5) | (promotion << 2) | castle};
}
bool move_get_en_passant(Move *m)
{
    return (bool)((m->special & MOVE_EN_PASSANT_MASK) >> 5);
}
Promotion move_get_promotion(Move *m)
{
    return (Promotion)((m->special & MOVE_PROMOTION_MASK) >> 2);
}
Castle move_get_castle(Move *m)
{
    return (Castle)(m->special & MOVE_CASTLE_MASK);
}

bool move_equals(Move a, Move b)
{
    return a.to.x == b.to.x && a.to.y == b.to.y && a.from.x == b.from.x && a.from.y == b.from.y &&
           a.special == b.special;
}

void move_to_long_notation(Move move, char buffer[6])
{
    assert(buffer != NULL);

    pos_to_string(move.from, buffer);
    pos_to_string(move.to, buffer + 2);

    switch (move_get_promotion(&move))
    {
    case PROMOTION_QUEEN:
        buffer[4] = 'q';
        break;
    case PROMOTION_ROOK:
        buffer[4] = 'r';
        break;
    case PROMOTION_BISHOP:
        buffer[4] = 'b';
        break;
    case PROMOTION_KNIGHT:
        buffer[4] = 'n';
        break;
    case PROMOTION_NONE:
        buffer[4] = '\0';
        break;
    }

    buffer[5] = '\0';
}

static Promotion char_to_promotion(char c)
{
    switch (c)
    {
    case 'q':
    case 'Q':
        return PROMOTION_QUEEN;

    case 'r':
    case 'R':
        return PROMOTION_ROOK;

    case 'b':
    case 'B':
        return PROMOTION_BISHOP;

    case 'n':
    case 'N':
        return PROMOTION_KNIGHT;

    default:
        return PROMOTION_NONE;
    }
}

Move parse_long_notation(BoardState *bs, char buffer[6])
{
    assert(buffer != NULL);

    Pos from = parse_pos(buffer);
    Pos to = parse_pos(buffer + 2);

    Promotion promotion = char_to_promotion(buffer[4]);

    // Check castle
    Castle castle = CASTLE_NONE;
    if (from.x == 4 && from.y == 0 && to.x == 6 && to.y == 0)
    {
        castle = CASTLE_KINGSIDE;
    }
    else if (from.x == 4 && from.y == 0 && to.x == 2 && to.y == 0)
    {
        castle = CASTLE_QUEENSIDE;
    }
    else if (from.x == 4 && from.y == 7 && to.x == 6 && to.y == 7)
    {
        castle = CASTLE_KINGSIDE;
    }
    else if (from.x == 4 && from.y == 7 && to.x == 2 && to.y == 7)
    {
        castle = CASTLE_QUEENSIDE;
    }

    // Check en passant
    int en_passant_y = bs->turn == C_WHITE ? 2 : 5;
    bool en_passant = to.y == en_passant_y && is_pawn(get_piece(bs, from)) && is_empty(get_piece(bs, to)) &&
                      is_pawn(get_piece(bs, (Pos){to.x, from.y}));

    return move_create(from, to, promotion, castle, en_passant);
}

#define POS_NOT_SPECIFIED -9

typedef struct AlgebraicNotation
{
    PieceType piece_type;
    Pos from; // x or y is POS_NOT_SPECIFIED if not specified
    bool capture;
    Pos to;
    Promotion promotion;
    Castle castle;
    bool check;
    bool checkmate;
} AlgebraicNotation;

static AlgebraicNotation parse_algebraic_notation_AN(char *buffer)
{
    // lazy compile regex
    static pcre2_code *algebraic_move_regex = NULL;
    if (algebraic_move_regex == NULL)
    {
        int errornumber;
        PCRE2_SIZE erroroffset;

        algebraic_move_regex = pcre2_compile((PCRE2_SPTR) "(?:"
                                                          "    (?:"
                                                          "        (?<piece>[KQRBN])?"
                                                          "        (?<file1>[a-h])?"
                                                          "        (?<rank1>[1-8])?"
                                                          "        (?<captures>x)?"
                                                          "        (?<file2>[a-h])"
                                                          "        (?<rank2>[1-8])"
                                                          "        (?:=(?<promotion>[QRBN]))?"
                                                          "    ) |"
                                                          "    (?<castle_queenside>O-O-O) |"
                                                          "    (?<castle_kingside>O-O)"
                                                          ")"
                                                          "(?<check_or_mate>[+#])?"

                                             ,
                                             PCRE2_ZERO_TERMINATED, PCRE2_EXTENDED, &errornumber, &erroroffset, NULL);

        if (algebraic_move_regex == NULL)
        {
            PCRE2_UCHAR error_buffer[256];
            pcre2_get_error_message(errornumber, error_buffer, sizeof(error_buffer));
            fprintf(stderr, "PCRE2 compilation failed at offset %d: %s\n", (int)erroroffset, error_buffer);

            abort();
        }
    }

    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(algebraic_move_regex, NULL);
    assert(match_data != NULL);

    int match_res =
        pcre2_match(algebraic_move_regex, (PCRE2_SPTR)buffer, PCRE2_ZERO_TERMINATED, 0, 0, match_data, NULL);
    assert(match_res > 0);

    // Regex match -> AlgebraicNotation
    AlgebraicNotation an = {0};
    PCRE2_UCHAR match_buffer[5] = {0};
    PCRE2_SIZE buffer_size = sizeof(match_buffer);

    // Castle
    if (pcre2_substring_length_byname(match_data, (PCRE2_SPTR) "castle_kingside", NULL) == 0)
    {
        an.castle = CASTLE_KINGSIDE;
    }
    else if (pcre2_substring_length_byname(match_data, (PCRE2_SPTR) "castle_queenside", NULL) == 0)
    {
        an.castle = CASTLE_QUEENSIDE;
    }
    // Piece move
    else
    {
        if (pcre2_substring_copy_byname(match_data, (PCRE2_SPTR) "piece", match_buffer, &buffer_size) == 0)
        {
            switch (match_buffer[0])
            {
            case 'K':
            case 'k':
                an.piece_type = PT_KING;
                break;
            case 'Q':
            case 'q':
                an.piece_type = PT_QUEEN;
                break;
            case 'R':
            case 'r':
                an.piece_type = PT_ROOK;
                break;
            case 'B':
            case 'b':
                an.piece_type = PT_BISHOP;
                break;
            case 'N':
            case 'n':
                an.piece_type = PT_KNIGHT;
                break;
            }
        }
        else
        {
            an.piece_type = PT_PAWN;
        }
        buffer_size = sizeof(match_buffer);

        if (pcre2_substring_copy_byname(match_data, (PCRE2_SPTR) "file1", match_buffer, &buffer_size) == 0)
        {
            an.from.x = match_buffer[0] - 'a';
        }
        else
        {
            an.from.x = POS_NOT_SPECIFIED;
        }
        buffer_size = sizeof(match_buffer);

        if (pcre2_substring_copy_byname(match_data, (PCRE2_SPTR) "rank1", match_buffer, &buffer_size) == 0)
        {
            an.from.y = 7 - (match_buffer[0] - '1');
        }
        else
        {
            an.from.y = POS_NOT_SPECIFIED;
        }
        buffer_size = sizeof(match_buffer);

        // if group exists, capture is true
        an.capture = pcre2_substring_length_byname(match_data, (PCRE2_SPTR) "captures", NULL) == 0;

        match_res = pcre2_substring_copy_byname(match_data, (PCRE2_SPTR) "file2", match_buffer, &buffer_size);
        assert(match_res == 0);
        an.to.x = match_buffer[0] - 'a';
        buffer_size = sizeof(match_buffer);

        match_res = pcre2_substring_copy_byname(match_data, (PCRE2_SPTR) "rank2", match_buffer, &buffer_size);
        assert(match_res == 0);
        an.to.y = 7 - (match_buffer[0] - '1');
        buffer_size = sizeof(match_buffer);

        if (pcre2_substring_copy_byname(match_data, (PCRE2_SPTR) "promotion", match_buffer, &buffer_size) == 0)
        {
            an.promotion = char_to_promotion(match_buffer[0]);
        }
        else
        {
            an.promotion = PROMOTION_NONE;
        }
        buffer_size = sizeof(match_buffer);
    }

    if (pcre2_substring_copy_byname(match_data, (PCRE2_SPTR) "check_or_mate", match_buffer, &buffer_size) == 0)
    {
        an.check = match_buffer[0] == '+';
        an.checkmate = match_buffer[0] == '#';
    }
    else
    {
        an.check = false;
        an.checkmate = false;
    }

    pcre2_match_data_free(match_data);

    return an;
}

Move parse_algebraic_notation(BoardState *bs, char *buffer)
{
    AlgebraicNotation an = parse_algebraic_notation_AN(buffer);

    // AlgebraicNotation -> Move
    Array(Move) moves = array_create_size(Move, 32);
    generate_legal_moves(bs, bs->turn, &moves);

    // find move that match the algebraic notation
    Move matching_move = {0};
    for (size_t i = 0; i < array_len(moves); i++)
    {
        Move move = moves[i];

        // For castles, only need to match castle property, rest is ignored
        if (an.castle != CASTLE_NONE && an.castle == move_get_castle(&move))
        {
            matching_move = move;
            break;
        }

        // match piece type
        if (get_type(get_piece(bs, move.from)) != an.piece_type)
            continue;

        // match to square
        if (an.to.x != move.to.x || an.to.y != move.to.y)
            continue;

        // match from square
        if (an.from.x != POS_NOT_SPECIFIED && an.from.x != move.from.x)
            continue;
        if (an.from.y != POS_NOT_SPECIFIED && an.from.y != move.from.y)
            continue;

        // match promotion
        if (an.promotion != PROMOTION_NONE && an.promotion != move_get_promotion(&move))
            continue;

        matching_move = move;
        break;
    }

    array_free(moves);

    return matching_move;
}

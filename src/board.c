#include "board.h"
#include "array.h"
#include "move.h"
#include "piece.h"
#include "zobrist.h"
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int8_t pieces_all_color_len(PiecesListLengths *pll)
{
    return pll->pawn_len + pll->knight_len + pll->bishop_len + pll->rook_len + pll->queen_len + pll->king_len;
}

void pieces_offset(PiecesList *pl, PieceType pt, Color c, int8_t *out_index, int8_t **out_len)
{
    assert(pl != NULL);
    assert(out_index != NULL);
    assert(out_len != NULL);

    int8_t index = c == C_WHITE ? 0 : pieces_all_color_len(&pl->white);
    PiecesListLengths *pll = c == C_WHITE ? &pl->white : &pl->black;

    if (pt == PT_PAWN)
    {
        *out_index = index;
        *out_len = &pll->pawn_len;
        return;
    }
    index += pll->pawn_len;

    if (pt == PT_KNIGHT)
    {
        *out_index = index;
        *out_len = &pll->knight_len;
        return;
    }
    index += pll->knight_len;

    if (pt == PT_BISHOP)
    {
        *out_index = index;
        *out_len = &pll->bishop_len;
        return;
    }
    index += pll->bishop_len;

    if (pt == PT_ROOK)
    {
        *out_index = index;
        *out_len = &pll->rook_len;
        return;
    }
    index += pll->rook_len;

    if (pt == PT_QUEEN)
    {
        *out_index = index;
        *out_len = &pll->queen_len;
        return;
    }
    index += pll->queen_len;

    if (pt == PT_KING)
    {
        *out_index = index;
        *out_len = &pll->king_len;
        return;
    }

    assert(false && "Invalid piece type");
}
void pieces_all_color_offset(PiecesList *pl, Color c, int8_t *out_index, int8_t *out_len)
{
    assert(pl != NULL);
    assert(out_index != NULL);
    assert(out_len != NULL);

    if (c == C_WHITE)
    {
        *out_index = 0;
        *out_len = pieces_all_color_len(&pl->white);
    }
    else
    {
        *out_index = pieces_all_color_len(&pl->white);
        *out_len = pieces_all_color_len(&pl->black);
    }
}

static int8_t pieces_total_len(PiecesList *pl)
{
    assert(pl != NULL);

    return pieces_all_color_len(&pl->white) + pieces_all_color_len(&pl->black);
}

static void pieces_insert(PiecesList *pl, Piece p, Pos pos)
{
    assert(pl != NULL);
    assert(pieces_total_len(pl) < 64);

    int8_t index;
    int8_t *len;
    pieces_offset(pl, get_type(p), get_color(p), &index, &len);

    memmove(&pl->list[index + 1], &pl->list[index], (pieces_total_len(pl) - index + 1) * sizeof(Pos));

    pl->list[index] = pos;
    (*len)++;
}
static void pieces_remove(PiecesList *pl, Piece p, Pos pos)
{
    assert(pl != NULL);
    assert(pieces_total_len(pl) > 0);

    int8_t index;
    int8_t *len;
    pieces_offset(pl, get_type(p), get_color(p), &index, &len);

    // Find the piece
    for (int8_t i = index; i < index + *len; i++)
    {
        if (pl->list[i].x == pos.x && pl->list[i].y == pos.y)
        {
            memmove(&pl->list[i], &pl->list[i + 1], (pieces_total_len(pl) - i - 1) * sizeof(Pos));
            (*len)--;
            return;
        }
    }
}

BoardState load_fen(const char *fen)
{
    assert(fen != NULL);

    BoardState bs = {0};
    bs.en_passant_x = NO_EN_PASSANT;
    bs.en_passant_y = NO_EN_PASSANT;

    int8_t x = 0;
    int8_t y = 0;

    while (!(x == 8 && y == 7))
    {
        switch (*fen++)
        {
        case 'p':
            set_piece(&bs, (Pos){x, y}, create_piece(PT_PAWN, C_BLACK));
            x++;
            break;
        case 'n':
            set_piece(&bs, (Pos){x, y}, create_piece(PT_KNIGHT, C_BLACK));
            x++;
            break;
        case 'b':
            set_piece(&bs, (Pos){x, y}, create_piece(PT_BISHOP, C_BLACK));
            x++;
            break;
        case 'r':
            set_piece(&bs, (Pos){x, y}, create_piece(PT_ROOK, C_BLACK));
            x++;
            break;
        case 'q':
            set_piece(&bs, (Pos){x, y}, create_piece(PT_QUEEN, C_BLACK));
            x++;
            break;
        case 'k':
            set_piece(&bs, (Pos){x, y}, create_piece(PT_KING, C_BLACK));
            x++;
            break;

        case 'P':
            set_piece(&bs, (Pos){x, y}, create_piece(PT_PAWN, C_WHITE));
            x++;
            break;
        case 'N':
            set_piece(&bs, (Pos){x, y}, create_piece(PT_KNIGHT, C_WHITE));
            x++;
            break;
        case 'B':
            set_piece(&bs, (Pos){x, y}, create_piece(PT_BISHOP, C_WHITE));
            x++;
            break;
        case 'R':
            set_piece(&bs, (Pos){x, y}, create_piece(PT_ROOK, C_WHITE));
            x++;
            break;
        case 'Q':
            set_piece(&bs, (Pos){x, y}, create_piece(PT_QUEEN, C_WHITE));
            x++;
            break;
        case 'K':
            set_piece(&bs, (Pos){x, y}, create_piece(PT_KING, C_WHITE));
            x++;
            break;

        case '1':
            x += 1;
            break;
        case '2':
            x += 2;
            break;
        case '3':
            x += 3;
            break;
        case '4':
            x += 4;
            break;
        case '5':
            x += 5;
            break;
        case '6':
            x += 6;
            break;
        case '7':
            x += 7;
            break;
        case '8':
            x += 8;
            break;

        case '/':
            x = 0;
            y++;
            break;
        }
    }

    fen++; // space
    bs.turn = *fen++ == 'w' ? C_WHITE : C_BLACK;
    if (bs.turn == C_BLACK)
    {
        bs.zobrist_hash ^= zobrist_black();
    }
    fen++; // space

    while (*fen != ' ')
    {
        switch (*fen++)
        {
        case 'K':
            update_castle_right(&bs, C_WHITE, true, true);
            break;
        case 'Q':
            update_castle_right(&bs, C_WHITE, false, true);
            break;
        case 'k':
            update_castle_right(&bs, C_BLACK, true, true);
            break;
        case 'q':
            update_castle_right(&bs, C_BLACK, false, true);
            break;
        }
    }
    fen++; // space

    if (*fen != '-')
    {
        bs.en_passant_x = *fen++ - 'a';       // file
        bs.en_passant_y = 7 - (*fen++ - '1'); // rank
        bs.zobrist_hash ^= zobrist_en_passant(bs.en_passant_y);
    }
    fen++; // space

    sscanf(fen, "%d %d", &bs.halfmove_clock, &bs.fullmove_number);

    return bs;
}

void print_board(BoardState *bs)
{
    assert(bs != NULL);

    fputs("+---+---+---+---+---+---+---+---+\n", stdout);

    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            fputs("| ", stdout);

            Piece p = bs->board[x][y];
            if (is_empty(p))
            {
                fputs(" ", stdout);
            }
            else if (is_white(p))
            {
                switch (get_type(p))
                {

                case PT_PAWN:
                    fputs("♙", stdout);
                    break;
                case PT_KNIGHT:
                    fputs("♘", stdout);
                    break;
                case PT_BISHOP:
                    fputs("♗", stdout);
                    break;
                case PT_ROOK:
                    fputs("♖", stdout);
                    break;
                case PT_QUEEN:
                    fputs("♕", stdout);
                    break;
                case PT_KING:
                    fputs("♔", stdout);
                    break;
                }
            }
            else
            {
                switch (get_type(p))
                {

                case PT_PAWN:
                    fputs("♟", stdout);
                    break;
                case PT_KNIGHT:
                    fputs("♞", stdout);
                    break;
                case PT_BISHOP:
                    fputs("♝", stdout);
                    break;
                case PT_ROOK:
                    fputs("♜", stdout);
                    break;
                case PT_QUEEN:
                    fputs("♛", stdout);
                    break;
                case PT_KING:
                    fputs("♚", stdout);
                    break;
                }
            }

            fputs(" ", stdout);
        }

        fputs("|\n+---+---+---+---+---+---+---+---+\n", stdout);
    }
}

void print_attack_map(bool attack_map[8][8])
{
    assert(attack_map != NULL);

    fputs("+---+---+---+---+---+---+---+---+\n", stdout);

    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            fputs("| ", stdout);

            fputs(attack_map[x][y] ? "x" : " ", stdout);

            fputs(" ", stdout);
        }

        fputs("|\n+---+---+---+---+---+---+---+---+\n", stdout);
    }
}

void set_piece(BoardState *bs, Pos pos, Piece p)
{
    assert(bs != NULL);
    assert(pos.x >= 0 && pos.x <= 7);
    assert(pos.y >= 0 && pos.y <= 7);

    if (!is_empty(bs->board[pos.x][pos.y]))
    {
        pieces_remove(&bs->pieces, bs->board[pos.x][pos.y], pos);
        bs->zobrist_hash ^= zobrist_piece(bs->board[pos.x][pos.y], pos);
    }

    bs->board[pos.x][pos.y] = p;

    if (!is_empty(p))
    {
        pieces_insert(&bs->pieces, p, pos);
        bs->zobrist_hash ^= zobrist_piece(bs->board[pos.x][pos.y], pos);
    }
}

Piece get_piece(BoardState *bs, Pos pos)
{
    assert(bs != NULL);
    assert(pos.x >= 0 && pos.x <= 7);
    assert(pos.y >= 0 && pos.y <= 7);

    return bs->board[pos.x][pos.y];
}

void update_castle_right(BoardState *bs, Color c, bool king_side, bool value)
{
    assert(bs != NULL);

    bool *castle_right;

    if (c == C_WHITE)
    {
        if (king_side)
        {
            castle_right = &bs->white_king_side_castle;
        }
        else
        {
            castle_right = &bs->white_queen_side_castle;
        }
    }
    else
    {
        if (king_side)
        {
            castle_right = &bs->black_king_side_castle;
        }
        else
        {
            castle_right = &bs->black_queen_side_castle;
        }
    }

    if (*castle_right != value)
    {
        bs->zobrist_hash ^= zobrist_castle_right(c, king_side);
        *castle_right = value;
    }
}

void make_move(BoardState *bs, Move move)
{
    assert(bs != NULL);

    Piece p = get_piece(bs, move.from);
    assert(is_empty(p) == false);

    bool capture = false;

    // Reset en passant
    if (bs->en_passant_y != NO_EN_PASSANT)
    {
        bs->zobrist_hash ^= zobrist_en_passant(bs->en_passant_y);
    }
    bs->en_passant_x = NO_EN_PASSANT;
    bs->en_passant_y = NO_EN_PASSANT;

    // Castle
    if (is_king(p) && move.castle != CASTLE_NONE)
    {
        if (move.castle == CASTLE_KINGSIDE)
        {
            // Move king
            Pos kingPos = move.from;
            kingPos.x = 6;
            set_piece(bs, kingPos, p);
            set_piece(bs, move.from, piece_empty());

            // Move rook
            set_piece(bs, (Pos){5, move.from.y}, create_piece(PT_ROOK, get_color(p)));
            set_piece(bs, (Pos){7, move.from.y}, piece_empty());
        }
        else
        {
            // Move king
            Pos kingPos = move.from;
            kingPos.x = 2;
            set_piece(bs, kingPos, p);
            set_piece(bs, move.from, piece_empty());

            // Move rook
            set_piece(bs, (Pos){3, move.from.y}, create_piece(PT_ROOK, get_color(p)));
            set_piece(bs, (Pos){0, move.from.y}, piece_empty());
        }
    }
    else
    {
        if (!is_empty(get_piece(bs, move.to)))
        {
            capture = true;
        }

        // Move piece
        set_piece(bs, move.to, p);
        set_piece(bs, move.from, piece_empty());

        // En passant
        if (is_pawn(p) && move.en_passant)
        {
            Pos enPassantPos = move.to;
            enPassantPos.y = move.from.y;
            set_piece(bs, enPassantPos, piece_empty());

            capture = true;
        }

        // Promotion
        if (is_pawn(p) && (move.to.y == 0 || move.to.y == 7) && move.promotion != PROMOTION_NONE)
        {
            set_piece(bs, move.to, create_piece(promotion_to_piece_type(move.promotion), get_color(p)));
        }

        // Double step set en passant
        if (is_pawn(p) && abs(move.to.y - move.from.y) == 2)
        {
            bs->en_passant_x = move.to.x;
            bs->en_passant_y = move.to.y + (get_color(p) == C_WHITE ? 1 : -1);
            bs->zobrist_hash ^= zobrist_en_passant(bs->en_passant_y);
        }
    }

    // Update turn
    bs->turn = get_color(p) == C_WHITE ? C_BLACK : C_WHITE;
    bs->zobrist_hash ^= zobrist_black();

    // Update fullmove clock
    if (bs->turn == C_WHITE)
    {
        bs->fullmove_number++;
    }

    // Update halfmove clock
    if (is_pawn(p) || capture)
    {
        bs->halfmove_clock = 0;
    }
    else
    {
        bs->halfmove_clock++;
    }

    // Update castling rights
    if (is_king(p))
    {
        update_castle_right(bs, get_color(p), true, false);
        update_castle_right(bs, get_color(p), false, false);
    }
    else if (is_rook(p))
    {
        if (move.from.x == 7)
        {
            update_castle_right(bs, get_color(p), true, false);
        }
        else if (move.from.x == 0)
        {
            update_castle_right(bs, get_color(p), false, false);
        }
    }

    // Castling rights rook captured
    if (capture)
    {
        if (move.to.x == 0 && move.to.y == 7)
        {
            update_castle_right(bs, C_WHITE, false, false);
        }
        else if (move.to.x == 7 && move.to.y == 7)
        {
            update_castle_right(bs, C_WHITE, true, false);
        }
        else if (move.to.x == 0 && move.to.y == 0)
        {
            update_castle_right(bs, C_BLACK, false, false);
        }
        else if (move.to.x == 7 && move.to.y == 0)
        {
            update_castle_right(bs, C_BLACK, true, false);
        }
    }
}

bool is_in_check(BoardState *bs, Color color)
{
    assert(bs != NULL);

    // Find king
    int8_t king_index;
    int8_t *king_len;
    pieces_offset(&bs->pieces, PT_KING, color, &king_index, &king_len);
    // No king found
    if (*king_len == 0)
    {
        return false;
    }
    Pos king_pos = bs->pieces.list[king_index];

    // Check if any piece can attack the king
    bool attack_map[8][8];
    generate_attack_map(bs, color == C_WHITE ? C_BLACK : C_WHITE, attack_map);

    return attack_map[king_pos.x][king_pos.y];
}

// Move generation //

static void generate_step_pseudo_move(BoardState *bs, Pos pos, Pos target_pos, Array(Move) * out_moves)
{
    assert(bs != NULL);
    assert(out_moves != NULL);

    // Out of board
    if (target_pos.x < 0 || target_pos.x > 7 || target_pos.y < 0 || target_pos.y > 7)
    {
        return;
    }

    // Allied piece
    if (get_color(get_piece(bs, target_pos)) == get_color(get_piece(bs, pos)))
    {
        return;
    }

    Move move = {pos, target_pos, false, CASTLE_NONE, PROMOTION_NONE};
    array_push(*out_moves, move);
}

static void generate_slide_pseudo_moves(BoardState *bs, Pos pos, int8_t direction_x, int8_t direction_y,
                                        Array(Move) * out_moves)
{
    assert(bs != NULL);
    assert(out_moves != NULL);

    for (Pos target_pos = {pos.x + direction_x, pos.y + direction_y};
         target_pos.x >= 0 && target_pos.x <= 7 && target_pos.y >= 0 && target_pos.y <= 7;
         target_pos.x += direction_x, target_pos.y += direction_y)
    {
        Piece p = get_piece(bs, target_pos);
        if (is_empty(p))
        {
            Move move = {pos, target_pos, PROMOTION_NONE, CASTLE_NONE, false};
            array_push(*out_moves, move);
        }
        else
        {
            // Capture
            if (get_color(p) != get_color(get_piece(bs, pos)))
            {
                Move move = {pos, target_pos, PROMOTION_NONE, CASTLE_NONE, false};
                array_push(*out_moves, move);
            }

            break;
        }
    }
}

// If final rank, add all promotions
static void generate_pawn_pseudo_moves_step(Pos pos, Pos target_pos, int8_t ending_y, bool en_passant,
                                            Array(Move) * out_moves)
{
    assert(out_moves != NULL);

    // Final rank, all promotions
    if (target_pos.y == ending_y)
    {
        Move move = {pos, target_pos, PROMOTION_QUEEN, CASTLE_NONE, en_passant};
        array_push(*out_moves, move);
        move.promotion = PROMOTION_ROOK;
        array_push(*out_moves, move);
        move.promotion = PROMOTION_BISHOP;
        array_push(*out_moves, move);
        move.promotion = PROMOTION_KNIGHT;
        array_push(*out_moves, move);
    }
    else
    {
        Move move = {pos, target_pos, PROMOTION_NONE, CASTLE_NONE, en_passant};
        array_push(*out_moves, move);
    }
}

static void generate_pawn_pseudo_moves_for_color(BoardState *bs, Pos pos, int8_t direction, int8_t starting_y,
                                                 int8_t ending_y, Array(Move) * out_moves)
{
    Pos forward_pos = {pos.x, pos.y + direction};
    if (forward_pos.y >= 0 && forward_pos.y <= 7 && get_piece(bs, forward_pos) == piece_empty())
    {
        generate_pawn_pseudo_moves_step(pos, forward_pos, ending_y, false, out_moves);

        // Double step
        if (pos.y == starting_y)
        {
            Pos double_step_pos = {pos.x, pos.y + 2 * direction};
            if (get_piece(bs, double_step_pos) == piece_empty())
            {
                Move move = {pos, double_step_pos, PROMOTION_NONE, CASTLE_NONE, false};
                array_push(*out_moves, move);
            }
        }
    }

    // Left
    if (pos.x != 0)
    {
        Pos left = (Pos){pos.x - 1, pos.y + direction};

        // Capture
        if (!is_empty(get_piece(bs, left)) && get_color(get_piece(bs, left)) != get_color(get_piece(bs, pos)))
        {
            generate_pawn_pseudo_moves_step(pos, left, ending_y, false, out_moves);
        }

        // En passant
        if (bs->en_passant_x == pos.x - 1 && bs->en_passant_y == pos.y + direction)
        {
            generate_pawn_pseudo_moves_step(pos, left, ending_y, true, out_moves);
        }
    }

    // Right
    if (pos.x != 7)
    {
        Pos right = (Pos){pos.x + 1, pos.y + direction};

        // Capture
        if (!is_empty(get_piece(bs, right)) && get_color(get_piece(bs, right)) != get_color(get_piece(bs, pos)))
        {
            generate_pawn_pseudo_moves_step(pos, right, ending_y, false, out_moves);
        }

        // En passant
        if (bs->en_passant_x == pos.x + 1 && bs->en_passant_y == pos.y + direction)
        {
            generate_pawn_pseudo_moves_step(pos, right, ending_y, true, out_moves);
        }
    }
}

void generate_pseudo_moves(BoardState *bs, Color color, Array(Move) * out_moves)
{
    assert(bs != NULL);
    assert(out_moves != NULL);

    int8_t index;
    int8_t *len;

    pieces_offset(&bs->pieces, PT_PAWN, color, &index, &len);
    for (int8_t i = index; i < index + *len; i++)
    {
        generate_pawn_pseudo_moves(bs, bs->pieces.list[i], out_moves);
    }

    pieces_offset(&bs->pieces, PT_KNIGHT, color, &index, &len);
    for (int8_t i = index; i < index + *len; i++)
    {
        generate_knight_pseudo_moves(bs, bs->pieces.list[i], out_moves);
    }

    pieces_offset(&bs->pieces, PT_BISHOP, color, &index, &len);
    for (int8_t i = index; i < index + *len; i++)
    {
        generate_bishop_pseudo_moves(bs, bs->pieces.list[i], out_moves);
    }

    pieces_offset(&bs->pieces, PT_ROOK, color, &index, &len);
    for (int8_t i = index; i < index + *len; i++)
    {
        generate_rook_pseudo_moves(bs, bs->pieces.list[i], out_moves);
    }

    pieces_offset(&bs->pieces, PT_QUEEN, color, &index, &len);
    for (int8_t i = index; i < index + *len; i++)
    {
        generate_queen_pseudo_moves(bs, bs->pieces.list[i], out_moves);
    }

    pieces_offset(&bs->pieces, PT_KING, color, &index, &len);
    for (int8_t i = index; i < index + *len; i++)
    {
        generate_king_pseudo_moves(bs, bs->pieces.list[i], out_moves);
    }
}

void generate_piece_pseudo_moves(BoardState *bs, Pos pos, Array(Move) * out_moves)
{
    assert(bs != NULL);
    assert(out_moves != NULL);
    assert(!is_empty(get_piece(bs, pos)));

    switch (get_type(get_piece(bs, pos)))
    {
    case PT_PAWN: {
        generate_pawn_pseudo_moves(bs, pos, out_moves);
        break;
    }
    case PT_KNIGHT: {
        generate_knight_pseudo_moves(bs, pos, out_moves);
        break;
    }
    case PT_BISHOP: {
        generate_bishop_pseudo_moves(bs, pos, out_moves);
        break;
    }
    case PT_ROOK: {
        generate_rook_pseudo_moves(bs, pos, out_moves);
        break;
    }
    case PT_QUEEN: {
        generate_queen_pseudo_moves(bs, pos, out_moves);
        break;
    }
    case PT_KING: {
        generate_king_pseudo_moves(bs, pos, out_moves);
        break;
    }
    }
}

void generate_pawn_pseudo_moves(BoardState *bs, Pos pos, Array(Move) * out_moves)
{
    assert(bs != NULL);
    assert(out_moves != NULL);
    assert(is_pawn(get_piece(bs, pos)));

    if (get_color(get_piece(bs, pos)) == C_WHITE)
    {
        generate_pawn_pseudo_moves_for_color(bs, pos, -1, 6, 0, out_moves);
    }
    else
    {
        generate_pawn_pseudo_moves_for_color(bs, pos, 1, 1, 7, out_moves);
    }
}
void generate_king_pseudo_moves(BoardState *bs, Pos pos, Array(Move) * out_moves)
{
    assert(bs != NULL);
    assert(out_moves != NULL);
    assert(is_king(get_piece(bs, pos)));

    generate_step_pseudo_move(bs, pos, (Pos){pos.x - 1, pos.y - 1}, out_moves);
    generate_step_pseudo_move(bs, pos, (Pos){pos.x, pos.y - 1}, out_moves);
    generate_step_pseudo_move(bs, pos, (Pos){pos.x + 1, pos.y - 1}, out_moves);

    generate_step_pseudo_move(bs, pos, (Pos){pos.x - 1, pos.y}, out_moves);
    generate_step_pseudo_move(bs, pos, (Pos){pos.x + 1, pos.y}, out_moves);

    generate_step_pseudo_move(bs, pos, (Pos){pos.x - 1, pos.y + 1}, out_moves);
    generate_step_pseudo_move(bs, pos, (Pos){pos.x, pos.y + 1}, out_moves);
    generate_step_pseudo_move(bs, pos, (Pos){pos.x + 1, pos.y + 1}, out_moves);

    // Castle
    Color color = get_color(get_piece(bs, pos));
    bool can_castle_kingside = (color == C_WHITE ? bs->white_king_side_castle : bs->black_king_side_castle) &&
                               is_empty(get_piece(bs, (Pos){5, pos.y})) && is_empty(get_piece(bs, (Pos){6, pos.y}));
    bool can_castle_queenside = (color == C_WHITE ? bs->white_queen_side_castle : bs->black_queen_side_castle) &&
                                is_empty(get_piece(bs, (Pos){3, pos.y})) && is_empty(get_piece(bs, (Pos){2, pos.y})) &&
                                is_empty(get_piece(bs, (Pos){1, pos.y}));

    // No need to compute attack map if can't castle
    if (!can_castle_kingside && !can_castle_queenside)
    {
        return;
    }

    bool attack_map[8][8];
    generate_attack_map(bs, color == C_WHITE ? C_BLACK : C_WHITE, attack_map);

    if (can_castle_kingside && !attack_map[4][pos.y] && !attack_map[5][pos.y] && !attack_map[6][pos.y])
    {
        Move move = (Move){pos, (Pos){pos.x + 2, pos.y}, PROMOTION_NONE, CASTLE_KINGSIDE, false};
        array_push(*out_moves, move);
    }

    if (can_castle_queenside && !attack_map[4][pos.y] && !attack_map[3][pos.y] && !attack_map[2][pos.y])
    {
        Move move = (Move){pos, (Pos){pos.x - 2, pos.y}, PROMOTION_NONE, CASTLE_QUEENSIDE, false};
        array_push(*out_moves, move);
    }
}
void generate_queen_pseudo_moves(BoardState *bs, Pos pos, Array(Move) * out_moves)
{
    assert(bs != NULL);
    assert(out_moves != NULL);
    assert(is_queen(get_piece(bs, pos)));

    generate_slide_pseudo_moves(bs, pos, 0, 1, out_moves);
    generate_slide_pseudo_moves(bs, pos, 0, -1, out_moves);
    generate_slide_pseudo_moves(bs, pos, 1, 0, out_moves);
    generate_slide_pseudo_moves(bs, pos, -1, 0, out_moves);

    generate_slide_pseudo_moves(bs, pos, 1, 1, out_moves);
    generate_slide_pseudo_moves(bs, pos, -1, -1, out_moves);
    generate_slide_pseudo_moves(bs, pos, 1, -1, out_moves);
    generate_slide_pseudo_moves(bs, pos, -1, 1, out_moves);
}
void generate_rook_pseudo_moves(BoardState *bs, Pos pos, Array(Move) * out_moves)
{
    assert(bs != NULL);
    assert(out_moves != NULL);
    assert(is_rook(get_piece(bs, pos)));

    generate_slide_pseudo_moves(bs, pos, 0, 1, out_moves);
    generate_slide_pseudo_moves(bs, pos, 0, -1, out_moves);
    generate_slide_pseudo_moves(bs, pos, 1, 0, out_moves);
    generate_slide_pseudo_moves(bs, pos, -1, 0, out_moves);
}
void generate_bishop_pseudo_moves(BoardState *bs, Pos pos, Array(Move) * out_moves)
{
    assert(bs != NULL);
    assert(out_moves != NULL);
    assert(is_bishop(get_piece(bs, pos)));

    generate_slide_pseudo_moves(bs, pos, 1, 1, out_moves);
    generate_slide_pseudo_moves(bs, pos, -1, -1, out_moves);
    generate_slide_pseudo_moves(bs, pos, 1, -1, out_moves);
    generate_slide_pseudo_moves(bs, pos, -1, 1, out_moves);
}

void generate_knight_pseudo_moves(BoardState *bs, Pos pos, Array(Move) * out_moves)
{
    assert(bs != NULL);
    assert(out_moves != NULL);
    assert(is_knight(get_piece(bs, pos)));

    generate_step_pseudo_move(bs, pos, (Pos){pos.x - 2, pos.y - 1}, out_moves);
    generate_step_pseudo_move(bs, pos, (Pos){pos.x - 2, pos.y + 1}, out_moves);

    generate_step_pseudo_move(bs, pos, (Pos){pos.x + 2, pos.y - 1}, out_moves);
    generate_step_pseudo_move(bs, pos, (Pos){pos.x + 2, pos.y + 1}, out_moves);

    generate_step_pseudo_move(bs, pos, (Pos){pos.x + 1, pos.y - 2}, out_moves);
    generate_step_pseudo_move(bs, pos, (Pos){pos.x - 1, pos.y - 2}, out_moves);

    generate_step_pseudo_move(bs, pos, (Pos){pos.x + 1, pos.y + 2}, out_moves);
    generate_step_pseudo_move(bs, pos, (Pos){pos.x - 1, pos.y + 2}, out_moves);
}

void generate_legal_moves(BoardState *bs, Color color, Array(Move) * out_moves)
{
    assert(bs != NULL);
    assert(out_moves != NULL);

    Array(Move) pseudo_moves = array_create_size(Move, 32);
    generate_pseudo_moves(bs, color, &pseudo_moves);

    // Filter out moves that put the king into check
    for (size_t i = 0; i < array_len(pseudo_moves); i++)
    {
        // Fix bug
        BoardState new_bs = *bs;

        make_move(&new_bs, pseudo_moves[i]);
        if (!is_in_check(&new_bs, color))
        {
            array_push(*out_moves, pseudo_moves[i]);
        }
    }
    array_free(pseudo_moves);
}

// Attack Map generation //
static void generate_slide_attack_map(BoardState *bs, Pos pos, int8_t direction_x, int8_t direction_y,
                                      bool out_map[8][8])
{
    assert(bs != NULL);
    assert(out_map != NULL);

    for (Pos target_pos = {pos.x + direction_x, pos.y + direction_y};
         target_pos.x >= 0 && target_pos.x <= 7 && target_pos.y >= 0 && target_pos.y <= 7;
         target_pos.x += direction_x, target_pos.y += direction_y)
    {

        out_map[target_pos.x][target_pos.y] = true;

        if (!is_empty(get_piece(bs, target_pos)))
        {
            break;
        }
    }
}
static void generate_queen_attack_map(BoardState *bs, Pos pos, bool out_map[8][8])
{
    assert(bs != NULL);
    assert(out_map != NULL);
    assert(is_queen(get_piece(bs, pos)));

    generate_slide_attack_map(bs, pos, 0, 1, out_map);
    generate_slide_attack_map(bs, pos, 0, -1, out_map);
    generate_slide_attack_map(bs, pos, 1, 0, out_map);
    generate_slide_attack_map(bs, pos, -1, 0, out_map);

    generate_slide_attack_map(bs, pos, 1, 1, out_map);
    generate_slide_attack_map(bs, pos, -1, -1, out_map);
    generate_slide_attack_map(bs, pos, 1, -1, out_map);
    generate_slide_attack_map(bs, pos, -1, 1, out_map);
}
static void generate_rook_attack_map(BoardState *bs, Pos pos, bool out_map[8][8])
{
    assert(bs != NULL);
    assert(out_map != NULL);
    assert(is_rook(get_piece(bs, pos)));

    generate_slide_attack_map(bs, pos, 0, 1, out_map);
    generate_slide_attack_map(bs, pos, 0, -1, out_map);
    generate_slide_attack_map(bs, pos, 1, 0, out_map);
    generate_slide_attack_map(bs, pos, -1, 0, out_map);
}
static void generate_bishop_attack_map(BoardState *bs, Pos pos, bool out_map[8][8])
{
    assert(bs != NULL);
    assert(out_map != NULL);
    assert(is_bishop(get_piece(bs, pos)));

    generate_slide_attack_map(bs, pos, 1, 1, out_map);
    generate_slide_attack_map(bs, pos, -1, -1, out_map);
    generate_slide_attack_map(bs, pos, 1, -1, out_map);
    generate_slide_attack_map(bs, pos, -1, 1, out_map);
}

static void generate_step_attack_map(Pos target_pos, bool out_map[8][8])
{

    assert(out_map != NULL);

    if (target_pos.x >= 0 && target_pos.x <= 7 && target_pos.y >= 0 && target_pos.y <= 7)
    {
        out_map[target_pos.x][target_pos.y] = true;
    }
}
static void generate_king_attack_map(Pos pos, bool out_map[8][8])
{
    assert(out_map != NULL);

    generate_step_attack_map((Pos){pos.x - 1, pos.y - 1}, out_map);
    generate_step_attack_map((Pos){pos.x - 1, pos.y}, out_map);
    generate_step_attack_map((Pos){pos.x - 1, pos.y + 1}, out_map);

    generate_step_attack_map((Pos){pos.x, pos.y - 1}, out_map);
    generate_step_attack_map((Pos){pos.x, pos.y + 1}, out_map);

    generate_step_attack_map((Pos){pos.x + 1, pos.y - 1}, out_map);
    generate_step_attack_map((Pos){pos.x + 1, pos.y}, out_map);
    generate_step_attack_map((Pos){pos.x + 1, pos.y + 1}, out_map);
}
static void generate_knight_attack_map(Pos pos, bool out_map[8][8])
{
    assert(out_map != NULL);

    generate_step_attack_map((Pos){pos.x - 2, pos.y - 1}, out_map);
    generate_step_attack_map((Pos){pos.x - 2, pos.y + 1}, out_map);

    generate_step_attack_map((Pos){pos.x + 2, pos.y - 1}, out_map);
    generate_step_attack_map((Pos){pos.x + 2, pos.y + 1}, out_map);

    generate_step_attack_map((Pos){pos.x + 1, pos.y - 2}, out_map);
    generate_step_attack_map((Pos){pos.x - 1, pos.y - 2}, out_map);

    generate_step_attack_map((Pos){pos.x + 1, pos.y + 2}, out_map);
    generate_step_attack_map((Pos){pos.x - 1, pos.y + 2}, out_map);
}
static void generate_pawn_attack_map(BoardState *bs, Pos pos, bool out_map[8][8])
{
    assert(bs != NULL);
    assert(out_map != NULL);
    assert(is_pawn(get_piece(bs, pos)));

    int8_t direction = get_color(get_piece(bs, pos)) == C_WHITE ? -1 : 1;

    generate_step_attack_map((Pos){pos.x - 1, pos.y + direction}, out_map);
    generate_step_attack_map((Pos){pos.x + 1, pos.y + direction}, out_map);
}

static void generate_piece_attack_map(BoardState *bs, Pos pos, bool out_map[8][8])
{
    assert(bs != NULL);
    assert(out_map != NULL);

    Piece piece = get_piece(bs, pos);
    switch (get_type(piece))
    {
    case PT_PAWN: {
        generate_pawn_attack_map(bs, pos, out_map);
        break;
    }
    case PT_KNIGHT: {
        generate_knight_attack_map(pos, out_map);
        break;
    }
    case PT_BISHOP: {
        generate_bishop_attack_map(bs, pos, out_map);
        break;
    }
    case PT_ROOK: {
        generate_rook_attack_map(bs, pos, out_map);
        break;
    }
    case PT_QUEEN: {
        generate_queen_attack_map(bs, pos, out_map);
        break;
    }
    case PT_KING: {
        generate_king_attack_map(pos, out_map);
        break;
    }
    }
}

void generate_attack_map(BoardState *bs, Color color, bool out_map[8][8])
{
    assert(bs != NULL);
    assert(out_map != NULL);

    memset(out_map, 0, sizeof(bool) * 8 * 8);

    int8_t index;
    int8_t *len;

    pieces_offset(&bs->pieces, PT_PAWN, color, &index, &len);
    for (int8_t i = index; i < index + *len; i++)
    {
        generate_pawn_attack_map(bs, bs->pieces.list[i], out_map);
    }

    pieces_offset(&bs->pieces, PT_KNIGHT, color, &index, &len);
    for (int8_t i = index; i < index + *len; i++)
    {
        generate_knight_attack_map(bs->pieces.list[i], out_map);
    }

    pieces_offset(&bs->pieces, PT_BISHOP, color, &index, &len);
    for (int8_t i = index; i < index + *len; i++)
    {
        generate_bishop_attack_map(bs, bs->pieces.list[i], out_map);
    }

    pieces_offset(&bs->pieces, PT_ROOK, color, &index, &len);
    for (int8_t i = index; i < index + *len; i++)
    {
        generate_rook_attack_map(bs, bs->pieces.list[i], out_map);
    }

    pieces_offset(&bs->pieces, PT_QUEEN, color, &index, &len);
    for (int8_t i = index; i < index + *len; i++)
    {
        generate_queen_attack_map(bs, bs->pieces.list[i], out_map);
    }

    pieces_offset(&bs->pieces, PT_KING, color, &index, &len);
    for (int8_t i = index; i < index + *len; i++)
    {
        generate_king_attack_map(bs->pieces.list[i], out_map);
    }
}

static int count_doubled_and_isolated_pawns(BoardState *bs, Color c)
{
    assert(bs != NULL);

    int8_t index;
    int8_t *len;
    pieces_offset(&bs->pieces, PT_PAWN, c, &index, &len);

    int pawns_on_file[8] = {0};
    for (int8_t i = index; i < index + *len; i++)
    {
        pawns_on_file[bs->pieces.list[i].x]++;
    }

    int doubled_pawns = 0;
    for (int8_t i = 0; i < 8; i++)
    {
        if (pawns_on_file[i] > 1)
        {
            doubled_pawns += pawns_on_file[i] - 1;
        }
    }

    int isolated_pawns = 0;
    for (int8_t i = index; i < index + *len; i++)
    {
        Pos pos = bs->pieces.list[i];

        bool left = pos.x == 0 || pawns_on_file[pos.x - 1] == 0;
        bool right = pos.x == 7 || pawns_on_file[pos.x + 1] == 0;
        if (left || right)
        {
            isolated_pawns++;
        }
    }

    return doubled_pawns + isolated_pawns;
}
static int count_blocked_pawns(BoardState *bs, Color c)
{
    assert(bs != NULL);

    int direction = c == C_WHITE ? -1 : 1;

    int8_t index;
    int8_t *len;
    pieces_offset(&bs->pieces, PT_PAWN, c, &index, &len);

    int blocked_pawns = 0;
    for (int8_t i = index; i < index + *len; i++)
    {
        Pos pos = bs->pieces.list[i];
        Pos forward = (Pos){pos.x, pos.y + direction};
        if (!is_empty(get_piece(bs, forward)))
        {
            blocked_pawns++;
        }
    }

    return blocked_pawns;
}

double evaluate(BoardState *bs)
{
    assert(bs != NULL);

    double score = 0;

    // material
    score += bs->pieces.white.king_len * 200;
    score += bs->pieces.black.king_len * -200;
    score += bs->pieces.white.queen_len * 9;
    score += bs->pieces.black.queen_len * -9;
    score += bs->pieces.white.rook_len * 5;
    score += bs->pieces.black.rook_len * -5;
    score += bs->pieces.white.bishop_len * 3;
    score += bs->pieces.black.bishop_len * -3;
    score += bs->pieces.white.knight_len * 3;
    score += bs->pieces.black.knight_len * -3;
    score += bs->pieces.white.pawn_len * 1;
    score += bs->pieces.black.pawn_len * -1;

    // pawn structure
    score += ((double)count_doubled_and_isolated_pawns(bs, C_WHITE)) * -0.5;
    score += ((double)count_doubled_and_isolated_pawns(bs, C_BLACK)) * 0.5;
    score += ((double)count_blocked_pawns(bs, C_WHITE)) * -0.5;
    score += ((double)count_blocked_pawns(bs, C_BLACK)) * 0.5;

    // mobility
    Array(Move) white_moves = array_create_size(Move, 32);
    generate_legal_moves(bs, C_WHITE, &white_moves);
    Array(Move) black_moves = array_create_size(Move, 32);
    generate_legal_moves(bs, C_BLACK, &black_moves);

    score += ((double)array_len(white_moves)) * 0.1;
    score += ((double)array_len(black_moves)) * -0.1;

    array_free(white_moves);
    array_free(black_moves);

    return score;
}

// forward declaration
static double alpha_beta_max(BoardState *bs, double alpha, double beta, int depth, Move *out_move);
static double alpha_beta_min(BoardState *bs, double alpha, double beta, int depth, Move *out_move);

static double alpha_beta_max(BoardState *bs, double alpha, double beta, int depth, Move *out_move)
{
    if (depth == 0)
    {
        return evaluate(bs);
    }

    Array(Move) moves = array_create_size(Move, 32);
    generate_legal_moves(bs, bs->turn, &moves);

    if (array_len(moves) == 0)
    {
        array_free(moves);
        return -1000000; // checkmate or draw
    }

    Move best_move = {0};
    for (size_t i = 0; i < array_len(moves); i++)
    {
        BoardState new_bs = *bs;
        make_move(&new_bs, moves[i]);

        double score = alpha_beta_min(&new_bs, alpha, beta, depth - 1, NULL);
        if (score >= beta)
        {
            array_free(moves);
            return beta; // fail hard beta-cutoff
        }
        if (score > alpha)
        {
            alpha = score; // alpha acts like max in MiniMax
            best_move = moves[i];
        }
    }

    array_free(moves);
    if (out_move != NULL)
    {
        *out_move = best_move;
    }
    return alpha;
}

static double alpha_beta_min(BoardState *bs, double alpha, double beta, int depth, Move *out_move)
{
    if (depth == 0)
    {
        return -evaluate(bs); // remove - ?
    }

    Array(Move) moves = array_create_size(Move, 32);
    generate_legal_moves(bs, bs->turn, &moves);

    if (array_len(moves) == 0)
    {
        array_free(moves);
        return 1000000; // checkmate or draw
    }

    Move best_move = {0};
    for (size_t i = 0; i < array_len(moves); i++)
    {
        BoardState new_bs = *bs;
        make_move(&new_bs, moves[i]);

        double score = alpha_beta_max(&new_bs, alpha, beta, depth - 1, NULL);
        if (score <= alpha)
        {
            array_free(moves);
            return alpha; // fail hard alpha-cutoff
        }
        if (score < beta)
        {
            beta = score; // beta acts like min in MiniMax
            best_move = moves[i];
        }
    }

    array_free(moves);
    if (out_move != NULL)
    {
        *out_move = best_move;
    }
    return beta;
}

Move search_move(BoardState *bs, int depth)
{
    assert(bs != NULL);
    assert(depth > 0);

    Move best_move = {0};

    alpha_beta_max(bs, INT_MIN, INT_MAX, depth, &best_move);

    return best_move;
}

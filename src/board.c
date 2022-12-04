#include "board.h"
#include "piece.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int8_t pieces_all_color_len(PiecesListLengths *pll)
{
    return pll->pawn_len + pll->knight_len + pll->bishop_len + pll->rook_len + pll->queen_len + pll->king_len;
}

void pieces_offset(PiecesList *pl, PieceType pt, Color c, int8_t *out_index, int8_t *out_len)
{
    assert(out_index != NULL);
    assert(out_len != NULL);

    int8_t index = c == C_WHITE ? 0 : pieces_all_color_len(&pl->white);
    PiecesListLengths pll = c == C_WHITE ? pl->white : pl->black;

    if (pt == PT_PAWN)
    {
        *out_index = index;
        *out_len = pll.pawn_len;
    }
    index += pll.pawn_len;

    if (pt == PT_KNIGHT)
    {
        *out_index = index;
        *out_len = pll.knight_len;
    }
    index += pll.knight_len;

    if (pt == PT_BISHOP)
    {
        *out_index = index;
        *out_len = pll.bishop_len;
    }
    index += pll.bishop_len;

    if (pt == PT_ROOK)
    {
        *out_index = index;
        *out_len = pll.rook_len;
    }
    index += pll.rook_len;

    if (pt == PT_QUEEN)
    {
        *out_index = index;
        *out_len = pll.queen_len;
    }
    index += pll.queen_len;

    if (pt == PT_KING)
    {
        *out_index = index;
        *out_len = pll.king_len;
    }

    assert(false && "Invalid piece type");
}
void pieces_all_color_offset(PiecesList *pl, Color c, int8_t *out_index, int8_t *out_len);

BoardState load_fen(const char *fen)
{
    assert(fen != NULL);

    BoardState bs = {0};
    bs.en_passant_x = NO_EN_PASSANT;
    bs.en_passant_y = NO_EN_PASSANT;

    int x = 0;
    int y = 0;

    while (!(x == 8 && y == 7))
    {
        switch (*fen++)
        {
        case 'p':
            bs.board[x][y] = create_piece(PT_PAWN, C_BLACK);
            x++;
            break;
        case 'n':
            bs.board[x][y] = create_piece(PT_KNIGHT, C_BLACK);
            x++;
            break;
        case 'b':
            bs.board[x][y] = create_piece(PT_BISHOP, C_BLACK);
            x++;
            break;
        case 'r':
            bs.board[x][y] = create_piece(PT_ROOK, C_BLACK);
            x++;
            break;
        case 'q':
            bs.board[x][y] = create_piece(PT_QUEEN, C_BLACK);
            x++;
            break;
        case 'k':
            bs.board[x][y] = create_piece(PT_KING, C_BLACK);
            x++;
            break;

        case 'P':
            bs.board[x][y] = create_piece(PT_PAWN, C_WHITE);
            x++;
            break;
        case 'N':
            bs.board[x][y] = create_piece(PT_KNIGHT, C_WHITE);
            x++;
            break;
        case 'B':
            bs.board[x][y] = create_piece(PT_BISHOP, C_WHITE);
            x++;
            break;
        case 'R':
            bs.board[x][y] = create_piece(PT_ROOK, C_WHITE);
            x++;
            break;
        case 'Q':
            bs.board[x][y] = create_piece(PT_QUEEN, C_WHITE);
            x++;
            break;
        case 'K':
            bs.board[x][y] = create_piece(PT_KING, C_WHITE);
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
    fen++; // space

    while (*fen != ' ')
    {
        switch (*fen++)
        {
        case 'K':
            bs.white_king_side_castle = true;
            break;
        case 'Q':
            bs.white_queen_side_castle = true;
            break;
        case 'k':
            bs.black_king_side_castle = true;
            break;
        case 'q':
            bs.black_queen_side_castle = true;
            break;
        }
    }
    fen++; // space

    if (*fen != '-')
    {
        bs.en_passant_x = *fen++ - 'a';       // file
        bs.en_passant_y = 7 - (*fen++ - '1'); // rank
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

    bs->board[pos.x][pos.y] = p;
}

Piece get_piece(BoardState *bs, Pos pos)
{
    assert(bs != NULL);
    assert(pos.x >= 0 && pos.x <= 7);
    assert(pos.y >= 0 && pos.y <= 7);

    return bs->board[pos.x][pos.y];
}

void make_move(BoardState *bs, Move move)
{
    assert(bs != NULL);

    Piece p = get_piece(bs, move.from);
    assert(is_empty(p) == false);

    bool capture = false;

    // Reset en passant
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
        }
    }

    // Update turn
    bs->turn = get_color(p) == C_WHITE ? C_BLACK : C_WHITE;

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
        if (get_color(p) == C_WHITE)
        {
            bs->white_king_side_castle = false;
            bs->white_queen_side_castle = false;
        }
        else
        {
            bs->black_king_side_castle = false;
            bs->black_queen_side_castle = false;
        }
    }
    else if (is_rook(p))
    {
        if (get_color(p) == C_WHITE)
        {
            if (move.from.x == 0)
            {
                bs->white_queen_side_castle = false;
            }
            else if (move.from.x == 7)
            {
                bs->white_king_side_castle = false;
            }
        }
        else
        {
            if (move.from.x == 0)
            {
                bs->black_queen_side_castle = false;
            }
            else if (move.from.x == 7)
            {
                bs->black_king_side_castle = false;
            }
        }
    }

    // Castling rights rook captured
    if (capture)
    {
        if (move.to.x == 0 && move.to.y == 7)
        {
            bs->white_queen_side_castle = false;
        }
        else if (move.to.x == 7 && move.to.y == 7)
        {
            bs->white_king_side_castle = false;
        }
        else if (move.to.x == 0 && move.to.y == 0)
        {
            bs->black_queen_side_castle = false;
        }
        else if (move.to.x == 7 && move.to.y == 0)
        {
            bs->black_king_side_castle = false;
        }
    }
}

bool is_in_check(BoardState *bs, Color color)
{
    assert(bs != NULL);

    // Find king
    Pos king_pos = {-1, -1};
    for (int8_t x = 0; x < 8; x++)
    {
        for (int8_t y = 0; y < 8; y++)
        {
            Piece p = get_piece(bs, (Pos){x, y});
            if (is_king(p) && get_color(p) == color)
            {
                king_pos = (Pos){x, y};
                break;
            }
        }
    }
    // No king found
    if (king_pos.x == -1)
    {
        return false;
    }

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

    for (Pos pos = {0, 0}; pos.y < 8; pos.y++)
    {
        for (pos.x = 0; pos.x < 8; pos.x++)
        {
            Piece p = get_piece(bs, pos);
            if (!is_empty(p) && get_color(p) == color)
            {
                generate_piece_pseudo_moves(bs, pos, out_moves);
            }
        }
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

    for (int8_t x = 0; x < 8; x++)
    {
        for (int8_t y = 0; y < 8; y++)
        {
            Pos pos = (Pos){x, y};
            Piece piece = get_piece(bs, pos);
            if (get_color(piece) == color)
            {
                generate_piece_attack_map(bs, pos, out_map);
            }
        }
    }
}

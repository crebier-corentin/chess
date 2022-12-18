#include "board.h"
#include "cache.h"
#include "evaluation.h"
#include "greatest.h"
#include "move.h"
#include "perft.h"
#include "piece.h"
#include "zobrist.h"

TEST test_perft_default()
{
    BoardState bs = load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ");
    ASSERT_EQ(perft_thread_sched(&bs, 1), 20);
    ASSERT_EQ(perft_thread_sched(&bs, 2), 400);
    ASSERT_EQ(perft_thread_sched(&bs, 3), 8902);
    ASSERT_EQ(perft_thread_sched(&bs, 4), 197281);
    ASSERT_EQ(perft_thread_sched(&bs, 5), 4865609);

    PASS();
}

TEST test_perft_kiwipete()
{
    BoardState bs = load_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    ASSERT_EQ(perft_thread_sched(&bs, 1), 48);
    ASSERT_EQ(perft_thread_sched(&bs, 2), 2039);
    ASSERT_EQ(perft_thread_sched(&bs, 3), 97862);

    PASS();
}

TEST test_perft_6()
{
    BoardState bs = load_fen("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
    ASSERT_EQ(perft_thread_sched(&bs, 1), 46);
    ASSERT_EQ(perft_thread_sched(&bs, 2), 2079);
    ASSERT_EQ(perft_thread_sched(&bs, 3), 89890);
    ASSERT_EQ(perft_thread_sched(&bs, 4), 3894594);

    PASS();
}

TEST test_piece_list()
{
    BoardState bs = load_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

    for (Color c = C_WHITE; c <= C_BLACK; c++)
    {
        for (PieceType pt = PT_PAWN; pt <= PT_KING; pt++)
        {
            Piece p = create_piece(pt, c);

            int8_t index;
            int8_t *len;
            pieces_offset(&bs.pieces, pt, c, &index, &len);
            for (int8_t i = index; i < index + *len; i++)
            {
                ASSERT_EQ(get_piece(&bs, bs.pieces.list[i]), p);
            }
        }
    }

    PASS();
}

TEST test_zobrist_hash()
{
    {
        BoardState bs1 = load_fen("k7/8/8/8/8/8/8/K7 w - - 0 1");
        make_move(&bs1, parse_algebraic_notation(&bs1, "Ka2"));

        BoardState bs2 = load_fen("k7/8/8/8/8/8/K7/8 b - - 0 1");

        ASSERT_EQ(bs1.zobrist_hash, bs2.zobrist_hash);
    }

    // Castle
    {
        BoardState bs1 = load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
        make_move(&bs1, parse_algebraic_notation(&bs1, "O-O"));

        BoardState bs2 = load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R4RK1 b kq - 1 1");

        ASSERT_EQ(bs1.zobrist_hash, bs2.zobrist_hash);
    }
    {
        BoardState bs1 = load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
        make_move(&bs1, parse_algebraic_notation(&bs1, "O-O-O"));

        BoardState bs2 = load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/2KR3R b kq - 1 1");

        ASSERT_EQ(bs1.zobrist_hash, bs2.zobrist_hash);
    }

    // En passant
    {
        BoardState bs1 = load_fen("rnbqkbnr/pp1ppppp/8/8/3p4/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        make_move(&bs1, parse_algebraic_notation(&bs1, "e4"));

        BoardState bs2 = load_fen("rnbqkbnr/pp1ppppp/8/8/3pP3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");

        ASSERT_EQ(bs1.zobrist_hash, bs2.zobrist_hash);
    }

    PASS();
}

TEST test_is_in_check()
{
    BoardState bs = load_fen("kbK5/pp6/1P6/8/8/8/8/R7 w - - 0 1");
    make_move(&bs, parse_algebraic_notation(&bs, "Ra6"));
    make_move(&bs, parse_algebraic_notation(&bs, "bxa6"));
    make_move(&bs, parse_algebraic_notation(&bs, "b7#"));

    ASSERT_EQ(is_in_check(&bs, C_BLACK), true);

    PASS();
}

TEST test_mate_in_one()
{
    {
        BoardState bs = load_fen("8/8/8/7R/4Q3/6k1/3R4/K7 w - - 13 9");
        char buffer[6] = {0};
        move_to_long_notation(search_move(&bs, 3), buffer);

        ASSERT_STR_EQ(buffer, "d2g2");
    }

    {
        BoardState bs = load_fen("k6r/8/8/8/8/8/PPP5/K7 b - - 0 1");
        char buffer[6] = {0};
        move_to_long_notation(search_move(&bs, 3), buffer);

        ASSERT_STR_EQ(buffer, "h8h1");
    }

    PASS();
}

TEST test_mate_in_two()
{
    {
        BoardState bs = load_fen("kbK5/pp6/1P6/8/8/8/8/R7 w - - 0 1");
        char buffer[6] = {0};
        move_to_long_notation(search_move(&bs, 5), buffer);
        ASSERT_STR_EQ(buffer, "a1a6");

        make_move(&bs, parse_algebraic_notation(&bs, "Ra6"));
        make_move(&bs, parse_algebraic_notation(&bs, "bxa6"));

        move_to_long_notation(search_move(&bs, 5), buffer);
        ASSERT_STR_EQ(buffer, "b6b7");
    }

    {
        BoardState bs = load_fen("r7/3krn2/8/pp3K2/q7/8/8/8 b - - 7 46");
        char buffer[6] = {0};
        move_to_long_notation(search_move(&bs, 5), buffer);
        ASSERT_STR_EQ(buffer, "a8g8");

        make_move(&bs, parse_algebraic_notation(&bs, "Rg8"));
        make_move(&bs, parse_algebraic_notation(&bs, "Kf6"));

        move_to_long_notation(search_move(&bs, 5), buffer);
        ASSERT_STR_EQ(buffer, "a4f4");
    }

    PASS();
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    zobrist_init();

    GREATEST_MAIN_BEGIN();

    RUN_TEST(test_perft_default);
    RUN_TEST(test_perft_kiwipete);
    RUN_TEST(test_perft_6);

    RUN_TEST(test_piece_list);

    RUN_TEST(test_zobrist_hash);

    RUN_TEST(test_is_in_check);

    RUN_TEST(test_mate_in_one);
    RUN_TEST(test_mate_in_two);

    GREATEST_MAIN_END();
}

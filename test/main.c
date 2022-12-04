#include "board.h"
#include "greatest.h"
#include "move.h"
#include "perft.h"
#include "piece.h"

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

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();

    RUN_TEST(test_perft_default);
    RUN_TEST(test_perft_kiwipete);

    RUN_TEST(test_piece_list);

    GREATEST_MAIN_END();
}

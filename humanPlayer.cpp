#include "humanPlayer.hpp"

HumanPlayer::HumanPlayer(std::istream& in) : in(in) {}

int HumanPlayer::MakeMove(const Board& board, Piece piece) {
    int column = -1;

    do {
        in >> column;
    } while (column == -1 || column < 0 || column >= BOARD_WIDTH);

    return column;
}

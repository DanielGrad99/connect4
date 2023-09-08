#include "boardDrawer.hpp"

#include <stdlib.h>

#include <cassert>

BoardDrawer::BoardDrawer(std::ostream& out) : out(out) {}

void BoardDrawer::Draw(const Board& board) {
    out << "\033[2J\033[1;1H";

    for (int row = BOARD_HEIGHT - 1; row >= 0; --row) {
        for (int col = 0; col < BOARD_WIDTH; ++col) {
            char c = convertPieceToString(board.GetPiece(row, col));
            out << c;

            if (col != BOARD_WIDTH - 1) {
                out << " ";
            }
        }

        out << std::endl;
    }
}

char BoardDrawer::convertPieceToString(Piece piece) {
    switch (piece) {
        case Piece::NONE:
            return '_';
        case Piece::RED:
            return 'R';
        case Piece::BLUE:
            return 'B';
    }

    assert(false);
}

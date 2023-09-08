#include "board.hpp"

#include <cassert>
#include <utility>

Board::Board() {}

Board::~Board() {}

Board::Board(Board &other) { NewGame(); }

Board::Board(Board &&other) {}

Piece Board::GetPiece(int row, int col) const {
    assert(row >= 0 && row < BOARD_HEIGHT);
    assert(col >= 0 && col < BOARD_WIDTH);

    return mBoard[col + row * BOARD_WIDTH];
}

void Board::NewGame() {
    for (int i = 0; i < BOARD_WIDTH * BOARD_HEIGHT; ++i) {
        mBoard[i] = Piece::NONE;
    }
}

bool Board::IsGameOver() {
    for (int i = 0; i < BOARD_WIDTH * BOARD_HEIGHT; ++i) {
        if (isBottomLeftMostStartOfWinning(i)) {
            return true;
        }
    }

    return false;
}

bool Board::PlacePiece(Piece piece, int column) {
    assert(column >= 0 && column < BOARD_WIDTH);

    for (int row = 0; row < BOARD_HEIGHT; ++row) {
        if (mBoard[column + row * BOARD_WIDTH] == Piece::NONE) {
            mBoard[column + row * BOARD_WIDTH] = piece;
            return true;
        }
    }

    return false;
}

int Board::getPieceAbove(int pos) {
    assert(pos >= 0 && pos < BOARD_WIDTH * BOARD_HEIGHT);

    pos += BOARD_WIDTH;
    if (pos >= BOARD_WIDTH * BOARD_HEIGHT) {
        return -1;
    }

    return pos;
}

int Board::getPieceRight(int pos) {
    assert(pos >= 0 && pos < BOARD_WIDTH * BOARD_HEIGHT);

    pos += 1;
    if (pos >= BOARD_WIDTH * BOARD_HEIGHT || pos % BOARD_WIDTH == 0) {
        return -1;
    }

    return pos;
}

bool Board::isBottomLeftMostStartOfWinning(int pos) {
    assert(pos >= 0 && pos < BOARD_WIDTH * BOARD_HEIGHT);
    return isWinningUp(pos) || isWinningRight(pos) || isWinningUpRight(pos);
}

bool Board::isWinningUp(int pos) {
    assert(pos >= 0 && pos < BOARD_WIDTH * BOARD_HEIGHT);

    Piece piece = mBoard[pos];
    if (piece == Piece::NONE) {
        return false;
    }

    for (int i = 0; i < NUM_IN_A_ROW - 1; ++i) {
        pos = getPieceAbove(pos);
        if (pos == -1) {
            return false;
        }

        if (mBoard[pos] != piece) {
            return false;
        }
    }

    return true;
}
bool Board::isWinningRight(int pos) {
    assert(pos >= 0 && pos < BOARD_WIDTH * BOARD_HEIGHT);

    Piece piece = mBoard[pos];
    if (piece == Piece::NONE) {
        return false;
    }

    for (int i = 0; i < NUM_IN_A_ROW - 1; ++i) {
        pos = getPieceRight(pos);
        if (pos == -1) {
            return false;
        }

        if (mBoard[pos] != piece) {
            return false;
        }
    }

    return true;
}

bool Board::isWinningUpRight(int pos) {
    assert(pos >= 0 && pos < BOARD_WIDTH * BOARD_HEIGHT);

    Piece piece = mBoard[pos];
    if (piece == Piece::NONE) {
        return false;
    }

    for (int i = 0; i < NUM_IN_A_ROW - 1; ++i) {
        pos = getPieceAbove(pos);
        if (pos == -1) {
            return false;
        }

        pos = getPieceRight(pos);
        if (pos == -1) {
            return false;
        }

        if (mBoard[pos] != piece) {
            return false;
        }
    }

    return true;
}
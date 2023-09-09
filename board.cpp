#include "board.hpp"

#include <cassert>
#include <utility>

Board::Board() { NewGame(); }

Board::~Board() {}

Board::Board(const Board &other) {
    for (int i = 0; i < BOARD_WIDTH * BOARD_HEIGHT; ++i) {
        mBoard[i] = mBoard[i];
    }
}

Board::Board(Board &&other) {
    for (int i = 0; i < BOARD_WIDTH * BOARD_HEIGHT; ++i) {
        mBoard[i] = mBoard[i];
    }
}

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

GameState Board::IsGameOver() const {
    bool allFilled = true;

    for (int i = 0; i < BOARD_WIDTH * BOARD_HEIGHT; ++i) {
        if (isBottomLeftMostStartOfWinning(i)) {
            if (mBoard[i] == Piece::RED) {
                return GameState::RED_WON;
            } else if (mBoard[i] == Piece::BLUE) {
                return GameState::BLUE_WON;
            } else {
                assert(false);
            }
        }

        allFilled = allFilled && mBoard[i] != Piece::NONE;
    }

    if (allFilled) {
        return GameState::TIE;
    }
    
    return GameState::PLAYING;
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

int Board::getPieceAbove(int pos) const {
    assert(pos >= 0 && pos < BOARD_WIDTH * BOARD_HEIGHT);

    pos += BOARD_WIDTH;
    if (pos >= BOARD_WIDTH * BOARD_HEIGHT) {
        return -1;
    }

    return pos;
}

int Board::getPieceRight(int pos) const {
    assert(pos >= 0 && pos < BOARD_WIDTH * BOARD_HEIGHT);

    pos += 1;
    if (pos >= BOARD_WIDTH * BOARD_HEIGHT || pos % BOARD_WIDTH == 0) {
        return -1;
    }

    return pos;
}

bool Board::isBottomLeftMostStartOfWinning(int pos) const {
    assert(pos >= 0 && pos < BOARD_WIDTH * BOARD_HEIGHT);
    return isWinningUp(pos) || isWinningRight(pos) || isWinningUpRight(pos);
}

bool Board::isWinningUp(int pos) const {
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
bool Board::isWinningRight(int pos) const {
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

bool Board::isWinningUpRight(int pos) const {
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

std::string to_string(const Board &board) {
    char ret[BOARD_WIDTH * BOARD_HEIGHT];
    for (int i = 0; i < BOARD_WIDTH * BOARD_HEIGHT; ++i) {
        switch (board.mBoard[i]) {
            case Piece::NONE:
                ret[i] = ' ';
                break;
            case Piece::RED:
                ret[i] = 'R';
                break;
            case Piece::BLUE:
                ret[i] = 'B';
                break;
            default:
                assert(false);
                break;
        }
    }

    return ret;
}

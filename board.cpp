#include "board.hpp"

#include <cassert>
#include <utility>

Board::Board() { NewGame(); }

Board::~Board() {}

Board::Board(const Board &other) {
    for (int i = 0; i < BOARD_WIDTH * BOARD_HEIGHT; ++i) {
        mBoard[i] = other.mBoard[i];
    }
}

Board::Board(Board &&other) {
    for (int i = 0; i < BOARD_WIDTH * BOARD_HEIGHT; ++i) {
        mBoard[i] = other.mBoard[i];
    }
}

Board Board::FlipMe() const {
    Board board;

    for (int row = 0; row < BOARD_HEIGHT; ++row) {
        for (int col = 0; col < BOARD_WIDTH; ++col) {
            board.mBoard[col + row * BOARD_WIDTH] =
                mBoard[(BOARD_WIDTH - 1 - col) + row * BOARD_WIDTH];
        }
    }

    return board;
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
        if (isPieceWinning(i)) {
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

Board::MoveResult Board::PlacePiece(Piece piece, int column) {
    assert(0 <= column && column < BOARD_WIDTH);

    int pos;
    for (int row = 0; row < BOARD_HEIGHT; ++row) {
        pos = column + row * BOARD_WIDTH;
        if (mBoard[pos] == Piece::NONE) {
            mBoard[pos] = piece;

            if (isPieceWinning(pos)) {
                if (mBoard[pos] == Piece::RED) {
                    return {true, GameState::RED_WON};
                } else if (mBoard[pos] == Piece::BLUE) {
                    return {true, GameState::BLUE_WON};
                }

                assert(false);
            } else if (AnyAvailableMoves()) {
                return {true, GameState::PLAYING};
            } else {
                return {true, GameState::TIE};
            }
        }
    }

    return {false, GameState::PLAYING};
}

bool Board::AnyAvailableMoves() const {
    int row = BOARD_HEIGHT - 1;
    for (int col = 0; col < BOARD_WIDTH; ++col) {
        if (mBoard[col + row * BOARD_WIDTH] == Piece::NONE) {
            return true;
        }
    }

    return false;
}

int Board::NumEmptySlots() const {
    int count = 0;
    for (int i = 0; i < BOARD_WIDTH * BOARD_HEIGHT; ++i) {
        if (mBoard[i] == Piece::NONE) {
            ++count;
        }
    }
    return count;
}

std::string to_string(const Board &board) {
    char ret[BOARD_WIDTH * BOARD_HEIGHT + 1] = {0};
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

bool Board::isPieceWinning(int pos) const {
    assert(pos >= 0 && pos < BOARD_WIDTH * BOARD_HEIGHT);

    Piece piece = mBoard[pos];
    if (piece == Piece::NONE) {
        return false;
    }

    return checkSequence(pos, getPieceUp, getPieceDown) ||
           checkSequence(pos, getPieceUpRight, getPieceDownLeft) ||
           checkSequence(pos, getPieceRight, getPieceLeft) ||
           checkSequence(pos, getPieceDownRight, getPieceUpLeft);
}

bool Board::checkSequence(int pos, int (*getNext)(int),
                          int (*getLast)(int)) const {
    assert(pos >= 0 && pos < BOARD_WIDTH * BOARD_HEIGHT);

    Piece piece = mBoard[pos];
    if (piece == Piece::NONE) {
        return false;
    }

    int piecesInRow = 1;
    int originalPos = pos;

    while (true) {
        pos = getNext(pos);
        if (pos == -1 || mBoard[pos] != piece) {
            break;
        }

        ++piecesInRow;
        if (piecesInRow == NUM_IN_A_ROW) {
            return true;
        }
    }

    pos = originalPos;
    while (true) {
        pos = getLast(pos);
        if (pos == -1 || mBoard[pos] != piece) {
            break;
        }

        ++piecesInRow;
        if (piecesInRow == NUM_IN_A_ROW) {
            return true;
        }
    }

    return false;
}

int Board::getPieceUp(int pos) {
    assert(pos >= 0 && pos < BOARD_WIDTH * BOARD_HEIGHT);

    pos += BOARD_WIDTH;
    if (pos >= BOARD_WIDTH * BOARD_HEIGHT) {
        return -1;
    }

    return pos;
}

int Board::getPieceUpRight(int pos) {
    assert(pos >= 0 && pos < BOARD_WIDTH * BOARD_HEIGHT);

    pos = getPieceUp(pos);
    if (pos == -1) {
        return -1;
    }

    pos = getPieceRight(pos);
    if (pos == -1) {
        return -1;
    }

    return pos;
}

int Board::getPieceRight(int pos) {
    assert(pos >= 0 && pos < BOARD_WIDTH * BOARD_HEIGHT);

    pos += 1;
    if (pos % BOARD_WIDTH == 0) {
        return -1;
    }

    return pos;
}

int Board::getPieceDownRight(int pos) {
    assert(pos >= 0 && pos < BOARD_WIDTH * BOARD_HEIGHT);

    pos = getPieceDown(pos);
    if (pos == -1) {
        return -1;
    }

    pos = getPieceRight(pos);
    if (pos == -1) {
        return -1;
    }

    return pos;
}

int Board::getPieceDown(int pos) {
    assert(pos >= 0 && pos < BOARD_WIDTH * BOARD_HEIGHT);

    pos -= BOARD_WIDTH;
    if (pos < 0) {
        return -1;
    }

    return pos;
}

int Board::getPieceDownLeft(int pos) {
    assert(pos >= 0 && pos < BOARD_WIDTH * BOARD_HEIGHT);

    pos = getPieceDown(pos);
    if (pos == -1) {
        return -1;
    }

    pos = getPieceLeft(pos);
    if (pos == -1) {
        return -1;
    }

    return pos;
}

int Board::getPieceLeft(int pos) {
    assert(pos >= 0 && pos < BOARD_WIDTH * BOARD_HEIGHT);

    if (pos % BOARD_WIDTH == 0) {
        return -1;
    }

    pos -= 1;
    return pos;
}

int Board::getPieceUpLeft(int pos) {
    assert(pos >= 0 && pos < BOARD_WIDTH * BOARD_HEIGHT);

    pos = getPieceUp(pos);
    if (pos == -1) {
        return -1;
    }

    pos = getPieceLeft(pos);
    if (pos == -1) {
        return -1;
    }

    return pos;
}

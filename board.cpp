#include "board.hpp"

#include <cassert>
#include <utility>

Board::Board() noexcept {
    mBoard = new Piece[BOARD_WIDTH * BOARD_HEIGHT];
    NewGame();
}

Board::~Board() noexcept { delete[] mBoard; }

Board::Board(const Board &other) noexcept
    : mBoard(allocateCopyOfBoard(other)) {}

Board::Board(Board &&other) noexcept
    : mBoard(std::exchange(other.mBoard, nullptr)) {}

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

void Board::RemoveTopPiece(int column) {
    assert(0 <= column && column < BOARD_WIDTH);

    int pos;
    for (int row = BOARD_HEIGHT - 1; row >= 0; --row) {
        pos = column + row * BOARD_WIDTH;
        if (mBoard[pos] == Piece::NONE) {
            continue;
        }

        mBoard[pos] = Piece::NONE;
        return;
    }

    assert(false);
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
    char ret[(BOARD_WIDTH * BOARD_HEIGHT) / 4 + 2] = {0};
    for (int i = 0; i < BOARD_WIDTH * BOARD_HEIGHT; ++i) {
        const int shift = i % 4;
        const int index = i / 4;

        switch (board.mBoard[i]) {
            case Piece::NONE:
                // ret[index] |= 0b0 << (shift * 2);
                break;
            case Piece::RED:
                ret[index] |= 0b01 << (shift * 2);
                break;
            case Piece::BLUE:
                ret[index] |= 0b10 << (shift * 2);
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

    int originalPos = pos;

    int inRow = 1;

    pos = getPieceUp(originalPos);
    while (piece == getPiece(pos) && inRow < NUM_IN_A_ROW) {
        pos = getPieceUp(pos);
        ++inRow;
    }
    pos = getPieceDown(originalPos);
    while (piece == getPiece(pos) && inRow < NUM_IN_A_ROW) {
        pos = getPieceDown(pos);
        ++inRow;
    }

    if (inRow == NUM_IN_A_ROW) {
        return true;
    }

    inRow = 1;

    pos = getPieceUpRight(originalPos);
    while (piece == getPiece(pos) && inRow < NUM_IN_A_ROW) {
        pos = getPieceUpRight(pos);
        ++inRow;
    }
    pos = getPieceDownLeft(originalPos);
    while (piece == getPiece(pos) && inRow < NUM_IN_A_ROW) {
        pos = getPieceDownLeft(pos);
        ++inRow;
    }

    if (inRow == NUM_IN_A_ROW) {
        return true;
    }

    inRow = 1;

    pos = getPieceRight(originalPos);
    while (piece == getPiece(pos) && inRow < NUM_IN_A_ROW) {
        pos = getPieceRight(pos);
        ++inRow;
    }
    pos = getPieceLeft(originalPos);
    while (piece == getPiece(pos) && inRow < NUM_IN_A_ROW) {
        pos = getPieceLeft(pos);
        ++inRow;
    }

    if (inRow == NUM_IN_A_ROW) {
        return true;
    }

    inRow = 1;

    pos = getPieceDownRight(originalPos);
    while (piece == getPiece(pos) && inRow < NUM_IN_A_ROW) {
        pos = getPieceDownRight(pos);
        ++inRow;
    }
    pos = getPieceUpLeft(originalPos);
    while (piece == getPiece(pos) && inRow < NUM_IN_A_ROW) {
        pos = getPieceUpLeft(pos);
        ++inRow;
    }

    if (inRow == NUM_IN_A_ROW) {
        return true;
    }

    return false;
}

Piece Board::getPiece(int pos) const {
    if (pos == -1) {
        return Piece::NONE;
    }

    assert(pos >= 0 && pos < BOARD_WIDTH * BOARD_HEIGHT);
    return mBoard[pos];
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

Piece *Board::allocateCopyOfBoard(const Board &other) {
    Piece *board = new Piece[BOARD_WIDTH * BOARD_HEIGHT];
    std::copy(other.mBoard, other.mBoard + BOARD_WIDTH * BOARD_HEIGHT, board);

    return board;
}

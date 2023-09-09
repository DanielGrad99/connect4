#include "machinePlayer.hpp"

#include <unistd.h>  // for sleep only

#include <cassert>
#include <iostream>
#include <string>

#define MIN_MOVES_FOR_CACHE 10

std::unordered_map<std::string, MachinePlayer::Move> MachinePlayer::cache = {};

MachinePlayer::MachinePlayer() {}

int MachinePlayer::MakeMove(const Board& board, Piece piece) {
    MachinePlayer::Move move = getBestMove(board, piece);

    if (surprise && iWon(piece, move.state)) {
        surprise = false;
        std::cout << "omae wa shindeiru" << std::endl;
        usleep(5000000);
    } else if (!iHaveLost && iWon(other(piece), move.state)) {
        iHaveLost = true;
        surprise = true;

        std::cout << "nani!?" << std::endl;
        usleep(5000000);
    }

    std::cout << "Making move - " << move.col << std::endl;

    return move.col;
}

MachinePlayer::Move MachinePlayer::getBestMove(const Board& originalBoard,
                                               Piece piece) {
    assert(originalBoard.IsGameOver() == GameState::PLAYING);

    MachinePlayer::Move cachedMove = getCachedMove(originalBoard);
    if (cachedMove.col != -1) {
        return cachedMove;
    }

    MachinePlayer::Move instantWin = getInstantWinMove(originalBoard, piece);
    if (instantWin.col != -1) {
        return instantWin;
    }

    MachinePlayer::Move instantLoss =
        getInstantWinMove(originalBoard, other(piece));
    if (instantLoss.col != -1) {
        Board board{originalBoard};
        Board::MoveResult moveResult = board.PlacePiece(piece, instantLoss.col);

        if (moveResult.gameState != GameState::PLAYING) {
            return {instantLoss.col, moveResult.gameState};
        }

        MachinePlayer::Move m = getBestMove(board, other(piece));
        writeToCache(board, m);

        assert(m.state != GameState::PLAYING);

        return {instantLoss.col, m.state};
    }

    int mandatoryMove = -1;
    GameState mandatoryMoveOutcome = GameState::PLAYING;

    int validMove = -1;
    int anyMove = -1;

    for (int col = 0; col < BOARD_WIDTH; ++col) {
        if (originalBoard.GetPiece(BOARD_HEIGHT - 1, col) != Piece::NONE) {
            // Cannot place on this row
            continue;
        }

        Board board{originalBoard};

        assert(0 <= col && col < BOARD_WIDTH);
        Board::MoveResult moveResult = board.PlacePiece(piece, col);
        assert(moveResult.ValidMove);
        assert(board.NumEmptySlots() + 1 == originalBoard.NumEmptySlots());

        anyMove = col;

        GameState state = moveResult.gameState;

        if (state == GameState::PLAYING) {
            MachinePlayer::Move m;
            if (piece == Piece::RED) {
                m = getBestMove(board, Piece::BLUE);
            } else {
                m = getBestMove(board, Piece::RED);
            }

            if (mandatoryMove == col) {
                mandatoryMoveOutcome = m.state;
            }

            writeToCache(board, m);

            assert(m.state != GameState::PLAYING);

            if (iWon(piece, m.state)) {
                return {col, m.state};  // wining move
            } else if (m.state == GameState::TIE) {
                validMove = col;
            } else {
                continue;  // lossing move
            }

        } else if (state == GameState::TIE) {
            validMove = col;
        } else if (state == GameState::BLUE_WON) {
            return {col, GameState::BLUE_WON};
        } else if (state == GameState::RED_WON) {
            return {col, GameState::RED_WON};
        } else {
            assert(false);
        }
    }

    if (mandatoryMove != -1) {
        return {mandatoryMove, mandatoryMoveOutcome};
    }

    if (validMove != -1) {
        return {validMove, GameState::TIE};
    }

    if (piece == Piece::RED) {
        return {anyMove, GameState::BLUE_WON};
    }

    return {anyMove, GameState::RED_WON};
}

MachinePlayer::Move MachinePlayer::getInstantWinMove(const Board& originalBoard,
                                                     Piece piece) {
    Board board{originalBoard};
    for (int col = 0; col < BOARD_WIDTH; ++col) {
        if (board.GetPiece(BOARD_HEIGHT - 1, col) != Piece::NONE) {
            // Cannot place on this row
            continue;
        }

        Board::MoveResult moveResult = board.PlacePiece(piece, col);
        assert(moveResult.ValidMove);

        if (iWon(piece, moveResult.gameState)) {
            return {col, moveResult.gameState};
        }

        board.RemoveTopPiece(col);
    }

    return {-1};
}

MachinePlayer::Move MachinePlayer::getCachedMove(const Board& board) {
    std::string serializedBoard = to_string(board);
    if (auto search = MachinePlayer::cache.find(serializedBoard);
        search != MachinePlayer::cache.end()) {
        return search->second;
    }

    std::string serializedFlipedBoard = to_string(board.FlipMe());
    if (auto search = MachinePlayer::cache.find(serializedBoard);
        search != MachinePlayer::cache.end()) {
        MachinePlayer::Move move = search->second;
        move.col = BOARD_WIDTH - 1 - move.col;

        return move;
    }

    return {-1};
}

void MachinePlayer::writeToCache(const Board& board, MachinePlayer::Move move) {
    if (board.NumEmptySlots() >= MIN_MOVES_FOR_CACHE) {
        cache[to_string(board)] = move;
    }
}

bool MachinePlayer::iWon(Piece piece, GameState state) {
    assert(piece != Piece::NONE);

    if (piece == Piece::BLUE) {
        return state == GameState::BLUE_WON;
    }

    return state == GameState::RED_WON;
}

Piece MachinePlayer::other(Piece piece) {
    if (piece == Piece::RED) {
        return Piece::BLUE;
    } else if (piece == Piece::BLUE) {
        return Piece::RED;
    }

    assert(false);
}

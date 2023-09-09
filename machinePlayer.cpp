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

    std::string serializedBoard = to_string(originalBoard);
    if (auto search = MachinePlayer::cache.find(serializedBoard);
        search != MachinePlayer::cache.end()) {
        return search->second;
    }

    std::string serializedFlipedBoard = to_string(originalBoard.FlipMe());
    if (auto search = MachinePlayer::cache.find(serializedBoard);
        search != MachinePlayer::cache.end()) {
        MachinePlayer::Move move = search->second;
        move.col = BOARD_WIDTH - 1 - move.col;

        return move;
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
            // what if oponent makes the same move
            {
                Board board{originalBoard};
                Board::MoveResult moveResult =
                    board.PlacePiece(other(piece), col);
                assert(moveResult.ValidMove);

                GameState state = moveResult.gameState;
                if (state == GameState::BLUE_WON ||
                    state == GameState::RED_WON) {
                    mandatoryMove = col;
                }
            }

            MachinePlayer::Move m;
            if (piece == Piece::RED) {
                m = getBestMove(board, Piece::BLUE);
            } else {
                m = getBestMove(board, Piece::RED);
            }

            if (mandatoryMove == col) {
                mandatoryMoveOutcome = m.state;
            }

            if (board.NumEmptySlots() >= MIN_MOVES_FOR_CACHE) {
                cache[to_string(board)] = m;
            }

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

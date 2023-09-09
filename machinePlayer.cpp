#include "machinePlayer.hpp"

#include <unistd.h>  // for sleep only

#include <cassert>
#include <iostream>
#include <string>

#define MIN_MOVES_FOR_CACHE 10

std::unordered_map<std::string, MachinePlayer::Move> MachinePlayer::cache = {};
std::mutex MachinePlayer::cache_lock = {};

#define MAX_THREADS 100
std::mutex MachinePlayer::thread_lock = {};
int MachinePlayer::curRunningThreads = 0;

MachinePlayer::MachinePlayer() {}

int MachinePlayer::MakeMove(const Board& board, Piece piece) {
    MachinePlayer::Move move = getBestMove(board, piece);

    if (surprise && iWon(piece, move.state)) {
        surprise = false;
        // std::cout << "omae wa shindeiru" << std::endl;
        // usleep(1000000);
    } else if (!iHaveLost && iWon(other(piece), move.state)) {
        iHaveLost = true;
        surprise = true;

        // std::cout << "nani!?" << std::endl;
        // usleep(1000000);
    }

    // std::cout << "Making move - " << move.col << std::endl;

    return move.col;
}

MachinePlayer::Move MachinePlayer::getBestMove(const Board& originalBoard,
                                               Piece piece) {
    // assert(originalBoard.IsGameOver() == GameState::PLAYING);

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
        GameState state = GameState::PLAYING;
        writeResultForMove(originalBoard, piece, &state, instantLoss.col);

        assert(state != GameState::PLAYING);
        return {instantLoss.col, state};
    }

    GameState states[BOARD_WIDTH] = {GameState::PLAYING};
    std::thread threads[BOARD_WIDTH];

    for (int col = 0; col < BOARD_WIDTH; ++col) {
        tryRunInThread(
            [&originalBoard, piece, &states, col] {
                writeResultForMove(originalBoard, piece, states + col, col);
            },
            threads + col);
    }

    for (int i = 0; i < BOARD_WIDTH; ++i) {
        if (threads[i].joinable()) {
            threads[i].join();
        }
    }

    int validCol = -1;
    int tiedCol = -1;

    for (int col = 0; col < BOARD_WIDTH; ++col) {
        if (states[col] == GameState::PLAYING) {
            continue;
        }

        validCol = col;

        if (iWon(piece, states[col])) {
            return {col, states[col]};
        } else if (states[col] == GameState::TIE) {
            tiedCol = col;
        }
    }

    assert(validCol != -1);

    if (tiedCol != -1) {
        return {tiedCol, GameState::TIE};
    }

    return {validCol, states[validCol]};
}

void MachinePlayer::writeResultForMove(const Board& originalBoard, Piece piece,
                                       GameState* stateDest, int col) {
    if (originalBoard.GetPiece(BOARD_HEIGHT - 1, col) != Piece::NONE) {
        return;
    }

    Board board{originalBoard};

    assert(0 <= col && col < BOARD_WIDTH);
    Board::MoveResult moveResult = board.PlacePiece(piece, col);
    assert(moveResult.ValidMove);
    // assert(board.NumEmptySlots() + 1 == originalBoard.NumEmptySlots());

    GameState state = moveResult.gameState;

    if (state == GameState::PLAYING) {
        MachinePlayer::Move m = getBestMove(board, other(piece));

        writeToCache(board, m);
        assert(m.state != GameState::PLAYING);

        *stateDest = m.state;

    } else if (state == GameState::TIE || state == GameState::BLUE_WON ||
               state == GameState::RED_WON) {
        *stateDest = state;
    } else {
        assert(false);
    }
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
    if (board.NumEmptySlots() < MIN_MOVES_FOR_CACHE) {
        return {-1};
    }

    // Do not want to serialize in lock (so better here)
    std::string serializedBoard = to_string(board);
    std::string serializedFlipedBoard = to_string(board.FlipMe());

    std::unique_lock<std::mutex> lck(MachinePlayer::cache_lock);

    if (auto search = MachinePlayer::cache.find(serializedBoard);
        search != MachinePlayer::cache.end()) {
        return search->second;
    }

    if (auto search = MachinePlayer::cache.find(serializedBoard);
        search != MachinePlayer::cache.end()) {
        MachinePlayer::Move move = search->second;
        move.col = BOARD_WIDTH - 1 - move.col;

        return {BOARD_WIDTH - 1 - move.col, move.state};
    }

    return {-1};
}

void MachinePlayer::writeToCache(const Board& board, MachinePlayer::Move move) {
    if (board.NumEmptySlots() >= MIN_MOVES_FOR_CACHE) {
        std::unique_lock<std::mutex> lck(MachinePlayer::cache_lock);

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

void MachinePlayer::tryRunInThread(std::function<void()> work,
                                   std::thread* threadDest) {
    bool canRunInThread = false;
    {
        std::unique_lock<std::mutex> lck(MachinePlayer::thread_lock);
        if (curRunningThreads < MAX_THREADS) {
            canRunInThread = true;
            ++curRunningThreads;
        }
    }

    if (canRunInThread) {
        *threadDest = std::thread([work] {
            work();

            std::unique_lock<std::mutex> lck(MachinePlayer::thread_lock);
            --curRunningThreads;

            assert(curRunningThreads >= 0);
        });
    } else {
        work();
    }
}

#pragma once

#include <iostream>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <functional>

#include "player.hpp"

class MachinePlayer : public Player {
   public:
    MachinePlayer();
    virtual int MakeMove(const Board& board, Piece piece);

   private:
    struct Move {
        int col;
        GameState state;
    };

    static Move getBestMove(const Board& board, Piece piece);
    static void writeResultForMove(const Board& originalBoard, Piece piece, GameState *stateDest, int col);

    static Move getInstantWinMove(const Board& board, Piece piece);

    static bool iWon(Piece piece, GameState state);
    static Piece other(Piece piece);

    // Cache
    static Move getCachedMove(const Board& board);
    static void writeToCache(const Board& board, Move move);

    static std::unordered_map<std::string, MachinePlayer::Move> cache;
    static std::mutex cache_lock;

    // Threads
    static void tryRunInThread(std::function<void()> work, std::thread* threadDest);

    static std::mutex thread_lock;
    static int curRunningThreads;

    bool surprise = true;
    bool iHaveLost = false;

};

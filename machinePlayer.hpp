#pragma once

#include <iostream>
#include <unordered_map>

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

    Move getBestMove(const Board& board, Piece piece);
    bool iWon(Piece piece, GameState state);
    Piece other(Piece piece);

    bool surprise = true;
    bool iHaveLost = false;

    static std::unordered_map<std::string, MachinePlayer::Move> cache;
};

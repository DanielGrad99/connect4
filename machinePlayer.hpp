#pragma once

#include <iostream>

#include "player.hpp"

class MachinePlayer : public Player {
   public:
    MachinePlayer();
    virtual int MakeMove(const Board& board, Piece piece);
};

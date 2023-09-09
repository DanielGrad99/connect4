#pragma once

#include "board.hpp"

class Player {
   public:
    virtual int MakeMove(const Board &board, Piece piece) = 0;
};

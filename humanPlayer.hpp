#pragma once

#include <iostream>

#include "player.hpp"

class HumanPlayer : public Player {
   public:
    HumanPlayer(std::istream& in);
    virtual int MakeMove(const Board& board, Piece piece);

   private:
    std::istream& in;
};

#pragma once

#include <iostream>

#include "board.hpp"


class BoardDrawer {
   public:
    BoardDrawer(std::ostream& out);

    void Draw(const Board &board);

   private:

   char convertPieceToString(Piece piece);

    std::ostream& out;
};

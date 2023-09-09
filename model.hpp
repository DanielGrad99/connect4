#pragma once

#include "board.hpp"
#include "player.hpp"

class Model {
   public:
    Model(Player &p1, Player &p2);
    ~Model();

    void StartGame();
    void NextTurn();

    const Board& GetBoard() const;

   private:
    Board board;

    Player &p1;
    Player &p2;
};

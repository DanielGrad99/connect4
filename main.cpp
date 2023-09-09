#include <iostream>

#include "board.hpp"
#include "boardDrawer.hpp"
#include "humanPlayer.hpp"
#include "model.hpp"

using namespace std;

int main() {
    cout << "Hello there" << endl;

    BoardDrawer boardDrawer = BoardDrawer(std::cout);

    HumanPlayer p1{std::cin};
    HumanPlayer p2{std::cin};
    Model model{p1, p2};

    const Board &board = model.GetBoard();

    model.StartGame();
    while (!board.IsGameOver()) {
        model.NextTurn();
        boardDrawer.Draw(board);
    }

    if (board.IsGameOver() == Piece::BLUE) {
        std::cout << "Blue won" << std::endl;
    } else if (board.IsGameOver() == Piece::RED) {
        std::cout << "Red won" << std::endl;
    } else {
        std::cout << "No one won? wtf!?" << std::endl;
    }

    return 0;
}
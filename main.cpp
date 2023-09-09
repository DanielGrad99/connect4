#include <iostream>

#include "board.hpp"
#include "boardDrawer.hpp"
#include "model.hpp"
#include "humanPlayer.hpp"

using namespace std;

int main() {
    cout << "Hello there" << endl;

    BoardDrawer boardDrawer = BoardDrawer(std::cout);

    HumanPlayer p1 {std::cin};
    HumanPlayer p2 {std::cin};
    Model model {p1, p2};

    const Board &board = model.GetBoard();

    model.StartGame();
    while (!board.IsGameOver()) {
        model.NextTurn();
        boardDrawer.Draw(board);
    }

    return 0;
}
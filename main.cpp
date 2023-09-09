#include <cassert>
#include <iostream>

#include "board.hpp"
#include "boardDrawer.hpp"
#include "humanPlayer.hpp"
#include "model.hpp"

using namespace std;

int main() {
    BoardDrawer boardDrawer = BoardDrawer(std::cout);

    HumanPlayer p1{std::cin};
    HumanPlayer p2{std::cin};
    Model model{p1, p2};

    const Board &board = model.GetBoard();

    model.StartGame();
    boardDrawer.Draw(board);

    while (!board.IsGameOver()) {
        model.NextTurn();
        boardDrawer.Draw(board);
    }

    if (board.IsGameOver() == GameState::BLUE_WON) {
        std::cout << "Blue won" << std::endl;
    } else if (board.IsGameOver() == GameState::RED_WON) {
        std::cout << "Red won" << std::endl;
    } else if (board.IsGameOver() == GameState::PLAYING) {
        std::cout << "Still playing" << std::endl;
    } else if (board.IsGameOver() == GameState::TIE) {
        std::cout << "It's a tie" << std::endl;
    } else {
        assert(false);
    }

    return 0;
}
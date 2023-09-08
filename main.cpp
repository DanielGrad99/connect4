#include <unistd.h>

#include <iostream>

#include "board.hpp"
#include "boardDrawer.hpp"

using namespace std;

int main() {
    cout << "Hello there" << endl;

    BoardDrawer boardDrawer = BoardDrawer(std::cout);

    Board board = Board();

    boardDrawer.Draw(board);
    usleep(1000000);

    board.PlacePiece(Piece::RED, 0);
    board.PlacePiece(Piece::RED, 0);
    board.PlacePiece(Piece::RED, 0);

    boardDrawer.Draw(board);
    usleep(1000000);

    board.PlacePiece(Piece::BLUE, 0);
    board.PlacePiece(Piece::BLUE, 0);
    board.PlacePiece(Piece::BLUE, 0);
    board.PlacePiece(Piece::BLUE, 0);
    board.PlacePiece(Piece::BLUE, 0);
    board.PlacePiece(Piece::BLUE, 0);

    boardDrawer.Draw(board);
    usleep(1000000);

    board.PlacePiece(Piece::BLUE, 1);
    board.PlacePiece(Piece::RED, 1);

    board.PlacePiece(Piece::BLUE, 2);
    board.PlacePiece(Piece::BLUE, 2);
    board.PlacePiece(Piece::RED, 2);

    board.PlacePiece(Piece::RED, 3);
    board.PlacePiece(Piece::BLUE, 3);
    board.PlacePiece(Piece::BLUE, 3);
    board.PlacePiece(Piece::RED, 3);

    board.PlacePiece(Piece::BLUE, 4);

    boardDrawer.Draw(board);
    usleep(1000000);

    if (board.IsGameOver()) {
        std::cout << "Game over" << std::endl;
    } else {
        std::cout << "Game not over" << std::endl;
    }

    return 0;
}
#include "model.hpp"

Model::Model(Player &p1, Player &p2) : p1(p1), p2(p2) {}

Model::~Model() {}

void Model::StartGame() { board.NewGame(); }

void Model::NextTurn() {
    int c = p1.MakeMove(board, Piece::RED);
    board.PlacePiece(Piece::RED, c);

    if (board.IsGameOver()) {
        return;
    }

    c = p2.MakeMove(board, Piece::BLUE);
    board.PlacePiece(Piece::BLUE, c);
}

const Board &Model::GetBoard() const { return board; }

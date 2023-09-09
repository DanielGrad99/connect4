#pragma once

#include <string>

#define BOARD_WIDTH 7
#define BOARD_HEIGHT 6
#define NUM_IN_A_ROW 4

enum class Piece { NONE, RED, BLUE };
enum GameState { PLAYING, TIE, BLUE_WON, RED_WON};

/*

// TOP
35 36 37 38 39 40 41
28 29 30 31 32 33 34
21 22 23 24 25 26 27
14 15 16 17 18 19 20
7  8  9  10 11 12 13
0  1  2  3  4  5  6
// BOTTOM

*/

class Board {
   public:
    Board();
    ~Board();

    Board(const Board &other);
    Board(Board &&other);

    Piece GetPiece(int row, int col) const;

    void NewGame();
    GameState IsGameOver() const;
    bool PlacePiece(Piece piece, int column);  // column is in [0, 6]

   private:
    Piece mBoard[BOARD_WIDTH * BOARD_HEIGHT] = {Piece::NONE};

    int getPieceAbove(int pos) const;  // -1 if not found
    int getPieceRight(int pos) const;  // -1 if not found
    bool isBottomLeftMostStartOfWinning(int pos) const;

    bool isWinningUp(int pos) const;
    bool isWinningRight(int pos) const;
    bool isWinningUpRight(int pos) const;

    friend std::string to_string(const Board &board);
};

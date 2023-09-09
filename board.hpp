#pragma once

#include <string>

#define BOARD_WIDTH 7
#define BOARD_HEIGHT 6
#define NUM_IN_A_ROW 4

enum class Piece { NONE, RED, BLUE };
enum GameState { PLAYING, TIE, BLUE_WON, RED_WON };

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
    struct MoveResult {
        bool ValidMove;
        GameState gameState;
    };

    Board();
    ~Board();

    Board(const Board &other);
    Board(Board &&other);

    Board FlipMe() const;

    Piece GetPiece(int row, int col) const;

    // Game control
    void NewGame();
    GameState IsGameOver() const;
    MoveResult PlacePiece(Piece piece, int column);  // column is in [0, 6]

    bool AnyAvailableMoves() const;
    int NumEmptySlots() const;

    friend std::string to_string(const Board &board);

   private:
    Piece mBoard[BOARD_WIDTH * BOARD_HEIGHT] = {Piece::NONE};

    bool isPieceWinning(int pos) const;
    bool checkSequence(int pos, int (*getNext)(int), int (*getLast)(int)) const;

    // Get next piece in direction
    static int getPieceUp(int pos);
    static int getPieceUpRight(int pos);
    static int getPieceRight(int pos);
    static int getPieceDownRight(int pos);
    static int getPieceDown(int pos);
    static int getPieceDownLeft(int pos);
    static int getPieceLeft(int pos);
    static int getPieceUpLeft(int pos);
};

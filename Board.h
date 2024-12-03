#pragma once
#include <vector>
#include <utility>

class Board {
public:
    static const int BOARD_SIZE = 8;
enum PieceType { EMPTY = 0, WHITE = 1, BLACK = 2, WHITE_KING = 3, BLACK_KING = 4 };

    Board();
    PieceType getPiece(int row, int col) const;
    void setPiece(int row, int col, PieceType piece);
bool movePiece(int fromRow, int fromCol, int toRow, int toCol, bool& captureOccurred);
    std::vector<std::pair<int, int>> getValidMoves(int row, int col) const;
    bool isValidMove(int fromRow, int fromCol, int toRow, int toCol) const;

private:
    PieceType board[BOARD_SIZE][BOARD_SIZE];
    bool isValidPosition(int row, int col) const;
bool isValidJump(int fromRow, int fromCol, int toRow, int toCol) const;
    
    // Helper methods for queen movements
    bool isValidDiagonalMove(int fromRow, int fromCol, int toRow, int toCol) const;
    std::vector<std::pair<int, int>> getCapturesInPath(int fromRow, int fromCol, int toRow, int toCol) const;
    bool isPathClear(int fromRow, int fromCol, int toRow, int toCol) const;
};

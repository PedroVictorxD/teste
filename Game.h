#pragma once
#include "Board.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

class Game {
public:
    Game();
    void init(GLFWwindow* window);
    void update();
    void render();
    void handleMouseClick(double xpos, double ypos);
    const Board& getBoard() const { return board; }
    bool isHighlighted(int row, int col) const;

private:
    Board board;
    GLFWwindow* window;
    bool pieceSelected;
    int selectedRow;
    int selectedCol;
    std::vector<std::pair<int, int>> validMoves;
    bool isWhiteTurn;
    int whiteScore;
    int blackScore;
    bool gameOver;
public:
    int getWhiteScore() const { return whiteScore; }
    int getBlackScore() const { return blackScore; }
    bool isGameOver() const { return gameOver; }
    const char* getWinner() const;

    void selectPiece(int row, int col);
    void movePiece(int row, int col);
    bool screenToBoard(double xpos, double ypos, int& row, int& col);
    void clearSelection();
};

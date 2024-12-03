#include "Game.h"
#include "Logger.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <sstream>

Game::Game() : pieceSelected(false), selectedRow(-1), selectedCol(-1), isWhiteTurn(true), 
               whiteScore(0), blackScore(0), gameOver(false) {
    LOG("Game initialized. White's turn to play.");
}

const char* Game::getWinner() const {
    if (!gameOver) return nullptr;
    if (whiteScore > blackScore) return "White Player Wins!";
    if (blackScore > whiteScore) return "Black Player Wins!";
    return "It's a Tie!";
}

void Game::init(GLFWwindow* window) {
    this->window = window;
}

void Game::update() {
    // Update game state if needed
}

void Game::handleMouseClick(double xpos, double ypos) {
    if (gameOver) {
        LOG("Game is over! No further moves allowed.");
        return;
    }
    
    int row, col;
    if (!screenToBoard(xpos, ypos, row, col)) {
        LOG("Click outside the board");
        return;
    }

    std::stringstream ss;
    // Print board position in chess-like notation (A1-H8)
    char colLetter = 'A' + col;
    int rowNumber = 8 - row;
    ss << "Board Position: " << colLetter << rowNumber;
    
    // Add piece information
    Board::PieceType piece = board.getPiece(row, col);
    ss << " [" << (piece == Board::WHITE ? "White" : 
                   piece == Board::BLACK ? "Black" : 
                   piece == Board::WHITE_KING ? "White King" :
                   piece == Board::BLACK_KING ? "Black King" : "Empty") << "]";
    LOG(ss.str());

    if (!pieceSelected) {
        // Try to select a piece
        if ((isWhiteTurn && (piece == Board::WHITE || piece == Board::WHITE_KING)) || 
            (!isWhiteTurn && (piece == Board::BLACK || piece == Board::BLACK_KING))) {
            selectPiece(row, col);
        } else if (piece != Board::EMPTY) {
            LOG("Invalid Selection: It's " + std::string(isWhiteTurn ? "White" : "Black") + "'s turn");
        }
    } else {
        // Try to move the selected piece
        if (row == selectedRow && col == selectedCol) {
            LOG("Deselecting current piece");
            clearSelection();
        } else {
            movePiece(row, col);
        }
    }
}

void Game::selectPiece(int row, int col) {
    selectedRow = row;
    selectedCol = col;
    pieceSelected = true;
    validMoves = board.getValidMoves(row, col);

    // Log piece selection with chess notation
    char fromCol = 'A' + col;
    int fromRow = 8 - row;
    std::stringstream ss;
    ss << (isWhiteTurn ? "White" : "Black") << " piece selected at " << fromCol << fromRow;
    LOG(ss.str());

    // Log valid moves in chess notation
    if (!validMoves.empty()) {
        ss.str("");
        ss << "Valid moves: ";
        for (const auto& move : validMoves) {
            char moveCol = 'A' + move.second;
            int moveRow = 8 - move.first;
            ss << moveCol << moveRow << " ";
        }
        LOG(ss.str());
    } else {
        LOG("No valid moves available for this piece");
    }
}

void Game::movePiece(int row, int col) {
    // Use chess notation for logging moves
    char fromCol = 'A' + selectedCol;
    int fromRow = 8 - selectedRow;
    char toCol = 'A' + col;
    int toRow = 8 - row;
    
    std::stringstream ss;
    ss << "Attempting move " << fromCol << fromRow << " to " << toCol << toRow;
    LOG(ss.str());

    // Check if the move is valid
    bool validMove = false;
    for (const auto& move : validMoves) {
        if (move.first == row && move.second == col) {
            validMove = true;
            break;
        }
    }

    if (validMove) {
        bool captureOccurred = false;
        if (board.movePiece(selectedRow, selectedCol, row, col, captureOccurred)) {
            ss.str("");
            ss << "Move successful: " << fromCol << fromRow << " -> " << toCol << toRow;
            LOG(ss.str());
            
            if (captureOccurred) {
                // Update scores based on the captured piece
                if (isWhiteTurn) {
                    blackScore--; // Decrement black score
                    LOG("White captures a piece! Score: " + std::to_string(whiteScore));
                } else {
                    whiteScore--; // Decrement white score
                    LOG("Black captures a piece! Score: " + std::to_string(blackScore));
                }
                
                // Check if game is over (no more pieces of one color)
                if (whiteScore <= 0 || blackScore <= 0) {
                    gameOver = true;
                    LOG("Game Over! " + std::string(getWinner()));
                } else {
                    LOG("Capture occurred! " + std::string(isWhiteTurn ? "White" : "Black") + " can move again.");
                }
            } else {
                isWhiteTurn = !isWhiteTurn;
                LOG(std::string(isWhiteTurn ? "White" : "Black") + "'s turn to play");
            }
        } else {
            LOG("Move failed: Invalid according to game rules");
        }
    } else {
        LOG("Invalid move: Not in list of valid moves");
    }

    clearSelection();
}

void Game::clearSelection() {
    pieceSelected = false;
    selectedRow = -1;
    selectedCol = -1;
    validMoves.clear();
    LOG("Selection cleared");
}

bool Game::screenToBoard(double xpos, double ypos, int& row, int& col) {
    // Get current window size

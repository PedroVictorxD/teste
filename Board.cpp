#include "Board.h"
#include <cstdlib>
#include <algorithm>

Board::Board() {
    // Initialize the board with starting positions
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            if (row < 3 && (row + col) % 2 == 1) {
                board[row][col] = BLACK;
            } else if (row > 4 && (row + col) % 2 == 1) {
                board[row][col] = WHITE;
            } else {
                board[row][col] = EMPTY;
            }
        }
    }
}

Board::PieceType Board::getPiece(int row, int col) const {
    if (!isValidPosition(row, col)) return EMPTY;
    return board[row][col];
}

void Board::setPiece(int row, int col, PieceType piece) {
    if (isValidPosition(row, col)) {
        board[row][col] = piece;
    }
}

bool Board::isValidPosition(int row, int col) const {
    return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
}

std::vector<std::pair<int, int>> Board::getValidMoves(int row, int col) const {
    std::vector<std::pair<int, int>> validMoves;
    
    if (!isValidPosition(row, col)) return validMoves;
    
    PieceType piece = board[row][col];
    if (piece == EMPTY) return validMoves;

// Directions of movement
    std::vector<int> directions;
    
    if (piece == WHITE_KING || piece == BLACK_KING) {
        // Kings can move in both directions
        directions = {-1, 1};
    } else if (piece == WHITE) {
        directions = {-1}; // White moves up
    } else if (piece == BLACK) {
        directions = {1};  // Black moves down
    }

if (piece == WHITE_KING || piece == BLACK_KING) {
        // Check all diagonal directions for kings
        for (int i = -BOARD_SIZE + 1; i < BOARD_SIZE; i++) {
            // Check all four diagonal directions
            std::vector<std::pair<int, int>> directions = {
                {i, i}, {i, -i}, {-i, i}, {-i, -i}
            };
            
            for (const auto& dir : directions) {
                int newRow = row + dir.first;
                int newCol = col + dir.second;
                
                if (isValidPosition(newRow, newCol)) {
                    if (isValidDiagonalMove(row, col, newRow, newCol)) {
                        validMoves.push_back({newRow, newCol});
                    }
                }
            }
        }
    } else {
        // Regular piece moves
        for (int direction : directions) {
            for (int dx : {-1, 1}) {
                int newRow = row + direction;
                int newCol = col + dx;
                
                if (isValidPosition(newRow, newCol) && board[newRow][newCol] == EMPTY) {
                    validMoves.push_back({newRow, newCol});
                }
            }
        }

        // Regular piece jumps
        for (int direction : directions) {
            for (int dx : {-2, 2}) {
                int newRow = row + 2 * direction;
                int newCol = col + dx;
                
                if (isValidJump(row, col, newRow, newCol)) {
                    validMoves.push_back({newRow, newCol});
                }
            }
        }
    }

    return validMoves;
}

bool Board::isValidJump(int fromRow, int fromCol, int toRow, int toCol) const {
    if (!isValidPosition(fromRow, fromCol) || !isValidPosition(toRow, toCol)) {
        return false;
    }

    // Check if destination is empty
    if (board[toRow][toCol] != EMPTY) {
        return false;
    }

    // Calculate middle position (piece to be jumped over)
    int midRow = (fromRow + toRow) / 2;
    int midCol = (fromCol + toCol) / 2;

    PieceType currentPiece = board[fromRow][fromCol];
    PieceType jumpedPiece = board[midRow][midCol];

// Check if jumping over opponent's piece
    bool isWhitePiece = (currentPiece == WHITE || currentPiece == WHITE_KING);
    bool isBlackPiece = (currentPiece == BLACK || currentPiece == BLACK_KING);
    bool isJumpingWhite = (jumpedPiece == WHITE || jumpedPiece == WHITE_KING);
    bool isJumpingBlack = (jumpedPiece == BLACK || jumpedPiece == BLACK_KING);
    
    return (isWhitePiece && isJumpingBlack) || (isBlackPiece && isJumpingWhite);
}

bool Board::isValidDiagonalMove(int fromRow, int fromCol, int toRow, int toCol) const {
    // Verificar se é um movimento diagonal
    if (abs(toRow - fromRow) != abs(toCol - fromCol)) {
        return false;
    }

    // Verificar se o destino está vazio
    if (board[toRow][toCol] != EMPTY) {
        return false;
    }

    // Se houver peças no caminho, verificar se podem ser capturadas
    std::vector<std::pair<int, int>> captures = getCapturesInPath(fromRow, fromCol, toRow, toCol);
    
    // Se não houver capturas, o caminho deve estar limpo
    if (captures.empty()) {
        return isPathClear(fromRow, fromCol, toRow, toCol);
    }
    
    return true;
}

bool Board::isPathClear(int fromRow, int fromCol, int toRow, int toCol) const {
    int rowDir = (toRow > fromRow) ? 1 : -1;
    int colDir = (toCol > fromCol) ? 1 : -1;
    
    int row = fromRow + rowDir;
    int col = fromCol + colDir;
    
    while (row != toRow && col != toCol) {
        if (board[row][col] != EMPTY) {
            return false;
        }
        row += rowDir;
        col += colDir;
    }
    
    return true;
}

std::vector<std::pair<int, int>> Board::getCapturesInPath(int fromRow, int fromCol, int toRow, int toCol) const {
    std::vector<std::pair<int, int>> captures;
    PieceType currentPiece = board[fromRow][fromCol];
    bool isWhitePiece = (currentPiece == WHITE_KING);
    bool isBlackPiece = (currentPiece == BLACK_KING);
    
    int rowDir = (toRow > fromRow) ? 1 : -1;
    int colDir = (toCol > fromCol) ? 1 : -1;
    
    int row = fromRow + rowDir;
    int col = fromCol + colDir;
    
    while (row != toRow && col != toCol) {
        PieceType pieceInPath = board[row][col];
        
        if (pieceInPath != EMPTY) {
            bool isOpponentPiece = (isWhitePiece && (pieceInPath == BLACK || pieceInPath == BLACK_KING)) ||
                                 (isBlackPiece && (pieceInPath == WHITE || pieceInPath == WHITE_KING));
            
            if (isOpponentPiece) {
                captures.push_back({row, col});
            } else {
                // Se encontrar uma peça própria, movimento inválido
                captures.clear();
                break;
            }
        }
        
        row += rowDir;
        col += colDir;
    }
    
    return captures;
}

bool Board::movePiece(int fromRow, int fromCol, int toRow, int toCol, bool& captureOccurred) {
    if (!isValidPosition(fromRow, fromCol) || !isValidPosition(toRow, toCol)) {
        captureOccurred = false;
        return false;
    }

    PieceType movingPiece = board[fromRow][fromCol];
    
    // Check if the move is in the valid moves list
    auto validMoves = getValidMoves(fromRow, fromCol);
    bool isValid = false;
    for (const auto& move : validMoves) {
        if (move.first == toRow && move.second == toCol) {
            isValid = true;
            break;
        }
    }

    if (!isValid) {
        captureOccurred = false;
        return false;
    }

    // Special handling for king pieces
    if (movingPiece == WHITE_KING || movingPiece == BLACK_KING) {
        auto captures = getCapturesInPath(fromRow, fromCol, toRow, toCol);
        captureOccurred = !captures.empty();
        
        // Remove all captured pieces
        for (const auto& capture : captures) {
            board[capture.first][capture.second] = EMPTY;
        }
    } else {
        // Regular piece capture
        captureOccurred = (abs(toRow - fromRow) == 2);
        if (captureOccurred) {
            int midRow = (fromRow + toRow) / 2;
            int midCol = (fromCol + toCol) / 2;
            board[midRow][midCol] = EMPTY;
        }
    }

    // Perform the move
    board[fromRow][fromCol] = EMPTY;

    // Check for king promotion
    if (movingPiece == WHITE && toRow == 0) {
        board[toRow][toCol] = WHITE_KING;
    } else if (movingPiece == BLACK && toRow == BOARD_SIZE - 1) {
        board[toRow][toCol] = BLACK_KING;
    } else {
        board[toRow][toCol] = movingPiece;
    }

    return true;
}

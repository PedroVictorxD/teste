#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include "Game.h"

class Renderer {
public:
    Renderer();
    ~Renderer();
    
    void init();
    void render(const Game& game);

private:
    // Shader program
    GLuint shaderProgram;
    
    // Board rendering
    GLuint boardVAO, boardVBO, boardEBO;
    std::vector<float> boardVertices;
    std::vector<unsigned int> boardIndices;
    
// Piece rendering
    GLuint pieceVAO, pieceVBO;
    std::vector<float> whitePieceVertices;
    std::vector<float> blackPieceVertices;
    std::vector<float> highlightedPieceVertices;
    std::vector<float> validMoveVertices;
    
    // Constants
    static const int SEGMENTS = 32;
    static const float PIECE_HEIGHT;
    static const float PIECE_RADIUS;
    
    // Helper functions
    void setupShaders();
    void setupBoard();
    void setupPieces();
std::vector<float> generateCylinderVertices(float radius, float height, int segments, const glm::vec3& color);
    std::vector<float> generateCrownVertices(float radius, float height, const glm::vec3& color);
    void drawBoard(const Game& game);
    void drawPieces(const Game& game);
void drawPiece(float x, float z, const std::vector<float>& vertices);
};

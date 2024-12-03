#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

const float Renderer::PIECE_HEIGHT = 0.25f;
const float Renderer::PIECE_RADIUS = 0.35f;

// Shader sources
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 position;
    layout (location = 1) in vec3 color;
    out vec3 ourColor;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main() {
        gl_Position = projection * view * model * vec4(position, 1.0f);
        ourColor = color;
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 ourColor;
    out vec4 color;
    void main() {
        color = vec4(ourColor, 1.0f);
    }
)";

Renderer::Renderer() {}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &boardVAO);
    glDeleteBuffers(1, &boardVBO);
    glDeleteBuffers(1, &boardEBO);
    glDeleteVertexArrays(1, &pieceVAO);
    glDeleteBuffers(1, &pieceVBO);
    glDeleteProgram(shaderProgram);
}

void Renderer::init() {
    setupShaders();
    setupBoard();
    setupPieces();
}

void Renderer::setupShaders() {
    // Create vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check vertex shader compilation
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Vertex shader compilation failed:\n" << infoLog << std::endl;
    }

    // Create fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check fragment shader compilation
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "Fragment shader compilation failed:\n" << infoLog << std::endl;
    }

    // Create shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check shader program linking
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Shader program linking failed:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Renderer::setupBoard() {
    float squareSize = 1.0f;
    float startX = -4.0f;
    float startZ = -4.0f;

    // Generate board vertices and indices
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            float x = startX + col * squareSize;
            float z = startZ + row * squareSize;
            float color = (row + col) % 2 == 0 ? 1.0f : 0.0f;

            // Add vertices for this square
            for (int i = 0; i < 4; i++) {
                float vx = x + (i == 1 || i == 2 ? squareSize : 0.0f);
                float vz = z + (i == 2 || i == 3 ? squareSize : 0.0f);
                
                boardVertices.push_back(vx);        // Position X
                boardVertices.push_back(0.0f);      // Position Y
                boardVertices.push_back(vz);        // Position Z
                boardVertices.push_back(color);     // Color R
                boardVertices.push_back(color);     // Color G
                boardVertices.push_back(color);     // Color B
            }

            // Add indices for this square
            unsigned int baseIndex = (row * 8 + col) * 4;
            boardIndices.push_back(baseIndex);
            boardIndices.push_back(baseIndex + 1);
            boardIndices.push_back(baseIndex + 2);
            boardIndices.push_back(baseIndex);
            boardIndices.push_back(baseIndex + 2);
            boardIndices.push_back(baseIndex + 3);
        }
    }

    // Create and setup board buffers
    glGenVertexArrays(1, &boardVAO);
    glGenBuffers(1, &boardVBO);
    glGenBuffers(1, &boardEBO);

    glBindVertexArray(boardVAO);

    glBindBuffer(GL_ARRAY_BUFFER, boardVBO);
    glBufferData(GL_ARRAY_BUFFER, boardVertices.size() * sizeof(float), boardVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boardEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, boardIndices.size() * sizeof(unsigned int), boardIndices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

std::vector<float> Renderer::generateCrownVertices(float radius, float height, const glm::vec3& color) {
    std::vector<float> vertices;
    const float PI = 3.14159265358979323846f;
    const int points = 5; // 5 points for the crown
    const float crownHeight = height * 0.5f;
    
    // Generate crown points
    for (int i = 0; i < points; i++) {
        float angle = (2.0f * PI * i) / points - PI/2;
        float nextAngle = (2.0f * PI * (i + 1)) / points - PI/2;
        
        // Base point
        float x1 = radius * 0.6f * cos(angle);
        float z1 = radius * 0.6f * sin(angle);
        
        // Peak point
        float x2 = radius * 0.8f * cos(angle + PI/points);
        float z2 = radius * 0.8f * sin(angle + PI/points);
        
        // Add triangle
        vertices.push_back(x1);
        vertices.push_back(height + crownHeight * 0.5f);
        vertices.push_back(z1);
        vertices.push_back(color.r);
        vertices.push_back(color.g);
        vertices.push_back(color.b);
        
        vertices.push_back(x2);
        vertices.push_back(height + crownHeight);
        vertices.push_back(z2);
        vertices.push_back(color.r);
        vertices.push_back(color.g);
        vertices.push_back(color.b);
        
        vertices.push_back(0.0f);
        vertices.push_back(height + crownHeight * 0.5f);
        vertices.push_back(0.0f);
        vertices.push_back(color.r);
        vertices.push_back(color.g);
        vertices.push_back(color.b);
    }
    
    return vertices;
}

void Renderer::setupPieces() {
    // Generate piece vertices
    whitePieceVertices = generateCylinderVertices(PIECE_RADIUS, PIECE_HEIGHT, SEGMENTS, glm::vec3(0.9f, 0.9f, 0.8f));
    blackPieceVertices = generateCylinderVertices(PIECE_RADIUS, PIECE_HEIGHT, SEGMENTS, glm::vec3(0.15f, 0.15f, 0.15f));
    highlightedPieceVertices = generateCylinderVertices(PIECE_RADIUS, PIECE_HEIGHT, SEGMENTS, glm::vec3(0.8f, 0.8f, 0.0f));
    validMoveVertices = generateCylinderVertices(PIECE_RADIUS * 0.5f, PIECE_HEIGHT * 0.1f, SEGMENTS, glm::vec3(0.0f, 0.8f, 0.0f));
    
    // Add crown vertices to king pieces
    std::vector<float> whiteCrown = generateCrownVertices(PIECE_RADIUS, PIECE_HEIGHT, glm::vec3(1.0f, 0.84f, 0.0f)); // Gold color
    std::vector<float> blackCrown = generateCrownVertices(PIECE_RADIUS, PIECE_HEIGHT, glm::vec3(0.7f, 0.7f, 0.7f)); // Silver color
    
    // Append crown vertices to piece vertices for kings
    whitePieceVertices.insert(whitePieceVertices.end(), whiteCrown.begin(), whiteCrown.end());
    blackPieceVertices.insert(blackPieceVertices.end(), blackCrown.begin(), blackCrown.end());

    // Create and setup piece buffers
    glGenVertexArrays(1, &pieceVAO);
    glGenBuffers(1, &pieceVBO);

    glBindVertexArray(pieceVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pieceVBO);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

std::vector<float> Renderer::generateCylinderVertices(float radius, float height, int segments, const glm::vec3& color) {
    std::vector<float> vertices;
    const float PI = 3.14159265358979323846f;

    // Top circle center
    vertices.push_back(0.0f);      // Position
    vertices.push_back(height);
    vertices.push_back(0.0f);
    vertices.push_back(color.r);   // Color
    vertices.push_back(color.g);
    vertices.push_back(color.b);

    // Top circle vertices
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * PI * float(i) / float(segments);
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        vertices.push_back(x);
        vertices.push_back(height);
        vertices.push_back(z);
        vertices.push_back(color.r);
        vertices.push_back(color.g);
        vertices.push_back(color.b);
    }

    // Bottom circle center
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(color.r);
    vertices.push_back(color.g);
    vertices.push_back(color.b);

    // Bottom circle vertices
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * PI * float(i) / float(segments);
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        vertices.push_back(x);
        vertices.push_back(0.0f);
        vertices.push_back(z);
        vertices.push_back(color.r);
        vertices.push_back(color.g);
        vertices.push_back(color.b);
    }

    // Side vertices
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * PI * float(i) / float(segments);
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        vertices.push_back(x);
        vertices.push_back(height);
        vertices.push_back(z);
        vertices.push_back(color.r);
        vertices.push_back(color.g);
        vertices.push_back(color.b);

        vertices.push_back(x);
        vertices.push_back(0.0f);
        vertices.push_back(z);
        vertices.push_back(color.r);
        vertices.push_back(color.g);
        vertices.push_back(color.b);
    }

    return vertices;
}

void Renderer::render(const Game& game) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);

    // Get current window size
    int width, height;
    glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
    
    // Set up view and projection matrices with adjusted camera position
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 8.0f, 6.0f),  // Camera positioned higher and closer for better view
        glm::vec3(0.0f, 0.0f, 0.0f),  // Looking at center of board
        glm::vec3(0.0f, 1.0f, 0.0f)   // Up vector
    );
    
    // Use current window dimensions for aspect ratio
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

    // Update viewport
    glViewport(0, 0, width, height);

    // Set uniforms
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    drawBoard(game);
    drawPieces(game);
}

void Renderer::drawBoard(const Game& game) {
    glm::mat4 model = glm::mat4(1.0f);
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(boardVAO);
    glDrawElements(GL_TRIANGLES, boardIndices.size(), GL_UNSIGNED_INT, 0);
}

void Renderer::drawPieces(const Game& game) {
    bool gameOver = game.isGameOver(); // Check if the game is over
    bool gameOver = game.isGameOver(); // Check if the game is over
    const Board& board = game.getBoard();
    glBindVertexArray(pieceVAO);
    
    // Draw valid move indicators first (so they appear under the pieces)
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (game.isHighlighted(row, col) && board.getPiece(row, col) == Board::EMPTY) {
                float x = -4.0f + col + 0.5f;
                float z = -4.0f + row + 0.5f;
                drawPiece(x, z, validMoveVertices);
            }
        }
    }

    // Draw pieces
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Board::PieceType piece = board.getPiece(row, col);
            if (piece != Board::EMPTY) {
                float x = -4.0f + col + 0.5f;
                float z = -4.0f + row + 0.5f;

                std::vector<float> vertices;
                if (game.isHighlighted(row, col)) {
                    vertices = highlightedPieceVertices;
                } else {
                    if (piece == Board::WHITE || piece == Board::WHITE_KING) {
                        vertices = whitePieceVertices;
                    } else {
                        vertices = blackPieceVertices;
                    }
                }

                // Apply scaling effect if the game is over
                if (gameOver) {
                    float scale = 1.2f + 0.1f * sin(glfwGetTime() * 2.0f); // Pulsating effect
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(x, 0.0f, z));
                    model = glm::scale(model, glm::vec3(scale, scale, scale)); // Scale up the pieces
                    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
                    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                } else {
                    drawPiece(x, z, vertices);
                }
                
                // Draw crown for king pieces
                if (piece == Board::WHITE_KING || piece == Board::BLACK_KING) {
                    // Draw additional triangles for crown
                    glDrawArrays(GL_TRIANGLES, (SEGMENTS + 2) * 2 + (SEGMENTS + 1) * 2, 15); // 5 triangles * 3 vertices
                }
            }
        }
    }
}
<environment_details>
# VSCode Visible Files
Renderer.cpp

# VSCode Open Tabs
Game.cpp
Renderer.cpp

# Actively Running Terminals
## Original command: `cd build && ./checkers`
    bool gameOver = game.isGameOver(); // Check if the game is over
    const Board& board = game.getBoard();
    glBindVertexArray(pieceVAO);

    // Draw valid move indicators first (so they appear under the pieces)
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (game.isHighlighted(row, col) && board.getPiece(row, col) == Board::EMPTY) {
                float x = -4.0f + col + 0.5f;
                float z = -4.0f + row + 0.5f;
                drawPiece(x, z, validMoveVertices);
            }
        }
    }

    // Draw pieces
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Board::PieceType piece = board.getPiece(row, col);
            if (piece != Board::EMPTY) {
                float x = -4.0f + col + 0.5f;
                float z = -4.0f + row + 0.5f;

std::vector<float> vertices;
                if (game.isHighlighted(row, col)) {
                    vertices = highlightedPieceVertices;
                } else {
                    if (piece == Board::WHITE || piece == Board::WHITE_KING) {
                        vertices = whitePieceVertices;
                    } else {
                        vertices = blackPieceVertices;
                    }
                }

                drawPiece(x, z, vertices);
                
                // Draw crown for king pieces
                if (piece == Board::WHITE_KING || piece == Board::BLACK_KING) {
                    // Draw additional triangles for crown
                    glDrawArrays(GL_TRIANGLES, (SEGMENTS + 2) * 2 + (SEGMENTS + 1) * 2, 15); // 5 triangles * 3 vertices
                }
            }
        }
    }
}

void Renderer::drawPiece(float x, float z, const std::vector<float>& vertices) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, 0.0f, z));
    
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindBuffer(GL_ARRAY_BUFFER, pieceVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

// Draw top circle
    glDrawArrays(GL_TRIANGLE_FAN, 0, SEGMENTS + 2);
    // Draw bottom circle
    glDrawArrays(GL_TRIANGLE_FAN, SEGMENTS + 2, SEGMENTS + 2);
    // Draw sides
    glDrawArrays(GL_TRIANGLE_STRIP, (SEGMENTS + 2) * 2, (SEGMENTS + 1) * 2);
}



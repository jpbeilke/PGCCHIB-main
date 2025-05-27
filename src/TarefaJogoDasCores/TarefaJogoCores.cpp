//João Pedro Beilke da Costa

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

// --- DEFINIÇÕES GERAIS E CONSTANTES ---
const GLuint WIDTH = 800, HEIGHT = 600;
const GLuint ROWS = 6, COLS = 8;
const GLuint QUAD_WIDTH = 100, QUAD_HEIGHT = 100;
const float dMax = sqrt(3.0);

// --- ESTRUTURA PARA QUADRADO (RETÂNGULO) ---
struct Quad {
    vec3 position;
    vec3 dimensions;
    vec3 color;
    bool eliminated;
};

// --- VARIÁVEIS GLOBAIS DO JOGO ---
Quad grid[ROWS][COLS];
int iSelected = -1;
int score = 0;
bool gameOver = false;

// --- SHADERS (GRÁFICOS) ---
const GLchar* vertexShaderSource = R"(
#version 400
layout (location = 0) in vec3 position;
uniform mat4 projection;
uniform mat4 model;
void main() {
    gl_Position = projection * model * vec4(position.x, position.y, position.z, 1.0);
}
)";
const GLchar* fragmentShaderSource = R"(
#version 400
uniform vec4 inputColor;
out vec4 color;
void main() {
    color = inputColor;
}
)";

// --- PROTÓTIPOS DAS FUNÇÕES PRINCIPAIS ---
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
GLuint createQuad();
int setupShader();
void eliminarSimilares(float tolerancia);
void inicializaGrid();

// =========================================================
//                  LOOP PRINCIPAL DO JOGO
// =========================================================
int main() {
    srand((unsigned int)time(0));
    glfwInit();

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Jogo das cores!", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    GLuint shaderID = setupShader();
    GLuint VAO = createQuad();

    // --- INICIALIZAÇÃO DO GRID ---
    inicializaGrid();
    score = 0;
    gameOver = false;

    glUseProgram(shaderID);
    GLint colorLoc = glGetUniformLocation(shaderID, "inputColor");
    mat4 projection = ortho(0.0, 800.0, 600.0, 0.0, -1.0, 1.0);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));

    // --- LOOP DE EXECUÇÃO ---
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);

        // --- LÓGICA DE ELIMINAÇÃO ---
        if (iSelected > -1 && !gameOver) {
            eliminarSimilares(0.2f);
        }

        int count = 0;
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                if (!grid[i][j].eliminated) {
                    count++;
                    mat4 model = mat4(1.0);
                    model = translate(model, grid[i][j].position);
                    model = scale(model, grid[i][j].dimensions);
                    glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));
                    glUniform4f(colorLoc, grid[i][j].color.r, grid[i][j].color.g, grid[i][j].color.b, 1.0f);
                    glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
                }
            }
        }
        glBindVertexArray(0);

        // --- ATUALIZAÇÃO DO TÍTULO COM PONTUAÇÃO ---
        string title;
        if (!gameOver)
            title = "Jogo das cores! | Pontuacao: " + to_string(score);
        else
            title = "FIM DE JOGO! Pontuacao final: " + to_string(score) + " | Pressione R para reiniciar";
        glfwSetWindowTitle(window, title.c_str());

        // --- CHECA SE O JOGO TERMINOU ---
        if (count == 0 && !gameOver) {
            gameOver = true;
            cout << "FIM DE JOGO! Pontuacao final: " << score << endl;
        }

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

// =========================================================
//            FUNÇÃO: INICIALIZAÇÃO DO GRID
// =========================================================
void inicializaGrid() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            Quad quad;
            vec2 ini_pos = vec2(QUAD_WIDTH / 2, QUAD_HEIGHT / 2);
            quad.position = vec3(ini_pos.x + j * QUAD_WIDTH, ini_pos.y + i * QUAD_HEIGHT, 0.0);
            quad.dimensions = vec3(QUAD_WIDTH, QUAD_HEIGHT, 1.0);
            quad.color = vec3(rand() % 256 / 255.0, rand() % 256 / 255.0, rand() % 256 / 255.0);
            quad.eliminated = false;
            grid[i][j] = quad;
        }
    }
    score = 0;
    gameOver = false;
}

// =========================================================
//         FUNÇÕES DE SHADER E GEOMETRIA PADRÃO
// =========================================================
int setupShader() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

GLuint createQuad() {
    GLuint VAO;
    GLfloat vertices[] = {
        -0.5, 0.5, 0.0,
        -0.5, -0.5, 0.0,
         0.5, 0.5, 0.0,
         0.5, -0.5, 0.0
    };
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return VAO;
}

// =========================================================
//    FUNÇÃO: ELIMINA QUADRADOS SIMILARES E SOMA PONTUAÇÃO
// =========================================================
void eliminarSimilares(float tolerancia) {
    int x = iSelected % COLS;
    int y = iSelected / COLS;
    vec3 C = grid[y][x].color;
    int eliminados = 0;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (!grid[i][j].eliminated) {
                vec3 O = grid[i][j].color;
                float d = sqrt(pow(C.r - O.r, 2) + pow(C.g - O.g, 2) + pow(C.b - O.b, 2));
                float dd = d / dMax;
                if (dd <= tolerancia) {
                    grid[i][j].eliminated = true;
                    eliminados++;
                }
            }
        }
    }
    score += eliminados; // Pontuação simples: soma quantos eliminou
    iSelected = -1;
}

// =========================================================
//         CALLBACK DO MOUSE: SELECIONAR QUADRADO
// =========================================================
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !gameOver) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        int x = xpos / QUAD_WIDTH;
        int y = ypos / QUAD_HEIGHT;
        if (x >= 0 && x < COLS && y >= 0 && y < ROWS && !grid[y][x].eliminated) {
            iSelected = x + y * COLS;
        }
    }
}

// =========================================================
//     CALLBACK DO TECLADO: ESC (sair) E F5 (reiniciar)
// =========================================================
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_F5 && action == GLFW_PRESS)
        inicializaGrid();
}


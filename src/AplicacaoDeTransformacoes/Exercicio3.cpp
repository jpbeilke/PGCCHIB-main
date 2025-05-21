#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ctime>
#include <cstdlib>

// Vertex shader atualizado para matriz de transformação
const char* vertexShaderSource = R"(
#version 400
layout (location = 0) in vec3 position;
uniform mat4 model;
void main() {
    gl_Position = model * vec4(position, 1.0);
}
)";

// Fragment shader aceita cor via uniform
const char* fragmentShaderSource = R"(
#version 400
uniform vec4 inputColor;
out vec4 color;
void main() {
    color = inputColor;
}
)";

// Struct simples para triângulo
struct Triangle {
    float x, y;      // posição central
    float r, g, b;   // cor (RGB)
};

std::vector<Triangle> triangles;
GLuint vao;

// Função para criar UM único VAO do triângulo padrão
GLuint createTriangleVAO() {
    float vertices[] = {
        -0.1f, -0.1f, 0.0f,
         0.1f, -0.1f, 0.0f,
         0.0f,  0.1f, 0.0f
    };
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return VAO;
}

// Função para compilar shaders
GLuint setupShader() {
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

// Callback do mouse
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glfwGetCursorPos(window, &xpos, &ypos);

        // Converter para coordenadas OpenGL [-1,+1]
        float x = ((float)xpos / width) * 2.0f - 1.0f;
        float y = 1.0f - ((float)ypos / height) * 2.0f;

        // Sorteia cor RGB
        float r = static_cast<float>(rand()) / RAND_MAX;
        float g = static_cast<float>(rand()) / RAND_MAX;
        float b = static_cast<float>(rand()) / RAND_MAX;

        // Adiciona triângulo na lista
        triangles.push_back({x, y, r, g, b});
    }
}

int main() {
    srand((unsigned int)time(0));
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(800, 600, "Exercicio 3 - Triângulos Dinâmicos", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Registra o callback do mouse
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    GLuint shaderID = setupShader();
    vao = createTriangleVAO();

    glUseProgram(shaderID);

    GLint colorLoc = glGetUniformLocation(shaderID, "inputColor");
    GLint modelLoc = glGetUniformLocation(shaderID, "model");

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderID);
        glBindVertexArray(vao);

        for (auto& tri : triangles) {
            // Matriz de translação para a posição do triângulo
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(tri.x, tri.y, 0.0f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            // Cor
            glUniform4f(colorLoc, tri.r, tri.g, tri.b, 1.0f);

            // Desenha triângulo
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

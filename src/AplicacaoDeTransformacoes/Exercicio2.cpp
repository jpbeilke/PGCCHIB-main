#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Vertex shader simples
const char* vertexShaderSource = R"(
#version 400
layout (location = 0) in vec3 position;
void main() {
    gl_Position = vec4(position, 1.0);
}
)";

// Fragment shader simples
const char* fragmentShaderSource = R"(
#version 400
out vec4 color;
void main() {
    color = vec4(0.8, 0.1, 0.5, 1.0);
}
)";

// Função para criar o VAO do triângulo
GLuint createTriangle(float x0, float y0, float x1, float y1, float x2, float y2) {
    float vertices[] = {
        x0, y0, 0.0f,
        x1, y1, 0.0f,
        x2, y2, 0.0f
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

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(800, 600, "Exercicio 2 - 5 Triângulos", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    GLuint shaderID = setupShader();

    // Array para guardar os 5 VAOs dos triângulos
    GLuint vaos[5];

    // Triângulo grande, começando mais abaixo (base centralizada em Y negativo)
    float base_x0 = -0.25f, base_y0 = -0.8f;
    float base_x1 =  0.25f, base_y1 = -0.8f;
    float base_x2 =  0.0f,   base_y2 = -0.5f;

    float vertical_spacing = 0.35f; // Espaço entre triângulos maior

    for (int i = 0; i < 5; ++i) {
        float offset = i * vertical_spacing;
        vaos[i] = createTriangle(
            base_x0, base_y0 + offset,
            base_x1, base_y1 + offset,
            base_x2, base_y2 + offset
        );
    }

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderID);

        // Desenhar os 5 triângulos
        for (int i = 0; i < 5; ++i) {
            glBindVertexArray(vaos[i]);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

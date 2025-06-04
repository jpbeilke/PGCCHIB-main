/*
 * Hello Triangle - Código adaptado de:
 *   - https://learnopengl.com/#!Getting-started/Hello-Triangle
 *   - https://antongerdelan.net/opengl/glcontext2.html
 *
 * Adaptado por: Rossana Baptista Queiroz
 *
 * Disciplinas:
 *   - Processamento Gráfico (Ciência da Computação - Híbrido)
 *   - Processamento Gráfico: Fundamentos (Ciência da Computação - Presencial)
 *   - Fundamentos de Computação Gráfica (Jogos Digitais)
 *
 * Descrição:
 *   Este código é o "Olá Mundo" da Computação Gráfica, utilizando OpenGL Moderna.
 *   No pipeline programável, o desenvolvedor pode implementar as etapas de
 *   Processamento de Geometria e Processamento de Pixel utilizando shaders.
 *   Um programa de shader precisa ter, obrigatoriamente, um Vertex Shader e um Fragment Shader,
 *   enquanto outros shaders, como o de geometria, são opcionais.
 *
 * Histórico:
 *   - Versão inicial: 07/04/2017
 *   - Última atualização: 18/03/2025
 *
 */

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>
#include <cmath>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// STB_IMAGE
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

// Protótipos das funções
int setupShader();
int setupGeometry();
int loadTexture(string filePath);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 800;

// Código fonte do Vertex Shader (modificado para suportar transformação e projeção)
const GLchar *vertexShaderSource = R"(
 #version 400
 layout (location = 0) in vec3 position;
 layout (location = 1) in vec3 color;
 layout (location = 2) in vec2 texc;
 out vec3 vColor;
 out vec2 tex_coord;
 uniform mat4 model;
 uniform mat4 projection;
 void main()
 {
    vColor = color;
    tex_coord = texc;
    gl_Position = projection * model * vec4(position, 1.0);
 }
 )";

// Código fonte do Fragment Shader
const GLchar *fragmentShaderSource = R"(
 #version 400
 in vec3 vColor;
 in vec2 tex_coord;
 out vec4 color;
 uniform sampler2D tex_buff;
 void main()
 {
     color = texture(tex_buff,tex_coord);//vec4(vColor,1.0);
 }
 )";

// Classe Sprite moderna para facilitar múltiplos objetos e transformações
class Sprite {
public:
    GLuint VAO;
    GLuint textureID;
    glm::vec2 position;
    glm::vec2 scale;
    float rotation;
    GLuint shaderID;

    Sprite(GLuint vao, GLuint shader, std::string texturePath, glm::vec2 pos, glm::vec2 scl, float rot)
        : VAO(vao), position(pos), scale(scl), rotation(rot), shaderID(shader)
    {
        textureID = loadTexture(texturePath);
    }

    void Draw(const glm::mat4& projection) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(position, 0.0f));
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0, 0, 1));
        model = glm::scale(model, glm::vec3(scale, 1.0f));

        glUseProgram(shaderID);
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(shaderID, "tex_buff"), 0);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
};

// Função MAIN
int main()
{
    // Inicialização da GLFW
	glfwInit();

    // Muita atenção aqui: alguns ambientes não aceitam essas configurações
    // Você deve adaptar para a versão do OpenGL suportada por sua placa
    // Sugestão: comente essas linhas de código para desobrir a versão e
    // depois atualize (por exemplo: 4.5 com 4 e 5)
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Ativa a suavização de serrilhado (MSAA) com 8 amostras por pixel
    glfwWindowHint(GLFW_SAMPLES, 8);

    // Essencial para computadores da Apple
    // #ifdef __APPLE__
    //  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // #endif

    // Criação da janela GLFW
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Objetos Texturizados -- João Beilke", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Falha ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Fazendo o registro da função de callback para a janela GLFW
    glfwSetKeyCallback(window, key_callback);

    // GLAD: carrega todos os ponteiros d funções da OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Falha ao inicializar GLAD" << std::endl;
        return -1;
    }

	stbi_set_flip_vertically_on_load(true);
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Obtendo as informações de versão
    const GLubyte *renderer = glGetString(GL_RENDERER); /* get renderer string */
    const GLubyte *version = glGetString(GL_VERSION);   /* version as a string */
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version supported " << version << endl;

    // Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Compilando e buildando o programa de shader
    GLuint shaderID = setupShader();

    // Gerando o buffer de geometria de um quad (retângulo 1x1 centrado na origem)
    GLuint quadVAO = setupGeometry();

    // Criando a matriz de projeção ortográfica para facilitar o posicionamento dos sprites
    glm::mat4 projection = glm::ortho(0.0f, (float)WIDTH, 0.0f, (float)HEIGHT, -1.0f, 1.0f);

    // >>>>  SPRITES
     std::vector<Sprite> sprites;
	sprites.push_back(Sprite(quadVAO, shaderID, "../assets/sprites/florescerespiritual.png", glm::vec2(400, 400), glm::vec2(800, 800), 0.0f));
    sprites.push_back(Sprite(quadVAO, shaderID, "../assets/sprites/aphelioschibi.png", glm::vec2(500, 200), glm::vec2(200, 200), 0.0f));
    sprites.push_back(Sprite(quadVAO, shaderID, "../assets/sprites/zerichibi.png", glm::vec2(200, 590), glm::vec2(200, 200), 0.0f));
    sprites.push_back(Sprite(quadVAO, shaderID, "../assets/sprites/passarinho.png", glm::vec2(500, 400), glm::vec2(100, 100), 0.0f));
	sprites.push_back(Sprite(quadVAO, shaderID, "../assets/sprites/passarinho.png", glm::vec2(550, 350), glm::vec2(100, 100), 0.0f));


    double prev_s = glfwGetTime();   // Define o "tempo anterior" inicial.
    double title_countdown_s = 0.1;  // Intervalo para atualizar o título da janela com o FPS.

    while (!glfwWindowShouldClose(window))
    {
        // FPS opcional
        {
            double curr_s = glfwGetTime();
            double elapsed_s = curr_s - prev_s;
            prev_s = curr_s;

            title_countdown_s -= elapsed_s;
            if (title_countdown_s <= 0.0 && elapsed_s > 0.0)
            {
                double fps = 1.0 / elapsed_s;
                char tmp[256];
                sprintf(tmp, "Objetos Texturizados -- FPS %.2lf", fps);
                glfwSetWindowTitle(window, tmp);
                title_countdown_s = 0.1;
            }
        }

        glfwPollEvents();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Desenha todos os sprites criados no vector
        for (auto& sprite : sprites) {
            sprite.Draw(projection);
        }

        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &quadVAO);
    glfwTerminate();
    return 0;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

// Compila e builda o programa de shader (igual ao seu original)
int setupShader()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

// Cria o buffer do QUAD (retângulo centrado), pronto para os sprites
int setupGeometry()
{
    // Quad centrado na origem (tamanho 1x1)
    GLfloat vertices[] = {
        //  x,    y,    z,    r, g, b,   s, t
        -0.5f, -0.5f, 0.0f,  1, 1, 1,   0.0f, 0.0f, // bottom left
         0.5f, -0.5f, 0.0f,  1, 1, 1,   1.0f, 0.0f, // bottom right
         0.5f,  0.5f, 0.0f,  1, 1, 1,   1.0f, 1.0f, // top right

        -0.5f, -0.5f, 0.0f,  1, 1, 1,   0.0f, 0.0f, // bottom left
         0.5f,  0.5f, 0.0f,  1, 1, 1,   1.0f, 1.0f, // top right
        -0.5f,  0.5f, 0.0f,  1, 1, 1,   0.0f, 1.0f  // top left
    };

    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

// Carregamento de textura (PNG/JPG)
int loadTexture(string filePath)
{
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        if (nrChannels == 3) // jpg, bmp
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else // png
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture: " << filePath << std::endl;
    }

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texID;
}

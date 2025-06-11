#include <iostream>
#include <string>
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

//GLM
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

// ===============================
// VARIÁVEIS GLOBAIS PARA CONTROLE
// ===============================

enum Direcao { BAIXO = 0, ESQUERDA, DIREITA, CIMA };
Direcao direcaoAtual = DIREITA;
bool andando = false;
struct Sprite* g_personagem = nullptr;

// ===============================

struct Sprite
{
    GLuint VAO;
    GLuint texID;
    vec3 position;
    vec3 dimensions; //tamanho do frame
    float ds, dt;
    int iAnimation, iFrame;
    int nAnimations, nFrames;
};

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

// Protótipos das funções
int setupShader();
int setupSprite(int nAnimations, int nFrames, float &ds, float &dt);
int loadTexture(string filePath, int &width, int &height);

// Dimensões da janela
const GLuint WIDTH = 800, HEIGHT = 600;

// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar *vertexShaderSource = R"(
 #version 400
 layout (location = 0) in vec3 position;
 layout (location = 1) in vec2 texc;
 out vec2 tex_coord;
 uniform mat4 model;
 uniform mat4 projection;
 void main()
 {
    tex_coord = vec2(texc.s, 1.0 - texc.t);
    gl_Position = projection * model * vec4(position, 1.0);
 }
 )";

// Código fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar *fragmentShaderSource = R"(
 #version 400
 in vec2 tex_coord;
 out vec4 color;
 uniform sampler2D tex_buff;
 uniform vec2 offsetTex;

 void main()
 {
     color = texture(tex_buff,tex_coord + offsetTex);
 }
 )";

// Função MAIN
int main()
{
    // Inicialização da GLFW
    glfwInit();

    // Ativa a suavização de serrilhado (MSAA) com 8 amostras por pixel
    glfwWindowHint(GLFW_SAMPLES, 8);

    // Criação da janela GLFW
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "PGCCHIB -- Wizard Run", nullptr, nullptr);
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

    // Obtendo as informações de versão
    const GLubyte *renderer = glGetString(GL_RENDERER); /* get renderer string */
    const GLubyte *version = glGetString(GL_VERSION);    /* version as a string */
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version supported " << version << endl;

    // Definindo as dimensões da viewport
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Compilando e buildando o programa de shader
    GLuint shaderID = setupShader();

    //Carregando a textura do personagem
    int imgWidth, imgHeight;
    GLuint texID = loadTexture("../assets/sprites/Wizard/Run.png",imgWidth,imgHeight);

    Sprite personagem;
    personagem.nAnimations = 1;
    personagem.nFrames = 8;
    personagem.VAO = setupSprite(personagem.nAnimations,personagem.nFrames,personagem.ds,personagem.dt);
    personagem.position = vec3(400.0, 400.0, 0.0);
    //personagem.dimensions = vec3(imgWidth/personagem.nFrames*2,imgHeight*1,1.0); // Escala de exibição
    personagem.dimensions = vec3(imgWidth/personagem.nFrames,imgHeight,1.0); // Escala de exibição
    personagem.texID = texID;
    personagem.iAnimation = 0;
    personagem.iFrame = 0;

    // background
    Sprite background;
    background.nAnimations = 1;
    background.nFrames = 1;
    background.VAO = setupSprite(background.nAnimations,background.nFrames,background.ds,background.dt);
    background.position = vec3(400.0, 300.0, 0.0);
    //background.texID = loadTexture("../assets/backgrounds/bg_pixelado.png",imgWidth,imgHeight);
    background.texID = loadTexture("../assets/backgrounds/fundoverde2.jpg",imgWidth,imgHeight);
    background.dimensions = vec3(imgWidth/background.nFrames*3,imgHeight/background.nAnimations*3,1.0);
    background.iAnimation = 0;
    background.iFrame = 0;

    // Para acessar personagem na callback
    g_personagem = &personagem;

    glUseProgram(shaderID);

    double prev_s = glfwGetTime();
    double title_countdown_s = 0.1;

    // Ativando o primeiro buffer de textura do OpenGL
    glActiveTexture(GL_TEXTURE0);

    // Uniform para textura
    glUniform1i(glGetUniformLocation(shaderID, "tex_buff"), 0);

    // Matriz de projeção ortográfica
    mat4 projection = ortho(0.0, 800.0, 0.0, 600.0, -1.0, 1.0);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    double lastTime = 0.0;
    double deltaT = 0.0;
    double currTime = glfwGetTime();
    double FPS = 12.0;

    vec2 offsetTexBg = vec2(0.0,0.0);

    // Loop da aplicação
    while (!glfwWindowShouldClose(window))
    {
        // FPS no título
        {
            double curr_s = glfwGetTime();
            double elapsed_s = curr_s - prev_s;
            prev_s = curr_s;

            title_countdown_s -= elapsed_s;
            if (title_countdown_s <= 0.0 && elapsed_s > 0.0)
            {
                double fps = 1.0 / elapsed_s;
                char tmp[256];
                sprintf(tmp, "PGCCHIB -- Wizard Run\tFPS %.2lf", fps);
                glfwSetWindowTitle(window, tmp);
                title_countdown_s = 0.1;
            }
        }

        glfwPollEvents();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glLineWidth(10);
        glPointSize(20);

        // Desenho do background
        mat4 model = mat4(1);
        model = translate(model,background.position);
        model = rotate(model, radians(0.0f), vec3(0.0, 0.0, 1.0));
        model = scale(model,background.dimensions);
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

        currTime = glfwGetTime();
        deltaT = currTime - lastTime;

        if (deltaT >= 1.0/FPS)
        {
            background.iFrame = (background.iFrame + 1) % 100;
        }
        offsetTexBg.s = background.iFrame * 0.01;
        offsetTexBg.t = 0.0;
        glUniform2f(glGetUniformLocation(shaderID, "offsetTex"),offsetTexBg.s, offsetTexBg.t);

        glBindVertexArray(background.VAO);
        glBindTexture(GL_TEXTURE_2D, background.texID);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // =============================================
        // DESENHO E CONTROLE DO PERSONAGEM (SPRITE)
        // =============================================

        // MOVIMENTO: altera posição
        float speed = 2.0f * (float)deltaT; // pixels por segundo

        if (andando) {
            switch (direcaoAtual) {
                case CIMA:
                    personagem.position.y += speed;
                    break;
                case BAIXO:
                    personagem.position.y -= speed;
                    break;
                case ESQUERDA:
                    personagem.position.x -= speed;
                    break;
                case DIREITA:
                    personagem.position.x += speed;
                    break;
            }
        }

        // Atualização de frames animados
        if (andando && deltaT >= 1.0/FPS) {
            personagem.iFrame = (personagem.iFrame + 1) % personagem.nFrames;
            lastTime = currTime;
        } else if (!andando) {
            personagem.iFrame = 0;
            lastTime = currTime;
        }

        // ==============================================
        // FLIP HORIZONTAL: vira para esquerda
        vec3 escala = personagem.dimensions;
        if (direcaoAtual == ESQUERDA) {
            escala.x = -escala.x;
        }
        // ==============================================

        // Model matrix
        model = mat4(1);
        model = translate(model,personagem.position);
        model = rotate(model, radians(0.0f), vec3(0.0, 0.0, 1.0));
        model = scale(model,escala);
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

        vec2 offsetTex;
        offsetTex.s = personagem.iFrame * personagem.ds;
        offsetTex.t = 0; // Só tem uma linha!
        glUniform2f(glGetUniformLocation(shaderID, "offsetTex"),offsetTex.s, offsetTex.t);

        glBindVertexArray(personagem.VAO);
        glBindTexture(GL_TEXTURE_2D, personagem.texID);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // =============================================

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

// ===========================================
// CALLBACK DE TECLADO
// ===========================================
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        andando = true;
        switch (key) {
            case GLFW_KEY_W: case GLFW_KEY_UP:
                direcaoAtual = CIMA;
                break;
            case GLFW_KEY_S: case GLFW_KEY_DOWN:
                direcaoAtual = BAIXO;
                break;
            case GLFW_KEY_A: case GLFW_KEY_LEFT:
                direcaoAtual = ESQUERDA;
                break;
            case GLFW_KEY_D: case GLFW_KEY_RIGHT:
                direcaoAtual = DIREITA;
                break;
            default:
                andando = false;
        }
    }
    if (action == GLFW_RELEASE) {
        andando = false;
    }
}
// ===========================================

int setupShader()
{
    // Vertex shader
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
    // Fragment shader
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
    // Programa de shader
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

int setupSprite(int nAnimations, int nFrames, float &ds, float &dt)
{
    ds = 1.0 / (float) nFrames;
    dt = 1.0 / (float) nAnimations;

    GLfloat vertices[] = {
        // x   y    z    s     t
        -0.5,  0.5, 0.0, 0.0, dt, //V0
        -0.5, -0.5, 0.0, 0.0, 0.0, //V1
         0.5,  0.5, 0.0, ds, dt, //V2
         0.5, -0.5, 0.0, ds, 0.0  //V3
        };

    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

int loadTexture(string filePath, int &width, int &height)
{
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int nrChannels;
    unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        if (nrChannels == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texID;
}

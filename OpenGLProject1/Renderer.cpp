#include "Renderer.h"

Renderer::Renderer() : shaderProgram(0), VAO(0), VBO(0), EBO(0) {}

Renderer::~Renderer()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
}

GLuint Renderer::cargarShader(const char* vertexPath, const char* fragmentPath)
{
    // Shaders embebidos directamente en el código
    const char* vShaderCode = R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec2 aTexCoord;

        out vec2 TexCoord;

        uniform mat4 projection;
        uniform mat4 model;

        void main()
        {
            gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    const char* fShaderCode = R"(
        #version 330 core
        in vec2 TexCoord;
        out vec4 FragColor;

        uniform vec3 color;
        uniform bool useTexture;
        uniform sampler2D textureSampler;

        void main()
        {
            if (useTexture)
            {
                FragColor = texture(textureSampler, TexCoord);
            }
            else
            {
                FragColor = vec4(color, 1.0);
            }
        }
    )";

    std::cout << "[Renderer] Compilando shaders embebidos...\n";

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);

    GLint success;
    char infoLog[512];

    // Vertex shader
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        std::cerr << "[Renderer] Error compilando vertex shader:\n" << infoLog << "\n";
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return 0;
    }

    // Fragment shader
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
        std::cerr << "[Renderer] Error compilando fragment shader:\n" << infoLog << "\n";
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return 0;
    }

    // Programa
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "[Renderer] Error enlazando shader program:\n" << infoLog << "\n";
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteProgram(program);
        return 0;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    std::cout << "[Renderer] Shaders embebidos compilados. Program ID = " << program << "\n";
    return program;
}

void Renderer::inicializar()
{
    std::cout << "[Renderer] Cargando shaders...\n";
    shaderProgram = cargarShader(nullptr, nullptr);
    if (shaderProgram == 0)
    {
        std::cerr << "[Renderer] NO se pudo crear el shaderProgram. Abortando inicialización de Renderer.\n";
        return;
    }

    std::cout << "[Renderer] shaderProgram = " << shaderProgram << "\n";

    // Definir un quad (rectángulo) con coordenadas normalizadas
    float vertices[] = {
        // Posiciones    // Coordenadas de textura
         0.5f,  0.5f,    1.0f, 1.0f,  // arriba derecha
         0.5f, -0.5f,    1.0f, 0.0f,  // abajo derecha
        -0.5f, -0.5f,    0.0f, 0.0f,  // abajo izquierda
        -0.5f,  0.5f,    0.0f, 1.0f   // arriba izquierda
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Posición
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Coordenadas de textura
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Renderer::dibujarRectangulo(float x, float y, float ancho, float alto, glm::vec3 color)
{   
    if (shaderProgram == 0)
        return; // no dibujar si no hay shader

    glUseProgram(shaderProgram);

    // Proyección ortográfica (960x640 como tu ventana)
    glm::mat4 projection = glm::ortho(0.0f, 960.0f, 640.0f, 0.0f, -1.0f, 1.0f);

    // Modelo: posición y escala
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x + ancho / 2.0f, y + alto / 2.0f, 0.0f));
    model = glm::scale(model, glm::vec3(ancho, alto, 1.0f));

    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint colorLoc = glGetUniformLocation(shaderProgram, "color");
    GLint useTexLoc = glGetUniformLocation(shaderProgram, "useTexture");

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(colorLoc, 1, glm::value_ptr(color));
    glUniform1i(useTexLoc, 0); // No usar textura, solo color

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
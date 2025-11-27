// Define esto ANTES de incluir el header para que la librería funcione
#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"

#include "Renderer.h"
#include <iostream>
#include <vector>

Renderer::Renderer()
    : shaderProgram(0), VAO(0), VBO(0), EBO(0)
{
}

Renderer::~Renderer()
{
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
    if (shaderProgram) glDeleteProgram(shaderProgram);
}

// Esta es la función CLAVE que faltaba actualizar
void Renderer::inicializar()
{
    std::cout << "[Renderer] Inicializando...\n";
    shaderProgram = cargarShader("vertex_shader.glsl", "fragment_shader.glsl");

    // === AQUÍ ESTABA EL PROBLEMA ===
    // Antes solo enviabas X, Y. Ahora enviamos X, Y, U, V.
    float quadVertices[] = {
        // Posición (X, Y)   // Textura (U, V)
        -0.5f,  0.5f,        0.0f, 1.0f, // Arriba Izquierda
         0.5f,  0.5f,        1.0f, 1.0f, // Arriba Derecha
         0.5f, -0.5f,        1.0f, 0.0f, // Abajo Derecha
        -0.5f, -0.5f,        0.0f, 0.0f  // Abajo Izquierda
    };

    unsigned int quadIndices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Nota el tamaño: sizeof(quadVertices) ahora es más grande
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    // Atributo 0: Posición (2 floats). Salto total de 4 floats (stride)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Atributo 1: Coordenadas de Textura (2 floats). Empieza en el float número 2
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Habilitar transparencias (para los PNGs)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::cout << "[Renderer] Inicializacion completa.\n";
}

// Función para cargar la imagen desde el archivo
GLuint Renderer::cargarTextura(const char* ruta)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Configuración para Pixel Art (GL_NEAREST para que no se vea borroso)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // Voltear eje Y
    unsigned char* data = stbi_load(ruta, &width, &height, &nrChannels, 0);

    if (data)
    {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "[Renderer] Textura cargada: " << ruta << "\n";
    }
    else
    {
        std::cerr << "[Renderer] Error al cargar textura: " << ruta << "\n";
    }
    stbi_image_free(data);
    return textureID;
}

// Función para dibujar sprites (Personajes y Fondo)
// Función para dibujar sprites (Personajes y Fondo) UPDATEADA
void Renderer::dibujarSprite(float x, float y, float ancho, float alto, GLuint texturaID, glm::vec3 tinte, bool voltearX)
{
    if (!shaderProgram || !VAO) return;
    glUseProgram(shaderProgram);

    // Activar textura
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturaID);

    // Obtener las ubicaciones de las variables uniformes en el shader
    GLint offsetLoc = glGetUniformLocation(shaderProgram, "offset");
    GLint scaleLoc = glGetUniformLocation(shaderProgram, "scale");
    GLint colorLoc = glGetUniformLocation(shaderProgram, "color");
    GLint useTexLoc = glGetUniformLocation(shaderProgram, "useTexture");
    GLint samplerLoc = glGetUniformLocation(shaderProgram, "spriteTexture");

    // === NUEVO: Ubicación del uniform de volteo ===
    GLint flipLoc = glGetUniformLocation(shaderProgram, "flipX");

    // Enviar datos al shader
    glUniform2f(offsetLoc, x, y);
    glUniform2f(scaleLoc, ancho, alto);
    glUniform3f(colorLoc, tinte.r, tinte.g, tinte.b);

    glUniform1i(useTexLoc, 1);  // 1 = ACTIVAR MODO TEXTURA
    glUniform1i(samplerLoc, 0); // Usar unidad de textura 0

    // === NUEVO: Enviar el booleano de volteo ===
    // OpenGL usa enteros (1 o 0) para representar booleanos en glUniform1i
    glUniform1i(flipLoc, voltearX ? 1 : 0);

    // Dibujar el cuadrado
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Función para dibujar rectángulos de colores (Barras de vida)
void Renderer::dibujarRectangulo(float x, float y, float ancho, float alto, glm::vec3 color, int shapeType)
{
    if (!shaderProgram || !VAO) return;

    glUseProgram(shaderProgram);

    // IMPORTANTE: Desactivar modo textura para dibujar formas sólidas
    glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 0);

    glBindVertexArray(VAO);

    GLint offsetLoc = glGetUniformLocation(shaderProgram, "offset");
    GLint scaleLoc = glGetUniformLocation(shaderProgram, "scale");
    GLint colorLoc = glGetUniformLocation(shaderProgram, "color");
    GLint shapeLoc = glGetUniformLocation(shaderProgram, "shapeType");

    glUniform2f(offsetLoc, x, y);
    glUniform2f(scaleLoc, ancho, alto);
    glUniform3f(colorLoc, color.r, color.g, color.b);
    glUniform1i(shapeLoc, shapeType);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// Helpers que redirigen a dibujarRectangulo
void Renderer::dibujarCirculo(float x, float y, float radio, glm::vec3 color) {
    dibujarRectangulo(x, y, radio * 2.0f, radio * 2.0f, color, 1);
}

void Renderer::dibujarElipse(float x, float y, float anchoEje, float altoEje, glm::vec3 color) {
    dibujarRectangulo(x, y, anchoEje * 2.0f, altoEje * 2.0f, color, 2);
}

void Renderer::dibujarRectRedondeado(float x, float y, float ancho, float alto, glm::vec3 color) {
    dibujarRectangulo(x, y, ancho, alto, color, 3);
}

GLuint Renderer::cargarShader(const char* vertexPath, const char* fragmentPath)
{
    auto leerArchivo = [](const char* path) -> std::string {
        std::ifstream file(path, std::ios::in);
        if (!file.is_open()) {
            std::cerr << "No se pudo abrir el archivo de shader: " << path << "\n";
            return "";
        }
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
        };

    std::cout << "[Renderer] Cargando shaders: " << vertexPath << " y " << fragmentPath << "\n";
    std::string vCode = leerArchivo(vertexPath);
    std::string fCode = leerArchivo(fragmentPath);

    if (vCode.empty() || fCode.empty()) return 0;

    const char* vShaderCode = vCode.c_str();
    const char* fShaderCode = fCode.c_str();

    GLuint vertex, fragment;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cerr << "ERROR VERTEX SHADER:\n" << infoLog << "\n";
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cerr << "ERROR FRAGMENT SHADER:\n" << infoLog << "\n";
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}
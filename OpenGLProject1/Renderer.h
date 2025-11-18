#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Renderer
{
public:
    GLuint shaderProgram;
    GLuint VAO, VBO, EBO;

    Renderer();
    ~Renderer();

    void inicializar();
    void dibujarRectangulo(float x, float y, float ancho, float alto, glm::vec3 color);

private:
    GLuint cargarShader(const char* vertexPath, const char* fragmentPath);
    //std::string leerArchivo(const char* path);
};
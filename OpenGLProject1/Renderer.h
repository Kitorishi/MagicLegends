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

    //=====Funciones de conveniencia=======

	// Funcion base de Rectangulo, shapeType: 0=normal, 1=con borde, 2=relleno con borde
    void dibujarRectangulo(float x, float y, float ancho, float alto, glm::vec3 color, int shapeType = 0);
	//Funcion de Circulo 
    void dibujarCirculo(float x, float y, float radio, glm::vec3 color);
    //Funcion de Elipse
    void dibujarElipse(float x, float y, float anchoEje, float altoEje, glm::vec3 color);
	//Funcion de Rectangulo Redondeado
    void dibujarRectRedondeado(float x, float y, float ancho, float alto, glm::vec3 color);
	//Funcion de Cargar Textura desde archivo
    GLuint cargarTextura(const char* ruta);
    void dibujarSprite(float x, float y, float ancho, float alto, GLuint texturaID, glm::vec3 tinte = glm::vec3(1.0f), bool voltearX = false);

private:
    GLuint cargarShader(const char* vertexPath, const char* fragmentPath);
};
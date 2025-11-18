/*
 * Práctica de Graficación - Librería de Funciones Básicas con GLFW/OpenGL
 * Implementación de primitivas gráficas usando OpenGL moderno
 * Incluye: píxeles, líneas, polígonos, círculos, elipses y curvas de Bézier
 */

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Windows.h>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include "Personaje.h"
//#include "Renderer.h"
#include <string>

const int WINDOW_WIDTH = 960;
const int WINDOW_HEIGHT = 640;
const char* WINDOW_TITLE = "Inspiracion Pokemon GBA - Prototype";

Personaje* crearPersonajePorOpcion(int opcion, bool esJugador1);

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

enum class EstadoJuego
{
    SeleccionPJ1,
    SeleccionPJ2,
    TurnoPJ1,
    TurnoPJ2,
    Fin
};

//Shaders embedidos 
const char* simpleVertexShaderSrc = R"(
    #version 330 core
    layout(location = 0) in vec2 aPos;

    uniform vec2 offset;  // posición del rectángulo
    uniform vec2 scale;   // tamaño del rectángulo

    void main()
    {
        vec2 pos = aPos * scale + offset;
        gl_Position = vec4(pos, 0.0, 1.0);
    }
)";

const char* simpleFragmentShaderSrc = R"(
    #version 330 core
    out vec4 FragColor;
    uniform vec3 color;

    void main()
    {
        FragColor = vec4(color, 1.0);
    }
)";

double tiempoUltimaAccion = 0.0;
const double COOLDOWN_ACCION = 0.5; // 0.5 segundos de cooldown entre acciones

int main()
{
    // Inicializar GLFW
    if (!glfwInit())
    {
        std::cerr << "Error: No se pudo inicializar GLFW\n";
        return -1;
    }

	// Configurar version GLFW y perfil de OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Crear ventana GLFW
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Error: No se pudo crear la ventana GLFW\n";
        glfwTerminate();
        return -1;
    }

	// Hacer el contexto de OpenGL actual
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glewExperimental = GL_TRUE;
    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK)
    {
        std::cerr << "Error al inicializar GLEW: "
            << glewGetErrorString(glewStatus) << "\n";
        glfwTerminate();
        return -1;
    }
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << "\n";

    //======SHADER SIMPLE DE PRUEBA =======
    std::cout << "[Main] Compilando shader mínimo...\n";

    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &simpleVertexShaderSrc, nullptr);
    glCompileShader(vShader);

    GLint success;
    char infoLog[512];
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vShader, 512, nullptr, infoLog);
        std::cerr << "[Main] Error compilando vertex shader mínimo:\n" << infoLog << "\n";
    }

    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &simpleFragmentShaderSrc, nullptr);
    glCompileShader(fShader);
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fShader, 512, nullptr, infoLog);
        std::cerr << "[Main] Error compilando fragment shader mínimo:\n" << infoLog << "\n";
    }

    GLuint simpleProgram = glCreateProgram();
    glAttachShader(simpleProgram, vShader);
    glAttachShader(simpleProgram, fShader);
    glLinkProgram(simpleProgram);
    glGetProgramiv(simpleProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(simpleProgram, 512, nullptr, infoLog);
        std::cerr << "[Main] Error enlazando shader mínimo:\n" << infoLog << "\n";
    }

    glDeleteShader(vShader);
    glDeleteShader(fShader);

    // ====== GEOMETRÍA MÍNIMA (triángulo) ======
    // Quad centrado en el origen, de tamaño 1x1 en NDC
    float quadVertices[] = {
        // x,    y
        -0.5f,  0.5f,   // arriba izquierda
         0.5f,  0.5f,   // arriba derecha
         0.5f, -0.5f,   // abajo derecha
        -0.5f, -0.5f    // abajo izquierda
    };

    unsigned int quadIndices[] = {
        0, 1, 2,
        2, 3, 0
    };

    GLuint quadVAO, quadVBO, quadEBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glGenBuffers(1, &quadEBO);

    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    std::cout << "[Main] Shader y quad mínimo preparados.\n";

    //Inicializar el Renderer
    /*std::cout << "Antes de crear Renderer\n";
    Renderer renderer;
    std::cout << "Antes de inicializar Renderer\n";
    renderer.inicializar();
    std::cout << "Renderer inicializado correctamente\n";
    */
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(0.0f, 0.05f, 0.25f, 1.0f);

    // ===== LÓGICA DEL JUEGO =====
    srand(static_cast<unsigned int>(time(nullptr)));

    EstadoJuego estado = EstadoJuego::SeleccionPJ1;
    Personaje* jugador1 = nullptr;
    Personaje* jugador2 = nullptr;

    std::cout << "\nSelecciona PJ1 (1-4):\n";
    std::cout << "1. Mĕilì\n2. Løla\n3. Mephistopheless\n4. Azrael\n";

    // Para evitar que la misma tecla se lea muchas veces en un frame
    bool tecla1Soltada = true, tecla2Soltada = true, tecla3Soltada = true, tecla4Soltada = true;

    auto leerOpcionTeclado = [&](GLFWwindow* w) -> int
        {
            int opcion = 0;

            int k1 = glfwGetKey(w, GLFW_KEY_1);
            int k2 = glfwGetKey(w, GLFW_KEY_2);
            int k3 = glfwGetKey(w, GLFW_KEY_3);
            int k4 = glfwGetKey(w, GLFW_KEY_4);

            if (k1 == GLFW_PRESS && tecla1Soltada) { opcion = 1; tecla1Soltada = false; }
            if (k2 == GLFW_PRESS && tecla2Soltada) { opcion = 2; tecla2Soltada = false; }
            if (k3 == GLFW_PRESS && tecla3Soltada) { opcion = 3; tecla3Soltada = false; }
            if (k4 == GLFW_PRESS && tecla4Soltada) { opcion = 4; tecla4Soltada = false; }

            if (k1 == GLFW_RELEASE) tecla1Soltada = true;
            if (k2 == GLFW_RELEASE) tecla2Soltada = true;
            if (k3 == GLFW_RELEASE) tecla3Soltada = true;
            if (k4 == GLFW_RELEASE) tecla4Soltada = true;

            return opcion;
        };

    while (!glfwWindowShouldClose(window))
    {
        // 1. Procesar eventos de ventana/teclado
        glfwPollEvents();

        // 2. Leer teclas SOLO con GLFW, NUNCA con cin
        int opcion = 0;
		double tiempoActual = glfwGetTime();

        if (tiempoActual - tiempoUltimaAccion > COOLDOWN_ACCION)
        {
            if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) opcion = 1;
            if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) opcion = 2;
            if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) opcion = 3;
            if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) opcion = 4;

            if (opcion != 0)
                tiempoUltimaAccion = tiempoActual;
        }
        // 3. Lógica de juego según el estado
        switch (estado)
        {
        case EstadoJuego::SeleccionPJ1:
            if (opcion >= 1 && opcion <= 4)
            {
                jugador1 = crearPersonajePorOpcion(opcion, true);
                std::cout << "Jugador 1 ha elegido a " << jugador1->obtenerNombre() << "\n";
                estado = EstadoJuego::SeleccionPJ2;
                std::cout << "Jugador 2, elige tu personaje (1-4)\n";
            }
            break;

        case EstadoJuego::SeleccionPJ2:
            if (opcion >= 1 && opcion <= 4)
            {
                jugador2 = crearPersonajePorOpcion(opcion, false);
                std::cout << "Jugador 2 ha elegido a " << jugador2->obtenerNombre() << "\n";
                estado = EstadoJuego::TurnoPJ1;
                std::cout << "Comienza el combate. Turno de " << jugador1->obtenerNombre()
                    << " (Jugador 1). Acciones (1-4)\n";
            }
            break;

        case EstadoJuego::TurnoPJ1:
            if (opcion >= 1 && opcion <= 4)
            {
                jugador1->ejecutarAccion(opcion, jugador2);
                if (!jugador2->estaVivo())
                {
                    std::cout << jugador2->obtenerNombre() << " ha sido derrotado. ¡Jugador 1 gana!\n";
                    estado = EstadoJuego::Fin;
                }
                else
                {
                    estado = EstadoJuego::TurnoPJ2;
                    std::cout << "Turno de " << jugador2->obtenerNombre()
                        << " (Jugador 2). Acciones (1-4)\n";
                }
            }
            break;

        case EstadoJuego::TurnoPJ2:
            if (opcion >= 1 && opcion <= 4)
            {
                jugador2->ejecutarAccion(opcion, jugador1);
                if (!jugador1->estaVivo())
                {
                    std::cout << jugador1->obtenerNombre() << " ha sido derrotado. ¡Jugador 2 gana!\n";
                    estado = EstadoJuego::Fin;
                }
                else
                {
                    estado = EstadoJuego::TurnoPJ1;
                    std::cout << "Turno de " << jugador1->obtenerNombre()
                        << " (Jugador 1). Acciones (1-4)\n";
                }
            }
            break;

        case EstadoJuego::Fin:
            // Si quieres cerrar la ventana al terminar:
            // glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        }

        // 4. Render (lo que ya tienes)
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(simpleProgram);
        glBindVertexArray(quadVAO);

        GLint offsetLoc = glGetUniformLocation(simpleProgram, "offset");
        GLint scaleLoc = glGetUniformLocation(simpleProgram, "scale");
        GLint colorLoc = glGetUniformLocation(simpleProgram, "color");

        // Dibujar solo si los jugadores ya existen
        if (jugador1 != nullptr)
        {
            glUniform3f(colorLoc, 1.0f, 0.2f, 0.2f);
            glUniform2f(offsetLoc, -0.6f, -0.2f);
            glUniform2f(scaleLoc, 0.25f, 0.25f);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        if (jugador2 != nullptr)
        {
            glUniform3f(colorLoc, 0.2f, 0.4f, 1.0f);
            glUniform2f(offsetLoc, 0.6f, 0.2f);
            glUniform2f(scaleLoc, 0.25f, 0.25f);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    delete jugador1;
    delete jugador2;
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

Personaje* crearPersonajePorOpcion(int opcion, bool /*esJugador1*/) {
    switch (opcion) {
    case 1:
        return new Meili("Mĕilì");
    case 2:
        return new Lola("Løla");
    case 3:
        return new Mephistopheless("Mephistopheless");
    case 4:
        return new Azrael("Azrael");
    default:
        std::cout << "Opcion Invalida. No se creo el personaje.\n";
        return nullptr;
    }
}

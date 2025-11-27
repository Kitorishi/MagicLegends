/*
 * Práctica de Graficación - Librería de Funciones Básicas con GLFW/OpenGL
 * Implementación de primitivas gráficas usando OpenGL moderno
 * Incluye: píxeles, líneas, polígonos, círculos, elipses y curvas de Bézier
 */

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <Windows.h>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include "Personaje.h"
#include "Renderer.h"
#include <string>
#include <algorithm>
#include <cmath> 

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

//Variables Globales para el los ids de texturas
GLuint texMeili, texLola, texMephisto, texAzrael;


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

    //Inicializar el Renderer
    Renderer renderer;
    std::cout << "Antes de inicializar Renderer\n";
    renderer.inicializar();
    std::cout << "Renderer inicializado correctamente\n";
    texMeili = renderer.cargarTextura("meili.png");
    texLola = renderer.cargarTextura("lola.png");
    texMephisto = renderer.cargarTextura("mephisto.png");
    texAzrael = renderer.cargarTextura("azrael.png");
    
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

	texMeili = renderer.cargarTextura("meili.png");
	texLola = renderer.cargarTextura("lola.png");
	texMephisto = renderer.cargarTextura("mephisto.png");
	texAzrael = renderer.cargarTextura("azrael.png");

	GLuint texFondo = renderer.cargarTextura("fondoJuego.png");

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
            break;
        }


        //-----------Variables para el estado visual del turno -----------
		bool turnoJugador1 = (estado == EstadoJuego::TurnoPJ1);
		bool turnoJugador2 = (estado == EstadoJuego::TurnoPJ2);
		float yPJ1 = -0.3f; //Altura de barra de vida PJ1
		float yPJ2 = -0.3f; //Altura de barra de vida PJ2
		const float offsetSalto = 0.28f; //Offset de salto visual

        // 4. Render, para el dibujo del ambiente
        glClear(GL_COLOR_BUFFER_BIT);
        renderer.dibujarSprite(0.0f, 0.0f, 2.0f, 2.0f, texFondo, glm::vec3(1.0f));

        /*
        * Dibujo de los personajes y casos especificos 
        * Resaltado de personajes cunado toda el turno 
        * Efecto de Golpear y curacion
        */
        if (jugador1 != nullptr)
        {
            float xPJ1 = -0.6f;
            float baseY = yPJ1;

            //Determinar que textura se va a usar
			GLuint texActual;
            if(jugador1->obtenerNombre() == "Mĕilì") texActual = texMeili;
            else if(jugador1->obtenerNombre() == "Løla") texActual = texLola;
            else if(jugador1->obtenerNombre() == "Mephistopheless") texActual = texMephisto;
			else if (jugador1->obtenerNombre() == "Azrael") texActual = texAzrael;

			glm::vec3 tinte = glm::vec3(1.0f); // Blanco por defecto (color original del sprite)
            if (turnoJugador1) tinte = glm::vec3(1.2f, 1.2f, 1.2f); // Brilla un poco
            if (jugador1->estaGolpeado()) tinte = glm::vec3(1.0f, 0.5f, 0.5f); // Rojo si le pegan
            if (!turnoJugador1) tinte = glm::vec3(0.7f, 0.7f, 0.7f); // Oscuro si no es su turno

            renderer.dibujarSprite(xPJ1, baseY + 0.25f, 0.40f, 0.60f, texActual, tinte);
            
            // Dibujar barra de vida
            float porcentaje = (float)jugador1->obtenerVida() / jugador1->obetenervidaMaxima();
            porcentaje = std::fmax(0.0f, std::fmin(porcentaje, 1.0f));
            renderer.dibujarRectangulo(
                xPJ1, baseY + 0.60f,
                0.25f * porcentaje, 0.03f,
                glm::vec3(0.0f, 1.0f, 0.0f)
            );
        }

        if (jugador2 != nullptr)
        {
            float xPJ2 = 0.6f;
            float baseY = yPJ2;

            //Determinar que textura se va a usar
            GLuint texActual;
            if (jugador2->obtenerNombre() == "Mĕilì") texActual = texMeili;
            else if (jugador2->obtenerNombre() == "Løla") texActual = texLola;
            else if (jugador2->obtenerNombre() == "Mephistopheless") texActual = texMephisto;
            else if (jugador2->obtenerNombre() == "Azrael") texActual = texAzrael;

            glm::vec3 tinte = glm::vec3(1.0f); // Blanco por defecto (color original del sprite)
            if (turnoJugador2) tinte = glm::vec3(1.2f, 1.2f, 1.2f); // Brilla un poco
            if (jugador2->estaGolpeado()) tinte = glm::vec3(1.0f, 0.5f, 0.5f); // Rojo si le pegan
            if (!turnoJugador2) tinte = glm::vec3(0.7f, 0.7f, 0.7f); // Oscuro si no es su turno

            renderer.dibujarSprite(xPJ2, baseY + 0.25f, 0.40f, 0.60f, texActual, tinte, true);

            // Barra de vida
            float porcentaje = (float)jugador2->obtenerVida() / jugador2->obetenervidaMaxima();
            porcentaje = std::fmax(0.0f, std::fmin(porcentaje, 1.0f));
            renderer.dibujarRectangulo(
                xPJ2, baseY + 0.60f,
                0.25f * porcentaje, 0.03f,
                glm::vec3(0.0f, 1.0f, 0.0f)
            );
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

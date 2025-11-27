/*
* Este archivo contiene la implementacion de la clase Personaje
* Las funciones de aplicar efectos de estado, recibir daño, curarse,
* Asi como los diferentes menus de acciones para cada personaje.
*/

#include "Personaje.h"

using namespace std;

//Constructor de la clase Personaje
Personaje::Personaje(string nombre, int vida, int ataque)
    : nombre(nombre), vida(vida), vidaMaxima(vida), ataqueBase(ataque) {
}


//Funcion que verifica si el personaje sigue vivo
bool Personaje::estaVivo() const { return vida > 0; }

//Funcion de recibir daño
void Personaje::recibirDanio(int danio)
{
    if (danio <= 0) return;
    vida -= danio;
    if (vida < 0) vida = 0;
    marcarGolpeado();
}

//Factor de curacion aleatorio entre 20 y 30 puntos de vida
void Personaje::curarse()
{
    int cuantoCura = 20 + rand() % 11;
    vida = std::min(vida + cuantoCura, vidaMaxima);
    cout << nombre << "se cura " << cuantoCura << " puntos de vida. Vida actual: " << vida << "\n";
}

//Funcion que aplica el veneno a un personaje durante un numero determinado de turnos
void Personaje::aplicarVeneno(int turnos)
{
	turnosVeneno = turnos;
	cout << nombre << " ha sido envenenado por " << turnos << " turnos.\n";
}

//Funcion que reduce el ataque de un personaje durante un numero determinado de turnos
void Personaje::reducirAtaque(int turnos)
{
	turnosAtaqueReducido = turnos;
	cout << nombre << " ha tenido su ataque reducido por " << turnos << " turnos.\n";
}


//Funcion que aplica los efectos de estado al personaje
void Personaje::aplicarEfectos()
{
    if (turnosVeneno > 0)
    {
        int danioVeneno = 5;
        vida -= danioVeneno;
        cout << nombre << " sufre " << danioVeneno << " de daño por el veneno. Vida restante: " << std::max(vida, 0) << "\n";
        turnosVeneno--;
    }
    if (turnosAtaqueReducido > 0)
    {
        turnosAtaqueReducido--;
        if (turnosAtaqueReducido == 0)
        {
            cout << nombre << " recupera su fuerza de ataque.\n";
        }
    }
}


/*
* Esta es la parte donde se encuentran los personajes
* especificos y sus menus de accion.
*/

//======= Meili =========
Meili::Meili(string nombre) : Personaje(nombre, 120, 25) {}

void Meili::ejecutarAccion(int opcion, Personaje* enemigo)
{
    int danio = 0;

    switch (opcion)
    {
    case 1:
        danio = ataqueBase + rand() % 11;
        std::cout << nombre << " usa Corte rápido causando " << danio << " de daño.\n";
        enemigo->recibirDanio(danio);
        break;
    case 2:
        danio = ataqueBase + 10 + rand() % 11;
        std::cout << nombre << " usa Estocada potente causando " << danio << " de daño.\n";
        enemigo->recibirDanio(danio);
        break;
    case 3:
        danio = ataqueBase + 5 + rand() % 6;
        std::cout << nombre << " usa Ataque giratorio causando " << danio << " de daño.\n";
        enemigo->recibirDanio(danio);
        break;
    case 4:
        curarse();
        break;
    default:
        std::cout << nombre << " se queda indeciso...\n";
        break;
    }

    aplicarEfectos();
}

// ===== Lola =====

Lola::Lola(string nombre) : Personaje(nombre, 110, 30) {}

void Lola::ejecutarAccion(int opcion, Personaje* enemigo)
{
    int danio = 0;
    switch (opcion)
    {
    case 1:
        danio = ataqueBase + rand() % 11;
        std::cout << nombre << " lanza Bola de fuego causando " << danio << " de daño.\n";
        enemigo->recibirDanio(danio);
        break;
    case 2:
        danio = ataqueBase + 5 + rand() % 6;
        std::cout << nombre << " lanza Congelar causando " << danio << " de daño.\n";
        enemigo->recibirDanio(danio);
        break;
    case 3:
        danio = ataqueBase + 3;
        std::cout << nombre << " lanza Rayo mágico causando " << danio << " de daño.\n";
        enemigo->recibirDanio(danio);
        break;
    case 4:
        curarse();
        break;
    default:
        std::cout << nombre << " se queda indeciso...\n";
        break;
    }
    aplicarEfectos();
}

// ===== Mephistopheless =====

Mephistopheless::Mephistopheless(string nombre) : Personaje(nombre, 115, 30) {}

void Mephistopheless::ejecutarAccion(int opcion, Personaje* enemigo)
{
    int danio = 0;
    switch (opcion)
    {
    case 1:
        danio = ataqueBase + rand() % 6;
        std::cout << nombre << " dispara una Flecha rápida causando " << danio << " de daño.\n";
        enemigo->recibirDanio(danio);
        break;
    case 2:
        danio = ataqueBase / 2 + rand() % 6 + ataqueBase / 2 + rand() % 6;
        std::cout << nombre << " usa Doble disparo causando " << danio << " de daño.\n";
        enemigo->recibirDanio(danio);
        break;
    case 3:
        danio = ataqueBase + 3 + rand() % 8;
        std::cout << nombre << " dispara Flecha envenenada causando " << danio << " de daño.\n";
        enemigo->recibirDanio(danio);
        enemigo->aplicarVeneno(2);
        std::cout << enemigo->obtenerNombre() << " ha sido envenenado por 2 turnos.\n";
        break;
    case 4:
        curarse();
        break;
    default:
        std::cout << nombre << " se queda indeciso...\n";
        break;
    }
    aplicarEfectos();
}

// ===== Azrael =====

Azrael::Azrael(string nombre) : Personaje(nombre, 125, 25) {}

void Azrael::ejecutarAccion(int opcion, Personaje* enemigo)
{
    int danio = 0;
    switch (opcion)
    {
    case 1:
        danio = ataqueBase - 5 + rand() % 6;
        std::cout << nombre << "  lanza Maldicion corrupta causando " << danio << " de daño.\n";
        enemigo->recibirDanio(danio);
        enemigo->aplicarVeneno(2);
        std::cout << enemigo->obtenerNombre() << " ha sido envenenado por 2 turnos.\n";
        break;
    case 2:
        danio = ataqueBase + rand() % 6;
        std::cout << nombre << " usa Toque Sombrio causando " << danio << " de daño.\n";
        enemigo->recibirDanio(danio);
        vida = std::min(vida + danio / 2, vidaMaxima);
        std::cout << nombre << " roba " << danio / 2 << " de vida. Vida Actual: " << vida << "\n";
        break;
    case 3:
        danio = ataqueBase + 5 + rand() % 6;
        std::cout << nombre << " lanza Grito maldito causando " << danio << " de daño.\n";
        enemigo->recibirDanio(danio);
        enemigo->reducirAtaque(2);
        std::cout << enemigo->obtenerNombre() << " tiene su ataque reducido por 2 turnos.\n";
        break;
    case 4:
        curarse();
        break;
    default:
        std::cout << nombre << " se queda indeciso...\n";
        break;
    }
    aplicarEfectos();
}


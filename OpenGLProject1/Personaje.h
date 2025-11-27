/*
*La parte de este codigo define una clase base llamada Personaje 
* y varias clases derivadas que representan personajes 
* especificos, tales como Azrael y Mephistopheless, entre otros.
*/

#pragma once

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <algorithm> 

//Estructura para definir movimientos
struct Movimiento {
    std::string nombre;
    int poder;              //valo base de daño
    bool esCuracion;        //true = cura, false = daño
    float precicion;        // 0.0f - 1.0f probabilidad de acierto
};

//Estructura principal de personaje
class Personaje
{
protected:
    std::string nombre;
    int vida;
    int vidaMaxima;
    int ataqueBase;
    int turnosVeneno = 0;
    int turnosAtaqueReducido = 0;

public:
    Personaje(std::string nombre, int vida, int ataque);
    virtual ~Personaje() {}

    bool estaVivo() const;
    bool estaDerrotado() const { return !estaVivo(); }
    void recibirDanio(int danio);
    void curarse();

    virtual void ejecutarAccion(int opcion, Personaje* enemigo) = 0;

    //Funciones getters
    std::string obtenerNombre() const { return nombre; }
    int obtenerVida() const { return vida; }
    int obetenervidaMaxima() const { return vidaMaxima; }
    
	//Funciones de estado
    void aplicarVeneno(int turnos);
    void reducirAtaque(int turnos);

    void mostrarBarraVida() const;

	//Campos para efectos visuales de golpeado y curado
    int framesGolpeado = 0;
    int framesCurado = 0;

    void marcarGolpeado() { framesGolpeado = 5; }
    void marcarCurado() { framesCurado = 5; }
    bool estaGolpeado() const { return framesGolpeado > 0; }
    bool estaCurado()   const { return framesCurado > 0; }
    void actualizarVisual() { if (framesGolpeado > 0) --framesGolpeado; if (framesCurado > 0) --framesCurado; }

protected:
    void aplicarEfectos();
};

// ===== Clases derivadas de Personajes =====
class Meili : public Personaje
{
public:
    Meili(std::string nombre);
    void ejecutarAccion(int opcion, Personaje* enemigo) override;
};

class Lola : public Personaje
{
public:
    Lola(std::string nombre);
    void ejecutarAccion(int opcion, Personaje* enemigo) override;
};

class Mephistopheless : public Personaje
{
public:
    Mephistopheless(std::string nombre);
    void ejecutarAccion(int opcion, Personaje* enemigo) override;
};

class Azrael : public Personaje
{
public:
    Azrael(std::string nombre);
    void ejecutarAccion(int opcion, Personaje* enemigo) override;
};



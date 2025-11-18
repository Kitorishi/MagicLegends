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

    std::string obtenerNombre() const { return nombre; }
    int obtenerVida() const { return vida; }

    void aplicarVeneno(int turnos);
    void reducirAtaque(int turnos);

    void mostrarBarraVida() const;

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

// Función auxiliar para elegir personaje
//int seleccionarPersonaje(int jugador);
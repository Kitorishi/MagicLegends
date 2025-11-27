# MagicLegends

MagicLegends es un videojuego 2D desarrollado en **C++** y **OpenGL**, diseñado para replicar la estética y sensaciones de los juegos clásicos de **Game Boy Advance**. Aunque toma inspiración del estilo de combate PVP de Pokémon, los personajes, mecánicas y assets son completamente originales.

---

## 🎯 Objetivo del Proyecto

Construir un juego 2D con:
- Gráficos tipo GBA (pixel-art, spritesets, paletas limitadas).
- Sistema de batalla **PVP por turnos**.
- Cuatro personajes propios con habilidades y estadísticas distintas.
- Motor ligero basado en OpenGL para controlar renderizado, animaciones y lógica del juego.

---

## 🧰 Requisitos de Pre-instalación

> El repositorio contiene una solución `OpenGLProject1.sln` y una carpeta `External/glm`, lo cual indica que el proyecto está preparado para compilarse en **Visual Studio** sobre Windows.

### Sistema
- Windows 10/11  
- Visual Studio 2017/2019/2022  
  (Cargar la solución con la carga de trabajo **Desktop development with C++**)

### Dependencias principales
- **OpenGL** (2.1+ recomendado 3.3+)  
- **GLM** (incluido en `External/glm/`)  
- **Librería de creación de ventana**  
  (GLFW / GLEW / freeglut — según la configuración del proyecto)
- **Librería de carga de texturas**  
  (stb_image o similar si se usa)

### Drivers
- Tarjeta gráfica con soporte OpenGL actualizado.

---

## ▶️ Cómo compilar y ejecutar

1. Clona el repositorio:
   ```bash
   git clone https://github.com/Kitorishi/MagicLegends.git

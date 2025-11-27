#version 330 core

// Atributos de entrada
layout (location = 0) in vec2 aPos;      // Posición del vértice (x,y)
layout (location = 1) in vec2 aTexCoord; // Coordenada de textura original (u,v)

// Salidas al Fragment Shader
out vec2 vLocalPos;
out vec2 vTexCoord; 

// Variables uniformes (configuración desde C++)
uniform vec2 offset;
uniform vec2 scale;
//  Interruptor para voltear horizontalmente ===
uniform bool flipX; 

void main()
{
    // Posición local para cálculos de formas geométricas
    vLocalPos = aPos + vec2(0.5, 0.5);
    
    // === LÓGICA DE VOLTEO ===
    // Si flipX es verdadero, invertimos la coordenada X de la textura.
    // La coordenada de textura va de 0.0 (izquierda) a 1.0 (derecha).
    // Al hacer (1.0 - x), el lado izquierdo se vuelve derecho y viceversa.
    if (flipX) {
        vTexCoord = vec2(1.0 - aTexCoord.x, aTexCoord.y);
    } else {
        // Si no, usamos la coordenada original
        vTexCoord = aTexCoord;
    }
    // ========================

    // Transformación de posición al mundo
    vec2 worldPos = aPos * scale + offset;
    gl_Position = vec4(worldPos, 0.0, 1.0);
}

#version 330 core

// Entradas que vienen del Vertex Shader
in vec2 vLocalPos; // Posición local [0,1] para calcular formas geométricas
in vec2 vTexCoord; // Coordenadas UV para mapear la textura (imagen)

out vec4 FragColor;

// Variables Uniformes (se envían desde C++)
uniform vec3 color;              // Color base o tinte
uniform int shapeType;           // 0=rect, 1=circle, 2=ellipse, 3=roundedRect
uniform sampler2D spriteTexture; // La imagen del personaje (sprite)
uniform int useTexture;          // 1 = Dibujar sprite, 0 = Dibujar forma geométrica

void main()
{
    // ==========================================
    // MODO 1: DIBUJAR SPRITE (TEXTURA)
    // ==========================================
    if (useTexture == 1) {
        // Obtener el color del pixel de la imagen en la coordenada UV actual
        vec4 texColor = texture(spriteTexture, vTexCoord);
        
        // Descartar píxeles transparentes (Alpha Testing)
        // Si el pixel es muy transparente, no lo dibujamos
        if(texColor.a < 0.1)
            discard;
        
        // Multiplicamos por 'color' para aplicar un tinte si es necesario.
        // Si 'color' es blanco (1.0, 1.0, 1.0), se ve la imagen original.
        // Esto es útil para efectos de daño (tinte rojo) o turnos.
        FragColor = texColor * vec4(color, 1.0);
    }
    // ==========================================
    // MODO 2: DIBUJAR FORMAS GEOMÉTRICAS (Tu código original)
    // ==========================================
    else {
        vec2 p = vLocalPos; // coordenadas [0,1] dentro del quad

        if (shapeType == 1) {
            // ===== CÍRCULO =====
            vec2 center = vec2(0.5, 0.5);
            float radius = 0.5;
            float dist = length(p - center);
            
            // Borde suave (antialiasing)
            float alpha = 1.0 - smoothstep(radius - 0.01, radius, dist);
            if (alpha < 0.01) discard;
            
            FragColor = vec4(color, alpha);
            
        } else if (shapeType == 2) {
            // ===== ELIPSE =====
            vec2 center = vec2(0.5, 0.5);
            vec2 radii = vec2(0.5, 0.5);
            vec2 q = (p - center) / radii;
            float v = dot(q, q);
            
            float alpha = 1.0 - smoothstep(0.98, 1.0, v);
            if (alpha < 0.01) discard;
            
            FragColor = vec4(color, alpha);
            
        } else if (shapeType == 3) {
            // ===== RECTÁNGULO CON ESQUINAS REDONDEADAS =====
            float cornerRadius = 0.15; // radio de las esquinas
            
            vec2 center = p - vec2(0.5, 0.5);
            vec2 halfSize = vec2(0.5, 0.5) - vec2(cornerRadius);
            
            vec2 d = abs(center) - halfSize;
            float dist = length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
            
            float alpha = 1.0 - smoothstep(cornerRadius - 0.01, cornerRadius, dist);
            if (alpha < 0.01) discard;
            
            FragColor = vec4(color, alpha);

        } else {
            // ===== RECTÁNGULO NORMAL (Por defecto) =====
            FragColor = vec4(color, 1.0);
        }
    }
}
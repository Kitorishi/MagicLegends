#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform vec3 color;
uniform bool useTexture;
uniform sampler2D textureSampler;

void main()
{
    if (useTexture)
    {
        FragColor = texture(textureSampler, TexCoord);
    }
    else
    {
        FragColor = vec4(color, 1.0);
    }
}
#version 330 core

in float vHeight;   // already in [-1, 1]
out vec4 FragColor;

void main()
{
    // normalize from [-1,1] to [0,1] and clamp to avoid NaNs/overflows
    float h = (vHeight + 1.0) * 0.5;

    // treat very-low values as deep blue; avoid exact float equality
    if (h <= 0.0f)
    {
        FragColor = vec4(0.0, 0.0, 1.0, 1.0); // opaque blue
    }
    else
    {
        vec3 color = mix(vec3(0.4, 0.1, 0.3), vec3(1.0, 1.0, 1.0), h);
        FragColor = vec4(color, 1.0); // opaque
    }
}
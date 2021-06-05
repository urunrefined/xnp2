#version 450 core

vec2 positions[6] = vec2[](
    vec2(-0.5, 0.5),  // lower left
    vec2(-0.5,-0.5),  // upper left
    vec2( 0.5,-0.5),  // upper right

    vec2(-0.5, 0.5),  // lower left
    vec2( 0.5,-0.5),  // upper right
    vec2( 0.5, 0.5)   // lower right
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}

#version 450 core

vec2 pos[6] = vec2[](
    vec2(-1.0, 1.0),  // lower left
    vec2(-1.0,-1.0),  // upper left
    vec2( 1.0,-1.0),  // upper right

    vec2(-1.0, 1.0),  // lower left
    vec2( 1.0,-1.0),  // upper right
    vec2( 1.0, 1.0)   // lower right
);

vec2 uv[6] = vec2[](
    vec2( 0.0, 1.0),
    vec2( 0.0, 0.0),
    vec2( 1.0, 0.0),

    vec2( 0.0, 1.0),
    vec2( 1.0, 0.0),
    vec2( 1.0, 1.0)
);

layout(location = 0) out vec2 fragTexCoord;

void main() {
    gl_Position = vec4(pos[gl_VertexIndex], 0.0, 1.0);
    fragTexCoord = vec2(uv[gl_VertexIndex]);
}

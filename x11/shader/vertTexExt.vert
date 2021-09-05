#version 450 core

layout(location = 0) in vec3 vertices;
layout(location = 1) in vec2 uv;

layout(binding = 0) uniform World {
        uniform mat4 world;
};

layout(binding = 1) uniform Model {
        uniform mat4 model;
};

layout(location = 0) out vec2 fragTexCoord;

void main() {
    gl_Position = vec4(vertices, 1) * model * world;
    fragTexCoord = uv;
}

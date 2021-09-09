#version 450 core

layout(location = 0) in vec2 vertices;
layout(location = 1) in vec2 uvs;

layout(binding = 0) uniform World {
        uniform mat4 world;
};

layout(binding = 1) uniform Model {
        uniform mat4 model;
};

layout(location = 0) out vec2 fragTexCoord;

void main() {
    //gl_Position = vec4(vertices, 0.0, 1.0) * model * world;
    gl_Position = vec4(vertices, 0.0, 1.0) * model * world;
    fragTexCoord = vec2(uvs);
}

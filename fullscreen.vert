#version 450

layout(push_constant) uniform PushConsts {
    float scale;
    vec2  translate;
} pc;

layout(location = 0) out vec2 uv;

void main() {
    vec2 pos = vec2((gl_VertexIndex == 1 || gl_VertexIndex == 2) ? 1.0 : -1.0,
                    (gl_VertexIndex == 2 || gl_VertexIndex == 3) ? 1.0 : -1.0);
    vec2 scaled = pos * pc.scale + pc.translate;
    gl_Position = vec4(scaled, 0.0, 1.0);
    uv = pos * 0.5 + 0.5;
}
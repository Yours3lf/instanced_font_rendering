#version 430

layout(location=0) uniform mat4 mvp;

layout(location=0) in vec2 in_vertex;
layout(location=1) in vec2 in_texture;
layout(location=2) in vec4 instance_vertscalebias;
layout(location=3) in vec4 instance_texscalebias;

out vec2 tex_coord;
flat out vec4 texscalebias;

void main()
{
  tex_coord = in_texture.xy;
  texscalebias = instance_texscalebias;
  gl_Position = mvp * vec4(in_vertex.xy * instance_vertscalebias.xy + instance_vertscalebias.zw, 0, 1);
}
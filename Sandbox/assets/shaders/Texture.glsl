#type vertex

#version 330 core
layout(location = 0) in vec3 A_POSITION;
layout(location = 1) in vec2 A_TEXCOORD;

uniform mat4 c_viewProjection;
uniform mat4 c_modelTransform;

out vec2 v_texCoord;

void main()
{
	v_texCoord = A_TEXCOORD;
	gl_Position = c_viewProjection * c_modelTransform * vec4(A_POSITION, 1.0);
}



#type pixel

#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_texCoord;

uniform sampler2D c_texture;

void main()
{
	color = texture(c_texture, v_texCoord);
}
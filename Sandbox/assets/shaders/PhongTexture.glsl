#type vertex

#version 330 core

layout(location = 0) in vec3 A_POSITION;
layout(location = 1) in vec2 A_TEXCOORD;
layout(location = 2) in vec3 A_NORMAL;

uniform mat4 c_viewProjection;
uniform mat4 c_modelTransform;

out vec2 v_texCoord;
out vec3 v_normal;

void main()
{
	v_texCoord = A_TEXCOORD;
	v_normal = mat3(c_modelTransform) * A_NORMAL;
	gl_Position = c_viewProjection * c_modelTransform * vec4(A_POSITION, 1.0f);
}



#type pixel

#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_texCoord;
in vec3 v_normal;

uniform sampler2D c_texture;

void main()
{
	vec3 n = normalize(v_normal);
	float brightness = dot(n, vec3(0.5f, 0.5f, 1.0f));
	brightness = max(brightness, 0.1f);
	color = texture(c_texture, v_texCoord);
	color.xyz *= brightness;
}
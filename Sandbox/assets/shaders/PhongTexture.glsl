#type vertex

#version 330 core

uniform mat4 c_viewProjection;
uniform mat4 c_modelTransform;

layout(location = 0) in vec3 A_POSITION;
layout(location = 1) in vec2 A_TEXCOORD;
layout(location = 2) in vec3 A_NORMAL;

out vec3 v_worldPos;
out vec2 v_texCoord;
out vec3 v_normal;

void main()
{
	vec4 temp = c_modelTransform * vec4(A_POSITION, 1.0f);

	v_worldPos = temp.xyz;
	gl_Position = c_viewProjection * temp;
	v_texCoord = A_TEXCOORD;
	v_normal = mat3(c_modelTransform) * A_NORMAL;

}



#type pixel

#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_worldPos;
in vec2 v_texCoord;
in vec3 v_normal;

uniform vec3 c_ambientColor;
uniform float c_diffuseIntensity;
uniform vec3 c_lightPos;
uniform float c_attConst;
uniform vec3 c_diffuseColor;
uniform float c_attLin;
uniform float c_attQuad;
uniform float c_specularIntensity;
uniform float c_specularPower;

uniform sampler2D c_texture;

void main()
{
	vec3 vToL = c_lightPos - v_worldPos;
	float distToL = length(vToL);
	vec3 dirToL = vToL / distToL;

	float att = 1.0f / (c_attConst + c_attLin * distToL + c_attQuad * (distToL * distToL));

	vec3 diffuse = c_diffuseColor * c_diffuseIntensity * att * max(0.0f, dot(dirToL, v_normal));

	vec3 w = v_normal * dot(vToL, v_normal);
	vec3 r = w * 2.0f - vToL;

	vec3 specular = att * (c_diffuseColor * c_diffuseIntensity) * c_specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(v_worldPos))), c_specularPower);

	vec4 materialColor = texture(c_texture, v_texCoord);

	vec4 lighting = vec4(diffuse + c_ambientColor + specular, 1.0f);

	color = clamp(lighting * materialColor, 0.0f, 1.0f);

	//color = lighting;
}
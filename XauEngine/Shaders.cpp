//DepthVertexShader
const char* dvs = {
	"#version 330 core\n\
	\
	layout(location = 0) in vec3 vertexPosition_modelspace;\
	uniform mat4 depthMVP;\
	\
	void main(){\
		gl_Position = depthMVP * vec4(vertexPosition_modelspace, 1);\
	}"
};

//DepthFragmentShader
const char* dfs = {
	"#version 330 core\n\
	\
	layout(location = 0) out float fragmentdepth;\
	\
	void main(){\
		fragmentdepth = gl_FragCoord.z;\
	}"
};

//SimpleVertexShader
const char* svs = {
	"#version 330 core\n\
	#extension GL_ARB_explicit_uniform_location: enable\n\
	\
	layout(location = 0) in vec3 vertexPosition_modelspace;\
	layout(location = 1) in vec2 vertexUV;\
	layout(location = 0) uniform mat4 MVP;\
	out vec2 UV;\
	\
	void main(){\
		gl_Position = MVP * vec4(vertexPosition_modelspace, 1);\
		UV = vertexUV;\
	}"
};


//SimpleFragmentShader
const char* sfs = {
	"#version 330 core\n\
	#extension GL_ARB_explicit_uniform_location: enable\n\
	\
	layout(location = 0) out vec4 color;\
	\
	layout(location = 1) uniform sampler2D myTextureSampler;\
	\
	in vec2 UV;\
	\
	void main(){\
		color = vec4(texture2D(myTextureSampler, UV).rgb, 1);\
	}"
};

//VertexShader
const char* vs = {
	"#version 330 core\n\
	#extension GL_ARB_explicit_uniform_location: enable\n\
	in vec3 vertexPosition_modelspace;\
	in vec3 vertexNormal_modelspace;\
	in vec2 vertexUV;\
	\
	uniform mat4 M;\
	uniform mat4 VP;\
	uniform mat4 DepthBiasMVP[5];\
	\
	out vec2 UV;\
	out vec3 normal;\
	out vec4 ShadowCoord[5];\
	out float zsize;\
	out mat4 inoutM;\
	\
	void main(){\
		for(int i = 0; i < DepthBiasMVP.length(); ++i){\
			ShadowCoord[i] = DepthBiasMVP[i] * vec4(vertexPosition_modelspace, 1);\
		}\
		inoutM = M;\
		normal = vertexNormal_modelspace;\
		UV = vertexUV;\
		gl_Position = VP * M * vec4(vertexPosition_modelspace, 1); \
		zsize = gl_Position.z; \
	}"
};

//FragmentShader
const char* fs = {
	"#version 330 core\n\
	#extension GL_ARB_explicit_uniform_location: enable\n\
	\
	uniform vec3 lightDir[5];\
	uniform sampler2D myTextureSampler;\
	uniform sampler2DShadow shadowMap[5];\
	\
	in vec2 UV;\
	in vec3 normal;\
	in vec4 ShadowCoord[5];\
	in float zsize;\
	in mat4 inoutM;\
	\
	layout(location = 0) out vec4 color;\
	\
	vec2 poissonDisk[16] = vec2[](\
		vec2(-0.94201624, -0.39906216),\
		vec2(0.94558609, -0.76890725),\
		vec2(-0.094184101, -0.92938870),\
		vec2(0.34495938, 0.29387760),\
		vec2(-0.91588581, 0.45771432),\
		vec2(-0.81544232, -0.87912464),\
		vec2(-0.38277543, 0.27676845),\
		vec2(0.97484398, 0.75648379),\
		vec2(0.44323325, -0.97511554),\
		vec2(0.53742981, -0.47373420),\
		vec2(-0.26496911, -0.41893023),\
		vec2(0.79197514, 0.19090188),\
		vec2(-0.24188840, 0.99706507),\
		vec2(-0.81409955, 0.91437590),\
		vec2(0.19984126, 0.78641367),\
		vec2(0.14383161, -0.14100790)\
		);\
	void main(){\
		float visibility = 1.0;\
		float totalDiffuse = 0;\
		for(int j = 0; j < ShadowCoord.length(); ++j){\
			float bias = 0.005*tan(acos(dot( normal,lightDir[j] )));\
			if(ShadowCoord[j].w != 0){\
				totalDiffuse += max(dot((vec4(lightDir[j],1) * inoutM).xyz , normal), 0);\
				for (int i = 0; i<4; i++){\
					visibility -= 0.2*(1.0 - texture(shadowMap[j], vec3(ShadowCoord[j].xy + poissonDisk[i] / 700.0, (ShadowCoord[j].z + clamp(bias, 0,0.01)) / ShadowCoord[j].w)));\
				}\
			}\
			else\
				break;\
		}\
		color = vec4(visibility * texture2D(myTextureSampler, UV).rgb * min(totalDiffuse+0.1, 1), 1);\
	}"
};



/*char buf[32];
_itoa_s(glGetUniformLocation(program, "shadowMap"), buf, 10);
MessageBoxA(0, buf, 0, 0);*/

/*===INDECIES OF SHADER VARIABLES===
vertexPosition_modelspace	0
vertexNormal_modelspace		1
vertexUV					2

MVP							5
DepthBiasMVP[]				0-4
lightDir					6-10
myTextureSampler			11
shadowMap					12-14*/

const char* vs2 = {
	"#version 330 core\n\
	uniform mat4 M;\
	uniform mat4 VP;\
	uniform mat4 DepthMVP;\
	\
	layout(location = 0) in vec3 vertex;\
	layout(location = 1) in vec3 normal;\
	layout(location = 2) in vec2 UV;\
	\
	out vec2 oUV;\
	out vec3 oNormal;\
	out vec3 fragVert;\
	out vec4 shadowCoord;\
	\
	void main(){\
		gl_Position = VP * M * vec4(vertex, 1);\
		oUV = UV;\
		oNormal = normal;\
		fragVert = vertex;\
		shadowCoord = DepthMVP * vec4(vertex, 1);\
	}"
};
const char* fs2 = {
	"#version 330 core\n\
	uniform mat4 M;\
	uniform sampler2D textureSampler;\
	uniform sampler2D shadowMap;\
	uniform vec3 ldir;\
	uniform vec3 camPos;\
	\
	in vec2 oUV;\
	in vec3 oNormal;\
	in vec3 fragVert;\
	in vec4 shadowCoord;\
	\
	out vec3 color;\
	\
	vec2 poissonDisk[4] = vec2[](\
	  vec2( -0.94201624, -0.39906216 ),\
	  vec2( 0.94558609, -0.76890725 ),\
	  vec2( -0.094184101, -0.92938870 ),\
	  vec2( 0.34495938, 0.29387760 )\
	);\
	void main(){\
		vec3 surfacePos = vec3(M * vec4(fragVert, 1));\
		vec3 surfaceToLight = normalize(ldir - surfacePos);\
		float diffuse = max(0, dot(ldir, oNormal));\
		float specular = 0;\
		if(diffuse > 0.0)\
			specular = pow(max(0.0, dot(normalize(camPos - surfacePos),\
				reflect(-surfaceToLight, oNormal))), 2);\
		float v = 1.0;\
		for (int i=0; i < 4;i++){\
			if(texture2D(shadowMap, shadowCoord.xy + poissonDisk[i] / 700.0).z < shadowCoord.z - 0.005 )\
				v -= 0.22;\
		}\
		color = texture2D(textureSampler, oUV).rgb * max(0.15, v * (diffuse + specular)); \
	}"
};
const char* dvs2 = {
	"#version 330 core\n\
	layout(location = 0) in vec3 vertex;\
	uniform mat4 MVP;\
	void main(){\
		gl_Position = MVP * vec4(vertex, 1);\
	}"
};
const char* dfs2 = {
	"#version 330 core\n\
	layout(location = 0) out float fragmentdepth;\
	void main(){\
		fragmentdepth = gl_FragCoord.z;\
	}"
};

/*M				1
VP				2
DepthMVP		0
textureSampler	6
shadowMap		5
ldir			4
camdir			3*/
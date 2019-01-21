#version 330 core 

in  vec2 vTexCoord;
in  vec3 vPosition;
in  vec3 vNormal;
out vec3 N;
out vec3 V;
out vec4 color;
out vec3 flightPos;
out vec2 VTPos;

uniform vec3 lightPos;	// 光源位置
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform vec4 draw_color;
uniform int is_shadow;		// 标记是否为阴影投影矩阵


void main() {
	flightPos = vec3(ViewMatrix * vec4(lightPos, 1.0));
	VTPos = vTexCoord;
	
	
	if(is_shadow == 0) {
		mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
		mat4 ModelViewProjMatrix =  ProjectionMatrix * ModelViewMatrix;
		
		gl_Position = ModelViewProjMatrix * vec4(vPosition, 1.0);

		// 将顶点变换到相机坐标系下
		vec4 vertPos_cameraspace = ModelViewMatrix * vec4(vPosition, 1.0);
		
		// 对顶点坐标做透视投影
		V = vertPos_cameraspace.xyz / vertPos_cameraspace.w;
		
		// 将法向量变换到相机坐标系下并传入片元着色器
		N = (ModelViewMatrix * vec4(vNormal, 0.0)).xyz;
	} else {
		mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
		
		vec4 v1 = ModelMatrix * vec4(vPosition, 1.0);  
		// 由于modelMatrix有可能为阴影矩阵，为了得到正确位置，我们需要做一次透视除法
		vec4 v2 = vec4(v1.xyz / v1.w, 1.0);
		// 考虑相机和投影
		vec4 v3 = ProjectionMatrix * ViewMatrix * v2;
		// 再做一次透视除法 (为了明确概念，我们显式做一次透视除法)
		gl_Position = v3;

		// 将顶点变换到相机坐标系下
		vec4 vertPos_cameraspace = ModelViewMatrix * vec4(vPosition, 1.0);
		
		// 对顶点坐标做透视投影
		V = vertPos_cameraspace.xyz / vertPos_cameraspace.w;
		
		// 将法向量变换到相机坐标系下并传入片元着色器
		N = (ModelViewMatrix * vec4(vNormal, 0.0)).xyz;
	}
	
	color = draw_color;

}

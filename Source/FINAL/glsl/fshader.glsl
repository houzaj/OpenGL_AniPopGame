#version 330 core 

in vec3 N;
in vec3 V;
in vec4 color;
in vec3 flightPos;		// 相机坐标系下的光源位置
in vec2 VTPos;

uniform float scale;	// 光照强度系数
uniform int is_lighted;
uniform int is_textured;
uniform sampler2D texture;

out vec4 fColor;


void main() {	
	if(is_lighted == 0 && is_textured == 0){
		fColor = color;
		return;
	} else if(is_lighted == 0 && is_textured == 1){
		fColor = texture2D(texture, VTPos);
		return;
	}
	
	// 设置三维物体的材质属性
	vec3 ambiColor = vec3(0.3, 0.3, 0.3);
	vec3 diffColor = vec3(0.5, 0.5, 0.5);
	vec3 specColor = vec3(0.3, 0.3, 0.3);

	// 计算N，L，V，R四个向量并归一化
	vec3 N_norm = normalize(N);
	vec3 L_norm = normalize(flightPos - V);
	vec3 V_norm = normalize(-V); 	
	vec3 R_norm = reflect(-L_norm, N_norm);

	// 计算漫反射系数和镜面反射系数
	float lambertian = clamp(dot(L_norm, N_norm), 0.0, 1.0);
	float specular = clamp(dot(R_norm, V_norm), 0.0, 1.0);
		
	// 高光系数
	float shininess = 10.0;
	
	// 最终输出的颜色
	vec4 fragmentColor  = scale * vec4(ambiColor + diffColor * lambertian + specColor * pow(specular, shininess), 1.0);
	
	if(is_textured == 0){
		fColor = fragmentColor * color;
	}else{
		fColor = fragmentColor * color * texture2D(texture, VTPos);
		// fColor = texture2D(texture, VTPos);
	}
    
} 
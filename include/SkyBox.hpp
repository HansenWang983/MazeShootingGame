#ifndef _SKYBOX_H_
#define _SKYBOX_H_
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "shader_m.h"
#include "camera.h"


#include <iostream>
using namespace std;

class SkyBox {
public:
	SkyBox();
	void render(Camera& camera);

private:
	// Shader
	Shader skyboxShader;
	Shader cubemapShader;

	// VAO VBO
	unsigned int cubeVAO, cubeVBO;
	unsigned int skyboxVAO, skyboxVBO;
	unsigned int cubeTexture;
	unsigned int cubemapTexture;

	// 加载函数
	unsigned int loadTexture(string path);
	unsigned int loadCubemap(vector<std::string> faces);

	int SCR_WIDTH = 800;
	int SCR_HEIGHT = 600;


};

#endif
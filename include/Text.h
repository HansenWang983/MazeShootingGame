#ifndef _TEXT_H_
#define _TEXT_H_
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H


#include "shader_m.h"
#include "camera.h"


#include <iostream>
#include <map>
using namespace std;

struct Character {
	GLuint TextureID;  // 字形纹理ID
	glm::ivec2 Size;   // 字形大小
	glm::ivec2 Bearing; // 字形基于基线和起点的位置
	GLuint Advance; // 起点到下一字形起点的距离
};

class Text {
public:
	Text();
	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

private:
	std::map<GLchar, Character> Characters;
	GLuint VAO, VBO;
	Shader textShader;
	const unsigned int SCR_WIDTH = 800;
	const unsigned int SCR_HEIGHT = 600;

};

#endif
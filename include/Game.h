﻿#pragma once
#include<memory>
#include "GameObject.h"
#include "Shader.h"
#include<GLFW/glfw3.h>
#include"EulerCamera.h"
#include <glm/glm.hpp>
#include <map>
#include <string>
#include "Skybox.h"
#include "PointLight.h"
#include "Ground.h"
#include "md2model.h"
#include "Text.h"
#include "Snow.h"
#include "Fire.h"
#include "FireShader.h"

///////text
#include <ft2build.h>
#include FT_FREETYPE_H

namespace GXProject{
	
	class Game{
	private:
		struct Character {
			GLuint     TextureID;  // 字形纹理ID
			glm::ivec2 Size;       // 字形大大小
			glm::ivec2 Bearing;    // 字形基于基线和起点的位置
			GLuint     Advance;    // 起点到下一个字形起点的距离
		};

		enum Direction{SOUTH,NORTH,EAST,WEST};
		std::map<std::string, GameObjectPtr> scene;
		ShaderPtr _genericShader;
		//Text myText;
		Skybox sky;
		Ground* _ground;
		Ground* _walls;
		Snow::Snow snow;
		Cube finalCube;

		Cube testCube;

    	unsigned int _texture;
    	unsigned int _VBO, _VAO;
		ShaderPtr _centerShader;

		unsigned int _texture_text;
		unsigned int _VBO_text, _VAO_text;
		ShaderPtr _textShader;

		// fire
		ShaderPtr _fireShader;

		//
		CMD2Model mymodel;
		animState_t model_anim_state;
		KeyFrameAnimationShader animatedModelShader;
		
		

		glm::mat4 model_m;
		glm::vec3 monsterPosition;
		Direction monsterDirection;
		glm::vec3 monsterPosTarget;
		glm::vec3 monsterRotate;
		//


		PointLight playerLight, Monsterlight, FireLight;
		glm::vec2 mousePosition;
		bool _turnOnMouseCamera;
		bool _debugMode;



		// fire
		

	public:
		EulerCamera camera;
		Game();
		~Game();

		void MonsterAI();
		void setupGround(int x, int y,ShaderPtr shdr,MeshPtr msh);
		void setupShaderUniforms(ShaderPtr shdr);
		void setupCenter();
		void update(double delta);
		void render();
		void cameraMouseMovement(int x, int y);
		// get model matrix
		glm::mat4 getModel(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
		//for handling the keyboard events
		void handleKeyboard(GLFWwindow* window,int key, int scancode,int action,int mods);
		//for handling the mouse buttons events
		void handleMouseButtons(GLFWwindow* window,int button,int action,int mods);
		//for handling mouse motion
		void handleMouseMotion(GLFWwindow* window,int x, int y);


		////文字部分
		
		std::map<GLchar, Character> Characters;
		void setupText();
		void RenderText(ShaderPtr s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

	};
	typedef std::shared_ptr<Game> GamePtr;
}
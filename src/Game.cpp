﻿#include <stb_image.h>
#include"Game.h"
#include "Geometry.h"
#include "Application.h"
#include "Text.h"
#include <glm/glm.hpp>
#include <iostream>
#include "Helper.h"
using namespace GXProject;
using namespace std;
using namespace glm;

const int nx = 256;
const int ny = 256;

float dt = 0.02f;
float iterations = 5;
float vorticity = 10.0f;

glm::vec3 position_, rotation_, scale_;

Fire myFire(nx, ny, dt, iterations, vorticity);


MeshPtr _mesh;
Game::Game(){
	auto windowpos = Application::get()->getWindowSize();
	Application::get()->setMousePosition(windowpos.x/2,windowpos.y/2);
	_turnOnMouseCamera = false;
	
	/// fire (fluid simulation)
	myFire.init();

	_genericShader = std::make_shared<Shader>("../src/Shaders/Model.vertexshader", "../src/Shaders/Model.fragmentshader");


	auto _skyboxShader = std::make_shared<Shader>("../src/Shaders/Skybox.vertexshader", "../src/Shaders/Skybox.fragmentshader");

	auto meshTest = Helper::loadMesh("../res/cube/cube.obj");

	sky.setShader(_skyboxShader);

	setupCenter();
	setupText();
	
	//setupGround(10,10,nullptr,nullptr);
	_ground = new Ground(20,20);
	_walls = Ground::loadFile("../res/defaultMaze.txt");

	meshTest->setShader(_genericShader);
	finalCube._cube->position = vec3(17.7, 0.7, 18.7);
	finalCube._cube->scale = vec3(0.5, 0.5, 0.5);
	finalCube._cube->mesh = meshTest;


	testCube._cube->position = vec3(17.7, 0.7, 18.7);
	testCube._cube->scale = vec3(0.5, 0.5, 0.5);
	testCube._cube->mesh = meshTest;
	
	playerLight.position = camera.getPosition();
	Monsterlight.intensity = 1;
	Monsterlight.color = vec3(1,0,0);
	Monsterlight.position = vec3(0,3,0);
	playerLight.intensity = 1;
	playerLight.color = vec3(1,1,1);

	FireLight.position = vec3(17.9, 2.0, 18.7);
	FireLight.intensity = 1;
	FireLight.color = vec3(1,1, 0);

	//monsterPosition = vec3(2.25,1.6,15.4);
	monsterPosition = vec3(0.5, 1.76, 0.5);
	monsterPosTarget = monsterPosition;
	monsterRotate = vec3(-90,0,180);
	model_m = glm::translate(monsterPosition) * glm::rotate(-90.0f,vec3(1.0f,0.0f,0.0f)) *glm::rotate(180.0f,vec3(0.0f,0.0f,1.0f)) * glm::scale(vec3(0.025f,0.025f,0.025f));

	//load model file..
    mymodel.LoadModel("../res/drfreak/drfreak.md2");

	//set curent animation to play..
	model_anim_state = mymodel.StartAnimation(animType_t::CROUCH_WALK);

	//load animation shader.
	animatedModelShader.LoadProgram();

	_debugMode = false;
	//camera.Reset(vec3(0),vec3(-0.2,4.8,15.6),vec3(0,1,0));
	camera.setPosition(vec3(0.5,1.76,0.5));


}

Game::~Game(){
	delete _ground;
	delete _walls;
}
//here for test purposes
void Game::setupGround(int x, int y, ShaderPtr shdr, MeshPtr msh)
{
	auto meshTest = Helper::loadMesh("../res/cube/cube.obj");
	auto _genericShader = std::make_shared<Shader>("../src/Shaders/Model.vertexshader", "../src/Shaders/Model.fragmentshader");
	meshTest->setShader(_genericShader);
	string id = "ground_";
	for(int i=0;i<x;i++)
	{
		for(int j=0;j<y;j++)
		{
			auto c = std::make_shared<GameObject>(meshTest);
			c->position.x = i;
			c->position.z = j;
			c->scale = glm::vec3(0.5);
			scene[id+std::to_string(i)+"_"+std::to_string(j)]=c;
		}
	}
}

void Game::setupShaderUniforms(ShaderPtr shdr)
{
		shdr->setUniform("TxSampler",0);
		shdr->setUniform("NrmSampler",1);

		shdr->setUniform("ViewMatrix",camera.getViewMatrix());
		//do the lights here
		shdr->setUniform("eyePos",camera.getPosition());

		shdr->setUniform("playerLightPosition",playerLight.position);
		shdr->setUniform("playerLightColor",playerLight.color);
		shdr->setUniform("playerLightIntensity",playerLight.intensity);

		shdr->setUniform("monsterLightPosition",Monsterlight.position);
		shdr->setUniform("monsterLightColor",Monsterlight.color);
		shdr->setUniform("monsterLightIntensity",Monsterlight.intensity);

		shdr->setUniform("fireLightPosition", FireLight.position);
		shdr->setUniform("fireLightColor", FireLight.color);
		shdr->setUniform("fireLightIntensity", FireLight.intensity);

}

void Game::update(double delta){
	//cout<<"update"<<endl;
	cameraMouseMovement(0,0);
	//moving the light with the camera.

	Monsterlight.position = monsterPosition;
	playerLight.position = camera.getPosition();
	cout<<playerLight.position.x<<" | "<<playerLight.position.y<<" | "<< playerLight.position.z<<endl;
	//cout<<_walls->Collide(playerLight.position)<<endl;
	
	mymodel.UpdateAnimation(&model_anim_state,delta / 1000);
	MonsterAI();
	auto mmx = glm::rotate(monsterRotate.x,vec3(1,0,0));
	auto mmy = glm::rotate(monsterRotate.y,vec3(0,1,0));
	auto mmz = glm::rotate(monsterRotate.z,vec3(0,0,1));
	auto mmr = mmx*mmy*mmz;
	model_m = glm::translate(monsterPosition) * mmr * glm::scale(vec3(0.025f,0.025f,0.025f));

	glm::mat4 projection(1.0f);
	glm::mat4 model(1.0f);
	glm::mat4 view = camera.getViewMatrix();
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 600, 0.1f, 2000.f);
	//floor.render(model,view,projection);
	//snow.Render(delta, model, view, projection);
}

void Game::render(){
	
	//cout<<"render"<<endl;
	glm::mat4 VP = camera.getProjectionMatrix()*camera.getViewMatrix();
	sky.draw(VP);
	for (auto obj : scene){
		auto objShader = obj.second->mesh->getMaterial()->shader;
		objShader->bind();
		objShader->setUniform("TxSampler",0);
		objShader->setUniform("NrmSampler",1);

		objShader->setUniform("ViewMatrix",camera.getViewMatrix());
		//do the lights here
		objShader->setUniform("eyePos",camera.getPosition());

		objShader->setUniform("playerLightPosition",playerLight.position);
		objShader->setUniform("playerLightColor",playerLight.color);
		objShader->setUniform("playerLightIntensity",playerLight.intensity);

		objShader->setUniform("monsterLightPosition",Monsterlight.position);
		objShader->setUniform("monsterLightColor",Monsterlight.color);
		objShader->setUniform("monsterLightIntensity",Monsterlight.intensity);

		objShader->setUniform("fireLightPosition", FireLight.position);
		objShader->setUniform("fireLightColor", FireLight.color);
		objShader->setUniform("fireLightIntensity", FireLight.intensity);

		//probably we can do it once for all objects but who cares it's a college project anyway.
		//end of the lights
		obj.second->draw(VP);
	}

	_ground->draw(VP);
	_walls->draw(VP);
	finalCube._cube->draw(VP);
	
	// render aiming center 
	_centerShader->bind();
	//  bind Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,_texture);
	
	// draw aiming cente
	glEnable(GL_BLEND);
	glBindVertexArray(_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_BLEND);

	

	//
	animatedModelShader.UseProgram();
	animatedModelShader.BindVPMatrix(&VP[0][0]); //need vp matrix to render model in..

	animatedModelShader.BindModelMatrix(&model_m[0][0]); //need model matrix itself..

	mymodel.RenderModel(&model_anim_state,&animatedModelShader);

	// Fire position (fluid simulation)
	position_ = vec3(17.9, 2.0, 18.7);
	//position_ = vec3(2.10359, 1.76, 0.562899);
	rotation_ = glm::vec3(0);
	scale_ = glm::vec3(2.5, 2.5, 2.5);

	glm::mat4 fmodel = getModel(position_, rotation_, scale_);
	cout << "fmodel " << fmodel[0][0] << endl;

	_fireShader->bind();
	_fireShader->setUniform("VP", VP);
	_fireShader->setUniform("ModelMatrix", fmodel);
	
	// render fire
	glEnable(GL_BLEND);
	glBindVertexArray(_VAO);
	myFire.on_frame(_fireShader, camera);
	glDisable(GL_BLEND);
	

}
void Game::MonsterAI(){
	int rnd = rand()%4;
	vec3 bpos = monsterPosition;
	if(monsterPosTarget == monsterPosition){
		if(rnd == 1){
			std::cout << "????" << std::endl;

			monsterPosTarget.x += 1;
			monsterDirection = Direction::NORTH;
			monsterRotate.z = 0;
		}else if(rnd==2){
			monsterPosTarget.x -= 1;
			monsterDirection = Direction::SOUTH;
			monsterRotate.z = 180;
		}else if(rnd==3){
			monsterPosTarget.z += 1;
			monsterDirection = Direction::EAST;
			monsterRotate.z = 270;
		}else if(rnd==0){
			monsterPosTarget.z -= 1;
			monsterDirection = Direction::WEST;
			monsterRotate.z = 90;
		}
		if(_walls->Collide(monsterPosTarget))
			monsterPosTarget = monsterPosition;
		//bpos = monsterPosTarget;
	}else{
		if(monsterDirection == Direction::NORTH){
			bpos.x += 0.05;
		}else if(monsterDirection == Direction::SOUTH){
			bpos.x -= 0.05;
		}else if(monsterDirection == Direction::EAST){
			bpos.z += 0.05;
		}else if(monsterDirection == Direction::WEST){
			bpos.z -= 0.05;
		}
	}

	monsterPosition = bpos;
	if(_walls->Collide(bpos)){
		monsterPosTarget = monsterPosition;

		if(monsterDirection == Direction::NORTH){
			monsterPosTarget.x -= 0.5;
		}else if(monsterDirection == Direction::SOUTH){
			monsterPosTarget.x += 0.5;
		}else if(monsterDirection == Direction::EAST){
			monsterPosTarget.z -= 0.5;
		}else if(monsterDirection == Direction::WEST){
			monsterPosTarget.z += 0.5;
		}
		monsterPosition = monsterPosTarget;
	}else{
		
	}

	//cout<<"Direction = "<<monsterDirection<<endl;
}
void Game::handleKeyboard(GLFWwindow* window,int key, int scancode,int action,int mods){
	//std::cout<<"Key Event"<<std::endl;
	vec3 bpos;
	switch (key)
	{
	case GLFW_KEY_W:
		bpos = camera.getPosition();
		camera.Walk(0.1);
		if(_walls->Collide(camera.getPosition())&& !_debugMode)
			camera.Walk(-0.1);
		break;
	case GLFW_KEY_S:
		bpos = camera.getPosition();
		camera.Walk(-0.1);
		if(_walls->Collide(camera.getPosition()) && !_debugMode)
			camera.Walk(0.1);
		break;
	case GLFW_KEY_A:
		bpos = camera.getPosition();
		camera.Strafe(-0.1);
		if(_walls->Collide(camera.getPosition())&& !_debugMode)
			camera.Strafe(0.1);
		break;
	case GLFW_KEY_D:
		bpos = camera.getPosition();
		camera.Strafe(0.1);
		if(_walls->Collide(camera.getPosition())&& !_debugMode)
			camera.Strafe(-0.1);
		break;
	case GLFW_KEY_ESCAPE:
		Application::get()->shutdown();
		break;
	case GLFW_KEY_UP:
		Monsterlight.position.y++;
		break;
	case GLFW_KEY_DOWN:
		Monsterlight.position.y--;
		break;
	case GLFW_KEY_HOME:
		if(action == GLFW_PRESS)
			_debugMode = !_debugMode;
		break;
	}
	if(!_debugMode){
		auto y = camera.getPosition();
		y.y = 1.76;
		camera.setPosition(y);
	}
	camera.updateViewMatrix();
	
}

void Game::handleMouseButtons(GLFWwindow* window,int button,int action,int mods){
	//std::cout<<"mouse button Event"<<std::endl;
}

void Game::handleMouseMotion(GLFWwindow* window,int x, int y){
	mousePosition = vec2(x,y);
}

void Game::cameraMouseMovement(int x, int y){
	auto windowSize = Application::get()->getWindowSize();
	if (mousePosition.x != windowSize.x/2
		|| mousePosition.y != windowSize.y/2)
	{
		//cout<<"MOUSE MOVED"<<endl;
		double mouseSpeed = 0.005; //it is just there to speed up or slow down the movements.
		double movedDistanceX;
		double movedDistanceY;

		//  the distance (old position - new position)
		// in other words:  how far is the mouse from the center of the window ? The bigger this value, the more we want to turn.
		// note that the old position (x_old, y_old) is fixed in this application (x_old = WindoSizeWidth/2, y_old = WindowSizeHeight/2)
		movedDistanceX = double(windowSize.x/2 - mousePosition.x)*mouseSpeed;
		movedDistanceY = double(windowSize.y/2 - mousePosition.y)*mouseSpeed;

		// Pass the two distances to the Renderer (our drawings)
		
			camera.Yaw(movedDistanceX);
			camera.Pitch(movedDistanceY);
			camera.updateViewMatrix();
		

		//Force the new position of the mouse to be in the middle of the window
		mousePosition.x = windowSize.x/2;
		mousePosition.y = windowSize.y/2;
		Application::get()->setMousePosition(mousePosition.x,mousePosition.y);
	}
}


void Game::setupCenter(){
	float center_vertices[] = {
        // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
        -0.1f,  0.1f,  0.0f,  0.0f,  0.0f,
        -0.1f, -0.1f,  0.0f,  0.0f,  1.0f,
         0.1f, -0.1f,  0.0f,  1.0f,  1.0f,

        -0.1f,  0.1f,  0.0f,  0.0f,  0.0f,
         0.1f, -0.1f,  0.0f,  1.0f,  1.0f,
         0.1f,  0.1f,  0.0f,  1.0f,  0.0f
    };

    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);

    glBindVertexArray(_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(center_vertices), center_vertices, GL_STATIC_DRAW);

    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // texture coord attribute
	glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    // load and create a texture 
    // -------------------------
	glGenTextures(1, &_texture);

    int width, height, nrComponents;
    unsigned char *data = stbi_load("../res/textures/center.png", &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

		glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texture);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
		glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << std::endl;
        stbi_image_free(data);
    }

	// set shader of aming center 
	_centerShader = std::make_shared<Shader>("../src/Shaders/center.vertexshader", "../src/Shaders/center.fragmentshader");
	_centerShader->bind();
	_centerShader->setUniform("centerTxc", 0);

	_fireShader = std::make_shared<Shader>("../src/Shader/fluid_m.vs", "../src/Shader/fluid.fs");
	
}

// something wrong
void Game::setupText() {
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, "arial.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	FT_Set_Pixel_Sizes(face, 0, 48);

	if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;

	


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //禁用byte-alignment限制
	for (GLubyte c = 0; c < 128; c++)
	{
		// 加载字符的字形 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// 生成字形纹理
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// 设置纹理选项
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// 将字符存储到字符表中备用
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Game::RenderText(ShaderPtr s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	// 激活合适的渲染状态
	s->bind();
	s->setUniform("textColor", color);
	//glUniform3f(glGetUniformLocation(s.Program, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(_VAO_text);

	// 对文本中的所有字符迭代
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// 当前字符的VBO
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// 在方块上绘制字形纹理
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// 更新当前字符的VBO
		glBindBuffer(GL_ARRAY_BUFFER, _VBO_text);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// 绘制方块
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// 更新位置到下一个字形的原点，注意单位是1/64像素
		x += (ch.Advance >> 6) * scale; //(2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

// get the model matrix
glm::mat4 Game::getModel(glm::vec3 position = glm::vec3(0), glm::vec3 rotation = glm::vec3(0), glm::vec3 scale = glm::vec3(1)) {
	glm::mat4 tm = glm::translate(glm::mat4(1), position);
	glm::mat4 rotx = glm::rotate(glm::mat4(1), rotation.x, glm::vec3(1, 0, 0));
	glm::mat4 roty = glm::rotate(glm::mat4(1), rotation.y, glm::vec3(0, 1, 0));
	glm::mat4 rotz = glm::rotate(glm::mat4(1), rotation.z, glm::vec3(0, 0, 1));
	glm::mat4 grot = rotx * roty * rotz;
	glm::mat4 sclMat = glm::scale(glm::mat4(1), scale);

	return tm * grot* sclMat;
}
#ifndef __YOCTO__ENGINE_TEST__
#define __YOCTO__ENGINE_TEST__

#include "engine/engine.h"
#include "engine/render/tex_manager.h"

#include "avatar.h"
#include "world.h"
#include "particles.h"

class MEngineMinicraft : public YEngine {
	//Textures
	YTexManager* texture_manager;
	YTexFile* texture_file;

	vector<std::string> faces
	{
		"right.jpg",
		"left.jpg",
		"top.jpg",
		"bottom.jpg",
		"front.jpg",
		"back.jpg"
	};
	//unsigned int cubemapTexture = loadCubemap(faces);
	YTexFile* skybox_right;
	YTexFile* skybox_left;
	YTexFile* skybox_top;
	YTexFile* skybox_bottom;
	YTexFile* skybox_front;
	YTexFile* skybox_back;

	//Objets géometriques
	YFbo* Fbo;
	YVbo* VboCube;
	YVbo* VboSun;
	YVbo* VboSkybox;

	//Particles
	ParticlesSystem* rainParticles;

	//Avatar
	YCamera* Camera = new YCamera();
	MAvatar* Avatar;
	YVec3f prevMousePos = { -1,-1,-1 };
	float camSpeed = 5.f;
	int boostTime;

	//World et shaders
	MWorld* World;
	GLuint ShaderSun = 0;
	GLuint ShaderWorld = 0;
	GLuint ShaderPostProcess = 0;
	GLuint ShaderRain = 0;
	GLuint ShaderSkybox = 0;
	YColor SunColor;
	YColor SkyColor;
	YVec3<float> SunPosition = YVec3<float>(1.0f, 1.0f, 1.0f);
	YVec3<float> SunDirection = YVec3<float>(1.0f, 1.0f, 1.0f);

public :
	//Gestion singleton
	static YEngine * getInstance()
	{
		if (Instance == NULL)
			Instance = new MEngineMinicraft();
		return Instance;
	}

	/*HANDLERS GENERAUX*/
	void loadShaders() {
		ShaderSun = Renderer->createProgram("shaders/sun");
		ShaderWorld = Renderer->createProgram("shaders/world");
		ShaderPostProcess = Renderer->createProgram("shaders/postprocess");
		//ShaderRain = Renderer->createProgram("shaders/rain");
		ShaderSkybox = Renderer->createProgram("shaders/skybox");
	}

	void init() 
	{
		//On cree le fichier pour les textures
		texture_manager = YTexManager::getInstance();
		texture_file = texture_manager -> loadTextureFromDisk("atlas.png");
		YTexManager::getInstance()->loadTextureToOgl(*texture_file);

		YTexFile* skybox_right = texture_manager->loadTextureFromDisk("right.jpg");
		YTexFile* skybox_left = texture_manager->loadTextureFromDisk("left.jpg");
		YTexFile* skybox_top = texture_manager->loadTextureFromDisk("top.jpg");
		YTexFile* skybox_bottom = texture_manager->loadTextureFromDisk("bottom.jpg");
		YTexFile* skybox_front = texture_manager->loadTextureFromDisk("front.jpg");
		YTexFile* skybox_back = texture_manager->loadTextureFromDisk("back.jpg");
			

		//On cree le FBO pour le post process
		Fbo = new YFbo(1);
		Fbo->init(Renderer->ScreenWidth, Renderer->ScreenHeight);

		Renderer->setBackgroundColor(YColor(0.0f, 0.0f, 0.0f, 1.0f));

		//Cube pour la génération procédurale
		YLog::log(YLog::ENGINE_INFO,"Minicraft Started : initialisation");

		VboCube = new YVbo(3, 36, YVbo::PACK_BY_ELEMENT_TYPE);
		VboCube->setElementDescription(0, YVbo::Element(3)); //Sommet
		VboCube->setElementDescription(1, YVbo::Element(4)); //Normale
		VboCube->setElementDescription(2, YVbo::Element(2)); //UV

		VboCube->createVboCpu();
		fillVBOCube(VboCube, 0, 0, 0, 2);
		VboCube->createVboGpu();
		VboCube->deleteVboCpu();

		//Cube pour la skybox
		VboSkybox = new YVbo(3, 36, YVbo::PACK_BY_ELEMENT_TYPE);
		VboSkybox->setElementDescription(0, YVbo::Element(3)); //Sommet
		VboSkybox->setElementDescription(1, YVbo::Element(4)); //Normale
		VboSkybox->setElementDescription(2, YVbo::Element(2)); //UV

		VboSkybox->createVboCpu();
		fillVBOCube(VboSkybox, 0, 0, 0, 100);
		VboSkybox->createVboGpu();
		VboSkybox->deleteVboCpu();

		//Cube pour le soleil
		VboSun = new YVbo(3, 36, YVbo::PACK_BY_ELEMENT_TYPE);
		VboSun->setElementDescription(0, YVbo::Element(3)); //Sommet
		VboSun->setElementDescription(1, YVbo::Element(4)); //Normale
		VboSun->setElementDescription(2, YVbo::Element(2)); //UV

		VboSun->createVboCpu();
		fillVBOCube(VboSun, 0, 0, 10, 2);
		VboSun->createVboGpu();
		VboSun->deleteVboCpu();

		//Caméra
		Camera->setPosition(YVec3f((MWorld::MAT_SIZE_METERS) / 2, (MWorld::MAT_SIZE_METERS) / 2, (MWorld::MAT_HEIGHT_METERS)));
		Camera->setLookAt(YVec3f());
		Renderer->Camera = Camera;

		//World et avatar
		World = new MWorld();
		Avatar = new MAvatar(Camera, World);
		World->init_world(0);

		//Particules de pluie
		//rainParticles = new ParticlesSystem(Camera, 100, 10, 30, 5);
	}

	void update(float elapsed) 
	{
		boostTime += elapsed;
		updateLights(boostTime);

		//rainParticles->updateParticles(elapsed);
		//rainParticles->renderParticules();

		//Avatar->update(elapsed);
		//Avatar->Run = GetKeyState(VK_LSHIFT) & 0x80;
		//Renderer->Camera->moveTo(Avatar->Position + YVec3f(0, 0, Avatar->CurrentHeight / 2));
	}

	void renderObjects()
	{
		Fbo->setAsOutFBO(true);

		glUseProgram(0);
		//Rendu des axes
		glDisable(GL_LIGHTING);
		glBegin(GL_LINES);
		glColor3d(1, 0, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(10000, 0, 0);
		glColor3d(0, 1, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 10000, 0);
		glColor3d(0, 0, 1);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 0, 10000);
		glEnd();

		//Shader world
		glPushMatrix();

		glUseProgram(ShaderWorld);
		//On envoie au shader world nos parametres
		YRenderer::getInstance()->sendTimeToShader(YEngine::getInstance()->DeltaTimeCumul, YRenderer::CURRENT_SHADER);
		GLuint sunPosParam = glGetUniformLocation(ShaderWorld, "sunPos");
		glUniform3f(sunPosParam, SunPosition.X, SunPosition.Y, SunPosition.Z);
		GLuint sunColorParam = glGetUniformLocation(ShaderWorld, "sunColor");
		glUniform3f(sunColorParam, SunColor.R, SunColor.V, SunColor.B);
		GLuint camPosParam = glGetUniformLocation(ShaderWorld, "camPos");
		glUniform3f(camPosParam, Camera->Position.X, Camera->Position.Y, Camera->Position.Z);

		texture_file->setAsShaderInput(ShaderWorld, GL_TEXTURE0, "myTexture");

		World->render_world_vbo(false, true);
		glPopMatrix();

		//Shader Skybox
		glPushMatrix();
		glUseProgram(ShaderSkybox);
		GLuint sunColorParamSky = glGetUniformLocation(ShaderSkybox, "sunColor");
		glUniform3f(sunColorParamSky, SunColor.R, SunColor.V, SunColor.B);
		//glRotatef(Camera->FovY, Camera->LookAt.X, Camera->LookAt.Y, Camera->LookAt.Z);
		glTranslatef(Camera->Position.X, Camera->Position.Y, Camera->Position.Z);
		glScalef(10, 10, 10);
		Renderer->updateMatricesFromOgl();
		Renderer->sendMatricesToShader(ShaderSkybox);

		skybox_right->setAsShaderInput(ShaderWorld, GL_TEXTURE0, "skybox_right");
		skybox_left->setAsShaderInput(ShaderWorld, GL_TEXTURE0, "skybox_left");
		skybox_top->setAsShaderInput(ShaderWorld, GL_TEXTURE0, "skybox_top");
		skybox_bottom->setAsShaderInput(ShaderWorld, GL_TEXTURE0, "skybox_bottom");
		skybox_front->setAsShaderInput(ShaderWorld, GL_TEXTURE0, "skybox_front");
		skybox_back->setAsShaderInput(ShaderWorld, GL_TEXTURE0, "skybox_back");

		VboSkybox->render();
		glPopMatrix();

		//Shader Sun
		glPushMatrix();
		glUseProgram(ShaderSun);
		GLuint sunPosParam2 = glGetUniformLocation(ShaderSun, "sunColor");
		glUniform3f(sunPosParam2, SunColor.R, SunColor.V, SunColor.B);
		glTranslatef(SunPosition.X, SunPosition.Y, SunPosition.Z);
		glScalef(10, 10, 10);
		Renderer->updateMatricesFromOgl();
		Renderer->sendMatricesToShader(ShaderSun);

		VboSun->render();
		glPopMatrix();

		//Particles
		/*glPushMatrix();
		glUseProgram(ShaderRain);
		glScalef(10, 10, 10);
		Renderer->updateMatricesFromOgl();
		Renderer->sendMatricesToShader(ShaderRain);

		rainParticles->renderParticules();

		glPopMatrix();*/


		//Post Process
		Fbo->setAsOutFBO(false);

		glUseProgram(ShaderPostProcess);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		Fbo->setColorAsShaderInput(0, GL_TEXTURE0, "TexColor");
		Fbo->setDepthAsShaderInput(GL_TEXTURE1, "TexDepth");
		Fbo->setAsOutFBO(false);

		glUseProgram(ShaderPostProcess);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		Fbo->setColorAsShaderInput(0, GL_TEXTURE0, "TexColor");
		Fbo->setDepthAsShaderInput(GL_TEXTURE1, "TexDepth");

		GLuint var3 = glGetUniformLocation(ShaderPostProcess, "sky_color");
		glUniform3f(var3, SkyColor.R, SkyColor.V, SkyColor.B);

		Renderer->sendScreenSizeToShader(ShaderPostProcess);
		Renderer->sendNearFarToShader(ShaderPostProcess);
		Renderer->drawFullScreenQuad();

		// Demande le rendu de la generation procedurale
		VboCube->render();

	}

	/* CUBE AND SUN */

	void fillVBOCube(YVbo* vbo, int x, int y, int z, float size = 5.0f)
	{
		int iVertice = 0;

		YVec3f a(size / 2.0f + x, -size / 2.0f + y, -size / 2.0f + z);
		YVec3f b(size / 2.0f + x, size / 2.0f + y, -size / 2.0f + z);
		YVec3f c(size / 2.0f + x, size / 2.0f + y, size / 2.0f + z);
		YVec3f d(size / 2.0f + x, -size / 2.0f + y, size / 2.0f + z);
		YVec3f e(-size / 2.0f + x, -size / 2.0f + y, -size / 2.0f + z);
		YVec3f f(-size / 2.0f + x, size / 2.0f + y, -size / 2.0f + z);
		YVec3f g(-size / 2.0f + x, size / 2.0f + y, size / 2.0f + z);
		YVec3f h(-size / 2.0f + x, -size / 2.0f + y, size / 2.0f + z);

		iVertice += CreateSquareFace(vbo, iVertice, a, b, c, d); //x+
		iVertice += CreateSquareFace(vbo, iVertice, f, e, h, g); //x-
		iVertice += CreateSquareFace(vbo, iVertice, b, f, g, c); //y+
		iVertice += CreateSquareFace(vbo, iVertice, e, a, d, h); //y-
		iVertice += CreateSquareFace(vbo, iVertice, c, g, h, d); //z+
		iVertice += CreateSquareFace(vbo, iVertice, e, f, b, a); //z-
	}

	int CreateSquareFace(YVbo* vbo, int iVertice, YVec3f& a, YVec3f& b, YVec3f& c, YVec3f& d)
	{
		YVec3f normal = (b - a).cross(d - a);
		normal.normalize();

		//Visualize a square face 
		// A   B
		//
		// D   C

		vbo->setElementValue(0, iVertice, a.X, a.Y, a.Z);
		vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z, 1);
		vbo->setElementValue(2, iVertice, 0, 0);

		iVertice++;

		vbo->setElementValue(0, iVertice, b.X, b.Y, b.Z);
		vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z, 1);
		vbo->setElementValue(2, iVertice, 1, 0);

		iVertice++;

		vbo->setElementValue(0, iVertice, c.X, c.Y, c.Z);
		vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z, 1);
		vbo->setElementValue(2, iVertice, 1, 1);

		iVertice++;

		vbo->setElementValue(0, iVertice, a.X, a.Y, a.Z);
		vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z, 1);
		vbo->setElementValue(2, iVertice, 0, 0);

		iVertice++;

		vbo->setElementValue(0, iVertice, c.X, c.Y, c.Z);
		vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z, 1);
		vbo->setElementValue(2, iVertice, 1, 1);

		iVertice++;

		vbo->setElementValue(0, iVertice, d.X, d.Y, d.Z);
		vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z, 1);
		vbo->setElementValue(2, iVertice, 0, 1);

		iVertice++;

		return 6;
	}

	bool getSunDirFromDayTime(YVec3f& sunDir, float mnLever, float mnCoucher, float boostTime)
	{
		bool nuit = false;

		SYSTEMTIME t;
		GetLocalTime(&t);

		//On borne le tweak time à une journée (cyclique)
		while (boostTime > 24 * 60)
			boostTime -= 24 * 60;

		//Temps écoulé depuis le début de la journée
		float fTime = (float)(t.wHour * 60 + t.wMinute);
		fTime += boostTime;
		while (fTime > 24 * 60)
			fTime -= 24 * 60;

		//Si c'est la nuit
		if (fTime < mnLever || fTime > mnCoucher)
		{
			nuit = true;
			if (fTime < mnLever)
				fTime += 24 * 60;
			fTime -= mnCoucher;
			fTime /= (mnLever + 24 * 60 - mnCoucher);
			fTime *= (float)M_PI;
		}
		else
		{
			//c'est le jour
			nuit = false;
			fTime -= mnLever;
			fTime /= (mnCoucher - mnLever);
			fTime *= (float)M_PI;
		}

		//Direction du soleil en fonction de l'heure
		sunDir.X = cos(fTime);
		sunDir.Y = 0.2f;
		sunDir.Z = sin(fTime);
		sunDir.normalize();

		return nuit;
	}

	void updateLights(float boostTime = 0)
	{
		//On recup la direciton du soleil
		bool nuit = getSunDirFromDayTime(SunDirection, 6.0f * 60.0f, 19.0f * 60.0f, boostTime);
		SunPosition = Renderer->Camera->Position + SunDirection * 500.0f;

		//Pendant la journée
		if (!nuit)
		{
			//On definit la couleur
			SunColor = YColor(1.0f, 1.0f, 0.8f, 1.0f);
			SkyColor = YColor(0.0f, 181.f / 255.f, 221.f / 255.f, 1.0f);
			YColor downColor(0.9f, 0.5f, 0.1f, 1);

			SunColor = SunColor.interpolate(downColor, (abs(SunDirection.X)));
			SkyColor = SkyColor.interpolate(downColor, (abs(SunDirection.X)));
		}
		else
		{
			//La nuit : lune blanche et ciel noir
			SunColor = YColor(1, 1, 1, 1);
			SkyColor = YColor(0, 0, 0, 1);
		}

		Renderer->setBackgroundColor(SkyColor);
		VboSun->render();
	}


	void resize(int width, int height) {
		Fbo->resize(width, height);
	}

	/* INPUTS */

	void keyPressed(int key, bool special, bool down, int p1, int p2) 
	{
		//Avancer le soleil
		if (key == 'g' && down)
		{
			boostTime += 10;
		}

		if (down && key == 'z') {
			Camera->move(Camera->Direction * camSpeed);
		}
		if (down && key == 's') {
			Camera->move(-Camera->Direction * camSpeed);
		}
		if (down && key == 'q') {
			Camera->move(-Camera->RightVec * camSpeed);
		}
		if (down && key == 'd') {
			Camera->move(Camera->RightVec * camSpeed);
		}

		/*if (key == 'z')
			Avatar->avance = down;
		if (key == 's')
			Avatar->recule = down;
		if (key == 'q')
			Avatar->gauche = down;
		if (key == 'd')
			Avatar->droite = down;
		if (key == ' ')
			Avatar->Jump = down;
		if (key == 'u')
			Avatar->Crouch = down;
		if (key == 'i')
			Avatar->Run = down;

		if (key == 'e' && !down) {
			int xC, yC, zC;
			YVec3f inter;
			World->getRayCollision(Camera->Position,
				Camera->Position + Camera->Direction * 30,
				inter, xC, yC, zC);
			World->deleteCube(xC, yC, zC);
		}*/
	}

	void mouseWheel(int wheel, int dir, int x, int y, bool inUi)
	{
		Renderer->Camera->move(Renderer->Camera->Direction * 10.0f * dir);
	}

	void mouseClick(int button, int state, int x, int y, bool inUi)
	{
		
	}

	void mouseMove(int x, int y, bool pressed, bool inUi)
	{
		static int lastx = -1;
		static int lasty = -1;

		if (!pressed)
		{
			lastx = x;
			lasty = y;
			showMouse(true);
		}
		else
		{
			if (lastx == -1 && lasty == -1)
			{
				lastx = x;
				lasty = y;
			}

			int dx = x - lastx;
			int dy = y - lasty;

			if (dx == 0 && dy == 0)
				return;

			lastx = x;
			lasty = y;

			if (MouseBtnState & GUI_MRBUTTON)
			{
				showMouse(false);
				if (GetKeyState(VK_LCONTROL) & 0x80)
				{
					Renderer->Camera->rotateAround((float)-dx / 300.0f);
					Renderer->Camera->rotateUpAround((float)-dy / 300.0f);
					glutWarpPointer(Renderer->ScreenWidth / 2, Renderer->ScreenHeight / 2);
					lastx = Renderer->ScreenWidth / 2;
					lasty = Renderer->ScreenHeight / 2;
				}
				else {
					showMouse(false);
					Renderer->Camera->rotate((float)-dx / 300.0f);
					Renderer->Camera->rotateUp((float)-dy / 300.0f);
					glutWarpPointer(Renderer->ScreenWidth / 2, Renderer->ScreenHeight / 2);
					lastx = Renderer->ScreenWidth / 2;
					lasty = Renderer->ScreenHeight / 2;
				}
			}

			if (MouseBtnState & GUI_MMBUTTON)
			{
				showMouse(false);
				if (GetKeyState(VK_LCONTROL) & 0x80)
				{
					YVec3f strafe = Renderer->Camera->RightVec;
					strafe.Z = 0;
					strafe.normalize();
					strafe *= (float)-dx / 2.0f;

					YVec3f avance = Renderer->Camera->Direction;
					avance.Z = 0;
					avance.normalize();
					avance *= (float)dy / 2.0f;

					Renderer->Camera->move(avance + strafe);
				}
				else {
					YVec3f strafe = Renderer->Camera->RightVec;
					strafe.Z = 0;
					strafe.normalize();
					strafe *= (float)-dx / 5.0f;

					Renderer->Camera->move(Renderer->Camera->UpRef * (float)dy / 5.0f);
					Renderer->Camera->move(strafe);
					glutWarpPointer(Renderer->ScreenWidth / 2, Renderer->ScreenHeight / 2);
					lastx = Renderer->ScreenWidth / 2;
					lasty = Renderer->ScreenHeight / 2;
				}
			}
		}
	}
};


#endif
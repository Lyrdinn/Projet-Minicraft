#ifndef __YOCTO__ENGINE_TEST__
#define __YOCTO__ENGINE_TEST__

#include "engine/engine.h"

#include "avatar.h"
#include "world.h"


class MEngineMinicraft : public YEngine {
	YVbo* Fbo;
	YVbo* VboCube;
	YVbo* VboSun;
	int boostTime;
	MWorld* World;
	GLuint ShaderSun = 0;
	GLuint ShaderCube = 0;
	GLuint ShaderWorld = 0;
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
		ShaderCube = Renderer->createProgram("shaders/cube");
		ShaderWorld = Renderer->createProgram("shaders/world");
	}

	void init() 
	{
		YLog::log(YLog::ENGINE_INFO,"Minicraft Started : initialisation");

		VboCube = new YVbo(3, 36, YVbo::PACK_BY_ELEMENT_TYPE);
		VboCube->setElementDescription(0, YVbo::Element(3)); //Sommet
		VboCube->setElementDescription(1, YVbo::Element(4)); //Normale
		VboCube->setElementDescription(2, YVbo::Element(3)); //Couleur

		VboCube->createVboCpu();
		CreateCube(VboCube, 0, 0, 0, 2);
		VboCube->createVboGpu();
		VboCube->deleteVboCpu();

		VboSun = new YVbo(3, 36, YVbo::PACK_BY_ELEMENT_TYPE);
		VboSun->setElementDescription(0, YVbo::Element(3)); //Sommet
		VboSun->setElementDescription(1, YVbo::Element(4)); //Normale
		VboSun->setElementDescription(2, YVbo::Element(3)); //Couleur

		VboSun->createVboCpu();
		CreateCube(VboSun, 0, 0, 30, 20);
		VboSun->createVboGpu();
		VboSun->deleteVboCpu();

		Renderer->setBackgroundColor(YColor(0.0f,0.0f,0.0f,1.0f));
		Renderer->Camera->setPosition(YVec3f(10, 10, 10));
		Renderer->Camera->setLookAt(YVec3f());

		//Pour créer le monde
		World = new MWorld();
		World->init_world(0);
	}

	void update(float elapsed) 
	{
		boostTime += elapsed;
		updateLights(boostTime);
	}

	void renderObjects() 
	{
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

		//Ne montre pas le calcul de SunPosition et SunColor...
		glPushMatrix();
		glUseProgram(ShaderSun);
		GLuint sun = glGetUniformLocation(ShaderSun, "sun_color");
		glUniform3f(sun, SunColor.R, SunColor.V, SunColor.B);
		glTranslatef(SunPosition.X, SunPosition.Y, SunPosition.Z);
		glScalef(10, 10, 10);
		Renderer->updateMatricesFromOgl();
		Renderer->sendMatricesToShader(ShaderSun);
		VboSun->render();
		glPopMatrix();

		//Shader du world
		glPushMatrix();
		glUseProgram(ShaderWorld);
		Renderer->sendTimeToShader(DeltaTimeCumul, ShaderWorld);
		GLuint var = glGetUniformLocation(ShaderWorld, "sunPos");
		glUniform3f(var, SunPosition.X, SunPosition.Y, SunPosition.Z);
		World->render_world_vbo(false, true);
		glPopMatrix();

		//Pour dessiner le monde (fonction à coder ensuite)
		glPushMatrix();
		glUseProgram(ShaderCube);
		World->render_world_basic(ShaderCube, VboCube);
		glPopMatrix();
		//Shader cube
		//glPushMatrix();
		//glUseProgram(ShaderCube);
		//Renderer->updateMatricesFromOgl();
		//Renderer->sendMatricesToShader(ShaderCube);
		//GLuint var2 = glGetUniformLocation(ShaderCube, "cube_color");
		//glUniform4f(var2, 40.0f / 255.0f, 25.0f / 255.0f, 0.0f, 1.0f)
		//glPopMatrix();
	}

	/* CUBE AND SUN */

	void CreateCube(YVbo* vbo, int x, int y, int z, float size = 5.0f)
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
		vbo->setElementValue(2, iVertice, 1, 1, 1);

		iVertice++;

		vbo->setElementValue(0, iVertice, b.X, b.Y, b.Z);
		vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z, 1);
		vbo->setElementValue(2, iVertice, 1, 1, 1);

		iVertice++;

		vbo->setElementValue(0, iVertice, c.X, c.Y, c.Z);
		vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z, 1);
		vbo->setElementValue(2, iVertice, 1, 1, 1);

		iVertice++;

		vbo->setElementValue(0, iVertice, a.X, a.Y, a.Z);
		vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z, 1);
		vbo->setElementValue(2, iVertice, 1, 1, 1);

		iVertice++;

		vbo->setElementValue(0, iVertice, c.X, c.Y, c.Z);
		vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z, 1);
		vbo->setElementValue(2, iVertice, 1, 1, 1);

		iVertice++;

		vbo->setElementValue(0, iVertice, d.X, d.Y, d.Z);
		vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z, 1);
		vbo->setElementValue(2, iVertice, 1, 1, 1);

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
	
	}

	/* INPUTS */

	void keyPressed(int key, bool special, bool down, int p1, int p2) 
	{
		if (key == 103 && down)
		{
			boostTime += 10;
		}
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
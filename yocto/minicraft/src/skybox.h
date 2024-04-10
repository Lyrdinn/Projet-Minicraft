#ifndef __SKYBOX__H
#define __SKYBOX__H

#include "engine/engine.h"
#include "engine/render/tex_manager.h"

class YSkybox
{
public:
	YVbo* VboSkybox;
	YTexManager* texture_manager;
	YTexFile* skybox;

	GLuint ShaderSkybox = 0;

	YSkybox(YRenderer* Renderer)
	{
		ShaderSkybox = Renderer->createProgram("shaders/skybox");

		texture_manager = YTexManager::getInstance();
		skybox = texture_manager->loadTextureFromDisk("FS000_Day_01.png");
		texture_manager->loadTextureToOgl(*skybox);

		VboSkybox = new YVbo(3, 36, YVbo::PACK_BY_ELEMENT_TYPE);
		VboSkybox->setElementDescription(0, YVbo::Element(3)); //Sommet
		VboSkybox->setElementDescription(1, YVbo::Element(4)); //Normale
		VboSkybox->setElementDescription(2, YVbo::Element(2)); //UV

		VboSkybox->createVboCpu();
		fillVBOCube(VboSkybox, 0, 0, -5, 1000);
		VboSkybox->createVboGpu();
		VboSkybox->deleteVboCpu();
	}

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

	void RenderSkyBox(YCamera* Camera, YColor SunColor, YRenderer* Renderer)
	{
		glPushMatrix();
		glUseProgram(ShaderSkybox);
		glDepthMask(GL_TRUE);
		GLuint sunColorParamSky = glGetUniformLocation(ShaderSkybox, "sunColor");
		glUniform3f(sunColorParamSky, SunColor.R, SunColor.V, SunColor.B);
		//glRotatef(Camera->FovY, Camera->LookAt.X, Camera->LookAt.Y, Camera->LookAt.Z);
		glTranslatef(Camera->Position.X, Camera->Position.Y, Camera->Position.Z);
		glScalef(1, 1, 1);

		skybox->setAsShaderInput(ShaderSkybox, GL_TEXTURE0, "skybox");

		Renderer->updateMatricesFromOgl();
		Renderer->sendMatricesToShader(ShaderSkybox);

		VboSkybox->render();
		glPopMatrix();
	}

};
	
#endif
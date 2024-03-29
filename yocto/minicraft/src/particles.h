#ifndef __PARTICLES__
#define __PARTICLES__

#include "engine/engine.h"
#include "chunk.h"
#include "engine/render/tex_manager.h"

class Particle
{
private:
	YVbo* VboParticle;
	int particle_life = 5;
	float particle_size = 1.0f;
	YVec3<float> particle_position;

	void fillVBOCube(YVbo* vbo, int x, int y, int z, float size = 5.0f)
	{
		int iVertice = 0;

		YVec3f a(x, -size + y, -size + z);
		YVec3f b(size + x, size + y, -size + z);
		YVec3f c(size + x, size + y, size + z);
		YVec3f d(size + x, -size + y, size + z);
		YVec3f e(-size + x, -size + y, -size + z);
		YVec3f f(-size + x, size + y, -size + z);
		YVec3f g(-size + x, size + y, size + z);
		YVec3f h(-size + x, -size + y, size + z);

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

public:
	Particle(YVec3<float> _particle_position)
	{
		particle_position = _particle_position;

		//Cube pour la pluie
		VboParticle = new YVbo(3, 36, YVbo::PACK_BY_ELEMENT_TYPE);
		VboParticle->setElementDescription(0, YVbo::Element(3)); //Sommet
		VboParticle->setElementDescription(1, YVbo::Element(4)); //Normale
		VboParticle->setElementDescription(2, YVbo::Element(3)); //Couleur

		VboParticle->createVboCpu();

		fillVBOCube(VboParticle, _particle_position.X, _particle_position.Z, _particle_position.Y, particle_size);
		VboParticle->createVboGpu();
		VboParticle->deleteVboCpu();
	}

	void updateParticle()
	{
		particle_position.Z -= 0.2;
		particle_life -= 1;

		VboParticle->render();
	}

	YVec3<float> getParticlePosition()
	{
		return particle_position;
	}

	void renderParticle()
	{
		VboParticle->render();
	}
};

class ParticlesSystem
{
private:


public:
	vector< Particle* > RainParticles;
	int nb_particles;

	ParticlesSystem(int _nb_particles)
	{
		nb_particles = _nb_particles;
		RainParticles = vector<Particle*>();

		Particle* part1 = new Particle(YVec3<float>(1.0, 10.0, 1.0));
		Particle* part2 = new Particle(YVec3<float>(10.0, 10.0, 1.0));
		Particle* part3 = new Particle(YVec3<float>(20.0, 10.0, 1.0));

		RainParticles.push_back(part1);
		RainParticles.push_back(part2);
		RainParticles.push_back(part3);
	}

	void updateParticles()
	{
		for (int i = 0; i < RainParticles.size(); i ++)
		{
			RainParticles[i] -> updateParticle();
		}
	}

	void renderParticules()
	{
		for (int i = 0; i < RainParticles.size(); i++)
		{
			RainParticles[i] ->renderParticle();
		}
	}
};

#endif
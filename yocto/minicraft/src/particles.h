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
	float particle_size = 0.5f;
	YVec3<float> particle_position;

	void fillVBOCube(YVbo* vbo, int x, int y, int z, float size = 5.0f)
	{
		int iVertice = 0;

		YVec3f a(size + x, -size + y, -size + z);
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

		fillVBOCube(VboParticle, _particle_position.X, _particle_position.Y, _particle_position.Z, particle_size);
		
		VboParticle->createVboGpu();
		VboParticle->deleteVboCpu();
	}

	void updateParticle(float elapsed)
	{
		particle_position.Z -= 2 * elapsed; // Modify the Z position based on falling speed and elapsed time
		particle_life -= 1;

		// Render the VBO
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
	vector< Particle* > particles;
	YCamera* Camera;
	int nb_particles;
	float p_radius;
	float p_height;
	float falling_speed;

	ParticlesSystem(YCamera* _Camera, int _nb_particles, float _p_radius, float _p_height, float _falling_speed)
	{
		Camera = _Camera;

		nb_particles = _nb_particles;
		p_radius = _p_radius;
		p_height = _p_height;
		falling_speed = _falling_speed;

		particles = vector<Particle*>();

		for (int i = 0; i < nb_particles; i++)
		{
			YVec3 _part_position = Camera->UpVec + Camera->Position;
			_part_position = _part_position * MChunk::CHUNK_SIZE;

			//float part_x = rand() % (int) (player_position.X + p_radius) + (_player_position.X - p_radius);
			//float part_y = rand() % (int) (player_position.Y + p_radius) + (_player_position.Y - p_height);
			//float part_z = rand() % (int) (player_position.Z + p_height) + (_player_position.Z - p_height);

			Particle* part = new Particle(YVec3<float>(_part_position.X, _part_position.Y, 10.0));

			particles.push_back(part);
		}
	}

	void updateParticles(float elapsed)
	{
		for (int i = 0; i < particles.size(); i ++)
		{
			particles[i] -> updateParticle(elapsed);
		}
	}

	void renderParticules()
	{
		for (int i = 0; i < particles.size(); i++)
		{
			particles[i] ->renderParticle();
		}
	}
};

#endif
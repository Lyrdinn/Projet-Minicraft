#ifndef PROCEDURAL_GENERATION
#define PROCEDURAL_GENERATION

#include "world.h"

//bool MWorld::can_place_tree(int x, int y, int z)
//{
//	MCube* cube = getCube(x, y, z - 1);
//
//	//Return false if we don't place it on top of dirt or herb or we place it in water.
//	if (cube->getType() != MCube::CUBE_TERRE && cube->getType() != MCube::CUBE_HERBE) return false;
//	if (getCube(x, y, z)->getType() == MCube::CUBE_EAU) return false;
//
//	//Else if we are in the dirt we recursevely check if we are in the correct surface plane else we go higher to
//	//still try to place our tree
//	int trunk_slot = z + 1;
//	cube = getCube(x, y, trunk_slot);
//
//	if (cube->getType() == MCube::CUBE_AIR)
//	{
//		create_tree(x, y, z);
//		return true;
//	}
//	return can_place_tree(x, y, z + 1);
//}
//
//void MWorld::create_tree(int x, int y, int z)
//{
//	//Random trunk size between 3 and 6;
//	int tree_size = rand() % 4 + 2;
//
//	//We fill our trunk
//	for (int trunk = 0; trunk < tree_size; trunk++)
//	{
//		MCube* cube = getCube(x, y, z + trunk);
//		cube->setType(MCube::CUBE_BOIS);
//	}
//
//	//We fill the trunk with our leaves
//	for (int x2 = x - 2; x2 < x + 2; x2++)
//	{
//		for (int y2 = y - 2; y2 < y + 2; y2++)
//		{
//			for (int z2 = z + tree_size - 1; z2 < z + tree_size + 3; z2++)
//			{
//				//75% of chance of adding a leaf
//				int rand_leaf = rand() % 100 + 1;
//				if (rand_leaf <= 75)
//				{
//					MCube* cube = getCube(x2, y2, z2);
//					cube->setType(MCube::CUBE_BRANCHES);
//				}
//			}
//		}
//	}
//}
//
//void MWorld::place_trees()
//{
//	int x = 0;
//	int y = 0;
//	int z = 0;
//
//	int nb_trees_placed = 0;
//
//	for (int i = 0; i < NB_OF_TREES_GEN; i++)
//	{
//		//We find a random number between 3 and our map size -3 because we don't want our trees to be cut
//		//because of the map
//		x = rand() % (MAT_SIZE_CUBES - 3) + 3;
//		y = rand() % (MAT_SIZE_CUBES - 3) + 3;
//
//		//We go between our surface level to our maximum surface level
//		z = rand() % (SURFACE_LEVEL)+(SURFACE_LEVEL + 7);
//
//		if (can_place_tree(x, y, z))
//		{
//			nb_trees_placed += 1;
//			if (nb_trees_placed == MAX_NB_TREES) return;
//		}
//	}
//}
//
//void create_cave_entrance()
//{
//	int x_rand = rand() % (MAT_SIZE_CUBES - 10) + 10;
//	int y_rand = rand() % (MAT_SIZE_CUBES - 10) + 10;
//
//	for (int x = x_rand - 3; x < x_rand + 3; x++)
//	{
//		for (int y = y_rand - 3; y < y_rand + 3; y++)
//		{
//			
//		}
//	}
//}
//void create_caves()
//{
//
//}
//void dig_tunnels_between_caves()
//{
//
//}

#endif
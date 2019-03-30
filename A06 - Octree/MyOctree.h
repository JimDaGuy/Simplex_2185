#pragma once

#include "Definitions.h"
#include "MyEntity.h"
#include "MyOctant.h"
using namespace Simplex;

class MyOctree
{
public:
	MyOctree(uint a_octreeLevels = 1, MyEntity** a_entityArray = nullptr, uint a_entityCount = 0);
	~MyOctree();
	void CreateSubdivisions(MyOctant a_parent);
	void Display();
	void CheckCollisions();
private:
	uint octreeLevels;
	uint subdivisionThreshold;
	vector3 octreeHalfWidth;
	vector3 octreeCenter;
	MyOctant root;
};


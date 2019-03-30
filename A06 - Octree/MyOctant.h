#pragma once
#include "Definitions.h"
#include "MyEntity.h"
using namespace Simplex;

class MyOctant
{
public:
	MyOctant(uint a_level = 1, uint a_subdivisionThreshold = 8, vector3 a_halfWidth = vector3(35.0f), vector3 a_center = vector3(0.0f));
	~MyOctant();
	uint GetLevel();
	uint GetSubdivisionThreshold();
	vector3 GetHalfWidth();
	vector3 GetCenter();
	bool GetSubdivided();
	std::vector<MyOctant> GetSubdivisions();
	std::vector<MyEntity*> GetEntityList();
	uint GetEntityCount();
	bool IsCollidingWithEntity(MyEntity* a_entity);
	void CheckCollision(MyOctant a_octant);
	void SetSubdivided(bool a_subdivsion);
	void AddSubdivision(MyOctant a_oct);
	void AddEntityToList(MyEntity * a_entity);
	void SetEntityList(MyEntity** a_entityList, uint entityCount);
	void SetEntityList(std::vector<MyEntity*> a_entityList);
private:
	// Eventually use this ID to decide which octant to display
	uint octantID;
	uint level;
	uint subdivisionThreshold;
	vector3 halfWidth;
	vector3 center;
	vector3 maxVec;
	vector3 minVec;
	bool subdivided;
	std::vector<MyOctant> subdivisions;
	std::vector<MyEntity*> entityList;
};


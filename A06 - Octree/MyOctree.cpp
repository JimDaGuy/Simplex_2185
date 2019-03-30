#include "MyOctree.h"

MyOctree::MyOctree(uint a_octreeLevels = 1, MyEntity** a_entityArray = nullptr, uint a_entityCount = 0)
{
	octreeLevels = a_octreeLevels;
	octreeHalfWidth = vector3(35.0f);
	octreeCenter = vector3(0.0f);
	subdivisionThreshold = 8;

	// Create initial/largest/main Octant
	root = MyOctant(1, subdivisionThreshold, octreeHalfWidth, octreeCenter);
	// Set root octant's entity list to the list of all entities
	root.SetEntityList(a_entityArray, a_entityCount);
	// Subdivide root octant however many times neccesary
	CreateSubdivisions(root);
}

MyOctree::~MyOctree()
{

}

void MyOctree::CreateSubdivisions(MyOctant a_parent)
{
	// Recursive function to subdivide octants as neccesary
	uint currentLevel = a_parent.GetLevel();

	// Return if octree has been subdivided enough
	if (currentLevel >= octreeLevels)
		return;
	
	// Return if the octant has too few entities to subdivide further
	if (a_parent.GetEntityCount() < a_parent.GetSubdivisionThreshold)
		return;

	// Return if the octant has already been subdivided
	if (a_parent.GetSubdivided())
		return;

	vector3 parentHalfWidth = a_parent.GetHalfWidth();
	vector3 parentCenter = a_parent.GetCenter();
	std::vector<MyEntity*> parentEntityList = a_parent.GetEntityList();
	uint parentEntityCount = a_parent.GetEntityCount();

	// Create octant, set center, half width, subdivision level, parent, entity list children
	vector3 oct1Center = parentCenter;
	oct1Center.x += parentHalfWidth.x / 2;
	oct1Center.y += parentHalfWidth.y / 2;
	oct1Center.z += parentHalfWidth.z / 2;
	MyOctant oct1 = MyOctant(currentLevel + 1, a_parent.GetSubdivisionThreshold(), parentHalfWidth / 2, oct1Center);
	// Add child octant to the parent's list
	a_parent.AddSubdivision(oct1);

	// Octant #2
	vector3 oct2Center = parentCenter;
	oct2Center.x -= parentHalfWidth.x / 2;
	oct2Center.y += parentHalfWidth.y / 2;
	oct2Center.z += parentHalfWidth.z / 2;
	MyOctant oct2 = MyOctant(currentLevel + 1, a_parent.GetSubdivisionThreshold(), parentHalfWidth / 2, oct2Center);
	a_parent.AddSubdivision(oct2);

	// Octant #3
	vector3 oct3Center = parentCenter;
	oct3Center.x -= parentHalfWidth.x / 2;
	oct3Center.y -= parentHalfWidth.y / 2;
	oct3Center.z += parentHalfWidth.z / 2;
	MyOctant oct3 = MyOctant(currentLevel + 1, a_parent.GetSubdivisionThreshold(), parentHalfWidth / 2, oct3Center);
	a_parent.AddSubdivision(oct3);

	// Octant #4
	vector3 oct4Center = parentCenter;
	oct4Center.x += parentHalfWidth.x / 2;
	oct4Center.y -= parentHalfWidth.y / 2;
	oct4Center.z += parentHalfWidth.z / 2;
	MyOctant oct4 = MyOctant(currentLevel + 1, a_parent.GetSubdivisionThreshold(), parentHalfWidth / 2, oct4Center);
	a_parent.AddSubdivision(oct4);

	// Octant #5
	vector3 oct5Center = parentCenter;
	oct5Center.x += parentHalfWidth.x / 2;
	oct5Center.y += parentHalfWidth.y / 2;
	oct5Center.z -= parentHalfWidth.z / 2;
	MyOctant oct5 = MyOctant(currentLevel + 1, a_parent.GetSubdivisionThreshold(), parentHalfWidth / 2, oct5Center);
	a_parent.AddSubdivision(oct5);

	// Octant #6
	vector3 oct6Center = parentCenter;
	oct6Center.x -= parentHalfWidth.x / 2;
	oct6Center.y += parentHalfWidth.y / 2;
	oct6Center.z -= parentHalfWidth.z / 2;
	MyOctant oct6 = MyOctant(currentLevel + 1, a_parent.GetSubdivisionThreshold(), parentHalfWidth / 2, oct6Center);
	a_parent.AddSubdivision(oct6);

	// Octant #7
	vector3 oct7Center = parentCenter;
	oct7Center.x -= parentHalfWidth.x / 2;
	oct7Center.y -= parentHalfWidth.y / 2;
	oct7Center.z -= parentHalfWidth.z / 2;
	MyOctant oct7 = MyOctant(currentLevel + 1, a_parent.GetSubdivisionThreshold(), parentHalfWidth / 2, oct7Center);
	a_parent.AddSubdivision(oct7);

	// Octant #8
	vector3 oct8Center = parentCenter;
	oct8Center.x += parentHalfWidth.x / 2;
	oct8Center.y -= parentHalfWidth.y / 2;
	oct8Center.z -= parentHalfWidth.z / 2;
	MyOctant oct8 = MyOctant(currentLevel + 1, a_parent.GetSubdivisionThreshold(), parentHalfWidth / 2, oct8Center);
	a_parent.AddSubdivision(oct8);

	// Check which entities from the parent's entityList are colliding with the current octant, add those to the entity list
	// Check collision with every entity in the entity list
	// Append each item found to be colliding to the entityList for the child octant
	for (uint i = 0; i < parentEntityCount; i++) {
		// Grab entity from the entity list
		MyEntity* currentEntity = parentEntityList[i];

		// Check if entity is colliding with the octant
		bool oct1EntColliding = oct1.IsCollidingWithEntity(currentEntity);
		bool oct2EntColliding = oct2.IsCollidingWithEntity(currentEntity);
		bool oct3EntColliding = oct3.IsCollidingWithEntity(currentEntity);
		bool oct4EntColliding = oct4.IsCollidingWithEntity(currentEntity);
		bool oct5EntColliding = oct5.IsCollidingWithEntity(currentEntity);
		bool oct6EntColliding = oct6.IsCollidingWithEntity(currentEntity);
		bool oct7EntColliding = oct7.IsCollidingWithEntity(currentEntity);
		bool oct8EntColliding = oct8.IsCollidingWithEntity(currentEntity);

		// If entity is colliding with the octant, add it to the Octant's entity list
		if (oct1EntColliding) {
			oct1.AddEntityToList(currentEntity);
		}
		if (oct2EntColliding) {
			oct2.AddEntityToList(currentEntity);
		}
		if (oct3EntColliding) {
			oct3.AddEntityToList(currentEntity);
		}
		if (oct4EntColliding) {
			oct4.AddEntityToList(currentEntity);
		}
		if (oct5EntColliding) {
			oct5.AddEntityToList(currentEntity);
		}
		if (oct6EntColliding) {
			oct6.AddEntityToList(currentEntity);
		}
		if (oct7EntColliding) {
			oct7.AddEntityToList(currentEntity);
		}
		if (oct8EntColliding) {
			oct8.AddEntityToList(currentEntity);
		}
	}

	// Create subdivisions for the new octants if neccesary
	CreateSubdivisions(oct1);
	CreateSubdivisions(oct2);
	CreateSubdivisions(oct3);
	CreateSubdivisions(oct4);
	CreateSubdivisions(oct5);
	CreateSubdivisions(oct6);
	CreateSubdivisions(oct7);
	CreateSubdivisions(oct8);

	// Set parent node subdivided property
	a_parent.SetSubdivided(true);
}

void MyOctree::Display()
{
}

void MyOctree::CheckCollisions()
{
	root.CheckCollision(root);
}
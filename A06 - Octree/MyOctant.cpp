#include "MyOctant.h"

MyOctant::MyOctant(uint a_level, uint a_subdivisionThreshold, vector3 a_halfWidth, vector3 a_center)
{
	level = a_level;
	halfWidth = a_halfWidth;
	center = a_center;
	maxVec = center + halfWidth;
	minVec = center - halfWidth;
	subdivided = false;
}

MyOctant::~MyOctant()
{
}

// Getters
uint MyOctant::GetLevel()
{
	return level;
}
uint MyOctant::GetSubdivisionThreshold()
{
	return subdivisionThreshold;
}
vector3 MyOctant::GetHalfWidth()
{
	return halfWidth;
}
vector3 MyOctant::GetCenter()
{
	return center;
}
bool MyOctant::GetSubdivided()
{
	return subdivided;
}
std::vector<MyOctant> MyOctant::GetSubdivisions() 
{
	return subdivisions;
}
std::vector<MyEntity*> MyOctant::GetEntityList()
{
	return entityList;
}
uint MyOctant::GetEntityCount()
{
	return entityList.size();
}
bool MyOctant::IsCollidingWithEntity(MyEntity * a_entity)
{
	MyRigidBody* entityRB = a_entity->GetRigidBody();
	vector3 entityMinG = entityRB->GetMinGlobal();
	vector3 entityMaxG = entityRB->GetMaxGlobal();

	bool colliding = true;

	if (maxVec.x < entityMinG.x) //this to the right of other
		colliding = false;
	if (minVec.x > entityMaxG.x) //this to the left of other
		colliding = false;

	if (maxVec.y < entityMinG.y) //this below of other
		colliding = false;
	if (minVec.y > entityMaxG.y) //this above of other
		colliding = false;

	if (maxVec.z < entityMinG.z) //this behind of other
		colliding = false;
	if (minVec.z > entityMaxG.z) //this in front of other
		colliding = false;

	return colliding;
}
void MyOctant::CheckCollision(MyOctant a_octant)
{
	// If the Octant is not a bottom level octant
	// Call this function on the child octants
	if (subdivided) {
		// Iterate through subdivisions of current Octant
		for (uint i = 0; i < subdivisions.size(); i++) {
			CheckCollision(subdivisions[i]);
		}
	}
	// If the Octant is not subdivided,
	// Check collisions with a child entities in the entity list
	else {
		for (uint j = 0; j < entityList.size() - 1; j++) {
			for (uint k = j + 1; k < entityList.size(); k++) {
				entityList[j]->IsColliding(entityList[k]);
			}
		}
	}
	
}

// Setters
void MyOctant::SetSubdivided(bool a_subdivision)
{
	subdivided = a_subdivision;
}
void MyOctant::AddSubdivision(MyOctant a_oct)
{
	subdivisions.push_back(a_oct);
}
void MyOctant::AddEntityToList(MyEntity* a_entity) {
	entityList.push_back(a_entity);
}
void MyOctant::SetEntityList(MyEntity** a_entityList, uint entityCount) {
	entityList.clear();
	for (uint i = 0; i < entityCount; i++) {
		entityList.push_back(a_entityList[i]);
	}
}
void MyOctant::SetEntityList(std::vector<MyEntity*> a_entityList) {
	entityList.clear();
	entityList = a_entityList;
}
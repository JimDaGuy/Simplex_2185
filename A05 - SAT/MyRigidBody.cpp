#include "MyRigidBody.h"
using namespace Simplex;
//Allocation
void MyRigidBody::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bVisibleBS = false;
	m_bVisibleOBB = true;
	m_bVisibleARBB = false;

	m_fRadius = 0.0f;

	m_v3ColorColliding = C_RED;
	m_v3ColorNotColliding = C_WHITE;

	m_v3Center = ZERO_V3;
	m_v3MinL = ZERO_V3;
	m_v3MaxL = ZERO_V3;

	m_v3MinG = ZERO_V3;
	m_v3MaxG = ZERO_V3;

	m_v3HalfWidth = ZERO_V3;
	m_v3ARBBSize = ZERO_V3;

	m_m4ToWorld = IDENTITY_M4;
}
void MyRigidBody::Swap(MyRigidBody& a_pOther)
{
	std::swap(m_pMeshMngr, a_pOther.m_pMeshMngr);
	std::swap(m_bVisibleBS, a_pOther.m_bVisibleBS);
	std::swap(m_bVisibleOBB, a_pOther.m_bVisibleOBB);
	std::swap(m_bVisibleARBB, a_pOther.m_bVisibleARBB);

	std::swap(m_fRadius, a_pOther.m_fRadius);

	std::swap(m_v3ColorColliding, a_pOther.m_v3ColorColliding);
	std::swap(m_v3ColorNotColliding, a_pOther.m_v3ColorNotColliding);

	std::swap(m_v3Center, a_pOther.m_v3Center);
	std::swap(m_v3MinL, a_pOther.m_v3MinL);
	std::swap(m_v3MaxL, a_pOther.m_v3MaxL);

	std::swap(m_v3MinG, a_pOther.m_v3MinG);
	std::swap(m_v3MaxG, a_pOther.m_v3MaxG);

	std::swap(m_v3HalfWidth, a_pOther.m_v3HalfWidth);
	std::swap(m_v3ARBBSize, a_pOther.m_v3ARBBSize);

	std::swap(m_m4ToWorld, a_pOther.m_m4ToWorld);

	std::swap(m_CollidingRBSet, a_pOther.m_CollidingRBSet);
}
void MyRigidBody::Release(void)
{
	m_pMeshMngr = nullptr;
	ClearCollidingList();
}
//Accessors
bool MyRigidBody::GetVisibleBS(void) { return m_bVisibleBS; }
void MyRigidBody::SetVisibleBS(bool a_bVisible) { m_bVisibleBS = a_bVisible; }
bool MyRigidBody::GetVisibleOBB(void) { return m_bVisibleOBB; }
void MyRigidBody::SetVisibleOBB(bool a_bVisible) { m_bVisibleOBB = a_bVisible; }
bool MyRigidBody::GetVisibleARBB(void) { return m_bVisibleARBB; }
void MyRigidBody::SetVisibleARBB(bool a_bVisible) { m_bVisibleARBB = a_bVisible; }
float MyRigidBody::GetRadius(void) { return m_fRadius; }
vector3 MyRigidBody::GetColorColliding(void) { return m_v3ColorColliding; }
vector3 MyRigidBody::GetColorNotColliding(void) { return m_v3ColorNotColliding; }
void MyRigidBody::SetColorColliding(vector3 a_v3Color) { m_v3ColorColliding = a_v3Color; }
void MyRigidBody::SetColorNotColliding(vector3 a_v3Color) { m_v3ColorNotColliding = a_v3Color; }
vector3 MyRigidBody::GetCenterLocal(void) { return m_v3Center; }
vector3 MyRigidBody::GetMinLocal(void) { return m_v3MinL; }
vector3 MyRigidBody::GetMaxLocal(void) { return m_v3MaxL; }
vector3 MyRigidBody::GetCenterGlobal(void){	return vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f)); }
vector3 MyRigidBody::GetMinGlobal(void) { return m_v3MinG; }
vector3 MyRigidBody::GetMaxGlobal(void) { return m_v3MaxG; }
vector3 MyRigidBody::GetHalfWidth(void) { return m_v3HalfWidth; }
matrix4 MyRigidBody::GetModelMatrix(void) { return m_m4ToWorld; }
void MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix)
{
	//to save some calculations if the model matrix is the same there is nothing to do here
	if (a_m4ModelMatrix == m_m4ToWorld)
		return;

	//Assign the model matrix
	m_m4ToWorld = a_m4ModelMatrix;

	//Calculate the 8 corners of the cube
	vector3 v3Corner[8];
	//Back square
	v3Corner[0] = m_v3MinL;
	v3Corner[1] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MinL.z);
	v3Corner[2] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MinL.z);
	v3Corner[3] = vector3(m_v3MaxL.x, m_v3MaxL.y, m_v3MinL.z);

	//Front square
	v3Corner[4] = vector3(m_v3MinL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[5] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[6] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MaxL.z);
	v3Corner[7] = m_v3MaxL;

	//Place them in world space
	for (uint uIndex = 0; uIndex < 8; ++uIndex)
	{
		v3Corner[uIndex] = vector3(m_m4ToWorld * vector4(v3Corner[uIndex], 1.0f));
	}

	//Identify the max and min as the first corner
	m_v3MaxG = m_v3MinG = v3Corner[0];

	//get the new max and min for the global box
	for (uint i = 1; i < 8; ++i)
	{
		if (m_v3MaxG.x < v3Corner[i].x) m_v3MaxG.x = v3Corner[i].x;
		else if (m_v3MinG.x > v3Corner[i].x) m_v3MinG.x = v3Corner[i].x;

		if (m_v3MaxG.y < v3Corner[i].y) m_v3MaxG.y = v3Corner[i].y;
		else if (m_v3MinG.y > v3Corner[i].y) m_v3MinG.y = v3Corner[i].y;

		if (m_v3MaxG.z < v3Corner[i].z) m_v3MaxG.z = v3Corner[i].z;
		else if (m_v3MinG.z > v3Corner[i].z) m_v3MinG.z = v3Corner[i].z;
	}

	//we calculate the distance between min and max vectors
	m_v3ARBBSize = m_v3MaxG - m_v3MinG;
}
//The big 3
MyRigidBody::MyRigidBody(std::vector<vector3> a_pointList)
{
	Init();
	//Count the points of the incoming list
	uint uVertexCount = a_pointList.size();

	//If there are none just return, we have no information to create the BS from
	if (uVertexCount == 0)
		return;

	//Max and min as the first vector of the list
	m_v3MaxL = m_v3MinL = a_pointList[0];

	//Get the max and min out of the list
	for (uint i = 1; i < uVertexCount; ++i)
	{
		if (m_v3MaxL.x < a_pointList[i].x) m_v3MaxL.x = a_pointList[i].x;
		else if (m_v3MinL.x > a_pointList[i].x) m_v3MinL.x = a_pointList[i].x;

		if (m_v3MaxL.y < a_pointList[i].y) m_v3MaxL.y = a_pointList[i].y;
		else if (m_v3MinL.y > a_pointList[i].y) m_v3MinL.y = a_pointList[i].y;

		if (m_v3MaxL.z < a_pointList[i].z) m_v3MaxL.z = a_pointList[i].z;
		else if (m_v3MinL.z > a_pointList[i].z) m_v3MinL.z = a_pointList[i].z;
	}

	//with model matrix being the identity, local and global are the same
	m_v3MinG = m_v3MinL;
	m_v3MaxG = m_v3MaxL;

	//with the max and the min we calculate the center
	m_v3Center = (m_v3MaxL + m_v3MinL) / 2.0f;

	//we calculate the distance between min and max vectors
	m_v3HalfWidth = (m_v3MaxL - m_v3MinL) / 2.0f;

	//Get the distance between the center and either the min or the max
	m_fRadius = glm::distance(m_v3Center, m_v3MinL);
}
MyRigidBody::MyRigidBody(MyRigidBody const& a_pOther)
{
	m_pMeshMngr = a_pOther.m_pMeshMngr;

	m_bVisibleBS = a_pOther.m_bVisibleBS;
	m_bVisibleOBB = a_pOther.m_bVisibleOBB;
	m_bVisibleARBB = a_pOther.m_bVisibleARBB;

	m_fRadius = a_pOther.m_fRadius;

	m_v3ColorColliding = a_pOther.m_v3ColorColliding;
	m_v3ColorNotColliding = a_pOther.m_v3ColorNotColliding;

	m_v3Center = a_pOther.m_v3Center;
	m_v3MinL = a_pOther.m_v3MinL;
	m_v3MaxL = a_pOther.m_v3MaxL;

	m_v3MinG = a_pOther.m_v3MinG;
	m_v3MaxG = a_pOther.m_v3MaxG;

	m_v3HalfWidth = a_pOther.m_v3HalfWidth;
	m_v3ARBBSize = a_pOther.m_v3ARBBSize;

	m_m4ToWorld = a_pOther.m_m4ToWorld;

	m_CollidingRBSet = a_pOther.m_CollidingRBSet;
}
MyRigidBody& MyRigidBody::operator=(MyRigidBody const& a_pOther)
{
	if (this != &a_pOther)
	{
		Release();
		Init();
		MyRigidBody temp(a_pOther);
		Swap(temp);
	}
	return *this;
}
MyRigidBody::~MyRigidBody() { Release(); };
//--- a_pOther Methods
void MyRigidBody::AddCollisionWith(MyRigidBody* a_pOther)
{
	/*
		check if the object is already in the colliding set, if
		the object is already there return with no changes
	*/
	auto element = m_CollidingRBSet.find(a_pOther);
	if (element != m_CollidingRBSet.end())
		return;
	// we couldn't find the object so add it
	m_CollidingRBSet.insert(a_pOther);
}
void MyRigidBody::RemoveCollisionWith(MyRigidBody* a_pOther)
{
	m_CollidingRBSet.erase(a_pOther);
}
void MyRigidBody::ClearCollidingList(void)
{
	m_CollidingRBSet.clear();
}
bool MyRigidBody::IsColliding(MyRigidBody* const a_pOther)
{
	//check if spheres are colliding as pre-test
	bool bColliding = (glm::distance(GetCenterGlobal(), a_pOther->GetCenterGlobal()) < m_fRadius + a_pOther->m_fRadius);
	
	//if they are colliding check the SAT
	if (bColliding)
	{
		matrix4 planeMatrix = IDENTITY_M4;
		// Get midpoint between obj1 and obj2
		vector3 midPoint = (a_pOther->GetCenterGlobal() + GetCenterGlobal()) / 2.0f;

		// Check for collision axis
		// Show first axis found or no axis if colliding
		switch (SAT(a_pOther)) {
		case eSATResults::SAT_AX:
			// Set plane matrix to obj1 model matrix
			planeMatrix = GetModelMatrix();
			planeMatrix[3].x = midPoint.x;
			planeMatrix[3].y = midPoint.y;
			planeMatrix[3].z = midPoint.z;

			// Increase plane size
			planeMatrix = glm::scale(planeMatrix, vector3(5.0f));

			// Rotate plane to obj1's X Axis
			planeMatrix = glm::rotate(planeMatrix, glm::radians(90.0f), AXIS_Y);
			m_pMeshMngr->AddPlaneToRenderList(planeMatrix, C_GREEN);
			// Flip to show plane from both sides
			planeMatrix = glm::rotate(planeMatrix, glm::radians(180.0f), AXIS_Y);
			m_pMeshMngr->AddPlaneToRenderList(planeMatrix, C_GREEN);
			// Plane found, cancel collision
			bColliding = false;
			break;
		case eSATResults::SAT_AY:
			// Set plane matrix to obj1 model matrix
			planeMatrix = GetModelMatrix();
			planeMatrix[3].x = midPoint.x;
			planeMatrix[3].y = midPoint.y;
			planeMatrix[3].z = midPoint.z;

			// Increase plane size
			planeMatrix = glm::scale(planeMatrix, vector3(5.0f));

			planeMatrix = glm::rotate(planeMatrix, glm::radians(90.0f), AXIS_X);
			m_pMeshMngr->AddPlaneToRenderList(planeMatrix, C_RED);
			planeMatrix = glm::rotate(planeMatrix, glm::radians(180.0f), AXIS_X);
			m_pMeshMngr->AddPlaneToRenderList(planeMatrix, C_RED);
			bColliding = false;
			break;
		case eSATResults::SAT_AZ:
			// Set plane matrix to obj1 model matrix
			planeMatrix = GetModelMatrix();
			planeMatrix[3].x = midPoint.x;
			planeMatrix[3].y = midPoint.y;
			planeMatrix[3].z = midPoint.z;

			// Increase plane size
			planeMatrix = glm::scale(planeMatrix, vector3(5.0f));


			m_pMeshMngr->AddPlaneToRenderList(planeMatrix, C_BLUE);
			// Flip to show plane from both sides
			planeMatrix = glm::rotate(planeMatrix, glm::radians(180.0f), AXIS_Y);
			m_pMeshMngr->AddPlaneToRenderList(planeMatrix, C_BLUE); 
			bColliding = false;
			break;
		case eSATResults::SAT_BX:
			// Set plane matrix to obj1 model matrix
			planeMatrix = a_pOther->GetModelMatrix();
			planeMatrix[3].x = midPoint.x;
			planeMatrix[3].y = midPoint.y;
			planeMatrix[3].z = midPoint.z;

			// Increase plane size
			planeMatrix = glm::scale(planeMatrix, vector3(5.0f));

			// Rotate plane to obj2's X Axis
			planeMatrix = glm::rotate(planeMatrix, glm::radians(90.0f), AXIS_Y);
			m_pMeshMngr->AddPlaneToRenderList(planeMatrix, C_YELLOW);
			// Flip to show plane from both sides
			planeMatrix = glm::rotate(planeMatrix, glm::radians(180.0f), AXIS_Y);
			m_pMeshMngr->AddPlaneToRenderList(planeMatrix, C_YELLOW);
			bColliding = false;
			break;
		case eSATResults::SAT_BY:
			// Set plane matrix to obj1 model matrix
			planeMatrix = a_pOther->GetModelMatrix();
			planeMatrix[3].x = midPoint.x;
			planeMatrix[3].y = midPoint.y;
			planeMatrix[3].z = midPoint.z;

			// Increase plane size
			planeMatrix = glm::scale(planeMatrix, vector3(5.0f));

			planeMatrix = glm::rotate(planeMatrix, glm::radians(90.0f), AXIS_X);
			m_pMeshMngr->AddPlaneToRenderList(planeMatrix, C_ORANGE);
			planeMatrix = glm::rotate(planeMatrix, glm::radians(180.0f), AXIS_X);
			m_pMeshMngr->AddPlaneToRenderList(planeMatrix, C_ORANGE);
			bColliding = false;
			break;
		case eSATResults::SAT_BZ:
			// Set plane matrix to obj1 model matrix
			planeMatrix = a_pOther->GetModelMatrix();
			planeMatrix[3].x = midPoint.x;
			planeMatrix[3].y = midPoint.y;
			planeMatrix[3].z = midPoint.z;

			// Increase plane size
			planeMatrix = glm::scale(planeMatrix, vector3(5.0f));

			m_pMeshMngr->AddPlaneToRenderList(planeMatrix, C_BLACK);
			// Flip to show plane from both sides
			planeMatrix = glm::rotate(planeMatrix, glm::radians(180.0f), AXIS_Y);
			m_pMeshMngr->AddPlaneToRenderList(planeMatrix, C_BLACK);
			bColliding = false;
			break;
		case eSATResults::SAT_AXxBX:
			bColliding = false;
			break;
		case eSATResults::SAT_AXxBY:
			bColliding = false;
			break;
		case eSATResults::SAT_AXxBZ:
			bColliding = false;
			break;
		case eSATResults::SAT_AYxBX:
			bColliding = false;
			break;
		case eSATResults::SAT_AYxBY:
			bColliding = false;
			break;
		case eSATResults::SAT_AYxBZ:
			bColliding = false;
			break;
		case eSATResults::SAT_AZxBX:
			bColliding = false;
			break;
		case eSATResults::SAT_AZxBY:
			bColliding = false;
			break;
		case eSATResults::SAT_AZxBZ:
			bColliding = false;
			break;
		case eSATResults::SAT_NONE:
			bColliding = true;
			break;
		default:
			break;
		}
	}

	if (bColliding) //they are colliding
	{
		// m_pMeshMngr->GeneratePlane(5.0f, C_RED);
		this->AddCollisionWith(a_pOther);
		a_pOther->AddCollisionWith(this);
	}
	else //they are not colliding
	{
		this->RemoveCollisionWith(a_pOther);
		a_pOther->RemoveCollisionWith(this);
	}

	return bColliding;
}
void MyRigidBody::AddToRenderList(void)
{
	if (m_bVisibleBS)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
		else
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
	}
	if (m_bVisibleOBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorColliding);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorNotColliding);
	}
	if (m_bVisibleARBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
	}
}

uint MyRigidBody::SAT(MyRigidBody* const a_pOther)
{	
	MyRigidBody* obj1 = this;
	MyRigidBody* obj2 = a_pOther;
	matrix4 m4Model1 = obj1->GetModelMatrix();
	matrix4 m4Model2 = obj2->GetModelMatrix();
	float ra, rb;
	matrix3 R, AbsR;

	// Initialize translation vector by getting the vector from obj1's center in global spaxe to obj2's center in global space
	vector3 translation = obj2->GetCenterGlobal() - obj1->GetCenterGlobal();
	// Use dot product to get translation in terms of a's coords
	translation = vector3(
		glm::dot(translation, vector3(m4Model1[0])),
		glm::dot(translation, vector3(m4Model1[1])),
		glm::dot(translation, vector3(m4Model1[2]))
	);

	// Compute obj2's rotation matrix in terms of a's coords
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			R[i][j] = glm::dot(m4Model1[i], m4Model2[j]);
		}
	}

	// Calc absolute values of the rotation matrix, add small float to account for near null cross products
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			AbsR[i][j] = abs(R[i][j]) + 0.001f;
		}
	}

		// Test axes L = A0, L = A1, L = A2
		// Test along x,y, and z axis in terms of a's coords
		for (int i = 0; i < 3; i++) {
			ra = obj1->m_v3HalfWidth[i];
			rb = obj2->m_v3HalfWidth[0] * AbsR[i][0] + obj2->m_v3HalfWidth[1] * AbsR[i][1] + obj2->m_v3HalfWidth[2] * AbsR[i][2];
			
			// Return matching separation axis
			if (abs(translation[i]) > ra + rb) {
				switch (i) {
					case 0:
						return eSATResults::SAT_AX;
					case 1:
						return eSATResults::SAT_AY;
					case 2:
						return eSATResults::SAT_AZ;
				}
			}
		}
		
		// Test axes L = B0, L = B1, L = B2
		// Test along x, y, and z axis in terms of b's coords
		for (int i = 0; i < 3; i++) {
			ra = obj1->m_v3HalfWidth[0] * AbsR[0][i] + obj1->m_v3HalfWidth[1] * AbsR[1][i] + obj1->m_v3HalfWidth[2] * AbsR[2][i];
			rb = obj2->m_v3HalfWidth[i];

			// Return matching separation axis
			if (abs(translation[0] * R[0][i] + translation[1] * R[1][i] + translation[2] * R[2][i]) > ra + rb) {
				switch (i) {
				case 0:
					return eSATResults::SAT_BX;
				case 1:
					return eSATResults::SAT_BY;
				case 2:
					return eSATResults::SAT_BZ;
				}
			}
		}

		// Test axis L = A0 x B0
		// Check along cross product of obj1's x axis and obj2's x axis
		ra = obj1->m_v3HalfWidth[1] * AbsR[2][0] + obj1->m_v3HalfWidth[2] * AbsR[1][0];
		rb = obj2->m_v3HalfWidth[1] * AbsR[0][2] + obj2->m_v3HalfWidth[2] * AbsR[0][1];

		if (abs(translation[2] * R[1][0] - translation[1] * R[2][0]) > ra + rb) 
			return eSATResults::SAT_AXxBX;

		// Test axis L = A0 x B1
		// Check along cross product of obj1's x axis and obj2's y axis
		ra = obj1->m_v3HalfWidth[1] * AbsR[2][1] + obj1->m_v3HalfWidth[2] * AbsR[1][1];
		rb = obj2->m_v3HalfWidth[0] * AbsR[0][2] + obj2->m_v3HalfWidth[2] * AbsR[0][0];

		if (abs(translation[2] * R[1][1] - translation[1] * R[2][1]) > ra + rb)
			return eSATResults::SAT_AXxBY;

		// Test axis L = A0 x B2
		// Check along cross product of obj1's x axis and obj2's z axis
		ra = obj1->m_v3HalfWidth[1] * AbsR[2][2] + obj1->m_v3HalfWidth[2] * AbsR[1][2];
		rb = obj2->m_v3HalfWidth[0] * AbsR[0][1] + obj2->m_v3HalfWidth[1] * AbsR[0][0];

		if (abs(translation[2] * R[1][2] - translation[1] * R[2][2]) > ra + rb)
			return eSATResults::SAT_AXxBZ;

		// Test axis L = A1 x B0
		// Check along cross product of obj1's y axis and obj2's x axis
		ra = obj1->m_v3HalfWidth[0] * AbsR[2][0] + obj1->m_v3HalfWidth[2] * AbsR[0][0];
		rb = obj2->m_v3HalfWidth[1] * AbsR[1][2] + obj2->m_v3HalfWidth[2] * AbsR[1][1];

		if (abs(translation[0] * R[2][0] - translation[2] * R[0][0]) > ra + rb)
			return eSATResults::SAT_AYxBX;

		// Test axis L = A1 x B1
		// Check along cross product of obj1's y axis and obj2's y axis
		ra = obj1->m_v3HalfWidth[0] * AbsR[2][1] + obj1->m_v3HalfWidth[2] * AbsR[0][1];
		rb = obj2->m_v3HalfWidth[0] * AbsR[1][2] + obj2->m_v3HalfWidth[2] * AbsR[1][0];

		if (abs(translation[0] * R[2][1] - translation[2] * R[0][1]) > ra + rb)
			return eSATResults::SAT_AYxBY;

		// Test axis L = A1 x B2
		// Check along cross product of obj1's y axis and obj2's z axis
		ra = obj1->m_v3HalfWidth[0] * AbsR[2][2] + obj1->m_v3HalfWidth[2] * AbsR[0][2];
		rb = obj2->m_v3HalfWidth[0] * AbsR[1][1] + obj2->m_v3HalfWidth[1] * AbsR[1][0];

		if (abs(translation[0] * R[2][2] - translation[2] * R[0][2]) > ra + rb)
			return eSATResults::SAT_AYxBZ;

		// Test axis L = A2 x B0
		// Check along cross product of obj1's z axis and obj2's x axis
		ra = obj1->m_v3HalfWidth[0] * AbsR[1][0] + obj1->m_v3HalfWidth[1] * AbsR[0][0];
		rb = obj2->m_v3HalfWidth[1] * AbsR[2][2] + obj2->m_v3HalfWidth[2] * AbsR[2][1];

		if (abs(translation[1] * R[0][0] - translation[0] * R[1][0]) > ra + rb)
			return eSATResults::SAT_AZxBX;

		// Test axis L = A2 x B1
		// Check along cross product of obj1's z axis and obj2's y axis
		ra = obj1->m_v3HalfWidth[0] * AbsR[1][1] + obj1->m_v3HalfWidth[1] * AbsR[0][1];
		rb = obj2->m_v3HalfWidth[0] * AbsR[2][2] + obj2->m_v3HalfWidth[2] * AbsR[2][0];

		if (abs(translation[1] * R[0][1] - translation[0] * R[1][1]) > ra + rb)
			return eSATResults::SAT_AZxBY;
		
		// Test axis L = A2 x B2
		// Check along cross product of obj1's z axis and obj2's z axis
		ra = obj1->m_v3HalfWidth[0] * AbsR[1][2] + obj1->m_v3HalfWidth[1] * AbsR[0][2];
		rb = obj2->m_v3HalfWidth[0] * AbsR[2][1] + obj2->m_v3HalfWidth[1] * AbsR[2][0];

		if (abs(translation[1] * R[0][2] - translation[0] * R[1][2]) > ra + rb)
			return eSATResults::SAT_AZxBZ;

	//No separating axis, objects colliding
	return eSATResults::SAT_NONE;
}
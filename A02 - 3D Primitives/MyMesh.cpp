#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	//Set some constants for calculations
	float halfHeight = a_fHeight * 0.5f;
	float radiansPerSubdivision = 2 * PI / a_nSubdivisions;

	//Get top and bottom points for drawing
	vector3 topPoint(0.0f, halfHeight, 0.0f);
	vector3 bottomPoint(0.0f, -halfHeight, 0.0f);

	//Iterate through subdivisions
	for (int i = 0; i < a_nSubdivisions; i++) {
		// Left point is at i * subdivision degress
		vector3 bottomLeftPoint(a_fRadius * cos(radiansPerSubdivision * i), -halfHeight, a_fRadius * sin(radiansPerSubdivision * i));
		// Right point is at (i+1) * subdivision degrees
		vector3 bottomRightPoint(a_fRadius * cos(radiansPerSubdivision * (i + 1)), -halfHeight, a_fRadius * sin(radiansPerSubdivision * (i + 1)));

		//Draw side tris
		AddTri(bottomRightPoint, bottomLeftPoint, topPoint);

		//Draw bottom tris
		AddTri(bottomLeftPoint, bottomRightPoint, bottomPoint);
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	float halfHeight = a_fHeight * 0.5f;
	float radiansPerSubdivision = 2 * PI / a_nSubdivisions;

	//Get top and bottom points for drawing
	vector3 topPoint(0.0f, halfHeight, 0.0f);
	vector3 bottomPoint(0.0f, -halfHeight, 0.0f);

	//Iterate through subdivisions
	for (int i = 0; i < a_nSubdivisions; i++) {
		// Left point is at i * subdivision degress
		vector3 topLeftPoint(a_fRadius * cos(radiansPerSubdivision * i), halfHeight, a_fRadius * sin(radiansPerSubdivision * i));
		// Right point is at (i+1) * subdivision degrees
		vector3 topRightPoint(a_fRadius * cos(radiansPerSubdivision * (i + 1)), halfHeight, a_fRadius * sin(radiansPerSubdivision * (i + 1)));

		vector3 bottomLeftPoint(a_fRadius * cos(radiansPerSubdivision * i), -halfHeight, a_fRadius * sin(radiansPerSubdivision * i));
		vector3 bottomRightPoint(a_fRadius * cos(radiansPerSubdivision * (i + 1)), -halfHeight, a_fRadius * sin(radiansPerSubdivision * (i + 1)));

		//Draw top tris
		AddTri(topRightPoint, topLeftPoint, topPoint);

		//Draw side quads
		AddQuad(topLeftPoint, topRightPoint, bottomLeftPoint, bottomRightPoint);

		//Draw bottom tris
		AddTri(bottomLeftPoint, bottomRightPoint, bottomPoint);
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	float halfHeight = a_fHeight * 0.5f;
	float radiansPerSubdivision = 2 * PI / a_nSubdivisions;

	//Iterate through subdivisions
	for (int i = 0; i < a_nSubdivisions; i++) {
		// Uses similar code from last two methods, get points for cylinders of two different radii
		vector3 innerTopLeftPoint(a_fInnerRadius * cos(radiansPerSubdivision * i), halfHeight, a_fInnerRadius * sin(radiansPerSubdivision * i));
		vector3 innerTopRightPoint(a_fInnerRadius * cos(radiansPerSubdivision * (i + 1)), halfHeight, a_fInnerRadius * sin(radiansPerSubdivision * (i + 1)));
		vector3 outerTopLeftPoint(a_fOuterRadius * cos(radiansPerSubdivision * i), halfHeight, a_fOuterRadius * sin(radiansPerSubdivision * i));
		vector3 outerTopRightPoint(a_fOuterRadius * cos(radiansPerSubdivision * (i + 1)), halfHeight, a_fOuterRadius * sin(radiansPerSubdivision * (i + 1)));

		//Bottom points are equal to top points with negative y
		vector3 innerBottomLeftPoint(innerTopLeftPoint.x, -innerTopLeftPoint.y, innerTopLeftPoint.z);
		vector3 innerBottomRightPoint(innerTopRightPoint.x, -innerTopRightPoint.y, innerTopRightPoint.z);
		vector3 outerBottomLeftPoint(outerTopLeftPoint.x, -outerTopLeftPoint.y, outerTopLeftPoint.z);
		vector3 outerBottomRightPoint(outerTopRightPoint.x, -outerTopRightPoint.y, outerTopRightPoint.z);

		//Draw top quads
		AddQuad(innerTopLeftPoint, innerTopRightPoint, outerTopLeftPoint, outerTopRightPoint);

		//Draw inner wall quads
		AddQuad(innerTopRightPoint, innerTopLeftPoint, innerBottomRightPoint, innerBottomLeftPoint);

		//Draw outer wall quads
		AddQuad(outerTopLeftPoint, outerTopRightPoint, outerBottomLeftPoint, outerBottomRightPoint);

		//Draw bottom quads
		AddQuad(outerBottomLeftPoint, outerBottomRightPoint, innerBottomLeftPoint, innerBottomRightPoint);
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	GenerateCube(a_fOuterRadius * 2.0f, a_v3Color);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	// Make an even number of subdivisions
	if (a_nSubdivisions % 2 != 0) {
		a_nSubdivisions++;
	}

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 6)
	{
		a_nSubdivisions = 6;
	}
	else if (a_nSubdivisions > 360) {
		a_nSubdivisions = 360;
	}

	Release();
	Init();

	//Radians per division constants
	float radiansPerSubdivision = 2.0f * PI / a_nSubdivisions;

	// Divide radius for each extending circle
	float radiusIncrement = a_fRadius / (a_nSubdivisions / 2.0f);

	vector3 topPoint(0.0f, a_fRadius, 0.0f);
	vector3 bottomPoint(0.0f, -a_fRadius, 0.0f);

	for (int i = 0; i < a_nSubdivisions / 2; i++) {
		//Create tris on the top and bottom
		if (i == 0) {
			for (int j = 0; j < a_nSubdivisions; j++) {
				// Start radians and end radians 
				float startRadians = radiansPerSubdivision * j;
				float endRadians = radiansPerSubdivision * (j + 1);
				//Radians for the y coord
				float vertRadians = (PI / 2) - (radiansPerSubdivision / 2);

				// X = Radius / Half of subdivisions * cos(radians)
				// Y = Radius * sin(theta)   And theta will be equal to PI/2 - radians
				// Z = Radius / Half of subdivisions * sin(radians)
				vector3 topTriBL(radiusIncrement * cos(startRadians), a_fRadius * sin(vertRadians), radiusIncrement * sin(startRadians));
				vector3 topTriBR(radiusIncrement * cos(endRadians), a_fRadius * sin(vertRadians), radiusIncrement * sin(endRadians));

				AddTri(topTriBR, topTriBL, topPoint);

				// Bottom is same, just flipped vertically

				vector3 botTriBL(topTriBL.x, -topTriBL.y, topTriBL.z);
				vector3 botTriBR(topTriBR.x, -topTriBR.y, topTriBR.z);

				AddTri(botTriBL, botTriBR, bottomPoint);
			}
		}
		else {
			// Create quads in between top and bottom
			for (int j = 0; j < a_nSubdivisions; j++) {
				// Inner and outer radii used for x and z calculations
				float innerRadius = i * radiusIncrement;
				float outerRadius = (i + 1) * radiusIncrement;
				// Start radians and end radians 
				float startRadians = radiansPerSubdivision * j;
				float endRadians = radiansPerSubdivision * (j + 1);

				float vertTopRadians = (PI / 2) - (radiansPerSubdivision * i / 2);
				float vertBottomRadians = (PI / 2) - (radiansPerSubdivision * (i + 1) / 2);

				//Create quad for top half of the sphere
				vector3 topLeftQuad1(innerRadius * cos(startRadians), a_fRadius * sin(vertTopRadians), innerRadius * sin(startRadians));
				vector3 topRightQuad1(innerRadius * cos(endRadians), a_fRadius * sin(vertTopRadians), innerRadius * sin(endRadians));
				vector3 bottomLeftQuad1(outerRadius * cos(startRadians), a_fRadius * sin(vertBottomRadians), outerRadius * sin(startRadians));
				vector3 bottomRightQuad1(outerRadius * cos(endRadians), a_fRadius * sin(vertBottomRadians), outerRadius * sin(endRadians));

				AddQuad(topLeftQuad1, topRightQuad1, bottomLeftQuad1, bottomRightQuad1);

				//Create matching lower quad
				vector3 topLeftQuad2(bottomLeftQuad1.x, -bottomLeftQuad1.y, bottomLeftQuad1.z);
				vector3 topRightQuad2(bottomRightQuad1.x, -bottomRightQuad1.y, bottomRightQuad1.z);
				vector3 bottomLeftQuad2(topLeftQuad1.x, -topLeftQuad1.y, topLeftQuad1.z);
				vector3 bottomRightQuad2(topRightQuad1.x, -topRightQuad1.y, topRightQuad1.z);

				AddQuad(topLeftQuad2, topRightQuad2, bottomLeftQuad2, bottomRightQuad2);
			}
		}
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
#include "AppClass.h"
void Application::InitVariables(void)
{
	//init the mesh
	m_pMesh = new MyMesh();
	m_pMesh->GenerateCube(1.0f, C_BLACK);
	//m_pMesh->GenerateSphere(1.0f, 5, C_WHITE);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();

	matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(1.0f, 1.0f, 1.0f));
	static float invaderOffset = -10.0f;

	std::vector<vector2> blocks;
	blocks.push_back(vector2(0.0f, 1.0f));
	blocks.push_back(vector2(0.0f, 2.0f));
	blocks.push_back(vector2(0.0f, 3.0f));
	blocks.push_back(vector2(1.0f, 3.0f));
	blocks.push_back(vector2(1.0f, 4.0f));
	blocks.push_back(vector2(2.0f, 1.0f));
	blocks.push_back(vector2(2.0f, 2.0f));
	blocks.push_back(vector2(2.0f, 3.0f));
	blocks.push_back(vector2(2.0f, 4.0f));
	blocks.push_back(vector2(2.0f, 5.0f));
	blocks.push_back(vector2(2.0f, 7.0f));
	blocks.push_back(vector2(3.0f, 0.0f));
	blocks.push_back(vector2(3.0f, 2.0f));
	blocks.push_back(vector2(3.0f, 3.0f));
	blocks.push_back(vector2(3.0f, 5.0f));
	blocks.push_back(vector2(3.0f, 6.0f));
	blocks.push_back(vector2(4.0f, 0.0f));
	blocks.push_back(vector2(4.0f, 2.0f));
	blocks.push_back(vector2(4.0f, 3.0f));
	blocks.push_back(vector2(4.0f, 4.0f));
	blocks.push_back(vector2(4.0f, 5.0f));
	blocks.push_back(vector2(5.0f, 2.0f));
	blocks.push_back(vector2(5.0f, 3.0f));
	blocks.push_back(vector2(5.0f, 4.0f));
	blocks.push_back(vector2(5.0f, 5.0f));
	blocks.push_back(vector2(6.0f, 0.0f));
	blocks.push_back(vector2(6.0f, 2.0f));
	blocks.push_back(vector2(6.0f, 3.0f));
	blocks.push_back(vector2(6.0f, 4.0f));
	blocks.push_back(vector2(6.0f, 5.0f));
	blocks.push_back(vector2(7.0f, 0.0f));
	blocks.push_back(vector2(7.0f, 2.0f));
	blocks.push_back(vector2(7.0f, 3.0f));
	blocks.push_back(vector2(7.0f, 5.0f));
	blocks.push_back(vector2(7.0f, 6.0f));
	blocks.push_back(vector2(8.0f, 1.0f));
	blocks.push_back(vector2(8.0f, 2.0f));
	blocks.push_back(vector2(8.0f, 3.0f));
	blocks.push_back(vector2(8.0f, 4.0f));
	blocks.push_back(vector2(8.0f, 5.0f));
	blocks.push_back(vector2(8.0f, 7.0f));
	blocks.push_back(vector2(9.0f, 3.0f));
	blocks.push_back(vector2(9.0f, 4.0f));
	blocks.push_back(vector2(10.0f, 1.0f));
	blocks.push_back(vector2(10.0f, 2.0f));
	blocks.push_back(vector2(10.0f, 3.0f));

	for (uint i = 0; i < blocks.size(); i++) {
		matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3(invaderOffset + blocks[i][0], blocks[i][1], 0.0f));
		matrix4 m4Model = m4Scale * m4Translate;
		m_pMesh->Render(m4Projection, m4View, m4Model);
	}

	// Increment offset
	invaderOffset += 0.01f;

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

	//draw gui
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	SafeDelete(m_pMesh);

	//release GUI
	ShutdownGUI();
}
#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	//m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));

	//Make MyMesh object
	m_pMesh = new MyMesh();
	m_pMesh->GenerateCube(1.0f, C_RED);

	//Make MyMesh object
	m_pMesh1 = new MyMesh();
	m_pMesh1->GenerateCube(1.0f, C_WHITE);
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

	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	static float fValue = 0.0f;

	//m_m4Object1 = glm::translate(vector3(1.0f, 1.0f, 0.0f)) * glm::rotate(IDENTITY_M4, fValue, AXIS_Z) * glm::translate(vector3(-1.0f, -1.0f, 0.0f));
	//fValue += 1.0f;

	//m_pMesh->Render(m4Projection, m4View, glm::translate(vector3(2.5f, 0.0f, 0.0f)));
	//m_pMesh->Render(m4Projection, m4View, m_m4Object1);
	for (float i = -3.0f; i < 3.0f; i++)
	{
		m_pMesh->Render(m4Projection, m4View, glm::translate(vector3(i, 0.0f, 0.0f)));
	}

	//m_pMesh1->Render(m4Projection, m4View, glm::translate(vector3( 3.0f, 0.0f, 0.0f)));
	
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
	if (m_pMesh != nullptr)
	{
		delete m_pMesh;
		m_pMesh = nullptr;
	}
	SafeDelete(m_pMesh1);
	//release GUI
	ShutdownGUI();
}
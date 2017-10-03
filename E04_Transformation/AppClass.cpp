#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	//m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";
	m_sProgrammer = "Tung Nguyen - thn4185@rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));

	//init the mesh
	for (uint i = 0; i < numObj; i++)
	{
		m_pMesh = new MyMesh();
		m_pMesh->GenerateCube(1.0f, C_BLACK);
		myList[i] = m_pMesh;
	}
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

	//Value to move objects horizontally
	static float valueX = 0.0f;

	//Get the coordinates for 4 cubes in the center
	for (float i = 0.0f; i < 4.0f; i++)
	{
		if (currObj < numObj)
		{
			myX[currObj] = 0;
			myY[currObj] = i;
			currObj++;
		}
	}

	//Get the coordinates for 42 other cubes
	for (float i = -2.0f; i < numRow - 2.0f; i++)
	{
		for (float j = 1.0f; j < numCol; j++)
		{
			if (currObj < numObj)
			{
				//Get coordinates of cubes by each row
				if (i == -2.0f && j <= 2.0f)
				{
					myX[currObj] = j;
					myY[currObj] = i;
					currObj++;

					myX[currObj] = -j;
					myY[currObj] = i;
					currObj++;
				}
				else if (i == -1.0f && j > 2.0f && j != 4.0f)
				{
					myX[currObj] = j;
					myY[currObj] = i;
					currObj++;

					myX[currObj] = -j;
					myY[currObj] = i;
					currObj++;
				}
				else if (i == 0.0f && j > 0.0f && j != 4.0f)
				{
					myX[currObj] = j;
					myY[currObj] = i;
					currObj++;

					myX[currObj] = -j;
					myY[currObj] = i;
					currObj++;
				}
				else if (i == 1.0f)
				{
					myX[currObj] = j;
					myY[currObj] = i;
					currObj++;

					myX[currObj] = -j;
					myY[currObj] = i;
					currObj++;
				}
				else if (i == 2.0f && j <= 4.0f && j != 2.0f)
				{
					myX[currObj] = j;
					myY[currObj] = i;
					currObj++;

					myX[currObj] = -j;
					myY[currObj] = i;
					currObj++;
				}
				else if (i == 3.0f && j <= 3.0f)
				{
					myX[currObj] = j;
					myY[currObj] = i;
					currObj++;

					myX[currObj] = -j;
					myY[currObj] = i;
					currObj++;
				}
				else if (i == 4.0f && j == 2.0f)
				{
					myX[currObj] = j;
					myY[currObj] = i;
					currObj++;

					myX[currObj] = -j;
					myY[currObj] = i;
					currObj++;
				}
				else if (i == 5.0f && j == 3.0f)
				{
					myX[currObj] = j;
					myY[currObj] = i;
					currObj++;

					myX[currObj] = -j;
					myY[currObj] = i;
					currObj++;
				}
			}
		}
	}

	//Set the translate matrix
	float offset = -5.0f; //Make the objects start at the left of the screen
	for (uint i = 0; i < numObj; i++)
	{
		translateList[i] = glm::translate(IDENTITY_M4, vector3(myX[i] + offset + valueX, myY[i], 0.0f));
		matrix4 m4Model = m4Scale * translateList[i];
		myList[i]->Render(m4Projection, m4View, m4Model);
	}
	valueX += 0.02f; //Make the objects move slowly from left to right
	
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
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

	// Replace this with your code
	vector3 midPoint(0, a_fHeight / 2, 0); //The point in the middle of the cone
	vector3 topPoint(0, midPoint.y, 0); //Top of the cone
	vector3 bottomPoint(0, -midPoint.y, 0); //Center of the base

	std::vector<vector3> pointsList; //List of other points on the base

	for (int i = 1; i < a_nSubdivisions + 1; i++)
	{
		float angle = 2.0f * 3.14f / a_nSubdivisions;

		//Get the coordinates of the points
		float a_x = a_fRadius * sin(angle * i);
		float a_z = a_fRadius * cos(angle * i);

		pointsList.push_back(vector3(a_x, -midPoint.y, a_z));
	}

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		AddTri(pointsList[(i + 1) % a_nSubdivisions], pointsList[i], bottomPoint); //Add the base
		AddTri(pointsList[i], pointsList[(i + 1) % a_nSubdivisions], topPoint); //Add the side triangles
	}

	// -------------------------------

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

	// Replace this with your code
	vector3 midPoint(0, a_fHeight / 2, 0); //The point in the middle of the cone
	vector3 topPoint(0, midPoint.y, 0); //Center of the top surface
	vector3 bottomPoint(0, -midPoint.y, 0); //Center of the bottom surface

	std::vector<vector3> topPointsList; //List of other points on the top surface
	std::vector<vector3> bottomPointsList; //List of other points on the bottom surface

	for (int i = 1; i < a_nSubdivisions + 1; i++)
	{
		float angle = 2.0f * 3.14f / a_nSubdivisions;

		//Get the coordinates of the points
		float a_x = a_fRadius * sin(angle * i);
		float a_z = a_fRadius * cos(angle * i);

		topPointsList.push_back(vector3(a_x, midPoint.y, a_z));
		bottomPointsList.push_back(vector3(a_x, -midPoint.y, a_z));
	}

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		AddTri(topPointsList[i], topPointsList[(i + 1) % a_nSubdivisions], topPoint); //Add the top
		AddTri(bottomPointsList[(i + 1) % a_nSubdivisions], bottomPointsList[i], bottomPoint); //Add the base
		AddQuad(bottomPointsList[i], bottomPointsList[(i + 1) % a_nSubdivisions], topPointsList[i], topPointsList[(i + 1) % a_nSubdivisions]); //Add the side quads
	}
	// -------------------------------

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

	// Replace this with your code
	vector3 midPoint(0, a_fHeight / 2, 0); //The point in the middle of the cone

	//Outer cone
	vector3 outerTopPoint(0, midPoint.y, 0); //Center of the top surface
	vector3 outerBottomPoint(0, -midPoint.y, 0); //Center of the bottom surface
	std::vector<vector3> topOuterPointsList; //List of other points on the top surface
	std::vector<vector3> bottomOuterPointsList; //List of other points on the bottom surface

	for (int i = 1; i < a_nSubdivisions + 1; i++)
	{
		float angle = 2.0f * 3.14f / a_nSubdivisions;

		//Get the coordinates of the points
		float a_x = a_fOuterRadius * sin(angle * i);
		float a_z = a_fOuterRadius * cos(angle * i);

		topOuterPointsList.push_back(vector3(a_x, midPoint.y, a_z));
		bottomOuterPointsList.push_back(vector3(a_x, -midPoint.y, a_z));
	}

	//Inner cone
	vector3 innerTopPoint(0, midPoint.y, 0); //Center of the top surface
	vector3 innerBottomPoint(0, -midPoint.y, 0); //Center of the bottom surface
	std::vector<vector3> topInnerPointsList; //List of other points on the top surface
	std::vector<vector3> bottomInnerPointsList; //List of other points on the bottom surface

	for (int i = 1; i < a_nSubdivisions + 1; i++)
	{
		float angle = 2.0f * 3.14f / a_nSubdivisions;

		//Get the coordinates of the points
		float a_x = a_fInnerRadius * sin(angle * i);
		float a_z = a_fInnerRadius * cos(angle * i);

		bottomInnerPointsList.push_back(vector3(a_x, -midPoint.y, a_z));
		topInnerPointsList.push_back(vector3(a_x, midPoint.y, a_z));
	}

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		AddQuad(topOuterPointsList[i], topOuterPointsList[(i + 1) % a_nSubdivisions], topInnerPointsList[i], topInnerPointsList[(i + 1) % a_nSubdivisions]); //Add the top quads
		AddQuad(bottomOuterPointsList[(i + 1) % a_nSubdivisions], bottomOuterPointsList[i], bottomInnerPointsList[(i + 1) % a_nSubdivisions], bottomInnerPointsList[i]); //Add the bottom quads
		AddQuad(bottomOuterPointsList[i], bottomOuterPointsList[(i + 1) % a_nSubdivisions], topOuterPointsList[i], topOuterPointsList[(i + 1) % a_nSubdivisions]); //Add the outer quads
		AddQuad(bottomInnerPointsList[(i + 1) % a_nSubdivisions], bottomInnerPointsList[i], topInnerPointsList[(i + 1) % a_nSubdivisions], topInnerPointsList[i]); //Add the inner quads
	}
	// -------------------------------

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

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	// Replace this with your code
	vector3 midPoint(0, 0, 0); //The point in the middle of the sphere
	vector3 topPoint(0, a_fRadius, 0); //Top point of the sphere
	vector3 bottomPoint(0, -a_fRadius, 0); //Bottom point of the sphere

	std::vector<vector3> topPointsList; //List of other points on the top surface
	std::vector<vector3> bottomPointsList; //List of other points on the bottom surface

	std::vector<std::vector<vector3>> midLists;

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		float angle = 2.0f * 3.14f / a_nSubdivisions;

		//Get the coordinates of the points
		float a_x = a_fRadius * sin(angle * i);
		float a_z = a_fRadius * cos(angle * i);
		topPointsList.push_back(vector3(a_x, a_fRadius / 1.5f, a_z));
		bottomPointsList.push_back(vector3(a_x, -a_fRadius / 1.5f, a_z));
	}

	if (a_nSubdivisions == 4)
	{
		std::vector<vector3> midPointsList;
		midLists.push_back(midPointsList);
		for (int i = 0; i < a_nSubdivisions; i++)
		{
			float angle = 2.0f * 3.14f / a_nSubdivisions;

			//Get the coordinates of the points
			float a_x = a_fRadius * sin(angle * i);
			float a_z = a_fRadius * cos(angle * i);
			midLists[0].push_back(vector3(a_x, 0, a_z));
			topPointsList[i].x /= 1.5f;
			topPointsList[i].z /= 1.5f;
			bottomPointsList[i].x /= 1.5f;
			bottomPointsList[i].z /= 1.5f;
		}
	}

	if (a_nSubdivisions == 5)
	{
		for (int i = 0; i < 2; i++)
		{
			std::vector<vector3> midPointsList;
			midLists.push_back(midPointsList);
		}
		for (int i = 0; i < a_nSubdivisions; i++)
		{
			float angle = 2.0f * 3.14f / a_nSubdivisions;

			//Get the coordinates of the points
			float a_x = a_fRadius * sin(angle * i);
			float a_z = a_fRadius * cos(angle * i);
			midLists[0].push_back(vector3(a_x, a_fRadius / 3.5f, a_z));
			midLists[1].push_back(vector3(a_x, -a_fRadius / 3.5f, a_z));
			topPointsList[i].x /= 1.5f;
			topPointsList[i].z /= 1.5f;
			bottomPointsList[i].x /= 1.5f;
			bottomPointsList[i].z /= 1.5f;
		}
	}

	if (a_nSubdivisions == 6)
	{
		for (int i = 0; i < 3; i++)
		{
			std::vector<vector3> midPointsList;
			midLists.push_back(midPointsList);
		}
		for (int i = 0; i < a_nSubdivisions; i++)
		{
			float angle = 2.0f * 3.14f / a_nSubdivisions;

			//Get the coordinates of the points
			float a_x = a_fRadius * sin(angle * i);
			float a_z = a_fRadius * cos(angle * i);
			midLists[0].push_back(vector3(a_x / 1.25f, a_fRadius / 3.0f, a_z / 1.25f));
			midLists[1].push_back(vector3(a_x, 0, a_z));
			midLists[2].push_back(vector3(a_x / 1.25f, -a_fRadius / 3.0f, a_z / 1.25f));
			topPointsList[i].x /= 1.75f;
			topPointsList[i].z /= 1.75f;
			bottomPointsList[i].x /= 1.75f;
			bottomPointsList[i].z /= 1.75f;
		}
	}

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		AddTri(topPointsList[i], topPointsList[(i + 1) % a_nSubdivisions], topPoint);
		AddTri(bottomPointsList[(i + 1) % a_nSubdivisions], bottomPointsList[i], bottomPoint);
		if (a_nSubdivisions < 3)
		{
			AddQuad(bottomPointsList[i], bottomPointsList[(i + 1) % a_nSubdivisions], topPointsList[i], topPointsList[(i + 1) % a_nSubdivisions]);
		}
		else if (a_nSubdivisions == 4)
		{
			AddQuad(bottomPointsList[i], bottomPointsList[(i + 1) % a_nSubdivisions], midLists[0][i], midLists[0][(i + 1) % a_nSubdivisions]);
			AddQuad(midLists[0][i], midLists[0][(i + 1) % a_nSubdivisions], topPointsList[i], topPointsList[(i + 1) % a_nSubdivisions]);
		}
		else if (a_nSubdivisions == 5)
		{
			AddQuad(bottomPointsList[i], bottomPointsList[(i + 1) % a_nSubdivisions], midLists[1][i], midLists[1][(i + 1) % a_nSubdivisions]);
			AddQuad(midLists[1][i], midLists[1][(i + 1) % a_nSubdivisions], midLists[0][i], midLists[0][(i + 1) % a_nSubdivisions]);
			AddQuad(midLists[0][i], midLists[0][(i + 1) % a_nSubdivisions], topPointsList[i], topPointsList[(i + 1) % a_nSubdivisions]);
		}
		else if (a_nSubdivisions == 6)
		{
			AddQuad(bottomPointsList[i], bottomPointsList[(i + 1) % a_nSubdivisions], midLists[2][i], midLists[2][(i + 1) % a_nSubdivisions]);
			AddQuad(midLists[2][i], midLists[2][(i + 1) % a_nSubdivisions], midLists[1][i], midLists[1][(i + 1) % a_nSubdivisions]);
			AddQuad(midLists[1][i], midLists[1][(i + 1) % a_nSubdivisions], midLists[0][i], midLists[0][(i + 1) % a_nSubdivisions]);
			AddQuad(midLists[0][i], midLists[0][(i + 1) % a_nSubdivisions], topPointsList[i], topPointsList[(i + 1) % a_nSubdivisions]);
		}
	}

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
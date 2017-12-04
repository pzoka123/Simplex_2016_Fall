#include "MyOctant.h"
using namespace Simplex;

void MyOctant::Init(void)
{
	m_uOctantCount = 0;
	m_uMaxLevel = 0;
	m_uIdealEntityCount = 0;

	m_uID = 0;
	m_uLevel = 0;
	m_uChildren = 0;

	m_fSize = 0.0f;

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	m_v3Center = vector3(0.0f);
	m_v3Min = vector3(0.0f);
	m_v3Max = vector3(0.0f);

	m_pParent = nullptr;
	m_pChild[8];

	m_EntityList;

	m_pRoot = nullptr;
	m_lChild;
}

void MyOctant::Release(void)
{
	m_pMeshMngr = nullptr;
	m_pEntityMngr = nullptr;
	ClearEntityList();
	KillBranches();
}

void MyOctant::ConstructList(void)
{
}


MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{

}

MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
}

MyOctant::MyOctant(MyOctant const & other)
{
}

MyOctant & MyOctant::operator=(MyOctant const & other)
{
	// TODO: insert return statement here
}

MyOctant::~MyOctant(void)
{
}

void MyOctant::Swap(MyOctant & other)
{
}

float MyOctant::GetSize(void) { return 0.0f; }

vector3 MyOctant::GetCenterGlobal(void) { return m_v3Center; }

vector3 MyOctant::GetMinGlobal(void) { return m_v3Min; }

vector3 MyOctant::GetMaxGlobal(void) { return m_v3Max; }

MyOctant * MyOctant::GetChild(uint a_nChild)
{
	if (a_nChild == 0)
	{
		return nullptr;
	}
	return m_pChild[a_nChild];
}

MyOctant * MyOctant::GetParent(void) { return m_pParent; }

uint MyOctant::GetOctantCount(void) { return m_uOctantCount; }

bool MyOctant::IsColliding(uint a_uRBIndex)
{
	return false;
}

void MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
}

void MyOctant::Display(vector3 a_v3Color)
{
}

void MyOctant::DisplayLeafs(vector3 a_v3Color)
{
}

void MyOctant::ClearEntityList(void)
{
}

void MyOctant::Subdivide(void)
{
}

bool MyOctant::IsLeaf(void)
{
	return false;
}

bool MyOctant::ContainsMoreThan(uint a_nEntities)
{
	return false;
}

void MyOctant::KillBranches(void)
{
}

void MyOctant::ConstructTree(uint a_nMaxLevel)
{
}

void MyOctant::AssignIDtoEntity(void)
{
}

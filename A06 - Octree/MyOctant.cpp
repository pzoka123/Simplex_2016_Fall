#include "MyOctant.h"
using namespace Simplex;

uint MyOctant::m_uOctantCount = 0;
uint MyOctant::m_uMaxLevel = 2;
uint MyOctant::m_uIdealEntityCount = 5;

void MyOctant::Init(void)
{
	m_uID = m_uOctantCount;
	m_uLevel = 0;
	m_uChildren = 0;

	m_fSize = 0.0f;

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	m_v3Center = vector3(0.0f);
	m_v3Min = vector3(0.0f);
	m_v3Max = vector3(0.0f);

	m_pParent = nullptr;
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i] = nullptr;
	}

	m_EntityList;

	m_pRoot = nullptr;
}

void MyOctant::Release(void)
{
	m_pMeshMngr = nullptr;
	m_pEntityMngr = nullptr;
	ClearEntityList();
	KillBranches();
}

MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	Init();

	m_pRoot = this;

	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;

	m_uOctantCount = 0;
	m_uID = m_uOctantCount;

	std::vector<vector3> bounds;
	uint uEntityNum = m_pEntityMngr->GetEntityCount();
	for (uint i = 0; i < uEntityNum; i++)
	{
		MyRigidBody* pRigidBody = m_pEntityMngr->GetRigidBody(i);

		vector3 v3RgbdMin = pRigidBody->GetMinGlobal();
		vector3 v3RgbdMax = pRigidBody->GetMaxGlobal();

		bounds.push_back(v3RgbdMin);
		bounds.push_back(v3RgbdMax);
	}

	MyRigidBody* pRgbd = new MyRigidBody(bounds);

	vector3 v3HalfWidth = pRgbd->GetHalfWidth();
	float fMax = v3HalfWidth.x;

	for (uint i = 1; i < 3; i++)
	{
		if (fMax < v3HalfWidth[i])
		{
			fMax = v3HalfWidth[i];
		}
	}

	vector3 v3Center = pRgbd->GetCenterGlobal();
	bounds.clear();
	SafeDelete(pRgbd);

	m_fSize = fMax * 2.0f;
	m_v3Center = v3Center;
	m_v3Min = m_v3Center - vector3(fMax);
	m_v3Max = m_v3Center + vector3(fMax);

	m_uOctantCount++;

	ConstructTree(a_nMaxLevel);
}

MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	Init();

	m_v3Center = a_v3Center;
	m_fSize = a_fSize;

	m_v3Min = m_v3Center - vector3(m_fSize / 2);
	m_v3Max = m_v3Center + vector3(m_fSize / 2);

	m_uOctantCount++;
}

MyOctant::MyOctant(MyOctant const & other)
{
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_uChildren = other.m_uChildren;

	m_fSize = other.m_fSize;

	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;

	m_pParent = other.m_pParent;
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i] = other.m_pChild[i];
	}

	for (uint i = 0; i < m_EntityList.size(); i++)
	{
		m_EntityList[i] = other.m_EntityList[i];
	}

	m_pRoot = other.m_pRoot;

	for (uint i = 0; i < m_lChild.size(); i++)
	{
		m_lChild = other.m_lChild;
	}
}

MyOctant & MyOctant::operator=(MyOctant const & other)
{
	// TODO: insert return statement here
	if (this != &other)
	{
		Release();
		Init();
		MyOctant temp(other);
		Swap(temp);
	}
	return *this;
}

MyOctant::~MyOctant(void)
{
	Release();
}

void MyOctant::Swap(MyOctant & other)
{
	std::swap(m_uID, other.m_uID);
	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_uChildren, other.m_uChildren);

	std::swap(m_fSize, other.m_fSize);

	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);

	std::swap(m_pParent, other.m_pParent);
	std::swap(m_pChild, other.m_pChild);
	for (uint i = 0; i < m_uChildren; i++)
	{
		std::swap(m_pChild[i], other.m_pChild[i]);
	}

	std::swap(m_EntityList, other.m_EntityList);
	for (uint i = 0; i < m_EntityList.size(); i++)
	{
		std::swap(m_EntityList[i], other.m_EntityList[i]);
	}

	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_lChild, other.m_lChild);
}

float MyOctant::GetSize(void) { return m_fSize; }
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
	MyRigidBody* pRigidbody = m_pEntityMngr->GetRigidBody(a_uRBIndex);

	vector3 v3RgbdMax = pRigidbody->GetMaxGlobal();
	vector3 v3RgbdMin = pRigidbody->GetMinGlobal();

	if (v3RgbdMax.x > m_v3Min.x
		&& v3RgbdMax.y > m_v3Min.y
		&& v3RgbdMax.z > m_v3Min.z
		&& v3RgbdMin.x < m_v3Max.x
		&& v3RgbdMin.y < m_v3Max.y
		&& v3RgbdMin.z < m_v3Max.z)
	{
		return true;
	}
	return false;
}

void MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
	m_lChild[a_nIndex]->Display(a_v3Color);
}

void MyOctant::Display(vector3 a_v3Color)
{
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->Display(a_v3Color);
	}
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color);
}

void MyOctant::DisplayLeafs(vector3 a_v3Color)
{
	if (IsLeaf())
	{
		Display(a_v3Color);
	}
	else
	{
		for (uint i = 0; i < m_uChildren; i++)
		{
			m_pChild[i]->DisplayLeafs(a_v3Color);
		}
	}
}

void MyOctant::ClearEntityList(void)
{
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->ClearEntityList();
	}

	m_EntityList.clear();
}

void MyOctant::Subdivide(void)
{
	if (m_uLevel >= m_uMaxLevel || m_uChildren != 0)
	{
		return;
	}

	m_uChildren = 8;

	float fSize = m_fSize / 4.0f;
	float fHalfSize = m_fSize / 2.0f;

	m_pChild[0] = new MyOctant(m_v3Center + vector3( fSize,  fSize,  fSize), fHalfSize); //RTF
	m_pChild[1] = new MyOctant(m_v3Center + vector3( fSize,  fSize, -fSize), fHalfSize); //RTB
	m_pChild[2] = new MyOctant(m_v3Center + vector3(-fSize,  fSize,  fSize), fHalfSize); //LTF
	m_pChild[3] = new MyOctant(m_v3Center + vector3(-fSize,  fSize, -fSize), fHalfSize); //LTB
	m_pChild[4] = new MyOctant(m_v3Center + vector3( fSize, -fSize,  fSize), fHalfSize); //RBF
	m_pChild[5] = new MyOctant(m_v3Center + vector3( fSize, -fSize, -fSize), fHalfSize); //RBB
	m_pChild[6] = new MyOctant(m_v3Center + vector3(-fSize, -fSize,  fSize), fHalfSize); //LBF
	m_pChild[7] = new MyOctant(m_v3Center + vector3(-fSize, -fSize, -fSize), fHalfSize); //LBB

	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->m_pRoot = m_pRoot;
		m_pChild[i]->m_pParent = this;
		m_pChild[i]->m_uLevel = m_uLevel + 1;

		if (m_pChild[i]->ContainsMoreThan(m_uIdealEntityCount))
		{
			m_pChild[i]->Subdivide();
		}
	}
}

bool MyOctant::IsLeaf(void)
{
	if (m_uChildren == 0)
	{
		return true;
	}
	return false;
}

bool MyOctant::ContainsMoreThan(uint a_nEntities)
{
	uint uCount = 0;
	uint uEntityNum = m_pEntityMngr->GetEntityCount();
	for (uint i = 0; i < uEntityNum; i++)
	{
		if (IsColliding(i))
		{
			uCount++;
		}
		if (uCount > a_nEntities)
		{
			return true;
		}
	}
	return false;
}

void MyOctant::KillBranches(void)
{
	if (IsLeaf())
	{
		return;
	}
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->KillBranches();
		SafeDelete(m_pChild[i]);
	}
	m_uChildren = 0;
}

void MyOctant::ConstructTree(uint a_nMaxLevel)
{
	if (m_uLevel != 0)
		return;

	m_uMaxLevel = a_nMaxLevel;
	m_uOctantCount = 1;
	
	m_EntityList.clear();
	KillBranches();
	m_lChild.clear();

	if (ContainsMoreThan(m_uIdealEntityCount))
	{
		Subdivide();
	}
	AssignIDtoEntity();
	ConstructList();
}

void MyOctant::ConstructList(void)
{
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->ConstructList();
	}
	if (m_EntityList.size() > 0)
	{
		m_lChild.push_back(this);
	}
}

void MyOctant::AssignIDtoEntity(void)
{
	if (IsLeaf())
	{
		uint uEntityNum = m_pEntityMngr->GetEntityCount();
		for (uint i = 0; i < uEntityNum; i++)
		{
			if (IsColliding(i))
			{
				m_EntityList.push_back(i);
				m_pEntityMngr->AddDimension(i, m_uID);
			}
		}
	}
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->AssignIDtoEntity();
	}
}

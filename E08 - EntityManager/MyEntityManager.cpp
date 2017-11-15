#include "MyEntityManager.h"
using namespace Simplex;
//  MyEntityManager
MyEntityManager* MyEntityManager::m_pInstance = nullptr;
void MyEntityManager::Init(void)
{
	m_uEntityCount = 0;
	m_entityList.clear();
}
void MyEntityManager::Release(void)
{
	for (uint i = 0; i < m_uEntityCount; i++)
	{
		SafeDelete(m_entityList[i]);
	}
	m_uEntityCount = 0;
	m_entityList.clear();
}
MyEntityManager* MyEntityManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new MyEntityManager();
	}
	return m_pInstance;
}
void MyEntityManager::ReleaseInstance()
{
	SafeDelete(m_pInstance);
}
int Simplex::MyEntityManager::GetEntityIndex(String a_sUniqueID)
{
	//Loop through the list to get the index of the entity with the corresponding ID
	for (uint i = 0; i < m_uEntityCount; i++)
	{
		if (a_sUniqueID == m_entityList[i]->GetUniqueID())
		{
			return i;
		}
	}
	return -1;
}
//Accessors
Model* Simplex::MyEntityManager::GetModel(uint a_uIndex)
{
	//If there is no entity in the list, return nothing
	if (m_uEntityCount == 0)
	{
		return nullptr;
	}

	//If the index is out of bounds, return the last one in the list
	if (a_uIndex >= m_uEntityCount || a_uIndex < 0)
	{
		a_uIndex = m_uEntityCount - 1;
	}
	return m_entityList[a_uIndex]->GetModel();
}
Model* Simplex::MyEntityManager::GetModel(String a_sUniqueID)
{
	//Create a temporary entity pointer based on the ID
	MyEntity* myEntity = MyEntity::GetEntity(a_sUniqueID);

	//If the entity exists
	if (myEntity)
	{
		return myEntity->GetModel();
	}
	return nullptr;
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(uint a_uIndex)
{
	//If there is no entity in the list, return nothing
	if (m_uEntityCount == 0)
	{
		return nullptr;
	}

	//If the index is out of bounds, return the last one in the list
	if (a_uIndex >= m_uEntityCount || a_uIndex < 0)
	{
		a_uIndex = m_uEntityCount - 1;
	}
	return m_entityList[a_uIndex]->GetRigidBody();
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(String a_sUniqueID)
{
	//Create a temporary entity pointer based on the ID
	MyEntity* myEntity = MyEntity::GetEntity(a_sUniqueID);

	//If the entity exists
	if (myEntity)
	{
		return myEntity->GetRigidBody();
	}
	return nullptr;
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	//If there is no entity in the list, return nothing
	if (m_uEntityCount == 0)
	{
		return IDENTITY_M4;
	}

	//If the index is out of bounds, return the last one in the list
	if (a_uIndex >= m_uEntityCount || a_uIndex < 0)
	{
		a_uIndex = m_uEntityCount - 1;
	}
	return m_entityList[a_uIndex]->GetModelMatrix();
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(String a_sUniqueID)
{
	//Create a temporary entity pointer based on the ID
	MyEntity* myEntity = MyEntity::GetEntity(a_sUniqueID);

	//If the entity exists
	if (myEntity)
	{
		return myEntity->GetModelMatrix();
	}
	return IDENTITY_M4;
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	//Create a temporary entity pointer based on the ID
	MyEntity* myEntity = MyEntity::GetEntity(a_sUniqueID);

	//If the entity exists
	if (myEntity)
	{
		myEntity->SetModelMatrix(a_m4ToWorld);
	}
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, uint a_uIndex)
{
	//If there is no entity in the list, return nothing
	if (m_uEntityCount == 0)
	{
		return;
	}

	//If the index is out of bounds, return the last one in the list
	if (a_uIndex >= m_uEntityCount || a_uIndex < 0)
	{
		a_uIndex = m_uEntityCount - 1;
	}
	m_entityList[a_uIndex]->SetModelMatrix(a_m4ToWorld);
}
//The big 3
MyEntityManager::MyEntityManager(){Init();}
MyEntityManager::MyEntityManager(MyEntityManager const& other){ }
MyEntityManager& MyEntityManager::operator=(MyEntityManager const& other) { return *this; }
MyEntityManager::~MyEntityManager(){Release();};
// other methods
void Simplex::MyEntityManager::Update(void)
{
	//Collision detection
	for (uint i = 0; i < m_uEntityCount - 1; i++)
	{
		for (uint j = i + 1; j < m_uEntityCount; j++)
		{
			m_entityList[i]->IsColliding(m_entityList[j]);
		}
	}
}
void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID)
{
	//Create a temporary entity pointer
	MyEntity* myEntity = new MyEntity(a_sFileName, a_sUniqueID);

	m_entityList.push_back(myEntity);
	m_uEntityCount = m_entityList.size();
}
void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{
	//If there is no entity in the list, return nothing
	if (m_uEntityCount == 0)
	{
		return;
	}

	//If the index is out of bounds, return the last one in the list
	if (a_uIndex >= m_uEntityCount || a_uIndex < 0)
	{
		a_uIndex = m_uEntityCount - 1;
	}

	//Create a temporary list
	std::vector<MyEntity*> myEntityList;

	//Copy all entities except the one to be removed from the main list to the temporary one
	for (uint i = 0; i < m_uEntityCount; i++)
	{
		myEntityList.push_back(m_entityList[i]);
		if (i == a_uIndex)
		{
			myEntityList.pop_back();
		}
	}

	//Clear the main list and add back the remaining entities to the list
	m_entityList.clear();
	for (uint i = 0; i < myEntityList.size(); i++)
	{
		m_entityList.push_back(myEntityList[i]);
		SafeDelete(myEntityList[i]);
	}
	myEntityList.clear();
	m_uEntityCount--; //Decrease the entity count
}
void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{ 
	//Get entity index from the ID
	int myEntityIndex = GetEntityIndex(a_sUniqueID);

	RemoveEntity((uint)myEntityIndex);
}
String Simplex::MyEntityManager::GetUniqueID(uint a_uIndex)
{
	//If there is no entity in the list, return nothing
	if (m_uEntityCount == 0)
	{
		return "";
	}

	//If the index is out of bounds, return the last one in the list
	if (a_uIndex >= m_uEntityCount)
	{
		a_uIndex = m_uEntityCount - 1;
	}
	return m_entityList[a_uIndex]->GetUniqueID();
}
MyEntity* Simplex::MyEntityManager::GetEntity(uint a_uIndex)
{
	//If there is no entity in the list, return nothing
	if (m_uEntityCount == 0)
	{
		return nullptr;
	}

	//If the index is out of bounds, return the last one in the list
	if (a_uIndex >= m_uEntityCount || a_uIndex < 0)
	{
		a_uIndex = m_uEntityCount - 1;
	}
	return m_entityList[a_uIndex];
}
void Simplex::MyEntityManager::AddEntityToRenderList(uint a_uIndex, bool a_bRigidBody)
{
	//If there is no entity in the list, add every entity to the render list
	if (a_uIndex >= m_uEntityCount || a_uIndex < 0)
	{
		for (uint i = 0; i < m_uEntityCount; i++)
		{
			m_entityList[i]->AddToRenderList(a_bRigidBody);
		}
	}
	else //Just add the specified one
	{
		m_entityList[a_uIndex]->AddToRenderList(a_bRigidBody);
	}
}
void Simplex::MyEntityManager::AddEntityToRenderList(String a_sUniqueID, bool a_bRigidBody)
{
	//Create a temporary entity pointer based on the ID
	MyEntity* myEntity = MyEntity::GetEntity(a_sUniqueID);

	//If the entity exists
	if (myEntity)
	{
		myEntity->AddToRenderList(a_bRigidBody);
	}
}
#include "LuaSystem.h"
#include "LuaEmbedder/LuaEmbedder.h"
#include "LuaComponent.h"
#include "../../NetworkInstance.h"
#include <assert.h>

namespace LuaBridge
{
	LuaSystem::LuaSystem(lua_State* L)
	{
	  m_L = nullptr;
	}

	void LuaSystem::Embed(lua_State* L)
	{
		LuaEmbedder::EmbedClass<LuaSystem>(L, "System", false);
		LuaEmbedder::EmbedClassFunction<LuaSystem>(L, "System", "AddComponentTypeToFilter", &LuaSystem::AddComponentTypeToFilter);
		LuaEmbedder::EmbedClassFunction<LuaSystem>(L, "System", "GetEntities", &LuaSystem::GetEntities);
		LuaEmbedder::EmbedClassFunction<LuaSystem>(L, "System", "GetComponent", &LuaSystem::GetComponent);
		LuaEmbedder::EmbedClassFunction<LuaSystem>(L, "System", "EntityHasComponent", &LuaSystem::EntityHasComponent);
		LuaEmbedder::EmbedClassFunction<LuaSystem>(L, "System", "InitializeNetworkEvents", &LuaSystem::InitializeNetworkEvents);

		LuaEmbedder::EmbedClassFunction<LuaSystem>(L, "System", "SetUpdateTaskCount", &LuaSystem::SetUpdateTaskCount);
		LuaEmbedder::EmbedClassFunction<LuaSystem>(L, "System", "SetEntitiesAddedTaskCount", &LuaSystem::SetEntitiesAddedTaskCount);
		LuaEmbedder::EmbedClassFunction<LuaSystem>(L, "System", "SetEntitiesRemovedTaskCount", &LuaSystem::SetEntitiesRemovedTaskCount);

		LuaEmbedder::EmbedClassFunction<LuaSystem>(L, "System", "SetName", &LuaSystem::SetName);

		LuaEmbedder::AddInt(L, "Mandatory", (int)ECSL::FilterType::Mandatory, "FilterType");
		LuaEmbedder::AddInt(L, "RequiresOneOf", (int)ECSL::FilterType::RequiresOneOf, "FilterType");
		LuaEmbedder::AddInt(L, "Excluded", (int)ECSL::FilterType::Excluded, "FilterType");
	}

	void LuaSystem::Update(const ECSL::RuntimeInfo& _runtime)
	{
		assert(m_L);
		
		if (LuaEmbedder::HasFunction<LuaSystem>(m_L, this, "Update"))
		{
			LuaEmbedder::PushFloat(m_L, _runtime.Dt);
			LuaEmbedder::PushInt(m_L, _runtime.TaskIndex);
			LuaEmbedder::PushInt(m_L, _runtime.TaskCount);
			LuaEmbedder::CallMethod<LuaSystem>(m_L, "System", "Update", this, 3);
		}
	}

	void LuaSystem::Initialize()
	{
		assert(m_L);
		
		if (LuaEmbedder::HasFunction<LuaSystem>(m_L, this, "Initialize"))
		{
			LuaEmbedder::CallMethod<LuaSystem>(m_L, "System", "Initialize", this);
		}
	}

	void LuaSystem::EntitiesAdded(const ECSL::RuntimeInfo& _runtime, const std::vector<unsigned int>& _entities)
	{
		assert(m_L);
		
		LuaEmbedder::PushFloat(m_L, _runtime.Dt);
		LuaEmbedder::PushInt(m_L, _runtime.TaskIndex);
		LuaEmbedder::PushInt(m_L, _runtime.TaskCount);
		LuaEmbedder::PushUnsignedIntArray(m_L, _entities.data(), _entities.size(), false);
		LuaEmbedder::CallMethod<LuaSystem>(m_L, "System", "EntitiesAdded", this, 4);
	}

	void LuaSystem::EntitiesRemoved(const ECSL::RuntimeInfo& _runtime, const std::vector<unsigned int>& _entities)
	{
		assert(m_L);
		
		LuaEmbedder::PushFloat(m_L, _runtime.Dt);
		LuaEmbedder::PushInt(m_L, _runtime.TaskIndex);
		LuaEmbedder::PushInt(m_L, _runtime.TaskCount);
		LuaEmbedder::PushUnsignedIntArray(m_L, _entities.data(), _entities.size(), false);
		LuaEmbedder::CallMethod<LuaSystem>(m_L, "System", "EntitiesRemoved", this, 4);
	}

	void LuaSystem::PostInitialize()
	{
		assert(m_L);
		
		if (LuaEmbedder::HasFunction<LuaSystem>(m_L, this, "PostInitialize"))
		{
			LuaEmbedder::CallMethod<LuaSystem>(m_L, "System", "PostInitialize", this);
		}
	}

	int LuaSystem::GetComponent(lua_State* L)
	{
		ECSL::DataLocation dataLocation;
		unsigned int entityId = (unsigned int)LuaEmbedder::PullInt(L, 1);
		std::string componentType = LuaEmbedder::PullString(L, 2);
		if (LuaEmbedder::IsInt(L, 3))
		{
			unsigned int index = (unsigned int)LuaEmbedder::PullInt(L, 3);
			dataLocation = System::GetComponent(entityId, componentType, index);
		}
		else
		{
			std::string variableName = LuaEmbedder::PullString(L, 3);
			dataLocation = System::GetComponent(entityId, componentType, variableName);
		}
		LuaComponent* component = new LuaComponent(dataLocation, this, entityId, componentType);
		LuaEmbedder::PushObject<LuaComponent>(L, "Component", component, true);
		return 1;
	}

	int LuaSystem::SetName(lua_State* L)
	{
		std::string systemName = LuaEmbedder::PullString(L, 1);
		SetSystemName(systemName);
		return 0;
	}

	int LuaSystem::AddComponentTypeToFilter(lua_State* L)
	{
		std::string componentType = LuaEmbedder::PullString(L, 1);
		ECSL::FilterType filterType = (ECSL::FilterType)LuaEmbedder::PullInt(L, 2);
		System::AddComponentTypeToFilter(componentType, filterType);
		return 0;
	}

	int LuaSystem::GetEntities(lua_State* L)
	{
		const std::vector<unsigned int>* entities = System::GetEntities();

		if (LuaEmbedder::IsString(L, 1))
		{
			std::string comp = LuaEmbedder::PullString(L, 1);
			unsigned int componentTypeId = ECSL::ComponentTypeManager::GetInstance().GetTableId(comp);

			unsigned int* selectedEntities = new unsigned int[entities->size()];

			int index = 0;
			for (int i = 0; i < entities->size(); ++i)
			{
				if (System::EntityHasComponent(entities->at(i), componentTypeId))
				{
					selectedEntities[index] = entities->at(i);
					++index;
				}
			}
			LuaEmbedder::PushUnsignedIntArray(L, selectedEntities, index);
		}
		else
		{
			LuaEmbedder::PushUnsignedIntArray(L, entities->data(), entities->size(), false);
		}

		return 1;
	}
	
	int LuaSystem::EntityHasComponent(lua_State* L)
	{
		unsigned int entityId = (unsigned int)LuaEmbedder::PullInt(L, 1);
		unsigned int componentTypeId = ECSL::ComponentTypeManager::GetInstance().GetTableId(LuaEmbedder::PullString(L, 2));
		LuaEmbedder::PushBool(L, System::EntityHasComponent(entityId, componentTypeId));
		return 1;
	}

	int LuaSystem::InitializeNetworkEvents(lua_State* L)
	{
		Network::NetEvent  hook;

		hook = std::bind(&LuaSystem::OnBannedFromServer, this, std::placeholders::_1, std::placeholders::_2);
		NetworkInstance::GetClient()->SetOnBannedFromServer(hook);

		hook = std::bind(&LuaSystem::OnConnectedToServer, this, std::placeholders::_1, std::placeholders::_2);
		NetworkInstance::GetClient()->SetOnConnectedToServer(hook);

		hook = std::bind(&LuaSystem::OnDisconnectedFromServer, this, std::placeholders::_1, std::placeholders::_2);
		NetworkInstance::GetClient()->SetOnDisconnectedFromServer(hook);

		hook = std::bind(&LuaSystem::OnFailedToConnect, this, std::placeholders::_1, std::placeholders::_2);
		NetworkInstance::GetClient()->SetOnFailedToConnect(hook);

		hook = std::bind(&LuaSystem::OnKickedFromServer, this, std::placeholders::_1, std::placeholders::_2);
		NetworkInstance::GetClient()->SetOnKickedFromServer(hook);

		hook = std::bind(&LuaSystem::OnPasswordInvalid, this, std::placeholders::_1, std::placeholders::_2);
		NetworkInstance::GetClient()->SetOnPasswordInvalid(hook);

		hook = std::bind(&LuaSystem::OnRemotePlayerBanned, this, std::placeholders::_1, std::placeholders::_2);
		NetworkInstance::GetClient()->SetOnRemotePlayerBanned(hook);

		hook = std::bind(&LuaSystem::OnRemotePlayerConnected, this, std::placeholders::_1, std::placeholders::_2);
		NetworkInstance::GetClient()->SetOnRemotePlayerConnected(hook);

		hook = std::bind(&LuaSystem::OnRemotePlayerDisconnected, this, std::placeholders::_1, std::placeholders::_2);
		NetworkInstance::GetClient()->SetOnRemotePlayerDisconnected(hook);

		hook = std::bind(&LuaSystem::OnRemotePlayerKicked, this, std::placeholders::_1, std::placeholders::_2);
		NetworkInstance::GetClient()->SetOnRemotePlayerKicked(hook);

		hook = std::bind(&LuaSystem::OnServerFull, this, std::placeholders::_1, std::placeholders::_2);
		NetworkInstance::GetClient()->SetOnServerFull(hook);

		hook = std::bind(&LuaSystem::OnTimedOutFromServer, this, std::placeholders::_1, std::placeholders::_2);
		NetworkInstance::GetClient()->SetOnTimedOutFromServer(hook);



		hook = std::bind(&LuaSystem::OnPlayerConnected, this, std::placeholders::_1, std::placeholders::_2);
		NetworkInstance::GetServer()->SetOnPlayerConnected(hook);

		hook = std::bind(&LuaSystem::OnPlayerDisconnected, this, std::placeholders::_1, std::placeholders::_2);
		NetworkInstance::GetServer()->SetOnPlayerDisconnected(hook);

		hook = std::bind(&LuaSystem::OnPlayerTimedOut, this, std::placeholders::_1, std::placeholders::_2);
		NetworkInstance::GetServer()->SetOnPlayerTimedOut(hook);

		return 0;
	}

	int LuaSystem::SetUpdateTaskCount(lua_State* L)
	{
		int updateTaskCount = LuaEmbedder::PullInt(L, 1);
		System::SetUpdateTaskCount(updateTaskCount);
		return 0;
	}
	int LuaSystem::SetEntitiesAddedTaskCount(lua_State* L)
	{
		int entitiesAddedTaskCount = LuaEmbedder::PullInt(L, 1);
		System::SetEntitiesAddedTaskCount(entitiesAddedTaskCount);
		return 0;
	}
	int LuaSystem::SetEntitiesRemovedTaskCount(lua_State* L)
	{
		int entitiesRemovedTaskCount = LuaEmbedder::PullInt(L, 1);
		System::SetEntitiesRemovedTaskCount(entitiesRemovedTaskCount);
		return 0;
	}

	void LuaSystem::OnBannedFromServer(Network::NetConnection _nc, const char* _message)
	{
		assert(m_L);
		
		if (LuaEmbedder::HasFunction<LuaSystem>(m_L, this, "OnBannedFromServer"))
		{
			LuaEmbedder::PushString(m_L, _nc.GetIpAddress());
			LuaEmbedder::PushInt(m_L, (int)_nc.GetPort());
			LuaEmbedder::PushString(m_L, "OnBannedFromServer");
			LuaEmbedder::CallMethod<LuaSystem>(m_L, "System", "OnBannedFromServer", this, 3);
		}
	}
	void LuaSystem::OnConnectedToServer(Network::NetConnection _nc, const char* _message)
	{
		assert(m_L);
		
		if (LuaEmbedder::HasFunction<LuaSystem>(m_L, this, "OnConnectedToServer"))
		{
			LuaEmbedder::PushString(m_L, _nc.GetIpAddress());
			LuaEmbedder::PushInt(m_L, (int)_nc.GetPort());
			LuaEmbedder::CallMethod<LuaSystem>(m_L, "System", "OnConnectedToServer", this, 2);
		}
	}
	void LuaSystem::OnDisconnectedFromServer(Network::NetConnection _nc, const char* _message)
	{
		assert(m_L);
		
		if (LuaEmbedder::HasFunction<LuaSystem>(m_L, this, "OnDisconnectedFromServer"))
		{
			LuaEmbedder::PushString(m_L, _nc.GetIpAddress());
			LuaEmbedder::PushInt(m_L, (int)_nc.GetPort());
			LuaEmbedder::CallMethod<LuaSystem>(m_L, "System", "OnDisconnectedFromServer", this, 2);
		}
	}
	void LuaSystem::OnFailedToConnect(Network::NetConnection _nc, const char* _message)
	{
		assert(m_L);
		
		if (LuaEmbedder::HasFunction<LuaSystem>(m_L, this, "OnFailedToConnect"))
		{
			LuaEmbedder::PushString(m_L, _nc.GetIpAddress());
			LuaEmbedder::PushInt(m_L, (int)_nc.GetPort());
			LuaEmbedder::CallMethod<LuaSystem>(m_L, "System", "OnFailedToConnect", this, 2);
		}
	}
	void LuaSystem::OnKickedFromServer(Network::NetConnection _nc, const char* _message)
	{
		assert(m_L);
		
		if (LuaEmbedder::HasFunction<LuaSystem>(m_L, this, "OnKickedFromServer"))
		{
			LuaEmbedder::PushString(m_L, _nc.GetIpAddress());
			LuaEmbedder::PushInt(m_L, (int)_nc.GetPort());
			LuaEmbedder::PushString(m_L, _message);
			LuaEmbedder::CallMethod<LuaSystem>(m_L, "System", "OnKickedFromServer", this, 3);
		}
	}
	void LuaSystem::OnPasswordInvalid(Network::NetConnection _nc, const char* _message)
	{
		assert(m_L);
		
		if (LuaEmbedder::HasFunction<LuaSystem>(m_L, this, "OnPasswordInvalid"))
		{
			LuaEmbedder::PushString(m_L, _nc.GetIpAddress());
			LuaEmbedder::PushInt(m_L, (int)_nc.GetPort());
			LuaEmbedder::CallMethod<LuaSystem>(m_L, "System", "OnPasswordInvalid", this, 2);
		}
	}
	void LuaSystem::OnRemotePlayerBanned(Network::NetConnection _nc, const char* _message)
	{
		assert(m_L);
		
		if (LuaEmbedder::HasFunction<LuaSystem>(m_L, this, "OnRemotePlayerBanned"))
		{
			LuaEmbedder::PushString(m_L, "OnRemotePlayerBanned");
			LuaEmbedder::CallMethod<LuaSystem>(m_L, "System", "OnRemotePlayerBanned", this, 1);
		}
	}
	void LuaSystem::OnRemotePlayerConnected(Network::NetConnection _nc, const char* _message)
	{
		assert(m_L);
		
		if (LuaEmbedder::HasFunction<LuaSystem>(m_L, this, "OnRemotePlayerConnected"))
		{
			LuaEmbedder::PushString(m_L, "OnRemotePlayerConnected");
			LuaEmbedder::CallMethod<LuaSystem>(m_L, "System", "OnRemotePlayerConnected", this, 1);
		}
	}
	void LuaSystem::OnRemotePlayerDisconnected(Network::NetConnection _nc, const char* _message)
	{
		assert(m_L);
		
		if (LuaEmbedder::HasFunction<LuaSystem>(m_L, this, "OnRemotePlayerDisconnected"))
		{
			LuaEmbedder::PushString(m_L, "OnRemotePlayerDisconnected");
			LuaEmbedder::CallMethod<LuaSystem>(m_L, "System", "OnRemotePlayerDisconnected", this, 1);

		}
	}
	void LuaSystem::OnRemotePlayerKicked(Network::NetConnection _nc, const char* _message)
	{
		assert(m_L);
		
		if (LuaEmbedder::HasFunction<LuaSystem>(m_L, this, "OnRemotePlayerKicked"))
		{
			LuaEmbedder::PushString(m_L, "OnRemotePlayerKicked");
			LuaEmbedder::CallMethod<LuaSystem>(m_L, "System", "OnRemotePlayerKicked", this, 1);
		}
	}
	void LuaSystem::OnServerFull(Network::NetConnection _nc, const char* _message)
	{
		assert(m_L);
		
		if (LuaEmbedder::HasFunction<LuaSystem>(m_L, this, "OnServerFull"))
		{
			LuaEmbedder::PushString(m_L, _nc.GetIpAddress());
			LuaEmbedder::PushInt(m_L, (int)_nc.GetPort());
			LuaEmbedder::CallMethod<LuaSystem>(m_L, "System", "OnServerFull", this, 2);
		}
	}
	void LuaSystem::OnTimedOutFromServer(Network::NetConnection _nc, const char* _message)
	{
		assert(m_L);
		
		if (LuaEmbedder::HasFunction<LuaSystem>(m_L, this, "OnTimedOutFromServer"))
		{
			LuaEmbedder::PushString(m_L, _nc.GetIpAddress());
			LuaEmbedder::PushInt(m_L, (int)_nc.GetPort());
			LuaEmbedder::CallMethod<LuaSystem>(m_L, "System", "OnTimedOutFromServer", this, 2);
		}
	}

	void LuaSystem::OnPlayerConnected(Network::NetConnection _nc, const char* _message)
	{
		assert(m_L);
		
		if (LuaEmbedder::HasFunction<LuaSystem>(m_L, this, "OnPlayerConnected"))
		{
			LuaEmbedder::PushString(m_L, _nc.GetIpAddress());
			LuaEmbedder::PushInt(m_L, (int)_nc.GetPort());
			LuaEmbedder::PushString(m_L, "OnPlayerConnected");
			LuaEmbedder::CallMethod<LuaSystem>(m_L, "System", "OnPlayerConnected", this, 3);
		}
	}
	void LuaSystem::OnPlayerDisconnected(Network::NetConnection _nc, const char* _message)
	{
		assert(m_L);
		
		if (LuaEmbedder::HasFunction<LuaSystem>(m_L, this, "OnPlayerDisconnected"))
		{
			LuaEmbedder::PushString(m_L, _nc.GetIpAddress());
			LuaEmbedder::PushInt(m_L, (int)_nc.GetPort());
			LuaEmbedder::PushString(m_L, "OnPlayerDisconnected");
			LuaEmbedder::CallMethod<LuaSystem>(m_L, "System", "OnPlayerDisconnected", this, 3);
		}
	}
	void LuaSystem::OnPlayerTimedOut(Network::NetConnection _nc, const char* _message)
	{
		assert(m_L);
		
		if (LuaEmbedder::HasFunction<LuaSystem>(m_L, this, "OnPlayerTimedOut"))
		{
			LuaEmbedder::PushString(m_L, _nc.GetIpAddress());
			LuaEmbedder::PushInt(m_L, (int)_nc.GetPort());
			LuaEmbedder::PushString(m_L, "OnPlayerTimedOut");
			LuaEmbedder::CallMethod<LuaSystem>(m_L, "System", "OnPlayerTimedOut", this, 3);
		}
	}
}
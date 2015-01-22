#ifndef LUASYSTEM_H
#define LUASYSTEM_H

#include "ECSL/Interfaces/System.h"
#include "Network/Packet.h"

#include <string>
#include <map>

namespace LuaBridge
{
  class LuaSystem : public ECSL::System
  {
  public:
    LuaSystem();
    ~LuaSystem() { }
    
    static void Embed();
    
    void Initialize();
	void Update(const ECSL::RuntimeInfo& _runtime);
	void EntitiesAdded(const ECSL::RuntimeInfo& _runtime, const std::vector<unsigned int>& _entities);
	void EntitiesRemoved(const ECSL::RuntimeInfo& _runtime, const std::vector<unsigned int>& _entities);
	void PostInitialize();
    
  private:
	int SetName();

    int GetComponent();
    
    int AddComponentTypeToFilter();
    int GetEntities();
    
    int EntityHasComponent();

	int InitializeNetworkEvents();

	int SetUpdateTaskCount();
	int SetEntitiesAddedTaskCount();
	int SetEntitiesRemovedTaskCount();

	void OnBannedFromServer(Network::NetConnection _nc, const char* _message);
	void OnConnectedToServer(Network::NetConnection _nc, const char* _message);
	void OnDisconnectedFromServer(Network::NetConnection _nc, const char* _message);
	void OnFailedToConnect(Network::NetConnection _nc, const char* _message);
	void OnKickedFromServer(Network::NetConnection _nc, const char* _message);
	void OnPasswordInvalid(Network::NetConnection _nc, const char* _message);
	void OnRemotePlayerBanned(Network::NetConnection _nc, const char* _message);
	void OnRemotePlayerConnected(Network::NetConnection _nc, const char* _message);
	void OnRemotePlayerDisconnected(Network::NetConnection _nc, const char* _message);
	void OnRemotePlayerKicked(Network::NetConnection _nc, const char* _message);
	void OnServerFull(Network::NetConnection _nc, const char* _message);
	void OnTimedOutFromServer(Network::NetConnection _nc, const char* _message);

	void OnPlayerConnected(Network::NetConnection _nc, const char* _message);
	void OnPlayerDisconnected(Network::NetConnection _nc, const char* _message);
	void OnPlayerTimedOut(Network::NetConnection _nc, const char* _message);
  };
}

#endif
#include <algorithm>
#include "GameCreator.h"
#include "Systems/MovementSystem.h"
#include "Systems/RenderSystem.h"
#include "Systems/CameraSystem.h"
#include "Systems/RotationSystem.h"
#include "Systems/ModelSystem.h"
#include "Systems/SyncEntitiesSystem.h"
#include "Systems/RenderRemoveSystem.h"
#include "Systems/ResetChangedSystem.h"
#include "Systems/PointlightSystem.h"
#include "Systems/DirectionalLightSystem.h"
#include "Systems/MasterServerSystem.h"
#include "Systems/SlerpRotationSystem.h"

#include "Network/NetworkInstance.h"
#include "ECSL/ECSL.h"
#include "ECSL/Managers/EntityTemplateManager.h"

#include "LuaBridge/ECSL/LuaSystem.h"
#include "LuaBridge/ECSL/LuaWorldCreator.h"
#include "LuaBridge/Renderer/LuaGraphicDevice.h"
#include "LuaBridge/ECSL/LuaEntityTemplateManager.h"
#include "LuaBridge/Network/LuaNetwork.h"

#include "Logger/Managers/Logger.h"

#include <iomanip>

GameCreator::GameCreator() :
m_graphics(0), m_input(0), m_clientWorld(0), m_serverWorld(0), m_worldProfiler(0), m_console(0), m_remoteConsole(0), m_consoleManager(Console::ConsoleManager::GetInstance()), m_frameCounter(new Utility::FrameCounter()), m_running(true),
m_graphicalSystems(std::vector<GraphicalSystem*>())
{
  
}

GameCreator::~GameCreator()
{
	NetworkInstance::DestroyClient();
    NetworkInstance::DestroyServer();
    NetworkInstance::DestroyClientNetworkHelper();
    NetworkInstance::DestroyServerNetworkHelper();

	if (m_clientWorld)
		delete m_clientWorld;
    
    if (m_serverWorld)
        delete m_serverWorld;

	if (m_graphics)
	{
		TextRenderer::Clean();
		delete m_graphics;
		SDL_Quit();
	}

	if (m_input)
		delete m_input;

	if (m_console)
		delete m_console;	

	if (m_remoteConsole)
		delete m_remoteConsole;

	if (m_worldProfiler)
		delete m_worldProfiler;

	LuaEmbedder::Quit();

	delete(&ECSL::ComponentTypeManager::GetInstance());
	delete(&ECSL::EntityTemplateManager::GetInstance());
	delete(m_frameCounter);
}

void GameCreator::InitializeGraphics()
{
#if defined(__IOS__) || defined(__ANDROID__)
	m_graphics = new Renderer::GraphicDevice();
    m_graphics->Init();
#else
    m_graphics = new Renderer::GraphicsHigh();
    if (!m_graphics->Init())
    {
        SDL_Log("Switching to OpenGL 4.0");
        m_graphics = new Renderer::GraphicsLow();
        m_graphics->Init();
    }
#endif
    
    
    
	
	//LuaEmbedder::AddObject<Renderer::GraphicDevice>("GraphicDevice", m_graphics, "graphics");
}

void GameCreator::InitializeConsole()
{
	//m_consoleManager->AddCommand("SpawnWorld", &SpawnShit);
}

void GameCreator::InitializeInput()
{
	m_input = &Input::InputWrapper::GetInstance();
}

void GameCreator::InitializeNetwork()
{
	NetworkInstance::InitClient();
	NetworkInstance::InitServer();
	NetworkInstance::InitClientNetworkHelper(&m_clientWorld);
    NetworkInstance::InitServerNetworkHelper(&m_serverWorld);

    Network::NetMessageHook hook = std::bind(&GameCreator::LuaPacket, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	NetworkInstance::GetClient()->AddNetworkHook("LuaPacket", hook);
	NetworkInstance::GetServer()->AddNetworkHook("LuaPacket", hook);

	hook = std::bind(&GameCreator::NetworkGameMode, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	NetworkInstance::GetClient()->AddNetworkHook("Gamemode", hook);

	m_remoteConsole = new RemoteConsole();

	InitializeNetworkEvents();

}

void GameCreator::InitializeNetworkEvents()
{
	NetworkInstance::GetClient()->ResetNetworkEvents();
	NetworkInstance::GetServer()->ResetNetworkEvents();

	Network::NetEvent netEvent = std::bind(&GameCreator::OnConnectedToServer, this, std::placeholders::_1, std::placeholders::_2);
	NetworkInstance::GetClient()->SetOnConnectedToServer(netEvent);

	Network::NetMessageHook hook = std::bind(&NetworkHelper::ReceiveEntityAll, NetworkInstance::GetClientNetworkHelper(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	NetworkInstance::GetClient()->AddNetworkHook("Entity", hook);

	hook = std::bind(&NetworkHelper::ReceiveEntityDelta, NetworkInstance::GetClientNetworkHelper(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	NetworkInstance::GetClient()->AddNetworkHook("EntityDelta", hook);

	hook = std::bind(&NetworkHelper::ReceiveEntityKill, NetworkInstance::GetClientNetworkHelper(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	NetworkInstance::GetClient()->AddNetworkHook("EntityKill", hook);
}

void GameCreator::InitializeThreads()
{
	MPL::TaskManager::GetInstance().CreateSlaves();
}

void GameCreator::InitializeLobby()
{
    GameMode("lobbythreaded");
    //InitializeWorld("lobbythreaded", WorldType::Client, true);
    //InitializeWorld("lobbythreaded", WorldType::Server, false);
}

void GameCreator::InitializeLua(WorldType _worldType)
{
    if (_worldType == WorldType::Client)
    {
        m_clientLuaState = LuaEmbedder::CreateState();
        LuaBridge::Embed(m_clientLuaState);
        
        LuaEmbedder::AddBool(m_clientLuaState, "Server", false);
        LuaEmbedder::AddBool(m_clientLuaState, "Client", true);
    }
    else
    {
        m_serverLuaState = LuaEmbedder::CreateState();
        LuaBridge::Embed(m_serverLuaState);
        
        LuaEmbedder::AddBool(m_serverLuaState, "Server", true);
        LuaEmbedder::AddBool(m_serverLuaState, "Client", false);
    }
}

void GameCreator::InitializeWorld(std::string _gameMode, WorldType _worldType, bool _isMainWorld)
{
    lua_State* luaState = _worldType == WorldType::Client ? m_clientLuaState : m_serverLuaState;

    
	LuaBridge::LuaWorldCreator worldCreator = LuaBridge::LuaWorldCreator(luaState);
    worldCreator.SkipComponentTypesAndTemplates(!_isMainWorld);
	LuaEmbedder::AddObject<LuaBridge::LuaWorldCreator>(luaState, "WorldCreator", &worldCreator, "worldCreator");

	std::stringstream gameMode;
#if defined(_DEBUG) && !defined(__ANDROID__) && !defined(__APPLE__)
	gameMode << "../../../Externals/content/scripting/";
#else
	gameMode << "content/scripting/";
#endif
	gameMode << _gameMode;
	gameMode << "/init.lua";

	if (!LuaEmbedder::Load(luaState, gameMode.str()))
		return;
	
	std::vector<LuaBridge::LuaSystem*>* systemsAdded = worldCreator.GetSystemsAdded();
	for (std::vector<LuaBridge::LuaSystem*>::iterator it = systemsAdded->begin(); it != systemsAdded->end(); it++)
	{
	  lua_State* luaStateCopy = LuaEmbedder::CreateChildState(luaState);
	  LuaBridge::Embed(luaStateCopy);
	  LuaEmbedder::CopyObject<LuaBridge::LuaSystem>(luaState, luaStateCopy, "System", (*it));
	  (*it)->SetLuaState(luaStateCopy);
	}
	systemsAdded->clear();

    
    if (_worldType == WorldType::Client)
    {
        LuaEmbedder::AddBool(m_clientLuaState, "Server", false);
        LuaEmbedder::AddBool(m_clientLuaState, "Client", true);
    }
    else
    {
        LuaEmbedder::AddBool(m_serverLuaState, "Server", true);
        LuaEmbedder::AddBool(m_serverLuaState, "Client", false);
    }
    
	LuaBridge::LuaGraphicDevice::SetGraphicDevice(m_graphics);
    LuaBridge::LuaGraphicDevice::SetLuaState(m_clientLuaState);
    
    LuaBridge::LuaNetwork::SetClientLuaState(m_clientLuaState);
    LuaBridge::LuaNetwork::SetServerLuaState(m_serverLuaState);
    
    
	m_gameMode = _gameMode;

	auto componentTypes = ECSL::ComponentTypeManager::GetInstance().GetComponentTypes();
	for (auto it = componentTypes->begin(); it != componentTypes->end(); ++it)
	{
		worldCreator.AddComponentType(it->second->GetName());
		int id = ECSL::ComponentTypeManager::GetInstance().GetTableId(it->second->GetName());
		printf("[#%d] %s\n", id, it->second->GetName().c_str());
	}

	/*	This component has to be added last!	*/
	unsigned int numberOfComponents = ECSL::ComponentTypeManager::GetInstance().GetComponentTypeCount() + 2;
	unsigned int numberOfInts = ECSL::BitSet::GetDataTypeCount(numberOfComponents);
	unsigned int numberOfBytes = numberOfInts*sizeof(ECSL::BitSet::DataType);
	std::map<std::string, ECSL::ComponentVariable> m_variables;
	ECSL::ComponentVariable start = ECSL::ComponentVariable("ChangedComponents", numberOfBytes);
	m_variables.insert(std::pair<std::string, ECSL::ComponentVariable>("ChangedComponents", start));
	std::map<unsigned int, ECSL::ComponentDataType> m_offsetToType;
	m_offsetToType[0] = ECSL::ComponentDataType::INT64;

    
    if (_isMainWorld)
    {
        ECSL::ComponentType* changedComponents = new ECSL::ComponentType("ChangedComponents", ECSL::TableType::Array, m_variables, m_offsetToType, false);
        ECSL::ComponentTypeManager::GetInstance().AddComponentType(*changedComponents);
        worldCreator.AddComponentType("ChangedComponents");

        ECSL::ComponentType* changedComponentsNetwork = new ECSL::ComponentType("ChangedComponentsNetwork", ECSL::TableType::Array, m_variables, m_offsetToType,    false);
        ECSL::ComponentTypeManager::GetInstance().AddComponentType(*changedComponentsNetwork);
        worldCreator.AddComponentType("ChangedComponentsNetwork");
    }
	//NetworkMessagesSystem* nms = new NetworkMessagesSystem();
	//nms->SetConsole(&m_consoleManager);
	
	m_graphicalSystems.clear();
	GraphicalSystem* graphicalSystem = 0;
	graphicalSystem = new PointlightSystem(m_graphics);
	m_graphicalSystems.push_back(graphicalSystem);
	
	worldCreator.AddSystemGroup();
	worldCreator.AddLuaSystemToCurrentGroup(new SlerpRotationSystem());
	worldCreator.AddSystemGroup();
	worldCreator.AddLuaSystemToCurrentGroup(graphicalSystem);


	graphicalSystem = new DirectionalLightSystem(m_graphics);
	m_graphicalSystems.push_back(graphicalSystem);
	worldCreator.AddSystemGroup();
	worldCreator.AddLuaSystemToCurrentGroup(graphicalSystem);

	worldCreator.AddSystemGroup();
	worldCreator.AddSystemToCurrentGroup<RotationSystem>();
    
    if (_worldType == WorldType::Client || _isMainWorld)
    {
        graphicalSystem = new CameraSystem(m_graphics);
        m_graphicalSystems.push_back(graphicalSystem);
        worldCreator.AddSystemGroup();
        worldCreator.AddLuaSystemToCurrentGroup(graphicalSystem);
        
        graphicalSystem = new ModelSystem(m_graphics);
        m_graphicalSystems.push_back(graphicalSystem);
        worldCreator.AddSystemGroup();
        worldCreator.AddLuaSystemToCurrentGroup(graphicalSystem);
    }
    
    if (_isMainWorld)
    {
        worldCreator.AddSystemGroup();
        worldCreator.AddSystemToCurrentGroup<MasterServerSystem>();
    }
    
    if (_worldType == WorldType::Server)
    {
        worldCreator.AddSystemGroup();
        worldCreator.AddSystemToCurrentGroup<SyncEntitiesSystem>();
    }
    
	//worldCreator.AddLuaSystemToCurrentGroup(new ReceivePacketSystem());
    
    if (_worldType == WorldType::Client || _isMainWorld)
    {
        graphicalSystem = new RenderSystem(m_graphics);
        m_graphicalSystems.push_back(graphicalSystem);
        worldCreator.AddSystemGroup();
        worldCreator.AddLuaSystemToCurrentGroup(graphicalSystem);
        //worldCreator.AddLuaSystemToCurrentGroup(new ReconnectSystem());
        graphicalSystem = new RenderRemoveSystem(m_graphics);
        m_graphicalSystems.push_back(graphicalSystem);
        worldCreator.AddSystemGroup();
        worldCreator.AddLuaSystemToCurrentGroup(graphicalSystem);
        worldCreator.AddSystemGroup();
        worldCreator.AddSystemToCurrentGroup<ResetChangedSystem>();
    }

    if (_worldType == WorldType::Client)
    {
        m_clientWorld = worldCreator.CreateWorld(1000);
		LuaEmbedder::CollectGarbageFull();
        LuaEmbedder::AddObject<ECSL::World>(luaState, "World", m_clientWorld, "world");
        m_clientWorld->PostInitializeSystems();
		LuaEmbedder::CollectGarbageFull();
    }
    else
    {
        m_serverWorld = worldCreator.CreateWorld(1000);
		LuaEmbedder::CollectGarbageFull();
        LuaEmbedder::AddObject<ECSL::World>(luaState, "World", m_serverWorld, "world");
        m_serverWorld->PostInitializeSystems();
		LuaEmbedder::CollectGarbageFull();
    }
	if (!m_worldProfiler)
		m_worldProfiler = new Profilers::ECSLProfiler(m_graphics);
}

void GameCreator::RunStartupCommands(int argc, char** argv)
{
	for (int i = 1; i < argc; ++i)
	{
		if (argv[i][0] == '-')
		{
			int start = i;
			int stop = i;

			for (++i; i < argc; ++i)
			{
				if (argv[i][0] == '-')
				{
					--i;
					break;
				}
				stop = i;
			}

			int size = 0;

			for (int j = start; j <= stop; ++j)
			{
				size += strlen(argv[j]);
			}

			size += stop - start;

			char* command = new char[size];
			memcpy(command, argv[start] + 1, strlen(argv[start]) - 1);
			int offset = strlen(argv[start]);
			command[offset - 1] = ' ';
			for (int j = start + 1; j <= stop; ++j)
			{
				memcpy(command + offset, argv[j], strlen(argv[j]));
				offset += strlen(argv[j]);

				if (start < stop)
					command[offset] = ' ';
			}

			command[size - 1] = '\0';
			Console::ConsoleManager::GetInstance().AddToCommandQueue(command);
		}
	}
}

void GameCreator::StartGame(int argc, char** argv)
{
	/*	If atleast one object is not initialized the game can't start	*/
	if (!m_graphics || !m_input)
		return;

	m_console = new GameConsole(m_graphics, m_serverWorld);

	m_consoleInput.SetTextHook(std::bind(&Console::ConsoleManager::AddToCommandQueue, &m_consoleManager, std::placeholders::_1));
	m_consoleInput.SetActive(false);
	m_input->GetKeyboard()->StopTextInput();

	/*	Hook console	*/
	m_console->SetupHooks(&m_consoleManager);
	m_consoleManager.AddCommand("Reload", std::bind(&GameCreator::ConsoleReload, this, std::placeholders::_1, std::placeholders::_2));
	m_consoleManager.AddCommand("Quit", std::bind(&GameCreator::ConsoleStopGame, this, std::placeholders::_1, std::placeholders::_2));
	m_consoleManager.AddCommand("GameMode", std::bind(&GameCreator::ConsoleGameMode, this, std::placeholders::_1, std::placeholders::_2));
	m_consoleManager.AddCommand("Start", std::bind(&GameCreator::ConsoleStartTemp, this, std::placeholders::_1, std::placeholders::_2));
	m_consoleManager.AddCommand("ChangeGraphics", std::bind(&GameCreator::ChangeGraphicsSettings, this, std::placeholders::_1, std::placeholders::_2));
	
    InitializeLobby();
    
	RunStartupCommands(argc, argv);
    
    //Console::ConsoleManager::GetInstance().AddToCommandQueue("connect 192.168.0.198");
	/* The GraphicsDevice should probably do this instead. New: int refreshrate = m_graphics->GetRefreshRate() */
	int refreshRate;
	SDL_DisplayMode mode;
	if (SDL_GetDisplayMode(0, 0, &mode) == 0)
		refreshRate = std::max(mode.refresh_rate, 60);
	else
	{
		refreshRate = 60;
		std::string message = "Couldn't fetch the refresh rate. Max frame rate is now forced to 60.\n";
		message += SDL_GetError();
		Logger::GetInstance().Log("Refresh rate", LogSeverity::Warning, message);
	}
    
	float maxDeltaTime = (float)(1.0f / 20.0f);
	float minDeltaTime = (1.0f / (float)refreshRate);
	float bytesToMegaBytes = 1.f / (1024.f*1024.f);
	bool showDebugInfo = false;
	Utility::FrameCounter totalCounter;

	while (m_running)
	{
        if (!(m_clientWorld || m_serverWorld) || !m_console)
        {
            break;
        }
        
		float dt = std::min(maxDeltaTime, m_frameCounter->GetDeltaTime());

		/* Enforce max fps by looping until max fps is reached. Alternative solution is worth looking at */
		while (dt < minDeltaTime)
		{
			m_frameCounter->Tick();
			dt += m_frameCounter->GetDeltaTime();
		}

		m_inputCounter.Reset();
		/*	Collect all input	*/
		m_input->Update();
		PollSDLEvent();
		m_consoleInput.Update();
		Console::ConsoleManager::GetInstance().ExecuteCommandQueue();
		UpdateConsole();
		if (m_input->GetKeyboard()->GetKeyState(SDL_SCANCODE_ESCAPE) == Input::InputState::PRESSED ||
		    m_input->GetKeyboard()->GetKeyState(SDL_SCANCODE_AC_BACK) == Input::InputState::PRESSED)
			break;
		m_inputCounter.Tick();

		m_worldCounter.Reset();
		/*	Update world (systems, entities, etc)	*/
		m_worldProfiler->Begin();
        
        if (m_serverWorld)
            m_serverWorld->Update(dt);
        
        if (m_clientWorld)
            m_clientWorld->Update(dt);
        
		m_worldProfiler->End();
		m_worldProfiler->Update(dt);
		m_worldProfiler->Render();
		m_worldCounter.Tick();

		m_luaGarbageCollectionCounter.Reset();
		LuaEmbedder::CollectGarbageForDuration(0.2f);
		m_luaGarbageCollectionCounter.Tick();

		m_networkCounter.Reset();
		UpdateNetwork(dt);
		m_networkCounter.Tick();

		/*	Update graphics	*/
		m_graphics->Update(dt);
		RenderConsole();
		m_graphicsCounter.Reset();
		m_graphics->Render();
		m_graphicsCounter.Tick();

		/*	DEBUG PRINT INFO	*/
		if (m_input->GetKeyboard()->GetKeyState(SDL_SCANCODE_Z) == Input::InputState::PRESSED)
			showDebugInfo = !showDebugInfo;

		if (m_input->GetKeyboard()->GetKeyState(SDL_SCANCODE_X) == Input::InputState::PRESSED)
			m_worldProfiler->Toggle();

		if (m_input->GetKeyboard()->GetKeyState(SDL_SCANCODE_C) == Input::InputState::PRESSED)
			m_worldProfiler->PreviousView();

		if (m_input->GetKeyboard()->GetKeyState(SDL_SCANCODE_V) == Input::InputState::PRESSED)
			m_worldProfiler->NextView();

		if (m_input->GetKeyboard()->GetKeyState(SDL_SCANCODE_F8) == Input::InputState::PRESSED)
			m_worldProfiler->LogDisplayedStatistics();

		if (m_input->GetKeyboard()->GetKeyState(SDL_SCANCODE_F1) == Input::InputState::PRESSED)
        {
            if (m_serverWorld)
                m_serverWorld->LogWorldData();
            if (m_clientWorld)
                m_clientWorld->LogWorldData();
        }
        

		if (showDebugInfo)
		{
			std::stringstream sstm;
			sstm << (int)(1 / dt) << " fps";
			m_graphics->RenderSimpleText(sstm.str(), 0, 0);

			std::stringstream vram;
			vram << "VRAM usage: " << (float)(m_graphics->GetVRamUsage()*bytesToMegaBytes) << " Mb ";
			m_graphics->RenderSimpleText(vram.str(), 20, 0);
			
			std::stringstream ss;
			ss << "Lua memory usage: " << LuaEmbedder::GetMemoryUsage() << " Kb";
			m_graphics->RenderSimpleText(ss.str(), 20, 1);

			m_graphics->RenderSimpleText("Time Statistics", 60, 0);
			PrintSectionTime("Total   ", m_frameCounter, 60, 1);
			PrintSectionTime("Input   ", &m_inputCounter, 60, 2);
			PrintSectionTime("World   ", &m_worldCounter, 60, 3);
			PrintSectionTime("Network ", &m_networkCounter, 60, 4);
			PrintSectionTime("Graphics", &m_graphicsCounter, 60, 5);
			PrintSectionTime("Lua GC  ", &m_luaGarbageCollectionCounter, 60, 6);
		}

		m_frameCounter->Tick();
	}
}

void GameCreator::UpdateNetwork(float _dt)
{
	Network::ServerNetwork* server = NetworkInstance::GetServer();
	if (server->IsRunning())
	{
		server->Update(_dt);
		while (server->PopAndExecutePacket() > 0) {}
	}

	Network::ClientNetwork* client = NetworkInstance::GetClient();
	if (client->IsConnected())
	{
		client->Update(_dt);
		while (client->PopAndExecutePacket() > 0) {}
	}
}

void GameCreator::GameMode(std::string _gamemode)
{
	if (_gamemode == "lobby")
		_gamemode = "lobbythreaded";
	m_gameMode = _gamemode;
	Reload();
}

void GameCreator::Reload()
{

    if (m_worldProfiler)
    {
        delete m_worldProfiler;
        m_worldProfiler = nullptr;
    }
    
    if (m_clientWorld)
    {
        delete m_clientWorld;
        m_clientWorld = nullptr;
    }
    
    if (m_serverWorld)
    {
        delete m_serverWorld;
        m_serverWorld = nullptr;
    }

		
	NetworkInstance::GetClientNetworkHelper()->ResetNetworkMaps();
    NetworkInstance::GetServerNetworkHelper()->ResetNetworkMaps();
	InitializeNetworkEvents();
	//bool server = LuaEmbedder::PullBool(m_serverLuaState, "Server");
	//bool client = LuaEmbedder::PullBool(m_clientLuaState, "Client");
	LuaEmbedder::Quit();
	ECSL::ComponentTypeManager::GetInstance().Clear();
	ECSL::EntityTemplateManager::GetInstance().Clear();

    if (NetworkInstance::GetClient()->IsConnected() && NetworkInstance::GetServer()->IsRunning())
    {
        InitializeLua(WorldType::Client);
        InitializeLua(WorldType::Server);
    }
    
    else if (NetworkInstance::GetServer()->IsRunning())
    {
        InitializeLua(WorldType::Server);
    }
    
    else
    {
        InitializeLua(WorldType::Client);
    }
    
	//InitializeLua();

	m_graphics->Clear();

	LuaBridge::LuaGraphicDevice::SetGraphicDevice(m_graphics);
    
    if (!NetworkInstance::GetClient()->IsConnected() && NetworkInstance::GetServer()->IsRunning())
    {
        LuaBridge::LuaGraphicDevice::SetLuaState(m_serverLuaState);
        LuaBridge::LuaEntityTemplateManager::SetLuaState(m_serverLuaState);
    }
    else
    {
        LuaBridge::LuaGraphicDevice::SetLuaState(m_clientLuaState);
        LuaBridge::LuaEntityTemplateManager::SetLuaState(m_clientLuaState);
    }

    
    LuaBridge::LuaNetwork::SetClientLuaState(m_clientLuaState);
    LuaBridge::LuaNetwork::SetServerLuaState(m_serverLuaState);

    if (NetworkInstance::GetClient()->IsConnected() && NetworkInstance::GetServer()->IsRunning())
    {
        InitializeWorld(m_gameMode, WorldType::Client, true);
        InitializeWorld(m_gameMode, WorldType::Server, false);
    }
    
    else if (NetworkInstance::GetServer()->IsRunning())
    {
        InitializeWorld(m_gameMode, WorldType::Server, true);
    }
    
    else
    {
        InitializeWorld(m_gameMode, WorldType::Client, true);
    }
    
	m_console->SetWorld(m_serverWorld);

	if (NetworkInstance::GetServer()->IsRunning())
	{
		Network::ServerNetwork* server = NetworkInstance::GetServer();
		Network::PacketHandler* ph = server->GetPacketHandler();
		uint64_t id = ph->StartPack("Gamemode");
		ph->WriteString(id, m_gameMode.c_str());
		NetworkInstance::GetServer()->Broadcast(ph->EndPack(id));
	}
}

void GameCreator::LuaPacket(Network::PacketHandler* _ph, uint64_t& _id, Network::NetConnection& _nc)
{
	std::ostringstream ss;
	ss << _id;

	char* function = _ph->ReadString(_id);
    
    lua_State* luaState = LuaEmbedder::GetFunctionLuaState(function);
    
    if (luaState)
    {
        LuaEmbedder::PushString(luaState, ss.str());
        LuaEmbedder::PushString(luaState, _nc.GetIpAddress());
        LuaEmbedder::PushInt(luaState, (int)_nc.GetPort());
        LuaEmbedder::CallSavedFunction(function, 3);
    }
}

void GameCreator::NetworkGameMode(Network::PacketHandler* _ph, uint64_t& _id, Network::NetConnection& _nc)
{
    if (!NetworkInstance::GetServer()->IsRunning())
        GameMode(_ph->ReadString(_id));
}

void GameCreator::UpdateConsole()
{
	/*	Toggle console	*/
	if (m_input->GetKeyboard()->GetKeyState(SDL_SCANCODE_GRAVE) == Input::InputState::PRESSED)
	{
		if (m_input->GetKeyboard()->IsTextInputActive())
		{
			m_input->GetKeyboard()->StopTextInput();
			m_consoleInput.SetActive(false);
			m_consoleManager.SetOpen(false);
		}
		else
		{
			m_input->GetKeyboard()->StartTextInput();
			m_consoleInput.SetActive(true);
			m_input->GetKeyboard()->ResetTextInput();
			m_consoleManager.SetOpen(true);
		}
		printf("%d average fps\n", m_frameCounter->GetAverageFPS());
	}

	// History, arrows up/down
	if (m_input->GetKeyboard()->GetKeyState(SDL_SCANCODE_UP) == Input::InputState::PRESSED)
	{

		auto previous = m_consoleManager.GetPreviousHistory();
		if(previous)
			m_input->GetKeyboard()->SetTextInput(previous);

	}
	else if (m_input->GetKeyboard()->GetKeyState(SDL_SCANCODE_DOWN) == Input::InputState::PRESSED)
	{
		auto next = m_consoleManager.GetNextHistory();
		if (next)
			m_input->GetKeyboard()->SetTextInput(next);
	}


	if (m_input->GetKeyboard()->GetKeyState(SDL_SCANCODE_PAGEUP) == Input::InputState::PRESSED)
		m_consoleManager.ScrollUp();
	else if (m_input->GetKeyboard()->GetKeyState(SDL_SCANCODE_PAGEDOWN) == Input::InputState::PRESSED)
		m_consoleManager.ScrollDown();


	if (m_input->GetKeyboard()->GetKeyState(SDL_SCANCODE_TAB) == Input::InputState::PRESSED)
	{
		auto match = m_consoleManager.GetMatch();

		if (match != "")
			m_input->GetKeyboard()->SetTextInput(match);
	}
}

void GameCreator::RenderConsole()
{
	if (!m_input->GetKeyboard()->IsTextInputActive())
		return;

	std::string command = m_consoleInput.GetText();
	m_graphics->RenderSimpleText("Console:", 0, 30);
	m_graphics->RenderSimpleText(command, 9, 30);
	m_graphics->RenderSimpleText("_", 9 + command.length(), 30);

	auto history = m_consoleManager.GetHistory();
	for (int i = 0; i < history.size(); ++i)
		m_graphics->RenderSimpleText(history[i], 0, 30 - history.size() + i);

	auto match = m_consoleManager.GetFunctionMatch(command.c_str());
	m_graphics->RenderSimpleText(match, 9, 31);

	Network::ClientNetwork* client = NetworkInstance::GetClient();

	if (client->IsConnected())
	{
		std::ostringstream ss;
		ss << "Ping: " << client->GetPing() << "ms";
		std::string s(ss.str());

		m_graphics->RenderSimpleText(s, 0, 32);
	}
}

void GameCreator::PollSDLEvent()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_WINDOWEVENT:
			m_graphics->PollEvent(e);
			break;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		case SDL_FINGERMOTION:
		case SDL_FINGERDOWN:
#ifdef __ANDROID__
			//exit(0);
#endif
		case SDL_FINGERUP:
		case SDL_TEXTINPUT:
		case SDL_JOYAXISMOTION:
		case SDL_JOYBALLMOTION:
		case SDL_JOYHATMOTION:
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
		case SDL_MOUSEMOTION:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEWHEEL:
		case SDL_MULTIGESTURE:
			m_input->PollEvent(e);
			break;
		}
	}
}

void GameCreator::ConsoleReload(std::string _command, std::vector<Console::Argument>* _args)
{
    if (NetworkInstance::GetServer()->IsRunning())
    {
        Reload();
    }
}

void GameCreator::ConsoleStopGame(std::string _command, std::vector<Console::Argument>* _args)
{
	m_running = false;
}

void GameCreator::OnConnectedToServer(Network::NetConnection _nc, const char* _message)
{
    if (!NetworkInstance::GetServer()->IsRunning())
        GameMode("storaspelthreaded");
}

void GameCreator::ConsoleGameMode(std::string _command, std::vector<Console::Argument>* _args)
{
    if (NetworkInstance::GetServer()->IsRunning() || !NetworkInstance::GetClient()->IsConnected())
    {
        if (_args->size() == 0)
        {
            GameMode("storaspelthreaded");
        }
        else if (_args->size() == 1)
        {
            if (_args->at(0).ArgType == Console::ArgumentType::Text)
            {
                GameMode(_args->at(0).Text);
            }
        }
    }
	return;
}

void GameCreator::ConsoleStartTemp(std::string _command, std::vector<Console::Argument>* _args)
{

}

void GameCreator::PrintSectionTime(const std::string& sectionName, Utility::FrameCounter* frameCounter, int x, int y)
{
	float average = 1000.f * frameCounter->GetAverageDeltaTime();
	float min = 1000.0f * frameCounter->GetMinDeltaTime();
	float max = 1000.0f * frameCounter->GetMaxDeltaTime();
	float anomality = 0.5f * ((average - min) + (max - average));
	
	std::stringstream ss;
	ss << sectionName <<
	  "   Average: " << std::fixed << std::setprecision(3) << average << " ms" <<
	  "   Anomality: " << std::fixed << std::setprecision(3) << anomality << " ms";
	m_graphics->RenderSimpleText(ss.str(), x, y);
}

void GameCreator::ChangeGraphicsSettings(std::string _command, std::vector<Console::Argument>* _args)
{
#if !defined(__IOS__) && !defined(__ANDROID__)
	if (_args->size() == 0)
		return;

	if ((*_args)[0].ArgType == Console::ArgumentType::Text)
	{
		for (int i = 0; i < 1000; ++i)
		{
			if (m_clientWorld->HasComponent(i, "Render"))
				m_clientWorld->RemoveComponentFrom("Render", i);
            
            if (m_serverWorld->HasComponent(i, "Render"))
                m_serverWorld->RemoveComponentFrom("Render", i);
		}

		if (strcmp((*_args)[0].Text, "high") == 0)
		{
			m_graphics->Clear();
			Camera tmpCam = *m_graphics->GetCamera();
			int windowx, windowy;
			m_graphics->GetWindowPos(windowx, windowy);

		//	SDL_Window*	tmpWindow = m_graphics->GetSDL_Window();
		//	SDL_GLContext* tmpContext = m_graphics->GetSDL_GLContext();*/
			delete(m_graphics);

			m_graphics = new Renderer::GraphicsHigh(tmpCam, windowx, windowy);
			m_graphics->Init();
		}

		else if (strcmp((*_args)[0].Text, "low") == 0)
		{
			m_graphics->Clear();
			Camera tmpCam				= *m_graphics->GetCamera();
			int windowx, windowy;
			m_graphics->GetWindowPos(windowx, windowy);
			//SDL_Window*	tmpWindow		=  m_graphics->GetSDL_Window();
			//SDL_GLContext tmpContext	=  m_graphics->GetSDL_GLContext();


			delete(m_graphics);

			m_graphics = new Renderer::GraphicsLow(tmpCam, windowx, windowy);
			m_graphics->Init();
		}
		if (m_input)
		{
			delete m_input;
			InitializeInput();
		}

			

		m_console->SetGraphicDevice(m_graphics);
		LuaBridge::LuaGraphicDevice::SetGraphicDevice(m_graphics);
		for (int n = 0; n < m_graphicalSystems.size(); ++n)
		{
			GraphicalSystem* tSystem = m_graphicalSystems.at(n);
			tSystem->SetGraphics(m_graphics);
		}
	}
#endif
}

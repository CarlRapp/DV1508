#include <SDL/SDL.h>
#include <ECSL/ECSL.h>
#include "Input/InputWrapper.h"
#include "Renderer/GraphicDevice.h"
#include "Network/ServerNetwork.h"
#include "Network/ClientNetwork.h"
#include "Console/Console.h"
#include "Timer.h"
#include "Game/TextInput.h"

#include <stdio.h>

#ifdef WIN32
	#include <stdlib.h>
	#include <crtdbg.h>
	#include <VLD/vld.h>
#endif

using namespace ECSL;
using namespace Console;

struct Player
{
	unsigned short Id = 0;
	bool Connected = false;
};

mat4 mat[1000];
Network::ServerNetwork* server = 0;
Network::ClientNetwork* client = 0;
Console::ConsoleManager consoleManager;
bool isServer = false;
bool isClient = false;
std::string newPlayer = "";
std::map<Network::NetConnection, Player*> m_players;
Renderer::GraphicDevice* RENDERER;
int lastModelId;

class TestSystem : public ECSL::System
{
public:
	TestSystem() { }
	~TestSystem() { }

	void Run(float _dt)
	{
		printf("Testsystem run()\n");
	}
	void Initialize()
	{
		AddComponentTypeToFilter("Velocity", ECSL::FilterType::Mandatory);
		AddComponentTypeToFilter("Position", ECSL::FilterType::Excluded);
		//AddComponentTypeToFilter("Velocity", ECSL::FilterType::RequiresOneOf);
		//AddComponentTypeToFilter("Velocity", ECSL::ComponentFilter::RequiresOneOf);
		//AddComponentTypeToFilter("Position", ECSL::ComponentFilter::Excluded);

		printf("Testsystem Initialize()\n");
	}

	void OnEntityAdded(unsigned int _entityId)
	{
		printf("Testsystem OnEntityAdded()\n");
	}
	void OnEntityRemoved(unsigned int _entityId)
	{
		printf("Testsystem OnEntityRemoved()\n");
	}
private:
};
class TestSystem2 : public ECSL::System
{
public:
	TestSystem2() { }
	~TestSystem2() { }

	void Run(float _dt)
	{
		printf("Testsystem2 run()\n");
	}
	void Initialize()
	{
		AddComponentTypeToFilter("Velocity", ECSL::FilterType::Mandatory);
		//AddComponentTypeToFilter("Velocity", ECSL::FilterType::RequiresOneOf);
		//AddComponentTypeToFilter("Velocity", ECSL::ComponentFilter::RequiresOneOf);
		//AddComponentTypeToFilter("Position", ECSL::ComponentFilter::Excluded);

		printf("Testsystem2 Initialize()\n");
	}

	void OnEntityAdded(unsigned int _entityId)
	{
		printf("Testsystem2 OnEntityAdded()\n");
	}
	void OnEntityRemoved(unsigned int _entityId)
	{
		printf("Testsystem2 OnEntityRemoved()\n");
	}
private:
};

void ClearConsole()
{
#ifdef WIN32
	system("cls");
#else
	system("clear");
#endif
}
void lol2()
{
	ComponentTypeManager::GetInstance().LoadComponentTypesFromDirectory("content/components");
	ECSL::WorldCreator worldCreator = ECSL::WorldCreator();
	worldCreator.AddSystemGroup();
	worldCreator.AddSystemToCurrentGroup<TestSystem2>();
	//worldCreator.AddLuaSystemToCurrentGroup(new TestSystem());
	auto componentTypes = ComponentTypeManager::GetInstance().GetComponentTypes();
	for (auto it = componentTypes->begin(); it != componentTypes->end(); ++it)
		worldCreator.AddComponentType(it->second->GetName());
	ECSL::World* world = worldCreator.CreateWorld(100);

	int id = world->CreateNewEntity();
	world->CreateComponentAndAddTo("Velocity", id);
	//world->CreateComponentAndAddTo("Position", id);
	//world->RemoveComponentFrom("Position", id);
	//world->CreateComponentAndAddTo("Position", id);

	//world->KillEntity(id);

	world->Update(0.01f);

	//world->KillEntity(id);
	world->CreateComponentAndAddTo("Position", id);

	world->Update(0.01f);

	world->RemoveComponentFrom("Position", id);

	world->Update(0.01f);

	delete(world);
	delete(&ComponentTypeManager::GetInstance());
}
void LoadAlotOfBoxes(Renderer::GraphicDevice* r)
{
	// ADDING TEMP OBJECTS
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			mat[y+x*10] = glm::translate(vec3(x - 5, -1, y - 5));
			r->LoadModel("content/models/head/", "head.object", &mat[y + x * 10]);
		}
	}
}
void OnConnected(Network::NetConnection nc)
{
	std::stringstream ss;
	ss << "connected to server " << nc.IpAddress << ":" << nc.Port << ".\n";
	newPlayer = ss.str();
}

void OnPlayerConnected(Network::NetConnection _nc)
{
	if (m_players.find(_nc) == m_players.end())
	{
		m_players[_nc] = new Player();
		m_players[_nc]->Connected = true;
		m_players[_nc]->Id = m_players.size();
	}

	Network::PacketHandler* ph = server->GetPacketHandler();
	uint64_t id = ph->StartPack("CubePos");
	ph->WriteFloat(id, mat[100][3][0]);
	ph->WriteFloat(id, mat[100][3][1]);
	ph->WriteFloat(id, mat[100][3][2]);
	Network::Packet* p = ph->EndPack(id);
	server->Send(p, _nc);


	std::stringstream ss;
	ss << _nc.IpAddress << ":" << _nc.Port << " connected to server.\n";
	newPlayer = ss.str();
}

void OnPlayerDisconnected(Network::NetConnection _nc)
{
	m_players[_nc]->Connected = false;
}

void OnKicked(Network::NetConnection _nc)
{
	client->Disconnect();
	printf("I was kicked from the server!\n");
}

void OnRemotePlayerKicked(Network::NetConnection _nc)
{
	printf("%s:%d was kicked from the server.\n", _nc.IpAddress, _nc.Port);
}

void CubePos(Network::PacketHandler* _ph, uint64_t _id, Network::NetConnection _nc)
{
	mat[100][3][0] = _ph->ReadFloat(_id);
	mat[100][3][1] = _ph->ReadFloat(_id);
	mat[100][3][2] = _ph->ReadFloat(_id);
}

void KickPlayer(std::vector<Argument>* _args)
{
	if (m_players.size() == 0)
		return;

	auto firstplayer = m_players.begin();
	server->Kick(firstplayer->first, "Why not?");
	m_players.erase(firstplayer->first);
}

void AddObject(std::vector<Argument>* _args)
{
	if (_args->at(0).ArgType != ArgumentType::Text)
		return;

	std::string modelName = _args->at(0).Text;
	modelName.append(".object");
	
	std::string folderPath;
	if (strcmp(_args->at(0).Text,"cube") == 0)
		folderPath = "content/models/cube/";
	else if (strcmp(_args->at(0).Text, "default") == 0)
		folderPath = "content/models/default_tile/";
	else if (strcmp(_args->at(0).Text, "spelpjaas") == 0)
		folderPath = "content/models/gamebrick/";
	else if (strcmp(_args->at(0).Text, "head") == 0)
		folderPath = "content/models/head/";

	vec3 position;


	if (_args->size() < 4 ||
		(_args->at(1).ArgType != ArgumentType::Number || _args->at(2).ArgType != ArgumentType::Number || _args->at(3).ArgType != ArgumentType::Number))
		position = vec3(0, 0, 0);
	else
		position = vec3(_args->at(1).Number, _args->at(2).Number, _args->at(3).Number);

	lastModelId = RENDERER->LoadModel(folderPath, modelName, &mat[lastModelId + 1]); // LOADMODEL RETURNS THE MODELID
	mat[lastModelId] = glm::translate(position);
	
}

void MoveCube(std::vector<Argument>* _vec)
{
	if (_vec->size() != 3)
		return;

	for (int i = 0; i < 3; ++i)
	{
		if (_vec->at(i).ArgType != ArgumentType::Number)
			return;
	}

	mat[100][3][0] = _vec->at(0).Number;
	mat[100][3][1] = _vec->at(1).Number;
	mat[100][3][2] = _vec->at(2).Number;

	Network::PacketHandler* ph = server->GetPacketHandler();
	uint64_t id = ph->StartPack("CubePos");
	ph->WriteFloat(id, mat[100][3][0]);
	ph->WriteFloat(id, mat[100][3][1]);
	ph->WriteFloat(id, mat[100][3][2]);
	Network::Packet* p = ph->EndPack(id);
	server->Broadcast(p);
}

void Disconnect()
{
	isClient = false;
	if (client)
		client->Disconnect();
}

void OnDisconnected(Network::NetConnection nc)
{
	isClient = false;
}

void Stop()
{
	consoleManager.RemoveCommand("movecube");
	consoleManager.RemoveCommand("kick");
	isServer = false;
	if (server)
		server->Stop();
}

bool Host(short _port)
{
	if (!isServer)
	{
		if (isClient)
		{
			Disconnect();
		}
		SAFE_DELETE(server);
		server = new Network::ServerNetwork();
		server->SetOnPlayerConnected(&OnPlayerConnected);
		server->SetOnPlayerDisconnected(&OnPlayerDisconnected);
		server->SetOnPlayerTimedOut(&OnPlayerDisconnected);
		if (server->Start(_port, "localhest", 8))
		{
			consoleManager.AddCommand("movecube", &MoveCube);
			consoleManager.AddCommand("kick", &KickPlayer);
			isServer = true;
			return true;
		}
		else
		{
			SAFE_DELETE(server);
		}
	}
	return false;
}

void Stop_Hook(std::vector<Argument>* _vec)
{
	Stop();
}


bool Connect(char* _ip, short _port)
{
	if (!isClient)
	{
		if (isServer)
		{
			Stop();
		}
		SAFE_DELETE(client);
		client = new Network::ClientNetwork();
		client->AddNetworkHook("CubePos", &CubePos);
		client->SetOnConnectedToServer(&OnConnected);
		client->SetOnDisconnectedFromServer(&OnDisconnected);
		client->SetOnKickedFromServer(&OnKicked);
		client->SetOnRemotePlayerKicked(&OnRemotePlayerKicked);
		if (client->Connect(_ip, "localhest", _port, 0))
		{
			isClient = true;
			return true;
		}
		else
		{
			SAFE_DELETE(client);
		}
	}
	return false;
}

void Disconnect_Hook(std::vector<Argument>* _vec)
{
	Disconnect();
}

void Connect_Hook(std::vector<Argument>* _vec)
{
	if (_vec->size() == 0)
		Connect("127.0.0.1", 6112);

	if (_vec->size() == 1)
	{
		if (_vec->at(0).ArgType == ArgumentType::Text)
			Connect((char*)_vec->at(0).Text, 6112);
	}

	if (_vec->size() == 2)
	{
		if (_vec->at(0).ArgType == ArgumentType::Text && _vec->at(1).ArgType == ArgumentType::Number)
			Connect((char*)_vec->at(0).Text, (short)_vec->at(1).Number);
	}
	
}

void Host_Hook(std::vector<Argument>* _vec)
{
	if (_vec->size() == 0)
		Host(6112);
	else if (_vec->size() == 1)
	{
		if (_vec->at(0).ArgType == ArgumentType::Number)
			Host((short)_vec->at(0).Number);
	}
}

bool lol;

void Quit_Hook(std::vector<Argument>* _vec)
{
	lol = false;
}

void Start()
{
	std::string input;

	consoleManager.AddCommand("connect", &Connect_Hook);
	consoleManager.AddCommand("disconnect", &Disconnect_Hook);
	consoleManager.AddCommand("host", &Host_Hook);
	consoleManager.AddCommand("stop", &Stop_Hook);
	consoleManager.AddCommand("quit", &Quit_Hook);
	consoleManager.AddCommand("addobject", &AddObject);

	RENDERER = new Renderer::GraphicDevice();
	/*	Initialize Renderer and Input	*/
	RENDERER->Init();

	Input::InputWrapper* INPUT = &Input::InputWrapper::GetInstance();

	LoadAlotOfBoxes(RENDERER);
	int modelid = RENDERER->LoadModel("content/models/gamebrick/", "spelpjaas.object", &mat[100]); // LOADMODEL RETURNS THE MODELID
	mat[modelid] = glm::translate(vec3(0, 0, 0));
	lastModelId = modelid;

	lol = true;
	lol2();
	float cd = 1.0f;
	Timer timer;

	TextInput ti;
	ti.SetTextHook(std::bind(&Console::ConsoleManager::ExecuteCommand, &consoleManager, std::placeholders::_1));
	ti.SetActive(true);

	while (lol)
	{
		// DT COUNTER
		float dt = timer.ElapsedTimeInSeconds();
		timer.Reset();

		float tBytesReceived = 0;
		float tBytesSent = 0;

		float cBytesReceived = 0;
		float cBytesSent = 0;

		unsigned int ping = 0;

		if (isServer)
		{
			server->Update(dt);
			while (server->TriggerPacket() > 0) {}

			tBytesReceived = server->GetTotalBytesReceived() / 1024;
			tBytesSent = server->GetTotalBytesSent() / 1024;

			cBytesReceived = server->GetCurrentBytesReceived() / 1024;
			cBytesSent = server->GetCurrentBytesSent() / 1024;

		}

		if (isClient)
		{
			client->Update(dt);
			while (client->TriggerPacket() > 0) {}

			tBytesReceived = client->GetTotalBytesReceived() / 1024;
			tBytesSent = client->GetTotalBytesSent() / 1024;

			cBytesReceived = client->GetCurrentBytesReceived() / 1024;
			cBytesSent = client->GetCurrentBytesSent() / 1024;

			ping = client->GetPing();
		}

		INPUT->Update();
		RENDERER->Update(dt);


		char buffer[256];
#ifdef WIN32
		sprintf_s(buffer, "Network usage:\nPing: %ums\nTotal received: %1.2f Kb\nTotal sent: %1.2f Kb\nCurrent received: %f Kb\nCurrent sent: %f Kb", ping, tBytesReceived, tBytesSent, cBytesReceived, cBytesSent);
#else
		sprintf(buffer, "Network usage:\nPing: %umsTotal received: %1.2f Kb\nTotal sent: %1.2f Kb\nCurrent received: %1.2f Kb\nCurrent sent: %1.2f Kb", ping, tBytesReceived, tBytesSent, cBytesReceived, cBytesSent);
#endif

		//std::string networkData = "Network usage:\nTotal received: " + std::to_string(tBytesReceived) + " Kb\nTotal sent: " + std::to_string(tBytesSent) + " Kb\nCurrent received: " + std::to_string(cBytesReceived) + " Kb\nCurrent sent: " + std::to_string(cBytesSent) + " Kb";
		RENDERER->RenderSimpleText(buffer, 0, 2);

		if (INPUT->GetKeyboard()->IsTextInputActive())
		{
			RENDERER->RenderSimpleText("Console:", 0, 9);
			RENDERER->RenderSimpleText(ti.GetText(), 0, 10);
		}

		RENDERER->Render();
		INPUT->Update();


		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_WINDOWEVENT:
				RENDERER->PollEvent(e);
				break;
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			case SDL_TEXTINPUT:
			case SDL_FINGERMOTION:
			case SDL_FINGERDOWN:
			case SDL_FINGERUP:
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
				INPUT->PollEvent(e);
				break;
			}
		}

		ti.Update();
		//char* test = ti.GetText();
		
		if (!INPUT->GetKeyboard()->IsTextInputActive())
		{
			if (isServer)
			{
				bool cubeMoved = false;

				// MOVE CUBE
				if (INPUT->GetKeyboard()->GetKeyState(SDL_SCANCODE_UP) == Input::InputState::DOWN)
				{
					cubeMoved = true;
					mat[100] *= glm::translate(vec3(0, 0, -0.01f));
				}

				if (INPUT->GetKeyboard()->GetKeyState(SDL_SCANCODE_DOWN) == Input::InputState::DOWN)
				{
					cubeMoved = true;
					mat[100] *= glm::translate(vec3(0, 0, 0.01f));
				}

				if (INPUT->GetKeyboard()->GetKeyState(SDL_SCANCODE_LEFT) == Input::InputState::DOWN)
				{
					cubeMoved = true;
					mat[100] *= glm::translate(vec3(-0.01f, 0, 0));
				}

				if (INPUT->GetKeyboard()->GetKeyState(SDL_SCANCODE_RIGHT) == Input::InputState::DOWN)
				{
					cubeMoved = true;
					mat[100] *= glm::translate(vec3(0.01f, 0, 0));
				}

				if (INPUT->GetKeyboard()->GetKeyState(SDL_SCANCODE_SPACE) == Input::InputState::DOWN)
				{
					cubeMoved = true;
					mat[100] *= glm::translate(vec3(0, 0.01f, 0));
				}

				if (INPUT->GetKeyboard()->GetKeyState(SDL_SCANCODE_LSHIFT) == Input::InputState::DOWN)
				{
					cubeMoved = true;
					mat[100] *= glm::translate(vec3(0, -0.01f, 0));
				}

				if (cubeMoved)
				{
					Network::PacketHandler* ph = server->GetPacketHandler();
					uint64_t id = ph->StartPack("CubePos");
					ph->WriteFloat(id, mat[100][3][0]);
					ph->WriteFloat(id, mat[100][3][1]);
					ph->WriteFloat(id, mat[100][3][2]);
					Network::Packet* p = ph->EndPack(id);
					server->Broadcast(p);
				}
			}

			// MOVE CAMERA
		
			if (INPUT->GetKeyboard()->GetKeyState(SDL_SCANCODE_W) == Input::InputState::DOWN)
				RENDERER->GetCamera()->MoveForward(dt);
			if (INPUT->GetKeyboard()->GetKeyState(SDL_SCANCODE_S) == Input::InputState::DOWN)
				RENDERER->GetCamera()->MoveBackward(dt);
			if (INPUT->GetKeyboard()->GetKeyState(SDL_SCANCODE_A) == Input::InputState::DOWN)
				RENDERER->GetCamera()->MoveLeft(dt);
			if (INPUT->GetKeyboard()->GetKeyState(SDL_SCANCODE_D) == Input::InputState::DOWN)
				RENDERER->GetCamera()->MoveRight(dt);
			

			// ROTATE CAMERA
			if (INPUT->GetMouse()->GetButtonState(Input::LeftButton) == Input::InputState::DOWN)
			{
				int sizeX, sizeY;
				RENDERER->GetWindowSize(sizeX, sizeY);

				RENDERER->GetCamera()->UpdateMouse(sizeX*0.5, sizeY*0.5, INPUT->GetMouse()->GetX(), INPUT->GetMouse()->GetY());
				INPUT->GetMouse()->SetPosition(sizeX*0.5, sizeY*0.5);
				INPUT->GetMouse()->HideCursor(true);
			}
			else
				INPUT->GetMouse()->HideCursor(false);

		}

		// ToggleConsole
		if (INPUT->GetKeyboard()->GetKeyState(SDL_SCANCODE_GRAVE) == Input::InputState::PRESSED)
		{
			if (INPUT->GetKeyboard()->IsTextInputActive())
			{
				INPUT->GetKeyboard()->StopTextInput();
				ti.SetActive(false);
			}
			else
			{
				INPUT->GetKeyboard()->StartTextInput();
				INPUT->GetKeyboard()->ResetTextInput();
				ti.SetActive(true);
			}
		}

		//-----------------------------------------------------------------------------------------------

		if (INPUT->GetKeyboard()->GetKeyState(SDL_SCANCODE_ESCAPE) == Input::InputState::PRESSED)
		{
			lol = false;
		}
	}

	for (auto it = m_players.begin(); it != m_players.end(); ++it)
		SAFE_DELETE(it->second);
	m_players.clear();


	SAFE_DELETE(INPUT);
	SAFE_DELETE(server);
	SAFE_DELETE(client);
	SAFE_DELETE(RENDERER);
}

int main(int argc, char** argv)
{

	Start();
	return 0;
}
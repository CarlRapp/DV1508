#include "GameServer.h"


GameServer::GameServer()
{
	m_currentState = 0;
}

GameServer::~GameServer()
{
	SAFE_DELETE(m_currentState);
}

ServerState::ServerState(GameServer* _gameServer)
{
	m_gameServer = _gameServer;
}

ServerState::~ServerState(void)
{
}


LobbyState::LobbyState(GameServer* _gameServer)
	: ServerState(_gameServer)
{

}

void LobbyState::Update()
{
	/*
	
	if (Enter �r tryckt && 2 spelare �r anslutna)
	{
		
		m_gameServer->SetState(new DealState());
		
	}
	
	*/
}

void LobbyState::SelectedCards(PacketHandler* _ph, NetConnection* _connection)
{
	//return
}

void LobbyState::SetUserName(PacketHandler* _ph, NetConnection* _connection)
{
	//�ndra namn
	//skicka ok
}

void LobbyState::OnPlayerConnected(char _token, NetConnection* _connection)
{
	//adda honom
}

void LobbyState::OnPlayerDisonnected(char _token, NetConnection* _connection)
{
	//ta bort honom
}


DealState::DealState(GameServer* _gameServer)
	: ServerState(_gameServer)
{

}

DealState::~DealState(void)
{

}

void DealState::Update()
{
	/*
	
	if (tiden �r slut)
	{
		sluta v�nta p� resterande spelare
	}

	if (antal spelare vi v�ntar p� == 0)
	{
		
		m_gameServer->SetState(new StepState());

	}
	
	*/
}

void DealState::SelectedCards(PacketHandler* _ph, NetConnection* _connection)
{
	//v�lj kort
}

void DealState::SetUserName(PacketHandler* _ph, NetConnection* _connection)
{
	//return
}

void DealState::OnPlayerConnected(char _token, NetConnection* _connection)
{
	//kicka honom
}

void DealState::OnPlayerDisonnected(char _token, NetConnection* _connection)
{
	//ta bort honom
	//sluta v�nta p� den spelaren
}




StepState::StepState(GameServer* _gameServer)
	: ServerState(_gameServer)
{

}

StepState::~StepState(void)
{

}

void StepState::Update()
{
	/*

	if (enter �r tryckt)
	{

		if (current_step < 5)
			Visa n�sta steg
			current_step++

		else
			m_gameServer->SetState(new DealState());
			

	}

	*/
}

void StepState::SelectedCards(PacketHandler* _ph, NetConnection* _connection)
{
	//return
}

void StepState::SetUserName(PacketHandler* _ph, NetConnection* _connection)
{
	//return
}

void StepState::OnPlayerConnected(char _token, NetConnection* _connection)
{
	//Kicka honom
}

void StepState::OnPlayerDisonnected(char _token, NetConnection* _connection)
{
	//ta bort honom
}
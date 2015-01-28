#include "GameCreator.h"

#ifdef WIN32
#ifdef _DEBUG
#include <VLD/vld.h>
#endif
#endif

#include "Logger/Logger.h"
#include "Game/Network/ClientDatabase.h"

int main(int argc, char** argv)
{
#if defined(__OSX__) || defined(__IOS__)
    printf("SIGPIPE\n");
    signal(SIGPIPE, SIG_IGN);
#endif
    
	Logger::GetInstance().AddGroup("Game");
	GameCreator* newGame = new GameCreator();


	newGame->InitializeLua();
	newGame->InitializeGraphics();
	newGame->InitializeInput();
	newGame->InitializeNetwork();
	newGame->InitializeWorld("lobby");


	newGame->StartGame(argc, argv);
	delete newGame;

	delete(&ClientDatabase::GetInstance());
	delete(&Logger::GetInstance());
	
	return 0;
}
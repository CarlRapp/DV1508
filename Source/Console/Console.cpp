#include "Console/Console.h"

using namespace Console;

ConsoleManager::ConsoleManager()
{
	int test = 0;
}

void ConsoleManager::ParseArgs(char* _args, std::vector<Argument>* _vector)
{
	std::string temp = "hest1 hest2 hest3 hest4";

	while (size_t position = temp.find_first_of(' '))
	{
	}

}

void ConsoleManager::ExecuteCommand(char* _command)
{
	std::vector<Argument> _vec;

	ParseArgs(_command, &_vec);

	m_consoleHooks[_command](&_vec);


}
void ConsoleManager::AddCommand(char* _name, ConsoleHook _hook)
{
	//kolla s� att _name inte inneh�ller ett mellanslag

	m_consoleHooks[_name] = _hook;

}


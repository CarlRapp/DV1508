#include "CLIWrapper.h"
#include "NativeInterface.h"
#using <System.Windows.Forms.dll>
using namespace System::Windows::Forms;
//using namespace ECSTool;

namespace ECSToolWrapper
{

#ifdef __cplusplus
	extern "C"
	{
#endif
		__declspec(dllexport) void ShowTool()
		{
		}

#ifdef __cplusplus
	}
#endif
}
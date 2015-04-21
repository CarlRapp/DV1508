#pragma once

using namespace System;
using namespace ECSTool;

namespace ECSToolWrapper
{
	public ref class ECST
	{
		ECST(void)
		{
			ECSTool::Class1::ShowTool();
		}
	public:
		static ECST^ ECSToolInstance = gcnew ECST();
	};
}
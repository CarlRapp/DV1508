#pragma once

using namespace System;
using namespace ECSTool;

namespace ECSToolWrapper
{
	public ref class ECST
	{
		ECST(void)
		{
			
		}
	public:
		static ECST^ ECSToolInstance = gcnew ECST();
	};
}
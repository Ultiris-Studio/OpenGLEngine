////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//////////////////////    --CREDITS: BROCOLARBRE--    //////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>

namespace OpenGLEngine
{
	class SymbolGenerator
	{
	public:
		virtual std::string generate() const = 0;
	};
}
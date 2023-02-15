////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//////////////////////    --CREDITS: BROCOLARBRE--    //////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "depch.h"

#include <DuckEngine/Shader/Symbols/Loader.h>

#include <fstream>
#include <sstream>

namespace DuckEngine
{
	std::string Loader::readPiece(const std::string& path)
	{
		std::ifstream file(path);
		std::ostringstream stream;

		if (!file.is_open())
			return "";

		stream << file.rdbuf();
		file.close();

		return stream.str();
	}
}

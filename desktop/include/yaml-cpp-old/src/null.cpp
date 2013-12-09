#include "yaml-cpp-old/null.h"
#include "yaml-cpp-old/node.h"

namespace YAMLold
{
	_Null Null;

	bool IsNull(const Node& node)
	{
		return node.Read(Null);
	}
}

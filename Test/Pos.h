#pragma once
#include "Vector.h"

namespace spatial
{
	template<typename T>
	using Pos = math::Vector<T>;

	using PosF = Pos<float>;
}

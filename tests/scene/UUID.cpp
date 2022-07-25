#include "UUID.h"

namespace Cheets
{
	std::random_device UUID::m_Device;
	std::mt19937 UUID::m_Gen(UUID::m_Device());
	std::uniform_int_distribution<std::mt19937::result_type> UUID::m_Dist(0, 35);

}
#include "Group.h"

namespace Cheets
{
	template<class ...T>
	void Group<T...>::Insert(ComponentHandle<T>... args)
	{
		
	}
	
	template<class ...T>
	template<class Tup, class R, class ...Args>
	std::vector<R> Group<T...>::Loop(std::function<R(Args...)> func, Tup input)
	{
		
	}
}

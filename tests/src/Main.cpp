#include <iostream>
#include <random>

#include "Container.h"

struct Okay
{
	int a = 5;
	Cheets::UUID p_Id;
};

int main(int argv, char** argc)
{
	srand(time(nullptr));

	Cheets::DynamicContainer container;
	const int i = 10000;
	Cheets::UUID ptrs[i];
	for(int x  = 0 ; x < i; x++)
	{
		Okay el;
		container.Insert(std::move(el));
		ptrs[x] = el.p_Id;
	}

	for (int x = 2000; x < 4001; x++) {
		container.Remove(ptrs[x]);
	}
}
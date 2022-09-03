#include <iostream>

#include <string>
#include <tuple>
#include <variant>
#include <typeinfo>
#include <stdio.h>

#include "UUID.hpp"


template<class Head>
void okay()
{
	std::cout << typeid(Head).name();
}

template<class Head, class First, class ...Tail>
void okay()
{
	std::cout << typeid(First).name() << std::endl;
	okay<Head, Tail...>();
}


int main(int argc, char** argv)
{
	okay<int, float, double, char>();
	return 0;
}

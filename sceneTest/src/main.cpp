#include <iostream>
#include <tuple>
#include <vector>
#include "Entity.h"
#include "ComponentHandle.h"
#include "Group.h"
#include "Core.h"
#include "TemplateContainer.hpp"
#include "Registry.h"
#include <stdlib.h>

struct Comp : Cheets::Component {};
struct Comp2 : Cheets::Component { int a = 5; };


int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL));

    Cheets::Registry reg;
    Cheets::UUID ent = reg.CreateEntity();
    Cheets::UUID ent2 = reg.CreateEntity();
    Cheets::UUID ent3 = reg.CreateEntity();

	Comp c;	std::cout << c.p_Id << std::endl;
	Comp2 c2;std::cout << c2.p_Id << std::endl;
	Comp c3; std::cout << c3.p_Id << std::endl;
	Comp2 c4;std::cout << c4.p_Id << std::endl;
	Comp c5; std::cout << c5.p_Id << std::endl;
	Comp2 c6; std::cout << c6.p_Id << std::endl;
	Comp c7; std::cout << c7.p_Id << std::endl;
	Comp2 c8; std::cout << c8.p_Id << std::endl;

    reg.AddComponent(ent, c);
    reg.AddComponent(ent, c2);
    reg.AddComponent(ent2, c3);
    reg.AddComponent(ent2, c4);
    reg.AddComponent(ent3, c5);
    reg.AddComponent(ent3, c6);

    reg.GroupComponents<Comp, Comp2>();

	int b = 5;
}
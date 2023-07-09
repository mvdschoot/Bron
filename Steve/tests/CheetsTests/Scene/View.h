#ifndef VIEW_TESTS_HEADER_
#define VIEW_TESTS_HEADER_

#include "Steve.h"

namespace Steve 
{
	struct C1 : public Component
	{
		int a = 5;
		C1() = default;
	};
	struct C2 : public Component
	{
		long long int a = 5;
		C2() = default;
	};

	class ViewTests
	{
	public:
		static void execute()
		{
			ent1 = scene.CreateEntity();
			scene.AddComponent(C1());
			scene.AddComponent(C2());
		}

		static void getStdView()
		{
		}
		
	private:
		static Scene scene;
		static UUID ent1;
	};
}

#endif
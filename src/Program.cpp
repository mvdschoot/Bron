#include "Cheets.h"


class Program : public Cheets::Application {
public:
	Program() {};
	~Program() {};

};


Cheets::Application* Cheets::createApplication() 
{
	return new Program();
}
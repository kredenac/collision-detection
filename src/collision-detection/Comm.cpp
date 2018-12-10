#include "Comm.h"

#include <iostream>



//class Duck : public duck {
//public:
//	Duck(int feet) {};
//	~Duck() {};
//
//	void quack(float volume) 
//	{
//		std::cout << "quack " << volume << std::endl;
//	};
//};

Duck::Duck(int feet) {};
Duck::~Duck() {};

void Duck::quack(float volume)
{
	std::cout << "quack " << volume << std::endl;
};

int test()
{
	return 77;
}

inline Duck* real(duck* d) { return static_cast<Duck*>(d); }

duck* new_duck(int feet) { return new Duck(feet); }
void delete_duck(duck* d) { delete real(d); }
void duck_quack(duck* d, float volume) { real(d)->quack(volume); }
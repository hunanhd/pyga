#include "ga.h"

int JLAdd(int a, int b)
{
	return a+b;
}

void JLTest(std::vector<double>& datas)
{
	for (std::vector<double>::iterator itr = datas.begin(); itr != datas.end(); itr++)
	{
		*itr = *itr+100;
	}
}

void JLPrint(const char* msg)
{
	std::cout << msg << std::endl;
}

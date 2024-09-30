#include <iostream>
#include <future>

using namespace std;

unsigned long longAlgorithm()
{
	unsigned long result = 0;
	for (unsigned int i = 0; i < 100000000; ++i)
	{
	}
	return result;
}

int main()
{
	std::future result = std::async(longAlgorithm);
	std::cout << "Result is: " << result.get();
	return 0;
}

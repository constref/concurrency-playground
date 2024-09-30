#include <iostream>
#include <thread>

using namespace std;

void doWork()
{
	//std::cout << "\tWorking..." << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(2));
	//std::cout << "\tDone!" << std::endl;
}

int main()
{
	std::thread t1(doWork);

	for (int i = 0; i < 100; ++i)
	{
		std::cout << "Downloading file " << i << "% " << '[';
		for (int p = 0; p < 10; ++p)
		{
			if (p <= i / 10)
			{
				std::cout << '#';
			}
			else
			{
				std::cout << ' ';
			}
		}
		std::cout << ']' << '\r' << std::flush;

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	t1.join();

	return 0;
}

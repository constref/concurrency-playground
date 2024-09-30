#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <curses.h>

using namespace std;

long sum = 0;
int number = 0;

std::condition_variable cond;
std::mutex mut;

void runSummator()
{
	while (true)
	{
		// need unique lock to be able to lock and unlock using wait()
		std::unique_lock<std::mutex> lock(mut);

		// wait for condition, locks mutex during check, unlocks if condition isn't met and waits
		cond.wait(lock, []() {
			return number > 0;
		});

		// add number to sum
		sum += number;
		number = 0;
	}
}

int main()
{
	std::thread t1(runSummator);
	t1.detach();

	// setup example UI
	initscr();

	WINDOW *dlWin = newwin(3, 40, 1, 1);
	refresh();

	box(dlWin, 0, 0);
	mvwprintw(dlWin, 1, 2, "Progress [ ");

	for (int p = 0; p < 23; ++p)
	{
		wprintw(dlWin, "#");
	}
	wprintw(dlWin, " ]");
	wrefresh(dlWin);

	getch();
	endwin();

	return 0;
}

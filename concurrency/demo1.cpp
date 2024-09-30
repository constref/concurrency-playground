#include <thread>
#include <atomic>
#include <future>

#include <chrono>
#include <iostream>
#include <random>
#include <algorithm>

#include "platform.h"
#include "sdlrenderer.h"
#include "pixel.h"
#include "demo1.h"

using namespace std;

Demo::Demo() : Platform(1920, 1080, "Concurrency Demo")
{
	// figure out the number of threads
	const int fallbackThreads = 2;
	const int hardwareThreads = std::thread::hardware_concurrency();
	numThreads = hardwareThreads == 0 ? fallbackThreads : hardwareThreads;
	threads.resize(numThreads - 1);

	// dispatch a loader thread to keep checking there is data to be load
	loaderThread = std::thread(&Demo::loadData, this);
	if (loaderThread.joinable())
	{
		loaderThread.detach();
	}












	loadBarColor = { .r = 154, .g = 112, .b = 73, .a = 255 };
	loadBarWidth = 350;
	loadBar = { .x = 100, .y = 100, .w = loadBarWidth, .h = 80 };

	SDL_Color buttonColor{ 255, 255, 255, 255 };
	buttonSave = renderer.createLargeText("Save Data", buttonColor);
	buttonSave.rect.x = width / 2 - buttonSave.rect.w / 2;
	buttonSave.rect.y = 450;
	buttonLoad = renderer.createLargeText("Load Data", buttonColor);
	buttonLoad.rect.x = width / 2 - buttonLoad.rect.w / 2;
	buttonLoad.rect.y = 550;

	SDL_Color white{ 255, 255, 255, 255 };
	textLoadingMsg = renderer.createSmallText("Loading Data", white);
	textLoadingMsg.rect.x = loadBar.x + 10;
	textLoadingMsg.rect.y = 130;
	textSavingMsg = renderer.createSmallText("Saving Data", white);
	textSavingMsg.rect.x = loadBar.x + 10;
	textSavingMsg.rect.y = 130;

	saving = false;
	loading = false;
}

Demo::~Demo()
{
	SDL_DestroyTexture(buttonLoad.texture);
	SDL_DestroyTexture(buttonSave.texture);
	SDL_DestroyTexture(textLoadingMsg.texture);
	SDL_DestroyTexture(textSavingMsg.texture);
}

void Demo::run()
{
	const int halfWidth = width / 2;
	const int halfHeight = height / 2;

	// function to fill a number of horizontal lines with pixels
	// fills range of startLine to endLine
	auto fillLines = [this, halfWidth, halfHeight](int startLine, int endLine, double timeSin)
	{
		for (int y = startLine; y <= endLine; ++y)
		{
			for (int x = -halfWidth; x < halfWidth; ++x)
			{
				const double dist = sqrt(x * x + y * y) * timeSin + globalTime;
				const double sinVal = (sin((y + halfHeight) / dist + globalTime) + 1) / 2;
				const float colorScale = dist + timeSin * 10;

				renderer.drawPixel(x, y, pixel(
					static_cast<uint8_t>(sinVal * colorScale),
					static_cast<uint8_t>((1.0f - timeSin) * colorScale * sinVal),
					static_cast<uint8_t>((1.0f - timeSin) * colorScale * sinVal),
					255
				).intValue);
			}
		}
	};

	while (!done)
	{
		const double timeSin = (sin(globalTime) + 1) / 2;

		startFrame();

		renderer.lockTexture(); // we'll be writing pixel data to it
		if (useSingleThread)
		{
			// using main thread to draw everything
			fillLines(-halfHeight, halfHeight, timeSin);
		}
		else
		{
			// divide up work by: number of horizontal lines / number of threads
			const int linesPerThread = height / numThreads; // Dividing work among all hardware-threads
			int startLine = -halfHeight;

			// spawn numThreads and divide work
			for (int t = 0; t < threads.size(); ++t)
			{
				int endLine = startLine + linesPerThread;
				threads[t] = std::thread(fillLines, startLine, endLine, timeSin);
				startLine += linesPerThread;
			}

			// finish the rest of the lines using main thread
			fillLines(startLine, halfHeight, timeSin);

			// wait for all of the threads we spawned
			for (auto &t : threads)
			{
				if (t.joinable())
				{
					t.join();
				}
			}
		}
		renderer.unlockTexture();

		// lock mutex while we're checking the work	queue
		{
			std::scoped_lock lock(workMutex);
			while (workItems.size())
			{
				const DataEvent event = workItems.front();
				workItems.pop();

				switch (event.type)
				{
					case EventType::saveStart:
					{
						saving = true;
						break;
					}
					case EventType::saveEnd:
					{
						saving = false;
						break;
					}
					case EventType::saveProgress:
					case EventType::loadProgress:
					{
						loadBar.w = static_cast<int>(event.progress * loadBarWidth);
						break;
					}
					case EventType::loadStart:
					{
						loading = true;
						SDL_Color textColor{ 255, 255, 0, 255 };
						currentFile = renderer.createSmallText(event.filename, textColor);
						currentFile.rect.x = 100;
						currentFile.rect.y = height - 80;

						break;
					}
					case EventType::loadEnd:
					{
						loading = false;
						SDL_DestroyTexture(currentFile.texture);
						break;
					}
				}
			}
		}

		// saving and loading indicators
		if (saving)
		{
			renderer.drawBox(loadBar, loadBarColor);
			renderer.drawText(textSavingMsg.rect, textSavingMsg.texture);
		}
		else if (loading)
		{
			renderer.drawBox(loadBar, loadBarColor);
			renderer.drawText(textLoadingMsg.rect, textLoadingMsg.texture);
			renderer.drawText(currentFile.rect, currentFile.texture);
		}
		else
		{
			// show menu if not saving/loading
			renderer.drawText(buttonSave.rect, buttonSave.texture);
			renderer.drawText(buttonLoad.rect, buttonLoad.texture);
		}

		endFrame();
	}
}

void Demo::saveData()
{
	workMutex.lock();
	workItems.push(DataEvent{ .type = EventType::saveStart, .progress = 0 });
	workMutex.unlock();

	// simulate data saving I/O
	for (float i = 0; i <= 1.0; i += 0.01f)
	{
		{
			std::scoped_lock lock(workMutex);
			workItems.push(DataEvent{ .type = EventType::saveProgress, .progress = i });
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(25));
	}

	// end save operation
	{
		std::scoped_lock lock(workMutex);
		workItems.push(DataEvent{ .type = EventType::saveEnd, .progress = 1.0f });
	}
}

void Demo::loadData()
{
	while (!done)
	{
		std::unique_lock lock(loadMutex);
		loadCondition.wait(lock, [this] {
			return filesToLoad.size() > 0;
		});

		const std::string filename = filesToLoad.front();
		filesToLoad.pop();
		lock.unlock();

		{
			std::scoped_lock lock(workMutex);
			workItems.push(DataEvent{ .type = EventType::loadStart, .filename = filename });
		}

		const int minSize = 15 * 1024 * 1024;
		const int maxSize = 150 * 1024 * 1024;
		std::random_device randomDevice;
		std::mt19937 generator(randomDevice());
		std::uniform_int_distribution randomDist(minSize, maxSize);

		// simulate file I/O
		const int fileSize = randomDist(generator);
		int bytesProcessed = 0;
		do
		{
			const int sizeToRead = std::min(64 * 1024, fileSize - bytesProcessed);
			std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(sizeToRead / 1000)));
			bytesProcessed += sizeToRead;

			std::scoped_lock lock(workMutex);
			workItems.push(DataEvent{ .type = EventType::loadProgress, .progress = bytesProcessed / static_cast<float>(fileSize) });
		} while (bytesProcessed < fileSize);


		{
			std::scoped_lock lock(workMutex);
			workItems.push(DataEvent{ .type = EventType::loadEnd, .filename = filename });
		}
	}
}

void Demo::mouseButton(int x, int y, bool isDown, int buttonIndex, int clicks)
{
	if (intersection(x, y, buttonSave.rect) && !isDown) // Save button
	{
		std::thread t(&Demo::saveData, this);
		if (t.joinable())
		{
			t.detach();
		}
		//std::future savFuture = std::async(&Demo::saveData, this);
	}
	else if (intersection(x, y, buttonLoad.rect) && !isDown) // Load button
	{
		std::scoped_lock lock(loadMutex);
		filesToLoad.emplace("random_filename_1.bin");
		filesToLoad.emplace("some_filename_2.bin");
		filesToLoad.emplace("music.bin");
		loadCondition.notify_one();
	}
}

int main(int argc, char *argv[])
{
	Demo app;
	app.run();

	return 0;
}


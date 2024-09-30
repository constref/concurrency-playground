#pragma once

#include <array>

#include <condition_variable>
#include <mutex>

#include <queue>
#include <string>
#include <SDL2/SDL_ttf.h>

#include "platform.h"
#include "sdlrenderer.h"

enum class EventType
{
	loadStart,
	loadProgress,
	loadEnd,
	saveStart,
	saveProgress,
	saveEnd
};

struct DataEvent
{
	EventType type;
	float progress;
	std::string filename;
};

class Demo : public Platform<SDLRenderer>
{
	TextLabel textLoadingMsg, textSavingMsg, buttonSave, buttonLoad, currentFile;

	int numThreads;
	std::vector<std::thread> threads;
	std::future<void> savFuture;
	std::thread loaderThread;

	std::queue<DataEvent> workItems;
	std::mutex workMutex, loadMutex;
	std::condition_variable loadCondition;
	std::queue<std::string> filesToLoad;

	bool saving, loading;
	int loadBarWidth;
	SDL_Rect loadBar;
	SDL_Color loadBarColor;

public:
	Demo();
	~Demo();

	void run();
	void saveData();
	void loadData();
	void mouseButton(int x, int y, bool isDown, int buttonIndex, int clicks) override;
};
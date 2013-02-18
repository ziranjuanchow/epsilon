#pragma once
#include <common/common.hpp>
#include <devices/devices.hpp>

class Interface
{
	public:
		WINDOW* window;
        std::string sceneFile, outputFile;
		double progress;
		size_t platform, device;
		size_t samples, width, height;

        DeviceList deviceList;
        void WriteLine(size_t line, std::string msg);
		
        Interface();
		~Interface();

        void DisplayStatus(std::string message, bool error);
		void DisplayProgress();

		void DrawFrame();
        void GetInput();
		void Redraw();
};

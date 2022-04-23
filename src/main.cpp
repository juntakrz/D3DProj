#include "pch.h"
#include "DFMain.h"

int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
{
	try
	{
		LOG("DIRECT3D STUDY PROJECT\n\nLaunching application and creating the main window.");
		DFactory::DFACTORY_INIT_DESC DFInitDesc{};
		DFMain App(&DFInitDesc);
		return App.Run();
	}
	catch (const CException& e)
	{
		MessageBoxA(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBoxA(nullptr, "Standard exception", "Standard exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBoxA(nullptr, "Unknown exception", "Unknown exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}
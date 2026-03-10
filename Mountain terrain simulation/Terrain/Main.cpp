#include "SystemClass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* system = new SystemClass();

	if (!system) {
		return 0;
	}

	// Initialize and run the system object.
	if (system->Initialize()) {
		system->Run();
	}

	// Shutdown and release the system object.
	system->Shutdown();
	delete system;
	system = NULL;

	return 0;
}

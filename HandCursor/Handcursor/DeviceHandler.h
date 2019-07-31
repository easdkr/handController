
#include <Windows.h>
#include <WindowsX.h>
#include "RealSense/Session.h"
#include "RealSense/CaptureManager.h"

using namespace Intel::RealSense;

class DeviceHandler: public Capture::Handler {

private:

public:

	HWND hwndDlg;
	void (*PopulateDevice)(HWND hwndDlg);

	virtual void __stdcall OnDeviceListChanged(void)
	{
		// Callback
		PopulateDevice(hwndDlg);

	}

};

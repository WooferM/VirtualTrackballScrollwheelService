// VirtualTrackballScrollwheelService.cpp : Implementation of WinMain


#include "stdafx.h"
#include "resource.h"
#include "VirtualTrackballScrollwheelService_i.h"


#include <stdio.h>

class CVirtualTrackballScrollwheelServiceModule : public ATL::CAtlServiceModuleT< CVirtualTrackballScrollwheelServiceModule, IDS_SERVICENAME >
	{
public :
	DECLARE_LIBID(LIBID_VirtualTrackballScrollwheelServiceLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_VIRTUALTRACKBALLSCROLLWHEELSERVICE, "{0B7B685E-051E-4641-88FD-F85575F89177}")
		HRESULT InitializeSecurity() throw()
	{
		// TODO : Call CoInitializeSecurity and provide the appropriate security settings for your service
		// Suggested - PKT Level Authentication, 
		// Impersonation Level of RPC_C_IMP_LEVEL_IDENTIFY 
		// and an appropiate Non NULL Security Descriptor.

		return S_OK;
	}
	};

CVirtualTrackballScrollwheelServiceModule _AtlModule;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

const int Y_THRESHOLD = 10;   // threshold in pixels to trigger wheel event
const DWORD WHEEL_FACTOR = 1; // number of wheel events. The lines scrolled per wheel event are determined by the Microsoft Windows mouse wheel settings.
enum State {
	NORMAL = 0, // default state
	DOWN,       // mouse XButton pressed, no movement
	SCROLL,     // mouse XButton pressed + moved
	L_DOWN,		// left mouse button pressed
	R_DOWN,		// right mouse button pressed
	LR_DOWN,	// left and right mouse button pressed
	LR_SCROLL,	// left and right mouse button pressed + moved
};
HHOOK g_Hook;           // callback function hook, called for every mouse event
State g_state = NORMAL; // initial state
POINT g_origin;         // cursor position when entering state DOWN
int g_ycount = 0;       // accumulated vertical movement while in state SCROLL

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode != HC_ACTION)
		return CallNextHookEx(g_Hook, nCode, wParam, lParam);
	BOOL preventCallNextHookEx = FALSE;
	const MSLLHOOKSTRUCT *p = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
	switch (g_state) {
	case NORMAL:
		if (wParam == WM_XBUTTONDOWN) { // NORMAL->DOWN: remember position
			preventCallNextHookEx = TRUE;
			g_state = DOWN;
			g_origin = p->pt;
		} else if (wParam == WM_LBUTTONDOWN) { // NORMAL->L_DOWN: remember position
			preventCallNextHookEx = FALSE;
			g_state = L_DOWN;
			g_origin = p->pt;
		}
		break;
	case DOWN:
		if (wParam == WM_MOUSEMOVE) { // DOWN->SCROLL
			preventCallNextHookEx = TRUE;
			g_state = SCROLL;
			g_ycount = 0;
			SetCursorPos(g_origin.x, g_origin.y);
		}
		break;
	case SCROLL:
		if (wParam == WM_XBUTTONUP) { // SCROLL->NORMAL
			preventCallNextHookEx = TRUE;
			g_state = NORMAL;
		}
		else if (wParam == WM_MOUSEMOVE) { // SCROLL->SCROLL: wheel event
			preventCallNextHookEx = TRUE;
			g_ycount += p->pt.y - g_origin.y;
			SetCursorPos(g_origin.x, g_origin.y);
			if (g_ycount < -Y_THRESHOLD || g_ycount > Y_THRESHOLD){
				DWORD mouseData = (g_ycount > 0 ? -WHEEL_DELTA : +WHEEL_DELTA); // scroll direction
				g_ycount = 0;
				INPUT input[WHEEL_FACTOR];
				for (size_t i = 0; i < WHEEL_FACTOR; ++i) {
					input[i].type = INPUT_MOUSE;
					input[i].mi.dx = p->pt.x;
					input[i].mi.dy = p->pt.y;
					input[i].mi.mouseData = mouseData;
					input[i].mi.dwFlags = MOUSEEVENTF_WHEEL; // wheel
					input[i].mi.time = (DWORD)0x0;
					input[i].mi.dwExtraInfo = (ULONG_PTR)NULL;
				}
				SendInput(WHEEL_FACTOR, input, sizeof(INPUT));
			}
		}
		break;
	case L_DOWN:
		 if (wParam == WM_RBUTTONDOWN) { // L_DOWN-> LR_DOWN: remember position
			preventCallNextHookEx = TRUE;
			g_state = LR_DOWN;
			g_origin = p->pt;
		} else if (wParam == WM_LBUTTONUP) { // L_DOWN->NORMAL
			preventCallNextHookEx = FALSE;
			g_state = NORMAL;
		}

		break;
	case LR_DOWN:
		if (wParam == WM_MOUSEMOVE) { // LR_DOWN->LR_SCROLL
			preventCallNextHookEx = TRUE;
			g_state = LR_SCROLL;
			g_ycount = 0;
			SetCursorPos(g_origin.x, g_origin.y);
		} else if (wParam == WM_LBUTTONUP) { // LR_DOWN->NORMAL
			preventCallNextHookEx = FALSE;
			g_state = NORMAL;
		}
		else if (wParam == WM_RBUTTONUP) { // LR_DOWN-> L_DOWN
			preventCallNextHookEx = TRUE;
			g_state = L_DOWN;
		}
		break;
	case LR_SCROLL:
		if (wParam == WM_LBUTTONUP) { // LR_SCROLL->NORMAL
			preventCallNextHookEx = TRUE;
			g_state = NORMAL;
		}
		else if (wParam == WM_RBUTTONUP) { // LR_SCROLL-> L_DOWN
			preventCallNextHookEx = TRUE;
			g_state = L_DOWN;
		}
		else if (wParam == WM_MOUSEMOVE) { // SCROLL->SCROLL: wheel event
			preventCallNextHookEx = TRUE;
			g_ycount += p->pt.y - g_origin.y;
			SetCursorPos(g_origin.x, g_origin.y);
			if (g_ycount < -Y_THRESHOLD || g_ycount > Y_THRESHOLD){
				DWORD mouseData = (g_ycount > 0 ? -WHEEL_DELTA : +WHEEL_DELTA); // scroll direction
				g_ycount = 0;
				INPUT input[WHEEL_FACTOR];
				for (size_t i = 0; i < WHEEL_FACTOR; ++i) {
					input[i].type = INPUT_MOUSE;
					input[i].mi.dx = p->pt.x;
					input[i].mi.dy = p->pt.y;
					input[i].mi.mouseData = mouseData;
					input[i].mi.dwFlags = MOUSEEVENTF_WHEEL; // wheel
					input[i].mi.time = (DWORD)0x0;
					input[i].mi.dwExtraInfo = (ULONG_PTR)NULL;
				}
				SendInput(WHEEL_FACTOR, input, sizeof(INPUT));
			}
		}
		break;
	}
	return (preventCallNextHookEx ? 1 : CallNextHookEx(g_Hook, nCode, wParam, lParam));
}

//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
								LPTSTR /*lpCmdLine*/, int nShowCmd)
{
		MSG msg;
	HACCEL hAccelTable;

//attach VirtualTrackballScrollwhell hook
	g_Hook = SetWindowsHookEx(WH_MOUSE_LL, &LowLevelMouseProc, GetModuleHandle(0), 0);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	UnhookWindowsHookEx(g_Hook); // remove hook	
	return _AtlModule.WinMain(nShowCmd);
}


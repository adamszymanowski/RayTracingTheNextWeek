#include <windows.h>


#define global_variable static
#define internal_function static

global_variable bool Running;

global_variable int Scale = 3;
global_variable int BitmapWidth = 256;
global_variable int BitmapHeight = 128;


global_variable int WindowWidth;
global_variable int WindowHeight;

global_variable BITMAPINFO BitmapInfo;
global_variable void* BitmapMemory;

internal_function void
Win32ResizeDIBSection()
{
	if (BitmapMemory)
	{
		VirtualFree(BitmapMemory, 0, MEM_RELEASE);
	}

	BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biWidth = BitmapWidth;
	BitmapInfo.bmiHeader.biHeight = -BitmapHeight; // why negative height (https://docs.microsoft.com/en-us/previous-versions/dd183376(v=vs.85))
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;

	int BytesPerPixel = 4;
	int BitmapMemorySize = BitmapWidth * BitmapHeight * BytesPerPixel;
	int Pitch = BitmapWidth * BytesPerPixel;
	BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

	unsigned char* Row = (unsigned char*)BitmapMemory;
	for (int Y = 0; Y < BitmapHeight; Y++)
	{
		unsigned int* Pixel = (unsigned int*)Row;
		for (int X = 0; X < BitmapWidth; X++)
		{
			unsigned char ir = unsigned char(0);
			unsigned char ig = unsigned char(128+Y);
			unsigned char ib = unsigned char(X);

			// B G R 0, so reversed when bit or-ing: 0 R G B
			*Pixel++ = (ir << 16) | (ig << 8) | ib;
		}

		Row += Pitch;
	}
}

internal_function void
Win32UpdateWindow(HDC DeviceContext, RECT* WindowRect, int X, int Y, int Width, int Height)
{
	int WindowWidth = WindowRect->right - WindowRect->left;
	int WindowHeight = WindowRect->bottom - WindowRect->top;

	StretchDIBits(
		DeviceContext,
		0, 0, WindowWidth, WindowHeight, // Destination
		0, 0, BitmapWidth, BitmapHeight, // Source
		BitmapMemory,
		&BitmapInfo,
		DIB_RGB_COLORS,
		SRCCOPY
	);
}

LRESULT CALLBACK
Win32MainWindowCallback(
	HWND   Window,
	UINT   Message,
	WPARAM WParam,
	LPARAM LParam)

{
	LRESULT Result = 0;
	switch (Message)
	{
	case WM_SIZE:
	{
		Win32ResizeDIBSection();
		OutputDebugStringA("WM_SIZE\n");
	} break;

	case WM_CLOSE:
	{
		OutputDebugStringA("WM_CLOSE\n");
		DestroyWindow(Window);
	} break;

	case WM_DESTROY:
	{
		OutputDebugStringA("WM_DESTROY\n");
		PostQuitMessage(0);
		Result = 0;
	} break;

	case WM_ACTIVATEAPP:
	{
		OutputDebugStringA("WM_ACTIVATEAPP\n");
	} break;

	case WM_PAINT:
	{
		PAINTSTRUCT Paint;
		HDC DeviceContext = BeginPaint(Window, &Paint);
		int X = Paint.rcPaint.left;
		int Y = Paint.rcPaint.top;
		int Width = Paint.rcPaint.right - Paint.rcPaint.left;
		int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;

		RECT ClientRect;
		GetClientRect(Window, &ClientRect);

		Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
		EndPaint(Window, &Paint);
	} break;

	default:
	{
		OutputDebugStringA("default\n");
		Result = DefWindowProc(Window, Message, WParam, LParam);
	}
	}

	return Result;
}

int WINAPI WinMain(
	HINSTANCE Instance,
	HINSTANCE PrevInstance,
	LPSTR     CommandLine,
	int       ShowCode)
{
	WNDCLASS WindowClass = {};

	WindowClass.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
	WindowClass.lpfnWndProc = Win32MainWindowCallback;
	WindowClass.hInstance = Instance;
	//WindowClass.hIcon;
	WindowClass.lpszClassName = "RTIOW_Class";



	if (RegisterClass(&WindowClass))
	{
		HWND WindowHandle = CreateWindowExA(
			0,
			WindowClass.lpszClassName,
			"Ray Tracing In One Weekend",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,		// X
			CW_USEDEFAULT,		// Y
			CW_USEDEFAULT,	// width
			CW_USEDEFAULT,	// height
			0,
			0,
			Instance,
			0
		);

		if (WindowHandle)
		{
			// This need to be a part of code for some reason
			ShowWindow(WindowHandle, ShowCode);

			MSG Message;
			Running = true;
			while (Running)
			{
				MSG Message;
				while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
				{
					if (Message.message == WM_QUIT)
					{
						Running = false;
					}

					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}
			}

		}
		else
		{
			OutputDebugStringA("WindowHandle Fail.\n");
		}
	}
	else
	{
		OutputDebugStringA("RegisterClass Fail.\n");
	}
}
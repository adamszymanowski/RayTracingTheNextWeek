#include <windows.h>


#define global_variable static
#define internal_function static


global_variable bool Running;
global_variable bool RunOnce = true;

global_variable int BitmapScale = 3;
global_variable int BitmapWidth = 128 * BitmapScale;
global_variable int BitmapHeight = 128 * BitmapScale;

global_variable int WindowScale = 1;
global_variable int WindowWidth = BitmapWidth * WindowScale;
global_variable int WindowHeight = BitmapHeight * WindowScale;

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
			auto r = double(X) / (BitmapHeight - 1);
			auto g = double(BitmapWidth - Y) / (BitmapWidth - 1);
			auto b = 0.25;
			unsigned char ir = static_cast<unsigned char>(255.999 * r);
			unsigned char ig = static_cast<unsigned char>(255.999 * g);
			unsigned char ib = static_cast<unsigned char>(255.999 * b);

			// B G R 0, so reversed when bit or-ing: 0 R G B
			*Pixel++ = (ir << 16) | (ig << 8) | ib;
		}

		Row += Pitch;
	}
}

internal_function void
Win32UpdateWindow(HDC DeviceContext, RECT* ClientRect)
{
	int WindowClientWidth = ClientRect->right - ClientRect->left;
	int WindowClientHeight = ClientRect->bottom - ClientRect->top;

	StretchDIBits(
		DeviceContext,
		0, 0, WindowClientWidth, WindowClientHeight, // Destination
		0, 0, BitmapWidth, BitmapHeight,			 // Source
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

		RECT ClientRect;
		GetClientRect(Window, &ClientRect);

		Win32UpdateWindow(DeviceContext, &ClientRect);
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
	WindowClass.lpszClassName = "RTTNW_Class";



	if (RegisterClass(&WindowClass))
	{
		RECT WindowRect = { 0, 0, (LONG)WindowWidth, (LONG)WindowHeight };

		if (!AdjustWindowRectEx(&WindowRect, WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0))
		{
			OutputDebugStringA("Adjust Window Rect Fail.\n");
		}

		int WidthToCreateWindow = WindowRect.right - WindowRect.left;
		int HeightToCreateWindow = WindowRect.bottom - WindowRect.top;

		HWND WindowHandle = CreateWindowExA(
			0,
			WindowClass.lpszClassName,
			"Ray Tracing In One Weekend",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT,		// X
			CW_USEDEFAULT,		// Y
			WidthToCreateWindow,	// width
			HeightToCreateWindow,	// height
			0,
			0,
			Instance,
			0
		);

		if (WindowHandle)
		{
			// This need to be a part of code for some reason
			ShowWindow(WindowHandle, ShowCode);

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
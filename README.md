# Ray Tracing The Next Week

My attempt at doing the:
[Ray Tracing: The Next Week](https://raytracing.github.io/books/RayTracingTheNextWeek.html)

It's build upon:
[Ray Tracing in One Weekend](hhttps://raytracing.github.io/books/RayTracingTheNextWeek.html)
So I'll be doing it first, which is a redo of [this](https://github.com/adamszymanowski/RayTracingInOneWeekend),
the code is different, and I want to revise it anyway, so that's this.'

The idea is to do the book, but using Win32 Window as a renderer.

I want to utilize simple win32 code from Handmade Hero.
- [Handmade Hero Day 002 - Opening a Win32 Window](https://www.youtube.com/watch?v=4ROiWonnWGk)
- [Handmade Hero Day 003 - Allocating a Backbuffer](https://www.youtube.com/watch?v=GAi_nTx1zG8)
- [Handmade Hero Day 004 - Animating the Backbuffer](https://www.youtube.com/watch?v=hNKU8Jiza2g&t=3485s&ab_channel=MollyRocket)

But I want to use just Visual Studio in the process.

# How to compile inside Visual Studio
You need to adjust 2 settings in Solution Explorer (Project properties):

Solution Explorer:
RayTracingInOneWeekend -> Properties -> (look for options and switches): Entry -> Entry Point -> (enter value):
WinMainCRTStartup

Solution Explorer:
RayTracingInOneWeekend -> Properties -> Advanced -> Character Set -> (choose value):
Use Multi-Byte Character Set


First avoids dumb *C28251*, *LNK2019*, *LNK1120* errors

[/ENTRY (Entry-Point Symbol)](https://docs.microsoft.com/en-us/cpp/build/reference/entry-entry-point-symbol?redirectedfrom=MSDN&view=msvc-160)

Second avoids *error *C2440* cannot convert from 'const char [12]' to 'LPCWSTR'

## What if I have chosen Windows Console Application instead of Window Application?
RayTracingInOneWeekend -> Properties -> Linker-> System -> SubSystem -> (choose value):
Windows (/SUBSYSTEM:WINDOWS)
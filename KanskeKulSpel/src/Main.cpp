#include "Window.h"


int main()
{
#ifdef _DEBUG
    int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    flag |= _CRTDBG_LEAK_CHECK_DF; // Turn on leak-checking bit
    _CrtSetDbgFlag(flag);
    //_CrtSetBreakAlloc(742196); // Comment or un-comment on need basis
#endif

    Window window;

    return window.run();
};
#include "utils/win.h"

#include <string>

#include "windows.h"

void alert(std::wstring msg, std::wstring title)
{
     MessageBoxW(NULL, msg.c_str(), title.c_str(), MB_OK);   
}
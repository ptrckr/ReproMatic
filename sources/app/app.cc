#include "app/app.h"

#include <string>  // std::to_string

#include "plugin_data.h"
#include "app/procedure.h"
#include "resources.h"  // IDR_REPROMATIC_MENU

#ifndef MAC_PLATFORM
  #include "PIMain.h"  // gHINSTANCE
  #include "PIHeaders.h"
#endif

ACCB1 void ACCB2 LaunchRepromaticWindow(void *clientData) {
  WNDCLASSEXW window_class;
  if (GetClassInfoExW(gHINSTANCE, PluginData::FULL_PLUGIN_NAME.c_str(), &window_class) == 0) {
    window_class.cbSize = sizeof(WNDCLASSEXW);

    window_class.style = 0;
    window_class.cbClsExtra = 0;
    window_class.cbWndExtra = 0;
    window_class.hInstance = gHINSTANCE;
    window_class.hCursor = NULL;
    window_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    window_class.lpszMenuName = MAKEINTRESOURCE(IDR_REPROMATIC_MENU);
    window_class.lpszClassName = PluginData::FULL_PLUGIN_NAME.c_str();
    window_class.lpfnWndProc = RepromaticWndProc;

    window_class.hIcon = reinterpret_cast<HICON>(LoadImage(
      gHINSTANCE,
      MAKEINTRESOURCE(IDB_OVERVIEW_ICON_32), IMAGE_ICON,
      GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON),
      0
    ));
    window_class.hIconSm = reinterpret_cast<HICON>(LoadImage(
      gHINSTANCE,
      MAKEINTRESOURCE(IDB_OVERVIEW_ICON_16), IMAGE_ICON,
      GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON),
      0
    ));
  } else {
    AVAlertNote("Could not create `WNDCLASSEXA'.");
    return;
  }

  ATOM window_atom;
  window_atom = RegisterClassExW(&window_class);
  if(!window_atom) {
    DWORD a = GetLastError();
    AVAlertNote(std::to_string(a).c_str());
    AVAlertNote("Could not register `WNDCLASSEXA'.");
    return;
  }

  HWND window_handle;
  window_handle = CreateWindowExW(
    WS_EX_TOPMOST | WS_EX_ACCEPTFILES | WS_EX_APPWINDOW,
    reinterpret_cast<LPCTSTR>(window_atom),
    (PluginData::FULL_PLUGIN_NAME + L"  \u2014  ptrc.kr").c_str(),
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,
    PluginData::WINDOW_WIDTH, PluginData::WINDOW_HEIGHT, 0, 0,
    gHINSTANCE, NULL
  );

  if(window_handle == NULL) {
    AVAlertNote("Window could not be created.");
    UnregisterClassW(window_class.lpszClassName, gHINSTANCE);
    return;
  }

  ShowWindow(window_handle, SW_SHOWDEFAULT);
  UpdateWindow(window_handle);

  BOOL message_return;
  MSG window_message;
  while(message_return = GetMessage(&window_message, NULL, 0, 0) > 0) {
    if (message_return == -1) {
      AVAlertNote("Invalid `HWND' in GetMessage.");
      return;
    }

    TranslateMessage(&window_message);
    DispatchMessage(&window_message);
  }

  UnregisterClassW(window_class.lpszClassName, gHINSTANCE);
}

ACCB1 ASBool ACCB2 LaunchRepromaticWindowIsEnabled(void *clientData) {
  return true;
}
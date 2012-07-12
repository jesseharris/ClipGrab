#include <windows.h>

#define IDM_FILE_EXIT 9001
#define IDM_SAVE 9002
#define IDM_SAVE_AS 9003

LPCTSTR ClsName = L"BasicApp";
LPCTSTR WndName = L"Clipboard Logger";

HWND hWndNextViewer;
HWND hEdit;

LRESULT CALLBACK WndProcedure(HWND hwnd, UINT uMsg,
			      WPARAM wParam, LPARAM lParam);

void AppendTextToEditCtrl(HWND hWndEdit, LPCTSTR pszText);
BOOL SaveTextFileFromEdit(HWND hEdit, LPCTSTR pszFileName);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
               LPSTR lpCmdLine, int nCmdShow)
{
	MSG        Msg;
	HWND       hwnd;
	WNDCLASSEX WndClsEx;

	// Create the application window
	WndClsEx.cbSize        = sizeof(WNDCLASSEX);
	WndClsEx.style         = CS_HREDRAW | CS_VREDRAW;
	WndClsEx.lpfnWndProc   = WndProcedure;
	WndClsEx.cbClsExtra    = 0;
	WndClsEx.cbWndExtra    = 0;
	WndClsEx.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	WndClsEx.hCursor       = LoadCursor(NULL, IDC_ARROW);
	WndClsEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClsEx.lpszMenuName  = NULL;
	WndClsEx.lpszClassName = ClsName;
	WndClsEx.hInstance     = hInstance;
	WndClsEx.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

	// Register the application
	RegisterClassEx(&WndClsEx);

	// Create the window object
	hwnd = CreateWindow(ClsName,
			  WndName,
			  WS_OVERLAPPEDWINDOW,
			  CW_USEDEFAULT,
			  CW_USEDEFAULT,
			  CW_USEDEFAULT,
			  CW_USEDEFAULT,
			  NULL,
			  NULL,
			  hInstance,
			  NULL);
	
	// Find out if the window was created
	if( !hwnd ) // If the window was not created,
		return 0; // stop the application

	// Display the window to the user
	ShowWindow(hwnd, SW_SHOWNORMAL);
	UpdateWindow(hwnd);

	// Decode and treat the messages
	// as long as the application is running
	while( GetMessage(&Msg, NULL, 0, 0) > 0)
	{
             TranslateMessage(&Msg);
             DispatchMessage(&Msg);
	}

	return Msg.wParam;
}

LRESULT CALLBACK WndProcedure(HWND hwnd, UINT Msg,
			   WPARAM wParam, LPARAM lParam)
{
    switch(Msg)
    {
    case WM_CREATE: 
        // Add the window to the clipboard viewer chain. 
        hWndNextViewer = SetClipboardViewer(hwnd); 
        {
			HFONT hfDefault;

			RECT rect;
			GetClientRect(hwnd, &rect);

			hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", 
				WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_READONLY, 
				0, 0, rect.right, rect.bottom, hwnd, (HMENU)101, GetModuleHandle(NULL), NULL);
			if(hEdit == NULL)
				MessageBox(hwnd, L"Could not create edit box.", L"Error", MB_OK | MB_ICONERROR);

			hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			SendMessage(hEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

			// Menu Section
			HMENU hMenu, hSubMenu;

            hMenu = CreateMenu();

            hSubMenu = CreatePopupMenu();
			AppendMenu(hSubMenu, MF_STRING, IDM_SAVE_AS, L"Save &As");
			AppendMenu(hSubMenu, MF_SEPARATOR, 0, NULL) ;
            AppendMenu(hSubMenu, MF_STRING, IDM_FILE_EXIT, L"E&xit");
			AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, L"&File");

            //hSubMenu = CreatePopupMenu();
            //AppendMenu(hSubMenu, MF_STRING, IDM_SAVE_AS, L"Save &As");
            //AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, L"&Stuff");

            SetMenu(hwnd, hMenu);
        }
		break;
    case WM_CHANGECBCHAIN: 
        // If the next window is closing, repair the chain.  
        if ((HWND) wParam == hWndNextViewer) 
            hWndNextViewer = (HWND) lParam; 
        // Otherwise, pass the message to the next link. 
        else if (hWndNextViewer != NULL) 
            SendMessage(hWndNextViewer, Msg, wParam, lParam); 
        break;
    case WM_DRAWCLIPBOARD:  // clipboard contents changed. 
        // Get the TEXT contents of the clipboard(if any)
		// and display in a dialog 
		// Do not get the clipboard if the hEdit set it
		if (GetClipboardOwner() == hEdit)
		{
			break;
		}
		if (OpenClipboard(hwnd)) 
		{
			HGLOBAL hglb = GetClipboardData(CF_UNICODETEXT);  
			LPCWSTR lpstr =  (LPCWSTR)GlobalLock(hglb); 
 
			GlobalUnlock(hglb); 
			CloseClipboard(); 
 
			if(GetWindowTextLength(hEdit) > 0)
			{
				AppendTextToEditCtrl(hEdit, L"\r\n");
			}

			AppendTextToEditCtrl(hEdit, lpstr);

			// Pass the message to the next window in clipboard 
			// viewer chain. 
			SendMessage(hWndNextViewer, Msg, wParam, lParam); 
		}
        break;
    
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		    case IDM_FILE_EXIT:
		    ChangeClipboardChain(hwnd, hWndNextViewer);
			PostQuitMessage(WM_QUIT);
			break;
			
			case IDM_SAVE:
			break;
			
			case IDM_SAVE_AS:
			//Save Dialog
			wchar_t fileName[MAX_PATH] = L"";
		    OPENFILENAME saveFileDialog;
			ZeroMemory(&saveFileDialog, sizeof(OPENFILENAME));
			saveFileDialog.lStructSize= sizeof(OPENFILENAME);
			saveFileDialog.hwndOwner = hwnd;
			saveFileDialog.lpstrFilter = L"Text Files (*.txt)\0*txt\0All Files (*.*)\0*.*\0";
			saveFileDialog.lpstrFile = fileName;
			saveFileDialog.nMaxFile = MAX_PATH;
			saveFileDialog.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY |OFN_OVERWRITEPROMPT;
			saveFileDialog.lpstrDefExt = L"txt";
			
		    if(GetSaveFileName(&saveFileDialog)){
			    LPWSTR returnstring = saveFileDialog.lpstrFile;
				OutputDebugString(L"Allow");
				OutputDebugString(returnstring);
                SaveTextFileFromEdit(hEdit, (LPCTSTR) returnstring);
			}
            break;
		}
		break;
	case WM_SIZE:
	{	
		RECT rect;
		GetClientRect(hwnd, &rect);

		SetWindowPos(hEdit, NULL, 0, 0, rect.right, rect.bottom, SWP_NOZORDER);
	}
	break;
    case WM_SIZING:
	{	
		RECT rect;
		GetClientRect(hwnd, &rect);

		SetWindowPos(hEdit, NULL, 0, 0, rect.right, rect.bottom, SWP_NOZORDER);
	}
	break;
	// If the user wants to close the application
	case WM_DESTROY:
        // then close it
        ChangeClipboardChain(hwnd, hWndNextViewer);
		PostQuitMessage(WM_QUIT);
        break;
    default:
        // Process the left-over messages
        return DefWindowProc(hwnd, Msg, wParam, lParam);
    }
    // If something was not done, let it go
    return 0;
}

void AppendTextToEditCtrl(HWND hWndEdit, LPCTSTR pszText)
{
   int nLength = GetWindowTextLength(hWndEdit); 
   SendMessage(hWndEdit, EM_SETSEL, (WPARAM)nLength, (LPARAM)nLength);
   SendMessage(hWndEdit, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)pszText);
}

BOOL SaveTextFileFromEdit(HWND hEdit, LPCTSTR pszFileName)
{
    HANDLE hFile;
    BOOL bSuccess = FALSE;

    hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile != INVALID_HANDLE_VALUE)
    {
        DWORD dwTextLength;

        dwTextLength = GetWindowTextLength(hEdit);
        // No need to bother if there's no text.
        if(dwTextLength > 0)
        {
            LPWSTR pszText;
            DWORD dwBufferSize = dwTextLength + 1;

            pszText = (LPWSTR)GlobalAlloc(GPTR, dwBufferSize * 2);
            if(pszText != NULL)
            {
                if(GetWindowText(hEdit, pszText, dwBufferSize))
                {
                    DWORD dwWritten;

					unsigned char Header[2]; //unicode text file header
					Header[0] = 0xFF;
					Header[1] = 0xFE;
					WriteFile(hFile, Header, 2, &dwWritten, NULL);
                    if(WriteFile(hFile, pszText, dwTextLength * 2, &dwWritten, NULL))
                        bSuccess = TRUE;
                }
                GlobalFree(pszText);
            }
        }
        CloseHandle(hFile);
    }
    return bSuccess;
}
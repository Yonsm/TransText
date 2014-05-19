


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Header
#include "Main.h"
#include "TransText.h"
#include "MemFile.h"

#include <CommCtrl.h>

#pragma warning(disable:4244)
#pragma comment(lib, "ShLwAPI.lib")
#pragma comment(lib, "ComCtl32.lib")
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global
CTransText TT;
HWND g_hWnd = NULL;
HWND g_hPage = NULL;
HINSTANCE g_hInst = NULL;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
#define STR_Magic	"Microsoft Visual Studio Solution File, Format Version "
#define STR_VSMagic	"Visual Studio 200"
UINT SmartVSVer(PCTSTR ptzPath)
{
	if (UStrCmp(ptzPath + UStrLen(ptzPath) - 4, TEXT(".sln")) == 0)
	{
		CMemFile mf(ptzPath, FALSE);
		if (PBYTE(mf))
		{
			PCHAR p = PCHAR(mf) + 5;
			if (UMemCmp(p, STR_Magic, sizeof(STR_Magic) - 1) == 0)
			{
				p += sizeof(STR_Magic) - 1;
				if ((*p == '9') || ((p[0] == '1') && (p[1] == '0')))
				{
					TCHAR tzStr[MAX_PATH];
					TCHAR tzMsg[MAX_PATH];
					UStrPrint(tzMsg, _GetStr(IDS_SmartVSVer), (*p == '9') ? '5' : '8', (*p == '9') ? '8' : '5');
					if (MessageBox(g_hWnd, tzMsg, STR_AppName, MB_YESNO | MB_ICONQUESTION) == IDYES)
					{
						PCHAR q = PCHAR(mf) + UINT(mf);
						for (PCHAR r = p + 6; r < q; r++)
						{
							if (UMemCmp(r, STR_VSMagic, sizeof(STR_VSMagic) - 1) == 0)
							{
								r += sizeof(STR_VSMagic) - 1;
								*r = (*r == '5') ? '8' : '5';
								UMemCopy(p, (*p == '9') ? "10.00\n" : "9.00\r\n", 6);
								return *r;
							}
						}
						UStrPrint(tzMsg, _GetStr(MSG_FetchNothing), TEXT(STR_VSMagic));
						MessageBox(g_hWnd, tzMsg, STR_AppName, MB_ICONERROR);
					}					
				}
			}
		}
	}
	return FALSE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Browse file path
BOOL BrowseFile(HWND hWnd, UINT uCtrl, UINT uFilter, BOOL bSave = FALSE)
{
	TCHAR tzPath[MAX_PATH];
	GetDlgItemText(hWnd, uCtrl, tzPath, MAX_PATH);

	TCHAR tzName[32];
	TCHAR tzFilter[MAX_PATH];
	UStrPrint(tzName, TEXT("Filter%d"), uFilter);
	if (!TT.GetStr(tzName, tzFilter))
	{
		_LoadStr(uFilter, tzFilter);
		TT.SetStr(tzName, tzFilter);
	}
	UStrRep(tzFilter, '|', 0);

	OPENFILENAME ofn = {0};
	ofn.hwndOwner = hWnd;
	ofn.hInstance = g_hInst;
	ofn.lpstrFile = tzPath;
	ofn.lpstrFilter = tzFilter;
	ofn.nMaxFile = MAX_PATH;
	ofn.lStructSize = sizeof(OPENFILENAME);

	BOOL bRet;
	if (bSave)
	{
		ofn.Flags = OFN_ENABLESIZING | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
		bRet = GetSaveFileName(&ofn);
	}
	else
	{
		ofn.Flags = OFN_ENABLESIZING | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
		bRet = GetOpenFileName(&ofn);
	}

	if (bRet)
	{
		SetDlgItemText(hWnd, uCtrl, tzPath);
	}

	return bRet;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Fetch dialog
INT_PTR CALLBACK PageDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TCHAR tzStr[MAX_PATH];

	switch (uMsg)
	{
	case WM_INITDIALOG:
		if (TT.m_uConfirm) CheckDlgButton(hWnd, IDC_Confirm, BST_CHECKED);
		if (TT.m_uRecursive) CheckDlgButton(hWnd, IDC_Recursive, BST_CHECKED);
		
		if (TT.m_uCurPage == 0)
		{
			SetDlgItemText(hWnd, IDC_Exclude, TT.m_tzSections);
			if (TT.m_uEnglishOnly) CheckDlgButton(hWnd, IDC_EnglishOnly, BST_CHECKED);

			SendDlgItemMessage(hWnd, IDC_MinCount, EM_LIMITTEXT, 3, 0);
			SetDlgItemInt(hWnd, IDC_MinCount, TT.m_uMinCount, FALSE);
			SendDlgItemMessage(hWnd, IDC_MinCountSpin, UDM_SETRANGE32, 2, 255);
		}
		else if (TT.m_uCurPage == 1)
		{
			if (TT.m_uNoCheck) CheckDlgButton(hWnd, IDC_NoCheck, BST_CHECKED);
		}
		return TRUE;

	case WM_CTLCOLORBTN:
		return NULL_BRUSH;

	case WM_CTLCOLORSTATIC:
		// FUCK! What's going on?
		if ((GetWindowLongPtr(HWND(lParam), GWL_STYLE) & BS_TYPEMASK) == BS_AUTOCHECKBOX)
		{
			return NULL_BRUSH;
		}
		return INT_PTR(GetStockObject(NULL_BRUSH));

	case WM_ERASEBKGND:
		SetBkMode(HDC(wParam), TRANSPARENT);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_EnglishOnly:
			TT.m_uEnglishOnly = IsDlgButtonChecked(hWnd, IDC_EnglishOnly);
			break;

		case IDC_Recursive:
			TT.m_uRecursive = IsDlgButtonChecked(hWnd, IDC_Recursive);
			break;

		case IDC_NoCheck:
			TT.m_uNoCheck = IsDlgButtonChecked(hWnd, IDC_NoCheck);
			break;

		case IDC_Confirm:
			TT.m_uConfirm = IsDlgButtonChecked(hWnd, IDC_Confirm);
			break;

		case IDC_Browse:
			BrowseFile(hWnd, IDC_Path, IDS_Filter);
			break;

		case IDC_MinCount:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				UINT n = GetDlgItemInt(hWnd, IDC_MinCount, NULL, FALSE);
				if ((n >= 2) && (n <= 255))
				{
					TT.m_uMinCount = n;
				}
				else
				{
					SetDlgItemInt(hWnd, IDC_MinCount, TT.m_uMinCount, FALSE);
				}
			}
			break;

		case IDC_Path:
		case IDC_Find:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				UINT i = GetWindowTextLength(GetDlgItem(hWnd, IDC_Path));
				if (TT.m_uCurPage == 2)
				{
					UINT j = GetWindowTextLength(GetDlgItem(hWnd, IDC_Find));
					//EnableWindow(GetDlgItem(hWnd, IDC_Replace), j);
					EnableWindow(GetDlgItem(g_hWnd, IDOK), i && j);
				}
				else
				{
					EnableWindow(GetDlgItem(g_hWnd, IDOK), i);
				}
			}
			break;

		case IDC_Replace:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				SetDlgItemText(g_hWnd, IDOK, _GetStr(GetWindowTextLength(GetDlgItem(hWnd, IDC_Replace)) ? IDS_Replace : IDS_Search));
			}
		}
		break;
	}

	return FALSE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Take action
VOID OnOK()
{
	TCHAR tzPath[MAX_PATH];
	GetDlgItemText(g_hPage, IDC_Path, tzPath, MAX_PATH);
	GetDlgItemText(g_hPage, IDC_Exclude, TT.m_tzSections, MAX_PATH);

	if (TT.m_uCurPage == 2)
	{
		TCHAR tzFind[MAX_PATH];
		TCHAR tzReplace[MAX_PATH];
		GetDlgItemText(g_hPage, IDC_Find, tzFind, MAX_PATH);
		GetDlgItemText(g_hPage, IDC_Replace, tzReplace, MAX_PATH);

		TT.m_pbFind = (PBYTE) tzFind;
		TT.m_pbReplace = tzReplace[0] ? (PBYTE) tzReplace : NULL;

		BYTE bFind[MAX_PATH * 2];
		BYTE bReplace[MAX_PATH * 2];
		if (TT.m_uEncoding == ENCODING::UTF16)
		{
			UStrToWStr((PWSTR) bReplace, tzReplace, MAX_PATH);
			TT.DecodeStr((PWSTR) tzReplace, (PWSTR) bReplace, MAX_PATH, TT.m_uLeadSize);
			UStrToWStr((PWSTR) bFind, tzFind, MAX_PATH);
			TT.m_uSize = TT.DecodeStr((PWSTR) tzFind, (PWSTR) bFind, MAX_PATH, TT.m_uLeadSize) * sizeof(WCHAR);
		}
		else
		{
			if (TT.m_uEncoding == ENCODING::UTF8)
			{
				WideCharToMultiByte(CP_UTF8, 0, tzReplace, -1, (PSTR) bReplace, sizeof(bReplace) - 1, NULL, NULL);
				WideCharToMultiByte(CP_UTF8, 0, tzFind, -1, (PSTR) bFind, sizeof(bReplace) - 1, NULL, NULL);
			}
			else
			{
				UStrToAStr((PSTR) bReplace, tzReplace, MAX_PATH);
				UStrToAStr((PSTR) bFind, tzFind, MAX_PATH);
			}
			TT.DecodeStr((PSTR) tzReplace, (PSTR) bReplace, MAX_PATH, TT.m_uLeadSize);
			TT.m_uSize = TT.DecodeStr((PSTR) tzFind, (PSTR) bFind, MAX_PATH, TT.m_uLeadSize) * sizeof(CHAR);
		}
	}

	SetFocus(GetDlgItem(g_hWnd, IDC_Log));
	SetDlgItemText(g_hWnd, IDC_Log, NULL);
	TT.EnumPath(tzPath);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tab control change
VOID OnTabChange(PTSTR ptzPath = NULL)
{
	TCHAR tzPath[MAX_PATH];
	if (g_hPage)
	{
		GetDlgItemText(g_hPage, IDC_Path, tzPath, MAX_PATH);
		DestroyWindow(g_hPage);
	}
	else
	{
		UStrCopyN(tzPath, ptzPath, MAX_PATH);
	}

	HWND hTab = GetDlgItem(g_hWnd, IDC_Tab);
	TT.m_uCurPage = TabCtrl_GetCurSel(hTab);
	g_hPage = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_Fetch + TT.m_uCurPage), g_hWnd, PageDlgProc);

	RECT rt;
	GetClientRect(hTab, &rt); 
	TabCtrl_AdjustRect(hTab, FALSE, &rt);
	MapWindowPoints(hTab, g_hWnd, (PPOINT) &rt, 2);
	SetWindowPos(g_hPage, HWND_TOP, rt.left, rt.top, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);

	if (tzPath[0])
	{
		SetDlgItemText(g_hPage, IDC_Path, UStrTrim(tzPath, '"'));
	}

	if (LoadString(g_hInst, IDS_Fetch + TT.m_uCurPage, tzPath, MAX_PATH))
	{
		SetDlgItemText(g_hWnd, IDOK, tzPath);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main dialog
INT_PTR CALLBACK MainDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TCHAR tzStr[MAX_STR];
	switch (uMsg)
	{
	case WM_INITDIALOG:
		g_hWnd = hWnd;
		MainDlgProc(hWnd, WM_COMMAND, MAKELONG(IDC_Log, EN_KILLFOCUS), 0);
		SetClassLongPtr(hWnd, GCL_HICON, (LONG_PTR) LoadIcon(GetModuleHandle(NULL), _MakeIntRes(IDI_Main)));
		
		if (TT.m_uLeadCount) CheckDlgButton(hWnd, IDC_LeadCount, BST_CHECKED);
		if (TT.m_uLeadSize) CheckDlgButton(hWnd, IDC_LeadSize, BST_CHECKED);
		CheckRadioButton(hWnd, IDC_ASCII, IDC_UTF16, IDC_ASCII + TT.m_uEncoding);
		EnableWindow(GetDlgItem(hWnd, IDC_LeadCount), TT.m_uLeadSize && (TT.m_uEncoding == UTF8));

		tzStr[10] = TT.m_uEndChar;
		TT.EncodeStr(tzStr, tzStr + 10, 1);
		SetDlgItemText(hWnd, IDC_EndChar, tzStr);
		SendDlgItemMessage(hWnd, IDC_EndChar, CB_LIMITTEXT, 2, 0);
		SendDlgItemMessage(hWnd, IDC_EndChar, CB_ADDSTRING, 0, (LPARAM) TEXT("\\0"));
		SendDlgItemMessage(hWnd, IDC_EndChar, CB_ADDSTRING, 0, (LPARAM) TEXT("\\r"));
		SendDlgItemMessage(hWnd, IDC_EndChar, CB_ADDSTRING, 0, (LPARAM) TEXT("\\n"));
		SendDlgItemMessage(hWnd, IDC_EndChar, CB_ADDSTRING, 0, (LPARAM) TEXT("\\t"));

		if (HWND hTab = GetDlgItem(hWnd, IDC_Tab))
		{
			TCITEM ti;
			ti.mask = TCIF_TEXT;
			ti.pszText = tzStr;
			for (UINT i = 0; LoadString(g_hInst, IDS_Fetch + i, tzStr, MAX_NAME); i++)
			{
				TabCtrl_InsertItem(hTab, i, &ti);
			}
			TabCtrl_SetCurSel(hTab, TT.m_uCurPage);
			OnTabChange((PTSTR) lParam);

			HFONT hFont = (HFONT) SendMessage(hWnd, WM_GETFONT, 0, 0);
			if (hFont)
			{
				LOGFONT lf;
				GetObject(hFont, sizeof(LOGFONT), &lf);
				lf.lfHeight *= 2;
				lf.lfItalic = TRUE;
				hFont = CreateFontIndirect(&lf);
				SendDlgItemMessage(hWnd, IDC_Brand, WM_SETFONT, (WPARAM) hFont, 0);
			}
		}
		return TRUE;

	case WM_DROPFILES:
		DragQueryFile((HDROP) wParam, 0, tzStr, MAX_PATH);
		DragFinish((HDROP) wParam);
		SetDlgItemText(g_hPage, IDC_Path, tzStr);
		if (TT.m_uCurPage == 2)
		{
			if (UINT n = SmartVSVer(tzStr))
			{
				SetDlgItemText(g_hPage, (n == '8') ? IDC_Find : IDC_Replace, TEXT("Version=\"8.00\""));
				SetDlgItemText(g_hPage, (n == '5') ? IDC_Find : IDC_Replace, TEXT("Version=\"9.00\""));
				CheckDlgButton(g_hPage, IDC_LeadCount, BST_UNCHECKED);
				CheckDlgButton(g_hPage, IDC_LeadSize, BST_UNCHECKED);
				CheckRadioButton(g_hPage, IDC_ASCII, IDC_UTF16, IDC_ASCII);
				UStrCopy(UStrRChr(tzStr, '\\') + 1, TEXT("*.vcproj"));
				SetDlgItemText(g_hPage, IDC_Path, tzStr);
				OnOK();
				CheckDlgButton(g_hPage, IDC_Recursive, BST_CHECKED);
			}
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			OnOK();
			break;

		case IDC_About:
			UStrCopy(tzStr, STR_VersionStamp TEXT("\r\n\r\n") STR_BuildStamp TEXT("\r\n\r\n") STR_Copyright TEXT("\r\n\r\n"));
			LoadString(g_hInst, IDS_Help, tzStr + UStrLen(tzStr), MAX_STR);
			MessageBox(hWnd, tzStr, STR_AppName, MB_ICONINFORMATION);
			break;

		case IDCANCEL:
			EndDialog(hWnd, 0);
			break;

		case IDC_LeadCount:
			TT.m_uLeadCount = IsDlgButtonChecked(hWnd, IDC_LeadCount);
			break;

		case IDC_LeadSize:
		case IDC_ASCII:
		case IDC_UTF8:
		case IDC_UTF16:
			if (LOWORD(wParam) == IDC_LeadSize)
			{
				TT.m_uLeadSize = IsDlgButtonChecked(hWnd, IDC_LeadSize);
			}
			else
			{
				TT.m_uEncoding = LOWORD(wParam) - IDC_ASCII;
			}
			{
				BOOL bEnabled = TT.m_uLeadSize && (TT.m_uEncoding == UTF8);
				EnableWindow(GetDlgItem(hWnd, IDC_LeadCount), bEnabled);
				if (!bEnabled)
				{
					TT.m_uLeadCount = FALSE;
					CheckDlgButton(hWnd, IDC_LeadCount, BST_UNCHECKED);
				}
			}
			break;

		case IDC_EndChar:
			TCHAR tzPadChar[4];
			if (GetDlgItemText(g_hPage, IDC_EndChar, tzPadChar, 4))
			{
				TT.DecodeStr((PTSTR) &TT.m_uFlags, tzPadChar, 1);
			}
			break;

		case IDC_Log:
			if ((HIWORD(wParam) == EN_KILLFOCUS) || (HIWORD(wParam) == EN_SETFOCUS))
			{
				static INT h = 0;
				static RECT rt = {0};
				if (h == 0)
				{
					GetWindowRect(GetDlgItem(hWnd, IDC_Log), &rt);
					h = rt.top;
					GetWindowRect(hWnd, &rt);
					h -= rt.top + 2;
					rt.right -= rt.left;
					rt.bottom -= rt.top;
				}
				MoveWindow(hWnd, rt.left, rt.top, rt.right, (HIWORD(wParam) == EN_KILLFOCUS) ? h : rt.bottom, TRUE);
			}
			break;
		}
		break;

	case LB_ADDSTRING:
		TCHAR tzLog[MAX_STR];
		UStrPrint(tzLog, _GetStr(wParam), lParam);
		SendDlgItemMessage(hWnd, IDC_Log, EM_SETSEL, -1, 0);
		SendDlgItemMessage(hWnd, IDC_Log, EM_REPLACESEL, FALSE, (LPARAM) tzLog);
		break;

	case WM_NOTIFY:
		if (((LPNMHDR) lParam)->code == TCN_SELCHANGE)
		{
			OnTabChange();
		}
		break;
	}

	return FALSE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Entry
INT APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PTSTR ptzCmdLine, INT iShowCmd)
{
	g_hInst = hInstance;
	InitCommonControls();
	DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_Main), NULL, MainDlgProc, (LPARAM) ptzCmdLine);
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

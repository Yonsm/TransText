


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Header
#include "Define.h"
#include "UniBase.h"
#include "Resource.h"
#include "Ini.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global
extern HWND g_hWnd;
extern HINSTANCE g_hInst;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
enum ENCODING {ASCII, UTF8, UTF16};
class COpt: public CIni
{
public:
	union
	{
		UINT m_uFlags;
		struct
		{
			UINT m_uEndChar:		8;

			UINT m_uCurPage:		2;
			UINT m_uConfirm:		1;
			UINT m_uRecursive:		1;

			UINT m_uLeadCount:		1;
			UINT m_uLeadSize:		1;
			UINT m_uEncoding:		2;

			UINT m_uMinCount:		8;

			UINT m_uNoCheck:		1;
			UINT m_uEnglishOnly:	1;
		};
	};
	
	TCHAR m_tzSections[MAX_PATH];

public:
	COpt(): CIni(STR_AppName)
	{
		m_uFlags = GetInt(STR_AppName, 0x80040000);
		GetStr(TEXT("Exclude"), m_tzSections, MAX_PATH, TEXT(".text|.bss|.rsrc|.edata|.idata|.pdata|.debug|"));
	}

	~COpt()
	{
		SetInt(STR_AppName, m_uFlags);
		SetStr(TEXT("Exclude"), m_tzSections);
	}
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
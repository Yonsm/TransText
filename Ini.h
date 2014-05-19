


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Header
#pragma once
#include <Windows.h>
#define INI_AppKey				STR_AppName
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CIni class
class CIni
{
protected:
	PCTSTR m_ptzKey;
	TCHAR m_tzPath[MAX_PATH];

public:
	CIni(PCTSTR ptzKey = INI_AppKey)
	{
		m_ptzKey = ptzKey;
		lstrcpy(m_tzPath + GetModuleFileName(NULL, m_tzPath, MAX_PATH) - 4, TEXT(".ini"));
	}

	CIni(PCTSTR ptzPath, PCTSTR ptzKey)
	{
		m_ptzKey = ptzKey;
		lstrcpy(m_tzPath, ptzPath);
	}

	PCTSTR operator =(PCTSTR ptzKey)
	{
		return m_ptzKey = ptzKey;
	}

	operator PCTSTR() const
	{
		return m_tzPath;
	}

public:
	INT GetInt(PCTSTR ptzName, INT iDef = 0)
	{
		return GetPrivateProfileInt(m_ptzKey, ptzName, iDef, m_tzPath);
	}

	BOOL SetInt(PCTSTR ptzName, INT iVal = 0)
	{
		TCHAR tzStr[16];
		wsprintf(tzStr, TEXT("%d"), iVal);
		return WritePrivateProfileString(m_ptzKey, ptzName, tzStr, m_tzPath);
	}

	UINT GetStr(PCTSTR ptzName, PTSTR ptzStr, UINT uLen = MAX_PATH, PCTSTR ptzDef = NULL)
	{
		return GetPrivateProfileString(m_ptzKey, ptzName, ptzDef, ptzStr, uLen, m_tzPath);
	}

	BOOL SetStr(PCTSTR ptzName = NULL, PCTSTR ptzStr = TEXT(""))
	{
		return WritePrivateProfileString(m_ptzKey, (ptzName ? ptzName : m_ptzKey), ptzStr, m_tzPath);
	}

	BOOL GetStruct(PCTSTR ptzName, PVOID pvStruct, UINT uSize)
	{
		return GetPrivateProfileStruct(m_ptzKey, ptzName, pvStruct, uSize, m_tzPath);
	}

	BOOL SetStruct(PCTSTR ptzName, PVOID pvStruct, UINT uSize)
	{
		return WritePrivateProfileStruct(m_ptzKey, ptzName, pvStruct, uSize, m_tzPath);
	}

public:
	BOOL DelVal(PCTSTR ptzName = NULL)
	{
		return WritePrivateProfileString(m_ptzKey, (ptzName ? ptzName : m_ptzKey), NULL, m_tzPath);
	}

	BOOL DelKey()
	{
		return WritePrivateProfileString(m_ptzKey, NULL, NULL, m_tzPath);
	}

	UINT EnumVal(PTSTR ptzBuf, UINT uLen)
	{
		return GetPrivateProfileSection(m_ptzKey, ptzBuf, uLen, m_tzPath);
	}

	UINT EnumKey(PTSTR ptzBuf, UINT uLen)
	{
		return GetPrivateProfileSectionNames(ptzBuf, uLen, m_tzPath);
	}

public:
	BOOL SetSection(PCTSTR ptzStr)
	{
		return WritePrivateProfileSection(m_ptzKey, ptzStr, m_tzPath);
	}
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

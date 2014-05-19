


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Header
#pragma once
#include <Windows.h>
typedef const TCHAR *PCTSTR;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMemFile class
class CMemFile
{
private:
	HANDLE m_hFile;
	HANDLE m_hMap;
	PVOID m_pvFile;
	DWORD m_dwSize;

public:
	BOOL operator !() {return !m_pvFile;}
	operator DWORD() const {return m_dwSize;}
	operator PVOID() const {return m_pvFile;}
	operator PBYTE() const {return (PBYTE) m_pvFile;}
	operator PCHAR() const {return (PCHAR) m_pvFile;}
	operator PWCHAR() const {return (PWCHAR) m_pvFile;}

public:
	CMemFile(PCTSTR ptzPath, BOOL bRead = TRUE, DWORD dwSize = 0)
	{
		m_dwSize = 0;
		m_pvFile = NULL;
		m_hMap = NULL;
		m_hFile = CreateFile(ptzPath,
			(bRead ? GENERIC_READ : (GENERIC_READ | GENERIC_WRITE)), FILE_SHARE_READ, NULL,
			((bRead || !dwSize) ? OPEN_EXISTING : OPEN_ALWAYS), FILE_ATTRIBUTE_NORMAL, NULL);
		if (m_hFile != INVALID_HANDLE_VALUE)
		{
			m_dwSize = dwSize ? dwSize : GetFileSize(m_hFile, NULL);
			if (m_dwSize)
			{
				m_hMap = CreateFileMapping(m_hFile, NULL, bRead ? PAGE_READONLY : PAGE_READWRITE, 0, m_dwSize, NULL);
				if (m_hMap)
				{
					m_pvFile = MapViewOfFile(m_hMap, bRead ? FILE_MAP_READ : FILE_MAP_ALL_ACCESS, 0, 0, m_dwSize);
				}
			}
		}
	}

	~CMemFile()
	{
		if (m_pvFile) UnmapViewOfFile(m_pvFile);
		if (m_hMap) CloseHandle(m_hMap);
		if (m_hFile != INVALID_HANDLE_VALUE) CloseHandle(m_hFile);
	}
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

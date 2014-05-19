


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Header
#include "Define.h"
#include "UniBase.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CTransText class
class COpt;
class CTransText: public COpt
{
public:
	UINT m_uSize;
	PBYTE m_pbFind;
	PBYTE m_pbReplace;

public:
	HRESULT Fetch(PTSTR ptzPath);
	HRESULT Trans(PTSTR ptzPath);
	HRESULT Search(PCTSTR ptzPath);

	HRESULT EnumPath(PCTSTR ptzPath);

public:
	BOOL IsExcludeSection(PCBYTE pbName);

	PBYTE FetchMem(PBYTE pbDst, PBYTE pbSrc, UINT uSize);
	PBYTE TransMem(PBYTE pbDst, PBYTE pbSrc);

	template <typename T> T* FetchBuf(T* ptDst, T* ptBase, T* ptSrc, UINT uLen);
	template <typename T> T* TransBuf(PBYTE pbDst, T* ptSrc);

	template <typename T> UINT EncodeStr(T* ptDst, T* ptSrc, UINT uLen);
	template <typename T> UINT DecodeStr(T* ptDst, T* ptSrc, UINT uLen, BOOL bBStrMode = FALSE);

private:
	template<typename T> inline BOOL IsPrintable(T t)
	{
		return ((t >= ' ') && (t <= '~' )) || ((t == '\r') || (t == '\n') || (t == '\t'));
	}

	template<typename T> BOOL IsBStr(T* p)
	{
		if (WORD(*p) < WORD(m_uMinCount))
		{
			return FALSE;
		}

		__try
		{
			T* q = p + WORD(*p);
			for (p++; p <= q; p++)
			{
				if ((*p == m_uEndChar) || (m_uEnglishOnly && !IsPrintable(*p)))
				{
					return FALSE;
				}
			}
			return (*p == m_uEndChar);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return FALSE;
		}
	}

	UINT UTF8StrLen(PSTR s)
	{
		UINT i = 0, j = 0;
		while (s[i])
		{
			if ((s[i] & 0xc0) != 0x80) j++;
			i++;
		}
		return j;
	}
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
template <typename T> INT TStrToInt(T* ptStr)
{
	if (sizeof(T) == sizeof(WCHAR))
	{
		return UWStrToInt((PWSTR) ptStr);
	}
	else
	{
		return UAStrToInt((PSTR) ptStr);
	}
}

template <typename T> T* TStrChr(T* ptStr, WCHAR wChr)
{
	if (sizeof(T) == sizeof(WCHAR))
	{
		return (T*) UWStrChr((PWSTR) ptStr, wChr);
	}
	else
	{
		return (T*) UAStrChr((PSTR) ptStr, (CHAR) wChr);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

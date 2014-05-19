


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Header
#include "Main.h"
#include "TransText.h"
#include "MemFile.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Encode string
template <typename T> UINT CTransText::EncodeStr(T* ptDst, T* ptSrc, UINT uLen)
{
	T* q = ptDst;
	for (; uLen--; ptSrc++)
	{
		switch (*ptSrc)
		{
		case '\r': q[1] = 'r'; break;
		case '\n': q[1] = 'n'; break;
		case '\t': q[1] = 't'; break;
		case '\0': q[1] = '0'; break;
		case '\\': q[1] = '\\'; break;

		default:
			//if	((*ptSrc >= ' ') && (*ptSrc <= '~' ))
			{
				*q++ = *ptSrc;
			}
			/*else
			{
				if (sizeof(T) == sizeof(WCHAR))
				{
					q += UWStrPrint((PWSTR) q, L"\\x%04X", (UINT) (WORD) (*ptSrc));
				}
				else
				{
					q += UAStrPrint((PSTR) q, "\\x%02X", (UINT) (BYTE) (*ptSrc));
				}
			}*/
			continue;
		}

		q[0] = '\\';
		q += 2;
	}

	*q = 0;
	return (UINT) (q - ptDst);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decode string
template <typename T> UINT CTransText::DecodeStr(T* ptDst, T* ptSrc, UINT uLen, BOOL bBStrMode)
{
	T* p = ptSrc;
	T* q = ptDst;
	
	if (bBStrMode)
	{
		q++;
	}

	for (; *p; q++)
	{
		if (uLen == 0)
		{
			if (!m_uNoCheck)
			{
				SendMessage(g_hWnd, LB_ADDSTRING, (sizeof(T) == sizeof(WCHAR)) ? MSG_ExceedWStr : MSG_ExceedAStr, (LPARAM) ptSrc);
				break;
			}
		}
		else
		{
			uLen--;
		}

		if (*p == '\\')
		{
			switch (p[1])
			{
			case 'r':
				*q = '\r';
				break;

			case 'n':
				*q = '\n';
				break;

			case 't':
				*q = '\t';
				break;

			case '0':
				*q = '\0';
				break;

			case 'x':
				T t, *r;
				r = p;
				p += 2 + 2 * sizeof(T);
				t = *p;
				*r = '0';
				*p = 0;
				*q = TStrToInt(r);
				*p = t;
				continue;

			default:
				*q = p[1];
				break;
			}
			p += 2;
		}
		else
		{
			*q = *p++;
		}
	}

	UINT uRet = (UINT) (q - ptDst);
	if (bBStrMode)
	{
		*ptDst = (uRet - 1);
		uLen--;
	}

	while (uLen)
	{
		uLen--;
		*q++ = m_uEndChar;
	}

	return uRet;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Fetch sting from buffer
template <typename T> T* CTransText::FetchBuf(T* ptDst, T* ptBase, T* ptSrc, UINT uLen)
{
	if (m_uLeadSize)
	{
		UINT i = 0;
		T* p = NULL;
		for (T* ptEnd = ptSrc + (uLen / sizeof(T)); ptSrc < ptEnd; ptSrc++)
		{
			if (IsBStr(ptSrc))
			{
				UINT uLen;
				if (sizeof(T) == sizeof(WCHAR))
				{
					uLen = (WORD)*ptSrc;
					ptDst += UWStrPrint((PWSTR) ptDst, L"\r\n0x%08X,%04d=", (ptSrc + 1 - ptBase) * sizeof(WCHAR), uLen);
				}
				else
				{
					// Further more checking
					if ((m_uEncoding == UTF8) && m_uLeadCount)
					{
						UINT n1 = (BYTE)ptSrc[-1];
						UINT n2 = UTF8StrLen(PSTR(ptSrc + 1));
						if (n1 != n2)
						{
							continue;
						}
					}

					uLen = (BYTE)*ptSrc;
					ptDst += UAStrPrint((PSTR) ptDst, "\r\n0x%08X,%04d=", (ptSrc + 1 - ptBase), uLen);
				}

				ptDst += EncodeStr(ptDst, ptSrc + 1, uLen);
				ptSrc += *ptSrc;
			}
		}
		return ptDst;
	}

	UINT i = 0;
	T* p = NULL;
	for (T* ptEnd = ptSrc + (uLen / sizeof(T)); ptSrc < ptEnd; ptSrc++)
	{
		if (*ptSrc == m_uEndChar)
		{
			if (i >= m_uMinCount)
			{
				if (sizeof(T) == sizeof(WCHAR))
				{
					ptDst += UWStrPrint((PWSTR) ptDst, L"\r\n0x%08X,%04d=", (p - ptBase) * sizeof(WCHAR), ptSrc - p);
				}
				else
				{
					ptDst += UAStrPrint((PSTR) ptDst, "\r\n0x%08X,%04d=", p - ptBase, ptSrc - p);
				}

				ptDst += EncodeStr(ptDst, p, (UINT) (ptSrc - p));
			}
			i = 0;
			p = NULL;
		}
		else if	(IsPrintable(*ptSrc))
		{
			if (p == NULL)
			{
				p = ptSrc;
			}

			if (UChrIsAlpha(*ptSrc))
			{
				i++;
			}
			else if (i < m_uMinCount)
			{
				i = 0;
			}
		}
		else
		{
			if (m_uEnglishOnly || (i < m_uMinCount))
			{
				i = 0;
				p = NULL;
			}
		}
	}
	return ptDst;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Exclude some sections
BOOL CTransText::IsExcludeSection(PCBYTE pbName)
{
	for (PTSTR p = m_tzSections; *p; p++)
	{
		UINT i = 0;
		while ((i < IMAGE_SIZEOF_SHORT_NAME) && (p[i] == pbName[i])) i++;
		if (p[i] == '|')
		{
			if ((i == IMAGE_SIZEOF_SHORT_NAME) || (pbName[i] == 0))
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Fetch from memory
PBYTE CTransText::FetchMem(PBYTE pbDst, PBYTE pbSrc, UINT uSize)
{
	__try
	{
		// Verify DOS signature
		if (m_tzSections[0] && (((PIMAGE_DOS_HEADER) pbSrc)->e_magic == IMAGE_DOS_SIGNATURE))
		{
			// Verify PE signature
			PIMAGE_NT_HEADERS pNTHdr = (PIMAGE_NT_HEADERS) (pbSrc + ((PIMAGE_DOS_HEADER) pbSrc)->e_lfanew);
			if (pNTHdr->Signature == IMAGE_NT_SIGNATURE)
			{
				// Lookup PE sections
				PIMAGE_SECTION_HEADER pSecHdr = (PIMAGE_SECTION_HEADER) (pNTHdr + 1);
				for (UINT i = pNTHdr->FileHeader.NumberOfSections; i; i--, pSecHdr++)
				{
					if (!IsExcludeSection(pSecHdr->Name))
					{
						if (m_uEncoding == ENCODING::UTF16)
						{
							pbDst = (PBYTE) FetchBuf((PWSTR) pbDst, (PWSTR) pbSrc, (PWSTR) (pbSrc + pSecHdr->PointerToRawData), pSecHdr->SizeOfRawData);
						}
						else
						{
							pbDst = (PBYTE) FetchBuf((PSTR) pbDst, (PSTR) pbSrc, (PSTR) (pbSrc + pSecHdr->PointerToRawData), pSecHdr->SizeOfRawData);
						}
					}
				}
				return pbDst;
			}
		}

		// Fetch the whole file
		if (m_uEncoding == ENCODING::UTF16)
		{
			return (PBYTE) FetchBuf((PWSTR) pbDst, (PWSTR) pbSrc, (PWSTR) pbSrc, uSize);
		}
		else
		{
			return (PBYTE) FetchBuf((PSTR) pbDst, (PSTR) pbSrc, (PSTR) pbSrc, uSize);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return NULL;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Fetch string from file
#define INI_ASCII "[TransText]"
#define INI_UTF8 "\xEF\xBB\xBF[TransText]"
#define INI_UNICODE L"\xFEFF[TransText]"
HRESULT CTransText::Fetch(PTSTR ptzPath)
{
	// Open source file
	CMemFile fSrc(ptzPath);
	if (!fSrc)
	{
		return ERR_MapFile;
	}
	else if (fSrc < 512)
	{
		return ERR_ShortSize;
	}

	// Alloc memory
	PBYTE pbDst = (PBYTE) UMemAlloc(fSrc * 2);
	if (!pbDst)
	{
		return ERR_AllocMemory;
	}

	// Write header
	PBYTE q = pbDst;
	if (m_uEncoding == ENCODING::UTF16)
	{
		UMemCopy(q, INI_UNICODE, sizeof(INI_UNICODE) - sizeof(WCHAR));
		q += sizeof(INI_UNICODE) - sizeof(WCHAR);
	}
	else if (m_uEncoding == ENCODING::UTF8)
	{
		UMemCopy(q, INI_UTF8, sizeof(INI_UTF8) - sizeof(CHAR));
		q += sizeof(INI_UTF8) - sizeof(CHAR);
	}
	else
	{
		UMemCopy(q, INI_ASCII, sizeof(INI_ASCII) - sizeof(CHAR));
		q += sizeof(INI_ASCII) - sizeof(CHAR);
	}

	// Fetch strings from memory
	q = FetchMem(q, fSrc, fSrc);
	if (!q)
	{
		return ERR_Exception;
	}

	// Write CR/LF
	if (m_uEncoding == ENCODING::UTF16)
	{
		*((PWSTR) q) = '\r'; q += 2;
		*((PWSTR) q) = '\n'; q += 2;
	}
	else
	{
		*q++ = '\r';
		*q++ = '\n';
	}

	// Check buffer size
	UINT uSize = (UINT) (q - pbDst);
	if (uSize < (sizeof(INI_UNICODE) + 4))
	{
		return MSG_FetchNothing;
	}

	// Write to destination file
	UStrCat(ptzPath, TEXT(".ttt"));
	return UFileSave(ptzPath, pbDst, uSize) ? MSG_FetchOK : ERR_SaveFile;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Translate all strings to target
template <typename T> T* CTransText::TransBuf(PBYTE pbDst, T* ptSrc)
{
	T* p = ptSrc;
	for (T* q = ptSrc; *q; q++)
	{
		if ((*q == '\r') || (*q == '\n'))
		{
			*q = 0;
			if (p[0] == '0')
			{
				UINT uOffset = TStrToInt(p);
				p = TStrChr(p, ',');
				if (p)
				{
					UINT uLen = TStrToInt(++p);
					p = TStrChr(p, '=');
					if (p)
					{
						T* ptDst = (T*) (pbDst + uOffset);
						UINT nRet = DecodeStr(ptDst, p + 1, uLen);
						if (m_uLeadSize)
						{
							if (!m_uNoCheck && (ptDst[-1] != uLen))
							{
								SendMessage(g_hWnd, LB_ADDSTRING, MSG_BStrNotMatch, uOffset);
							}
							else
							{
								ptDst[-1] = nRet;
								if ((m_uEncoding == UTF8) && m_uLeadCount)
								{
									BYTE n = UTF8StrLen((PSTR)ptDst);
									ptDst[-2] = n;
								}
							}
						}
					}
				}
			}
			for (q++; (*q == '\r') || (*q == '\n'); q++);
			p = q;
		}
	}
	return ptSrc;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Translate memory buffer
PBYTE CTransText::TransMem(PBYTE pbDst, PBYTE pbSrc)
{
	//__try
	{
		if (pbSrc[3])
		{
			return (PBYTE) TransBuf(pbDst, (PSTR) pbSrc);
		}
		else
		{
			return (PBYTE) TransBuf(pbDst, (PWSTR) pbSrc);
		}
	}
	//__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return NULL;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Write back to target file
HRESULT CTransText::Trans(PTSTR ptzPath)
{
	PTSTR ptzFile = ptzPath + UStrLen(ptzPath);
	UStrCopy(ptzFile, TEXT(".tt?"));

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(ptzPath, &fd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return MSG_FileNotFound;
	}

	*ptzFile = 0;
	CMemFile fDst(ptzPath, FALSE);
	if (!fDst)
	{
		return ERR_MapFile;
	}
	*ptzFile = '.';

	ptzFile = UStrRChr(ptzPath, '\\');
	ptzFile = ptzFile ? (ptzFile + 1) : ptzPath;

	HRESULT hResult = MSG_FileNotFound;
	do
	{
		if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			if (hResult != MSG_FileNotFound)
			{
				SendMessage(g_hWnd, LB_ADDSTRING, hResult, (LPARAM) ptzPath);
			}

			UStrPrint(ptzFile, fd.cFileName);
			PVOID pvSrc = UFileLoad(ptzPath);
			if (pvSrc)
			{
				hResult = TransMem(fDst, (PBYTE) pvSrc) ? MSG_TransOK : ERR_Exception;
				UMemFree(pvSrc);
			}
			else
			{
				hResult = ERR_OpenFile;
			}
		}
	}
	while (FindNextFile(hFind, &fd));
	FindClose(hFind);

	return hResult;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Search and replace string
HRESULT CTransText::Search(PCTSTR ptzPath)
{
	CMemFile f(ptzPath, !m_pbReplace);
	if (!f)
	{
		return ERR_MapFile;
	}

	TCHAR tzAction[32];
	GetDlgItemText(g_hWnd, IDOK, tzAction, MAX_PATH);

	PBYTE pbEnd = (PBYTE) f + (UINT) f - m_uSize;
	for (PBYTE p = f; p < pbEnd; )
	{
		if (UMemCmp(p, m_pbFind, m_uSize) == 0)
		{
			TCHAR tzStr[MAX_PATH];
			TCHAR tzMsg[MAX_PATH * 2];

			UStrPrint(tzMsg, TEXT("%s:%08X"), ptzPath, p - (PBYTE) f);
			SendMessage(g_hWnd, LB_ADDSTRING, MSG_FoundStr, (LPARAM) tzMsg);

			UStrPrint(tzMsg, _GetStr(IDS_SearchAsk), ptzPath, p - (PBYTE) f, tzAction);
			UINT i = m_uConfirm ? MessageBox(g_hWnd, tzMsg, STR_AppName, MB_ICONQUESTION | (m_pbReplace ? MB_YESNOCANCEL : MB_OKCANCEL)) : IDYES;
			if (i == IDYES)
			{
				if (m_pbReplace)
				{
					UMemCopy(p, m_pbReplace, m_uSize);
				}
			}
			else if (i == IDCANCEL)
			{
				return MSG_SearchAbort;
			}
			p += m_uSize;
		}
		else
		{
			p++;
		}
	}

	return MSG_SearchOK;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Enum path
HRESULT CTransText::EnumPath(PCTSTR ptzPath)
{
	PCTSTR ptzFile = UStrRChr(ptzPath, '\\');
	ptzFile = ptzFile ? (ptzFile + 1) : ptzPath;

	TCHAR tzFindPath[MAX_PATH];
	UStrCopy(tzFindPath, ptzPath);
	PTSTR ptzFindFile = tzFindPath + UINT(ptzFile - ptzPath);

	WIN32_FIND_DATA fd;
	HRESULT hResult = ERROR_FILE_NOT_FOUND;
	HANDLE hFind = FindFirstFile(tzFindPath, &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				UStrCopy(ptzFindFile, fd.cFileName);

				// Tack action
				if (m_uCurPage == 0)
				{
					hResult = Fetch(tzFindPath);
				}
				else if (m_uCurPage == 1)
				{
					hResult = Trans(tzFindPath);
				}
				else
				{
					hResult = Search(tzFindPath);
				}
				SendMessage(g_hWnd, LB_ADDSTRING, hResult, (LPARAM) tzFindPath);
				if (hResult == MSG_SearchAbort)
				{
					FindClose(hFind);
					return MSG_SearchAbort;
				}
			}
		}
		while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	}

	if (hResult == ERROR_FILE_NOT_FOUND)
	{
		SendMessage(g_hWnd, LB_ADDSTRING, MSG_FileNotFound, (LPARAM) tzFindPath);
	}

	if (m_uRecursive)
	{
		// Enum sub directory
		ptzFindFile[0] = '*';
		ptzFindFile[1] = 0;
		HANDLE hFind = FindFirstFile(tzFindPath, &fd);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if ((fd.cFileName[0] != '.') && (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					UStrPrint(ptzFindFile, TEXT("%s\\%s"), fd.cFileName, ptzFile);
					if (EnumPath(tzFindPath) == MSG_SearchAbort)
					{
						FindClose(hFind);
						return MSG_SearchAbort;
					}
				}
			}
			while (FindNextFile(hFind, &fd));
			FindClose(hFind);
		}
	}

	return hResult;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

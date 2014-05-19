

/**********************************************************************************************************************/
/* Uniform Base Library */
/* Version 1.0.6 WIN32 */
#pragma once

//#define _LOGMSG
//#define _LOGCON
//#define _LOGFILE

#define _WINSTR
#ifndef WINCE
#define _SHLSTR
#endif

#if defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS

#include <TChar.h>
#include <StdIO.h>
#include <StdLib.h>
#include <String.h>
#include <Memory.h>
#include <Windows.h>
#include <ShLwApi.h>
/**********************************************************************************************************************/



/**********************************************************************************************************************/
/* Type */
#define VOID						void
typedef void						*PVOID;
typedef void						*HANDLE;
typedef HANDLE						*PHANDLE;
typedef int							BOOL, *PBOOL;
typedef float						FLOAT, *PFLOAT;
typedef double						DOUBLE, *PDOUBLE;

typedef int							INT, *PINT;
typedef signed char					INT8, *PINT8;
typedef signed short				INT16, *PINT16;
typedef signed int					INT32, *PINT32;
typedef signed __int64				INT64, *PINT64;

typedef unsigned int				UINT, *PUINT;
typedef unsigned char				UINT8, *PUINT8;
typedef unsigned short				UINT16, *PUINT16;
typedef unsigned int				UINT32, *PUINT32;
typedef unsigned __int64			UINT64, *PUINT64;

typedef unsigned char				BYTE, *PBYTE;
typedef unsigned short				WORD, *PWORD;
typedef unsigned long				DWORD, *PDWORD;
typedef unsigned __int64			QWORD, *PQWORD;

#ifndef _WCHAR_T_DEFINED
#define _WCHAR_T_DEFINED
typedef unsigned short				wchar_t;
#endif

typedef char						CHAR, *PCHAR;
typedef wchar_t						WCHAR, *PWCHAR;
#ifdef _UNICODE
typedef WCHAR						TCHAR, *PTCHAR;
#else
typedef CHAR						TCHAR, *PTCHAR;
#endif

typedef CHAR						*PSTR;
typedef WCHAR						*PWSTR;
typedef TCHAR						*PTSTR;
typedef const CHAR					*PCSTR;
typedef const WCHAR					*PCWSTR;
typedef const TCHAR					*PCTSTR;

typedef const VOID					*PCVOID;
typedef const BYTE					*PCBYTE;

#ifndef VALIST
#define VALIST						va_list
#endif
#ifndef CONST
#define CONST						const
#endif

#ifndef STATIC
#define STATIC						static
#endif

#ifndef TEXT
#ifdef _UNICODE
#define TEXT(t)						L##t
#else
#define TEXT(t)						t
#endif
#endif
/**********************************************************************************************************************/



/**********************************************************************************************************************/
/* Const */
#ifndef TRUE
#define TRUE						1
#endif

#ifndef FALSE
#define FALSE						0
#endif

#ifndef NULL
#define NULL						0
#endif

#ifndef MAX_STR
#define MAX_STR						1024
#endif

#ifndef MAX_PATH
#define MAX_PATH					260
#endif

#ifndef MAX_NAME
#define MAX_NAME					80
#endif

#define UFILE_READ					GENERIC_READ
#define UFILE_WRITE					GENERIC_WRITE
#define UFILE_READWRITE				(GENERIC_READ | GENERIC_WRITE)

#define UFILE_BEGIN					FILE_BEGIN
#define UFILE_CURRENT				FILE_CURRENT
#define UFILE_END					FILE_END
/**********************************************************************************************************************/



/**********************************************************************************************************************/
// Define
#ifndef UCALL
#define UCALL						WINAPI
#endif

#ifndef UAPI
#define UAPI						__inline
#endif

#ifdef __cplusplus
#define UDEF(x)						= x
#else
#define UDEF(x)
#endif
typedef SYSTEMTIME					UTIME, *PUTIME;
typedef DWORD						(UCALL* UPROC)(PVOID pvParam);
/**********************************************************************************************************************/



/**********************************************************************************************************************/
/* Macro */
#define _NumOf(s)					(sizeof(s) / sizeof(s[0]))
#define _Zero(p)					UMemSet(p, 0, sizeof(*p))
#define _Max(a, b)					(((a) > (b)) ? (a) : (b))
#define _Min(a, b)					(((a) < (b)) ? (a) : (b))

#define _SafeFree(p)				if (p) {UMemFree(p); p = NULL;}
#define _SafeDelete(p)				if (p) {delete p; p = NULL;}
#define _SafeRelease(p)				if (p) {(p)->Release(); p = NULL;}


// WIN32 only
#define _RectWidth(r)				((r).right - (r).left)
#define _RectHeight(r)				((r).bottom - (r).top)

#define _LoadStr(i, t)				(LoadString(g_hInst, i, t, _NumOf(t)), t)
#define _GetStr(i)					_LoadStr(i, tzStr)

#define _IsIntRes(r)				((((DWORD) (r)) >> 16) == 0)
#define _MakeIntRes(i)				((PTSTR) ((DWORD) (i)))

#ifdef __cplusplus
template <typename T> VOID _Swap(T& t1, T& t2) {T t; t = t1; t1 = t2; t2 = t;}
#else
#define _Swap(t1, t2, T)			{T t; t = t1; t1 = t2; t2 = t;}
#endif
/**********************************************************************************************************************/



/**********************************************************************************************************************/
// Memory
UAPI PVOID UMemAlloc(UINT uSize)
{
	return malloc(uSize);
}

UAPI PVOID UMemRealloc(PVOID pvMem, UINT uSize)
{
	return realloc(pvMem, uSize);
}

UAPI VOID UMemFree(PVOID pvMem)
{
	free(pvMem);
}

UAPI PVOID UMemSet(PVOID pvMem, CHAR cVal, UINT uSize)
{
	return memset(pvMem, cVal, uSize);
}

UAPI PVOID UMemCopy(PVOID pvDst, PCVOID pvSrc, UINT uSize)
{
	return memcpy(pvDst, pvSrc, uSize);
}

UAPI PVOID UMemMove(PVOID pvDst, PCVOID pvSrc, UINT uSize)
{
	return memmove(pvDst, pvSrc, uSize);
}

UAPI INT UMemCmp(PCVOID pvMem1, PCVOID pvMem2, UINT uSize)
{
	return memcmp(pvMem1, pvMem2, uSize);
}
/**********************************************************************************************************************/



/**********************************************************************************************************************/
/* ASCII char */
UAPI BOOL UAChrIsNum(CHAR c)
{
	return (c >= '0') && (c <= '9');
}

UAPI BOOL UAChrIsAlpha(CHAR c)
{
	return (c >= 'A') && (c <= 'Z') || (c >= 'a') && (c <= 'z');
}

UAPI BOOL UAChrIsPrintable(CHAR c)
{
	return ((c >= ' ') && (c <= '~' )) || (c == '\r') || (c == '\n') || (c == '\t');
}

UAPI BOOL UAChrMatch(CHAR c1, CHAR c2)
{
	return ((c1 == c2) || (c1 == c2 - 'A' + 'a') || (c1 == c2 + 'A' - 'a'));
}

UAPI CHAR UAChrToLower(CHAR c)
{
	return ((c >= 'A') && (c <= 'Z')) ? (c - 'A' + 'a') : c;
}

UAPI CHAR UAChrToUpper(CHAR c)
{
	return ((c >= 'a') && (c <= 'z')) ? (c + 'A' - 'a') : c;
}
/**********************************************************************************************************************/



/**********************************************************************************************************************/
/* UNICODE char */
UAPI BOOL UWChrIsNum(WCHAR w)
{
	return (w >= '0') && (w <= '9');
}

UAPI BOOL UWChrIsAlpha(WCHAR w)
{
	return (w >= 'A') && (w <= 'Z') || (w >= 'a') && (w <= 'z');
}

UAPI BOOL UWChrIsPrintable(WCHAR w)
{
	return ((w >= ' ') && (w <= '~' )) || (w == '\r') || (w == '\n') || (w == '\t');
}

UAPI BOOL UWChrMatch(WCHAR w1, WCHAR w2)
{
	return ((w1 == w2) || (w1 == w2 - 'A' + 'a') || (w1 == w2 + 'A' - 'a'));
}

UAPI WCHAR UWChrToLower(WCHAR w)
{
	return ((w >= 'A') && (w <= 'Z')) ? (w - 'A' + 'a') : w;
}

UAPI WCHAR UWChrToUpper(WCHAR w)
{
	return ((w >= 'a') && (w <= 'z')) ? (w + 'A' - 'a') : w;
}
/**********************************************************************************************************************/



/**********************************************************************************************************************/
/* String conversion */
UAPI PWSTR UAStrToWStr(PWSTR pwzDst, PCSTR pszSrc, UINT uLen)
{
	MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, pwzDst, uLen);
	return pwzDst;
}

UAPI PSTR UWStrToAStr(PSTR pszDst, PCWSTR pwzSrc, UINT uLen)
{
	WideCharToMultiByte(CP_ACP, 0,pwzSrc, -1, pszDst, uLen, NULL, NULL);
	return pszDst;
}
/**********************************************************************************************************************/



/**********************************************************************************************************************/
/* ASCII string */
UAPI UINT UAStrLen(PCSTR pszStr)
{
#ifdef _WINSTR
	return (UINT) lstrlenA(pszStr);
#else
	return (UINT) strlen(pszStr);
#endif
}

UAPI PSTR UAStrCopy(PSTR pszDst, PCSTR pszSrc)
{
#ifdef _WINSTR
	return lstrcpyA(pszDst, pszSrc);
#else
	return strcpy(pszDst, pszSrc);
#endif
}

UAPI PSTR UAStrCopyN(PSTR pszDst, PCSTR pszSrc, UINT uLen)
{
#ifdef _SHLSTR
	return StrCpyNA(pszDst, pszSrc, uLen);
#else
	pszDst[--uLen] = 0;
	return strncpy(pszDst, pszSrc, uLen);
#endif
}

UAPI PSTR UAStrCat(PSTR pszDst, PCSTR pszSrc)
{
#ifdef _WINSTR
	return lstrcatA(pszDst, pszSrc);
#else
	return strcat(pszDst, pszSrc);
#endif
}

UAPI INT UAStrCmp(PCSTR pszStr1, PCSTR pszStr2)
{
#ifdef _WINSTR
	return lstrcmpA(pszStr1, pszStr2);
#else
	return strcmp(pszStr1, pszStr2);
#endif
}

UAPI INT UAStrCmpI(PCSTR pszStr1, PCSTR pszStr2)
{
#ifdef _WINSTR
	return lstrcmpiA(pszStr1, pszStr2);
#else
	return _stricmp(pszStr1, pszStr2);
#endif
}

UAPI INT UAStrCmpN(PCSTR pszStr1, PCSTR pszStr2, UINT uLen)
{
#ifdef _SHLSTR
	return StrCmpNA(pszStr1, pszStr2, uLen);
#else
	return strncmp(pszStr1, pszStr2, uLen);
#endif
}

UAPI INT UAStrCmpNI(PCSTR pszStr1, PCSTR pszStr2, UINT uLen)
{
#ifdef _SHLSTR
	return StrCmpNIA(pszStr1, pszStr2, uLen);
#else
	return _strnicmp(pszStr1, pszStr2, uLen);
#endif
}

UAPI PSTR UAStrChr(PCSTR pszStr, CHAR cChr)
{
#ifdef _SHLSTR
	return (PSTR) StrChrA(pszStr, cChr);
#else
	return (PSTR) strchr(pszStr, cChr);
#endif
}

UAPI PSTR UAStrRChr(PCSTR pszStr, CHAR cChr)
{
#ifdef _SHLSTR
	return (PSTR) StrRChrA(pszStr, NULL, cChr);
#else
	return (PSTR) strrchr(pszStr, cChr);
#endif
}

UAPI PSTR UAStrStr(PCSTR pszStr1, PCSTR pszStr2)
{
#ifdef _SHLSTR
	return (PSTR) StrStrA(pszStr1, pszStr2);
#else
	return (PSTR) strstr(pszStr1, pszStr2);
#endif
}

UAPI PSTR UAStrStrI(PCSTR pszStr1, PCSTR pszStr2)
{
#ifdef _SHLSTR
	return StrStrIA(pszStr1, pszStr2);
#else
	PSTR p = (PSTR) pszStr1;
	while (*p)
	{
		PSTR s1 = p;
		PSTR s2 = (PSTR) pszStr2;

		while (*s1 && *s2 && UAChrMatch(*s1, *s2))
		{
			s1++;
			s2++;
		}

		if (*s2 == 0)
		{
			return p;
		}

		p++;
	}
	return NULL;
#endif
}

UAPI PSTR UAStrRep(PSTR pszStr, CHAR cFind UDEF('|'), CHAR cRep UDEF(0))
{
	PSTR p = pszStr;
	for (; *p; p++)
	{
		if (*p == cFind)
		{
			*p = cRep;
		}
	}
	return pszStr;
}

UAPI PSTR UAStrTrim(PSTR pszStr, CHAR cTrim UDEF('"'))
{
	if (*pszStr == cTrim)
	{
		PSTR p = pszStr + UAStrLen(pszStr) - 1;
		if (*p == cTrim)
		{
			*p = 0;
		}
		return pszStr + 1;
	}
	return pszStr;
}

UAPI UINT UAStrMatch(PCSTR pszStr1, PCSTR pszStr2)
{
	UINT i = 0;
	while (pszStr1[i] && (pszStr1[i] == pszStr2[i])) i++;
	return i;
}

UAPI UINT UAStrMatchI(PCSTR pszStr1, PCSTR pszStr2)
{
	UINT i = 0;
	while (pszStr1[i] && UWChrMatch(pszStr1[i], pszStr2[i])) i++;
	return i;
}

UAPI PSTR UAStrToUpper(PSTR pszStr)
{
#ifdef _WINSTR
	return CharUpperA(pszStr);
#else
	return _strupr(pszStr);
#endif
}

UAPI PSTR UAStrToLower(PSTR pszStr)
{
#ifdef _WINSTR
	return CharLowerA(pszStr);
#else
	return _strlwr(pszStr);
#endif
}

UAPI INT UAStrToInt(PCSTR pszStr)
{
#ifdef _SHLSTR
	INT i = 0;
	StrToIntExA(pszStr, STIF_SUPPORT_HEX, &i);
	return i;
#else
	return atoi(pszStr);
#endif
}

UAPI PSTR UAStrFromInt(PSTR pszDst, INT iVal)
{
#ifdef _WINSTR
	wsprintfA(pszDst, "%d", iVal);
	return pszDst;
#else
	return _itoa(iVal, pszDst, 10);
#endif
}

#ifdef _WINSTR
#define UAStrPrint wsprintfA
#define UAStrPrintV wvsprintfA
#else
#define UAStrPrint sprintf
#define UAStrPrintV vsprintf
#endif
/**********************************************************************************************************************/



/**********************************************************************************************************************/
/* UNICODE string */
UAPI UINT UWStrLen(PCWSTR pwzStr)
{
#ifdef _WINSTR
	return (UINT) lstrlenW(pwzStr);
#else
	return (UINT) wcslen(pwzStr);
#endif
}

UAPI PWSTR UWStrCopy(PWSTR pwzDst, PCWSTR pwzSrc)
{
#ifdef _WINSTR
	return lstrcpyW(pwzDst, pwzSrc);
#else
	return wcscpy(pwzDst, pwzSrc);
#endif
}

UAPI PWSTR UWStrCopyN(PWSTR pwzDst, PCWSTR pwzSrc, UINT uLen)
{
#ifdef _SHLSTR
	return StrCpyNW(pwzDst, pwzSrc, uLen);
#else
	pwzDst[--uLen] = 0;
	return wcsncpy(pwzDst, pwzSrc, uLen);
#endif
}

UAPI PWSTR UWStrCat(PWSTR pwzDst, PCWSTR pwzSrc)
{
#ifdef _WINSTR
	return lstrcatW(pwzDst, pwzSrc);
#else
	return wcscat(pwzDst, pwzSrc);
#endif
}

UAPI INT UWStrCmp(PCWSTR pwzStr1, PCWSTR pwzStr2)
{
#ifdef _WINSTR
	return lstrcmpW(pwzStr1, pwzStr2);
#else
	return wcscmp(pwzStr1, pwzStr2);
#endif
}

UAPI INT UWStrCmpI(PCWSTR pwzStr1, PCWSTR pwzStr2)
{
#ifdef _WINSTR
	return lstrcmpiW(pwzStr1, pwzStr2);
#else
	return _wcsicmp(pwzStr1, pwzStr2);
#endif
}

UAPI INT UWStrCmpN(PCWSTR pwzStr1, PCWSTR pwzStr2, UINT uLen)
{
#ifdef _SHLSTR
	return StrCmpNW(pwzStr1, pwzStr2, uLen);
#else
	return wcsncmp(pwzStr1, pwzStr2, uLen);
#endif
}

UAPI INT UWStrCmpNI(PCWSTR pwzStr1, PCWSTR pwzStr2, UINT uLen)
{
#ifdef _SHLSTR
	return StrCmpNIW(pwzStr1, pwzStr2, uLen);
#else
	return _wcsnicmp(pwzStr1, pwzStr2, uLen);
#endif
}

UAPI PWSTR UWStrChr(PCWSTR pwzStr, WCHAR wChr)
{
#ifdef _SHLSTR
	return (PWSTR) StrChrW(pwzStr, wChr);
#else
	return (PWSTR) wcschr(pwzStr, wChr);
#endif
}

UAPI PWSTR UWStrRChr(PCWSTR pwzStr, WCHAR wChr)
{
#ifdef _SHLSTR
	return (PWSTR) StrRChrW(pwzStr, NULL, wChr);
#else
	return (PWSTR) wcsrchr(pwzStr, wChr);
#endif
}

UAPI PWSTR UWStrStr(PCWSTR pwzStr1, PCWSTR pwzStr2)
{
#ifdef _SHLSTR
	return (PWSTR) StrStrW(pwzStr1, pwzStr2);
#else
	return (PWSTR) wcsstr(pwzStr1, pwzStr2);
#endif
}

UAPI PWSTR UWStrStrI(PCWSTR pwzStr1, PCWSTR pwzStr2)
{
#ifdef _SHLSTR
	return (PWSTR) StrStrIW(pwzStr1, pwzStr2);
#else
	PWSTR p = (PWSTR) pwzStr1;
	while (*p)
	{
		PWSTR s1 = p;
		PWSTR s2 = (PWSTR) pwzStr2;

		while (*s1 && *s2 && UWChrMatch((TCHAR) *s1, (TCHAR) *s2))
		{
			s1++;
			s2++;
		}

		if (*s2 == 0)
		{
			return p;
		}

		p++;
	}
	return NULL;
#endif
}

UAPI PWSTR UWStrRep(PWSTR pwzStr, WCHAR wFind UDEF('|'), WCHAR wRep UDEF(0))
{
	PWSTR p = pwzStr;
	for (; *p; p++)
	{
		if (*p == wFind)
		{
			*p = wRep;
		}
	}
	return pwzStr;
}

UAPI PWSTR UWStrTrim(PWSTR pwzStr, WCHAR wTrim UDEF('"'))
{
	if (*pwzStr == wTrim)
	{
		PWSTR p = pwzStr + UWStrLen(pwzStr) - 1;
		if (*p == wTrim)
		{
			*p = 0;
		}
		return pwzStr + 1;
	}
	return pwzStr;
}

UAPI UINT UWStrMatch(PCWSTR pwzStr1, PCWSTR pwzStr2)
{
	UINT i = 0;
	while (pwzStr1[i] && (pwzStr1[i] == pwzStr2[i])) i++;
	return i;
}

UAPI UINT UWStrMatchI(PCWSTR pwzStr1, PCWSTR pwzStr2)
{
	UINT i = 0;
	while (pwzStr1[i] && UWChrMatch(pwzStr1[i], pwzStr2[i])) i++;
	return i;
}

UAPI PWSTR UWStrToUpper(PWSTR pwzStr)
{
#ifdef _WINSTR
	return CharUpperW(pwzStr);
#else
	return _wcsupr(pwzStr);
#endif
}

UAPI PWSTR UWStrToLower(PWSTR pwzStr)
{
#ifdef _WINSTR
	return CharLowerW(pwzStr);
#else
	return _wcslwr(pwzStr);
#endif
}

UAPI INT UWStrToInt(PCWSTR pwzStr)
{
#ifdef _SHLSTR
	INT i = 0;
	StrToIntExW(pwzStr, STIF_SUPPORT_HEX, &i);
	return i;
#else
	return _wtoi(pwzStr);
#endif
}

UAPI PWSTR UWStrFromInt(PWSTR pwzDst, INT iVal)
{
#ifdef _WINSTR
	wsprintfW(pwzDst, L"%d", iVal);
	return pwzDst;
#else
	return _itow(iVal, pwzDst, 10);
#endif
}

#ifdef _WINSTR
#define UWStrPrint wsprintfW
#define UWStrPrintV wvsprintfW
#else
#define UWStrPrint swprintf
#define UWStrPrintV vswprintf
#endif
/**********************************************************************************************************************/



/**********************************************************************************************************************/
/* Transformable string */
#ifdef _UNICODE

#define UChrIsNum					UWChrIsNum
#define UChrIsAlpha					UWChrIsAlpha
#define UChrIsPrintable				UWChrIsPrintable
#define UChrMatch					UWChrMatch
#define UChrToLower					UWChrToLower
#define UChrToUpper					UWChrToUpper

#define UStrLen						UWStrLen
#define UStrCopy					UWStrCopy
#define UStrCopyN					UWStrCopyN
#define UStrCat						UWStrCat
#define UStrCmp						UWStrCmp
#define UStrCmpI					UWStrCmpI
#define UStrCmpN					UWStrCmpN
#define UStrCmpNI					UWStrCmpNI

#define UStrChr						UWStrChr
#define UStrRChr					UWStrRChr
#define UStrStr						UWStrStr
#define UStrStrI					UWStrStrI

#define UStrRep						UWStrRep
#define UStrTrim					UWStrTrim
#define UStrMatch					UWStrMatch
#define UStrMatchI					UWStrMatchI
#define UStrToUpper					UWStrToUpper
#define UStrToLower					UWStrToLower
#define UStrToInt					UWStrToInt
#define UStrFromInt					UWStrFromInt
#define UStrPrint					UWStrPrint
#define UStrPrintV					UWStrPrintV

#define UStrToAStr(s, t, n)			UWStrToAStr(s, t, n)
#define UStrToWStr(w, t, n)			UWStrCopyN(w, t, n)
#define UAStrToStr(t, s, n)			UAStrToWStr(t, s, n)
#define UWStrToStr(t, w, n)			UWStrCopyN(t, w, n)

#else

#define UChrIsNum					UAChrIsNum
#define UChrIsAlpha					UAChrIsAlpha
#define UChrIsPrintable				UAChrIsPrintable
#define UChrMatch					UAChrMatch
#define UChrToLower					UAChrToLower
#define UChrToUpper					UAChrToUpper

#define UStrLen						UAStrLen
#define UStrCopy					UAStrCopy
#define UStrCopyN					UAStrCopyN
#define UStrCat						UAStrCat
#define UStrCmp						UAStrCmp
#define UStrCmpI					UAStrCmpI
#define UStrCmpN					UAStrCmpN
#define UStrCmpNI					UAStrCmpNI

#define UStrChr						UAStrChr
#define UStrRChr					UAStrRChr
#define UStrStr						UAStrStr
#define UStrStrI					UAStrStrI

#define UStrRep						UAStrRep
#define UStrTrim					UAStrTrim
#define UStrMatch					UAStrMatch
#define UStrMatchI					UAStrMatchI
#define UStrToUpper					UAStrToUpper
#define UStrToLower					UAStrToLower
#define UStrToInt					UAStrToInt
#define UStrFromInt					UAStrFromInt
#define UStrPrint					UAStrPrint
#define UStrPrintV					UAStrPrintV

#define UStrToAStr(s, t, n)			UAStrCopyN(s, t, n)
#define UStrToWStr(w, t, n)			UAStrToWStr(w, t, n)
#define UAStrToStr(t, s, n)			UAStrCopyN(t, s, n)
#define UWStrToStr(t, w, n)			UWStrToAStr(t, w, n)

#endif
/**********************************************************************************************************************/



/**********************************************************************************************************************/
/* File */
UAPI HANDLE UFileOpen(PCTSTR ptzPath, DWORD dwAccess UDEF(UFILE_READWRITE))
{
	DWORD dwCreate = (dwAccess == UFILE_WRITE) ? CREATE_ALWAYS :
		((dwAccess == UFILE_READWRITE) ? OPEN_ALWAYS : OPEN_EXISTING);
	HANDLE hFile = CreateFile(ptzPath, dwAccess, FILE_SHARE_READ, NULL, dwCreate, 0, NULL);
	return (hFile == INVALID_HANDLE_VALUE) ? NULL : hFile;
}

UAPI BOOL UFileClose(HANDLE hFile)
{
	return CloseHandle(hFile);
}

UAPI UINT UFileRead(HANDLE hFile, PVOID pvData, UINT uSize)
{
	DWORD dwRead = 0;
	ReadFile(hFile, pvData, uSize, &dwRead, NULL);
	return dwRead;
}

UAPI UINT UFileWrite(HANDLE hFile, PCVOID pvData, UINT uSize)
{
	DWORD dwWrite = 0;
	WriteFile(hFile, pvData, uSize, &dwWrite, NULL);
	return dwWrite;
}

UAPI UINT UFileSeek(HANDLE hFile, INT iOffset, DWORD dwOrigin UDEF(UFILE_BEGIN))
{
	return SetFilePointer(hFile, iOffset, NULL, dwOrigin);
}

UAPI UINT UFileTell(HANDLE hFile)
{
	return UFileSeek(hFile, 0, UFILE_CURRENT);
}

UAPI UINT UFileGetSize(HANDLE hFile)
{
	DWORD dwSize = GetFileSize(hFile, NULL);
	return (dwSize == INVALID_FILE_SIZE) ? 0 : dwSize;
}

UAPI BOOL UFileSetSize(HANDLE hFile, UINT uSize UDEF(0))
{
	UFileSeek(hFile, uSize, UFILE_BEGIN);
	return SetEndOfFile(hFile);
}

UAPI PVOID UFileLoad(PCTSTR ptzPath, PVOID pvData UDEF(NULL), PUINT puSize UDEF(NULL))
{
	UINT uSize;
	HANDLE hFile = UFileOpen(ptzPath, UFILE_READ);
	if (!hFile)
	{
		return NULL;
	}

	uSize = UFileGetSize(hFile);
	if (puSize)
	{
		if (*puSize > uSize)
		{
			*puSize = uSize;
		}
	}
	else
	{
		puSize = &uSize;
	}

	if (!pvData)
	{
		pvData = (PBYTE) UMemAlloc(uSize + 16);
	}

	if (pvData)
	{
		*puSize = UFileRead(hFile, pvData, *puSize);
		((PBYTE) pvData)[uSize] = 0;
		((PBYTE) pvData)[uSize + 1] = 0;
	}

	UFileClose(hFile);
	return pvData;
}

UAPI UINT UFileSave(PCTSTR ptzPath, PCVOID pvData, UINT uSize, BOOL bAppend UDEF(FALSE))
{
	UINT uRet;
	HANDLE hFile = UFileOpen(ptzPath, bAppend ? UFILE_READWRITE : UFILE_WRITE);
	if (!hFile)
	{
		return 0;
	}

	if (bAppend)
	{
		UFileSeek(hFile, 0, UFILE_END);
	}
	uRet = UFileWrite(hFile, pvData, uSize);
	UFileClose(hFile);
	return uRet;
}
/**********************************************************************************************************************/



/**********************************************************************************************************************/
/* File management */
UAPI PTSTR UPathMake(PTSTR ptzDir, PCTSTR ptzSub)
{
	PTSTR p = UStrChr(ptzDir, 0);
	if (p[-1] != '\\')
	{
		*p++ = '\\';
	}
	UStrCopy(p, ptzSub);
	return ptzDir;
}

UAPI PTSTR UPathSplit(PTSTR ptzPath)
{
	PTSTR p = UStrRChr(ptzPath, '\\');
	if (p)
	{
		*p = 0;
		return p + 1;
	}
	return ptzPath;
}

UAPI PTSTR UPathSplitExt(PTSTR ptzPath)
{
	PTSTR p = UStrRChr(ptzPath, '.');
	if (p)
	{
		*p = 0;
		return p + 1;
	}
	return ptzPath;
}

UAPI BOOL UFileDelete(PCTSTR ptzPath)
{
	return DeleteFile(ptzPath);
}

UAPI BOOL UFileCopy(PCTSTR ptzPath, PCTSTR ptzNewPath)
{
	return CopyFile(ptzPath, ptzNewPath, FALSE);
}

UAPI BOOL UFileMove(PCTSTR ptzPath, PCTSTR ptzNewPath)
{
	return MoveFile(ptzPath, ptzNewPath);
}

UAPI BOOL UFileExist(PCTSTR ptzPath)
{
	WIN32_FILE_ATTRIBUTE_DATA a;
	return GetFileAttributesEx(ptzPath, GetFileExInfoStandard, &a) && !(a.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

UAPI BOOL UDirExist(PCTSTR ptzDir)
{
	WIN32_FILE_ATTRIBUTE_DATA a;
	return GetFileAttributesEx(ptzDir, GetFileExInfoStandard, &a) && (a.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

UAPI BOOL UDirCreate(PTSTR ptzDir)
{
	PTSTR p;
	for (p = ptzDir; p = UStrChr(p, '\\'); *p++ = '\\')
	{
		*p = 0;
		if (!UDirExist(ptzDir))
		{
			CreateDirectory(ptzDir, NULL);
		}
	}
	return TRUE;
}

UAPI BOOL UDirDelete(PCTSTR ptzDir)
{
	return RemoveDirectory(ptzDir);
}

UAPI PTSTR UDirGetAppPath(PTSTR ptzPath)
{
#ifdef WINCE
	extern HINSTANCE g_hInst;
	GetModuleFileName(g_hInst, ptzPath, MAX_PATH);
#else
	MEMORY_BASIC_INFORMATION m = {0};
	VirtualQuery(UDirGetAppPath, &m, sizeof(MEMORY_BASIC_INFORMATION));
	GetModuleFileName((HMODULE) m.AllocationBase, ptzPath, MAX_PATH);
#endif
	return ptzPath;
}

UAPI PTSTR UDirGetAppFile(PTSTR ptzPath, PCTSTR ptzFile)
{
	PTSTR p;
	UDirGetAppPath(ptzPath);
	p = UStrRChr(ptzPath, '\\');
	if (p == NULL)
	{
		p = UStrChr(ptzPath, 0);
	}
	return UStrCopy(p + 1, ptzFile);
}

UAPI PTSTR UDirGetAppExt(PTSTR ptzPath, PCTSTR ptzExt)
{
	PTSTR p;
	UDirGetAppPath(ptzPath);
	p = UStrRChr(ptzPath, '.');
	if (p == NULL)
	{
		p = UStrChr(ptzPath, 0);
	}
	return UStrCopy(p + 1, ptzExt);
}

UAPI PTSTR UDirGetCurrent(PTSTR ptzDir)
{
#ifdef GetCurrentDirectory
	GetCurrentDirectory(MAX_PATH, ptzDir);
	return ptzDir;
#else
	UDirGetAppPath(ptzDir);
	return UPathSplit(ptzDir);
#endif
}

UAPI PTSTR UDirGetTemp(PTSTR ptzDir)
{
	GetTempPath(MAX_PATH, ptzDir);
	return ptzDir;
}

UAPI PTSTR UFileGetTemp(PTSTR ptzPath)
{
	UDirGetTemp(ptzPath);
	GetTempFileName(ptzPath, TEXT("UNI"), 0, ptzPath);
	return ptzPath;
}
/**********************************************************************************************************************/



/**********************************************************************************************************************/
/* Thread */
UAPI HANDLE UThreadCreate(UPROC upProc, PVOID pvParam)
{
	DWORD dwThread;
	return CreateThread(NULL, 0, upProc, pvParam, 0, &dwThread);
}

UAPI BOOL UThreadClose(HANDLE hThread)
{
	return CloseHandle(hThread);
}

UAPI BOOL UThreadTerminate(HANDLE hThread)
{
	return TerminateThread(hThread, 0);
}

UAPI BOOL UThreadSuspend(HANDLE hThread)
{
	return SuspendThread(hThread);
}

UAPI BOOL UThreadResume(HANDLE hThread)
{
	return ResumeThread(hThread);
}
/**********************************************************************************************************************/



/**********************************************************************************************************************/
/* Misc */
UAPI UINT UGetRandom()
{
	return rand();
}

UAPI VOID UGetTime(PUTIME pTime)
{
	GetLocalTime(pTime);
}

UAPI UINT UGetTimeStamp()
{
	return GetTickCount();
}

UAPI VOID UTrack(PCTSTR ptzFormat, ...)
{
	DWORD i;
	va_list va;
	TCHAR tz[MAX_STR];

	va_start(va, ptzFormat);
	i = UStrPrintV(tz, ptzFormat, va);
	va_end(va);

#if defined(_LOGFILE)
	UFileSave(TEXT("UniDebug.txt"), tz, i * sizeof(TCHAR), TRUE);
#elif defined(_LOGMSG)
	MessageBox(NULL, tz, TEXT("UniDebug"), MB_ICONINFORMATION | MB_TOPMOST);
#elif defined(_LOGCON)
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), tz, i, &i, NULL);
#else
	OutputDebugString(tz);
#endif
}

UAPI VOID UAssert(PCTSTR ptzExp, PCTSTR ptzFile, UINT uLine)
{
	UINT i;
	TCHAR tzText[MAX_STR];
	TCHAR tzModule[MAX_PATH];

	UStrPrint(tzText,
		TEXT("Assertion failed!\n\n")
		TEXT("Program: %s\n")
		TEXT("File: %s\n")
		TEXT("Line: %d\n\n")
		TEXT("Expression: %s\n\n")
		TEXT("Press Retry to debug the application - JIT must be enabled"),
		UDirGetAppPath(tzModule), ptzFile, uLine, ptzExp);

	i = MessageBox(NULL, tzText, TEXT("UniDebug"), MB_ICONERROR | MB_ABORTRETRYIGNORE);
	if (i == IDABORT)
	{
		ExitProcess(0);
	}
	else if (i == IDRETRY)
	{
		DebugBreak();
	}
}
/**********************************************************************************************************************/



/**********************************************************************************************************************/
/* Debug */
#ifdef __FUNCTION__
#define __FUNCFILE__				TEXT(__FUNCTION__)
#else
#define __FUNCFILE__				TEXT(__FILE__)
#endif

#ifdef __cplusplus
class CAutoTrack
{
private:
	UINT m_uLine;
	PCTSTR m_ptzName;
public:
	CAutoTrack(PCTSTR ptzName, UINT uLine): m_uLine(uLine), m_ptzName(ptzName)
	{
		UTrack(TEXT("Enter %s:%u\r\n"), ptzName, uLine);
	}
	~CAutoTrack()
	{
		UTrack(TEXT("Leave %s:%u\r\n"), m_ptzName, m_uLine);
	}
};
#define __AutoTrack()				CAutoTrack at(__FUNCFILE__, __LINE__)
#else
#define __AutoTrack()				UTrack(TEXT("Track %s:%u\r\n"), __FUNCFILE__, __LINE__)
#endif

#ifdef _DEBUG
#define _Track						UTrack
#define _AutoTrack()				__AutoTrack()
#define _LineTrack()				UTrack(TEXT("Track %s:%u\r\n"), __FUNCFILE__, __LINE__)
#else
#define _Track
#define _AutoTrack()
#define _LineTrack()
#endif

#ifdef _DEBUG
#define _Assert(e)					(VOID) ((e) || (UAssert(TEXT(#e), TEXT(__FILE__), __LINE__), 0))
#else
#define _Assert(e)					((VOID) 0)
#endif
/**********************************************************************************************************************/

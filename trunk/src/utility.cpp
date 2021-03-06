#include <Windows.h>
#include <tchar.h>
#include "Shlwapi.h"

#include "PluginInterface.h"
#include "IniFileProcessor.h"
#include "utility.h"

tstring GetConfigFilePath(HWND nppHandle)
{
	tstring strConfigFilePath;

	TCHAR szConfigDir[MAX_PATH];
	TCHAR szBuffer[MAX_PATH];
	szConfigDir[0] = 0;
	::SendMessage(nppHandle, NPPM_GETPLUGINSCONFIGDIR, 
		MAX_PATH, (LPARAM)szConfigDir);
	::PathCombine(szBuffer, szConfigDir, TEXT("JSMinNpp.ini"));
	strConfigFilePath = szBuffer;

	return strConfigFilePath;
}

void LoadOption(HWND nppHandle, StruOptions& struOptions)
{
	LoadDefaultOption(struOptions);
	tstring tsConfigFilePath = GetConfigFilePath(nppHandle);

	IniFileProcessor processor(tsConfigFilePath);
	IniFileProcessor::IniMap map;

	map = processor.GetInfo(true);
	IniFileProcessor::IniMap::iterator itrEnd = map.end();

	if(map.find(keyPutCR) != itrEnd)
	{
		if(!map[keyPutCR].GetStrValue().compare("0"))
			struOptions.bPutCR = false;
	}

	if(map.find(keyChIndent) != itrEnd)
	{
		string strIndent = map[keyChIndent].GetStrValue();
		if(!strIndent.compare("tab"))
			struOptions.chIndent = '\t';
		else if(!strIndent.compare("space"))
			struOptions.chIndent = ' ';
	}

	if(map.find(keyChPerInd) != itrEnd)
	{
		struOptions.nChPerInd = atoi(map[keyChPerInd].GetStrValue().c_str());
	}

	if(map.find(keyNLBracket) != itrEnd)
	{
		if(!map[keyNLBracket].GetStrValue().compare("1"))
			struOptions.bNLBracket = true;
	}

	if(map.find(keyKeepTopComt) != itrEnd)
	{
		if(!map[keyKeepTopComt].GetStrValue().compare("1"))
			struOptions.bKeepTopComt = true;
	}

	if(map.find(keyIndentInEmpty) != itrEnd)
	{
		if(!map[keyIndentInEmpty].GetStrValue().compare("1"))
			struOptions.bIndentInEmpty = true;
	}
}

void LoadDefaultOption(StruOptions& struOptions)
{
	struOptions.bPutCR = true;
	struOptions.chIndent = '\t';
	struOptions.nChPerInd = 1;
	struOptions.bNLBracket = false;
	struOptions.bKeepTopComt = false;
	struOptions.bIndentInEmpty = false;
}

void SaveOption(HWND nppHandle, StruOptions struOptions)
{
	tstring tsConfigFilePath = GetConfigFilePath(nppHandle);

	IniFileProcessor processor(tsConfigFilePath);
	IniFileProcessor::IniMap map;
	
	map[keyPutCR] = IniValue(struOptions.bPutCR ? string("1") : string("0"));
	map[keyChIndent] = IniValue(struOptions.chIndent == '\t' ? string("tab") : string("space"));
	char buffer[256];
	itoa(struOptions.nChPerInd, buffer, 10);
	map[keyChPerInd] = IniValue(string(buffer));
	map[keyNLBracket] = IniValue(struOptions.bNLBracket ? string("1") : string("0"));
	map[keyKeepTopComt] = IniValue(struOptions.bKeepTopComt ? string("1") : string("0"));
	map[keyIndentInEmpty] = IniValue(struOptions.bIndentInEmpty ? string("1") : string("0"));

	processor.SetMap(map);

	processor.Save();
}

void CopyText(LPCTSTR lpcText)
{
	HGLOBAL hMoveable;
	LPTSTR pszArr;

	size_t textLen = _tcslen(lpcText);

	size_t bytes = (textLen + 1)*sizeof(TCHAR);
	hMoveable = GlobalAlloc(GMEM_MOVEABLE, bytes);
	pszArr = (LPTSTR)GlobalLock(hMoveable);
	ZeroMemory(pszArr, bytes);
	_tcscpy_s(pszArr, textLen + 1, lpcText);
	GlobalUnlock(hMoveable);

	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, hMoveable);
	CloseClipboard();
}

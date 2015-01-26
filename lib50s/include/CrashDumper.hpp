#ifndef __CRASHDUMPER_HPP__
#define __CRASHDUMPER_HPP__

/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Jon Schiavo
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Creates a minidump file after encountering an unhandled exception.
 * This dump can be used for debugging offline.
 */

#include <DbgHelp.h>

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
									CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
									CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
									CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
									);

//---------------
// Crash dump writer
//---------------
long WINAPI dump_core( EXCEPTION_POINTERS *exceptionInfo )
{
	long retval = EXCEPTION_CONTINUE_SEARCH;
	HWND hParent = NULL;

	//Attempt to load the dbghelp DLL
	HMODULE hDll = NULL;
	hDll = ::LoadLibrary("DBGHELP.DLL");

	if(!hDll)
	{
		::MessageBoxA(NULL, "Could not load DBGHELP.DLL.  No core dump will be produced!", "Unhandled Exception!", MB_OK);
	}
	else
	{
		MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress(hDll, "MiniDumpWriteDump");

		if(::MessageBoxA(NULL, "SOMETHING HORRIBLE HAS HAPPENED!  Generate core dump for debugging?", "Unhandled Exception!", MB_YESNO) == IDYES)
		{
			HANDLE hFile = ::CreateFile( "core.dmp", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

			if(hFile != INVALID_HANDLE_VALUE)
			{
				_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

				ExInfo.ThreadId = ::GetCurrentThreadId();
				ExInfo.ExceptionPointers = exceptionInfo;
				ExInfo.ClientPointers = NULL;

				bool bOK = pDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL);
				if(bOK)
				{
					::MessageBoxA(NULL, "Core file is available at ./core.dmp", "Success", MB_OK);
					retval = EXCEPTION_EXECUTE_HANDLER;
				}
				else
				{
					::MessageBoxA(NULL, "Core file could not be generated", "Failure", MB_OK);
				}
			}
			else
			{
				::MessageBoxA(NULL, "Failed to create core file", "Error", MB_OK);
			}
		}
	}

	return retval;
}

#endif
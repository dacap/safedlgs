/* safedlgs - Copyright (c) 2024 David Capello

   This file is released under the terms of the MIT license.
   Read LICENSE.txt for more information.
*/

#include <windows.h>
#include <shlwapi.h>
#include <strsafe.h>

#define kBufSize 4096

const char* kDlgsName = "dlgs";

WCHAR last_path[kBufSize];

DWORD WINAPI read_child_data_thread(HANDLE childRead)
{
  WCHAR buf[kBufSize];
  BOOL result;
  DWORD i, j, read;
  ZeroMemory(buf, sizeof(buf));

  last_path[j=0] = 0;
  while (TRUE) {
    buf[0] = 0;
    read = 0;
    result = ReadFile(childRead, buf, sizeof(buf), &read, NULL);
    if (!result || read == 0)
      break;

    read /= 2; /* As "buf" is WCHAR* */

    for (i=0; i<read && j+1<kBufSize; ++i) {
      if (buf[i] == '\r')
        continue;
      if (buf[i] == '\n') {
        j = 0;
        continue;
      }
      last_path[j++] = buf[i];
      if (j < kBufSize)
        last_path[j] = 0;
    }
  }
  return 0;
}

int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
  BOOL again = TRUE;

  /* Get the command line to run dlgs.exe, replacing test.exe with dlgs.exe */
  WCHAR cmdLine[2*kBufSize];
  LPCWSTR cmdLine0 = GetCommandLineW();
  LPWSTR exeName = StrStrW(cmdLine0, L"test.exe");
  if (exeName) {
    int i;
    /* This works only because sizeof("test") == sizeof("dlgs") */
    for (i=0; i<4; ++i)
      exeName[i] = kDlgsName[i];
  }
  StringCbCopyW(cmdLine, sizeof(cmdLine), cmdLine0);

  /* Execute the dlgs.exe process */
  while (again) {
    DWORD waitResult = 0;
    DWORD exitCode = 0;
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES sa;
    HANDLE childRead = NULL;
    HANDLE childWrite = NULL;
    HANDLE readThread = NULL;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    /* Concatenate the last known location to the command line */
    StringCbCopyW(cmdLine, sizeof(cmdLine), cmdLine0);
    if (last_path[0]) {
      StringCbCatW(cmdLine, sizeof(cmdLine), L" -folder \"");
      StringCbCatW(cmdLine, sizeof(cmdLine), last_path);
      StringCbCatW(cmdLine, sizeof(cmdLine), L"\"");
    }

    again = FALSE;

    /* Create a pipe for the STDOUT of the child process, so we can
       receive its output */
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    if (!CreatePipe(&childRead, &childWrite, &sa, 0)) {
      MessageBoxW(NULL, L"Cannot create a pipe for IPC with dlgs.exe", L"Error", MB_OK | MB_ICONERROR);
      return 1;
    }

    si.cb = sizeof(si);
    si.hStdError = childWrite;
    si.hStdOutput = childWrite;
    si.hStdInput = INVALID_HANDLE_VALUE;
    si.dwFlags = STARTF_USESTDHANDLES;
    if (!CreateProcessW(
          NULL, (LPWSTR)cmdLine, NULL, NULL,
          TRUE,                                /* bInheritHandles */
          CREATE_NO_WINDOW | DETACHED_PROCESS, /* dwCreationFlags */
          NULL, NULL,
          &si, &pi)) {
      MessageBoxW(NULL, L"Cannot execute dlgs.exe process", L"Error", MB_OK | MB_ICONERROR);
      return 1;
    }

    /* Close handles that will not be used in this parent process */
    CloseHandle(pi.hThread);
    CloseHandle(childWrite);

    /* Create thread to read dlgs.exe output */
    readThread = CreateThread(NULL, 0, read_child_data_thread,
                              (LPVOID)childRead, 0, NULL);

    /* Wait the dlgs.exe process to finish (or crash) */
    waitResult = WaitForSingleObject(pi.hProcess, INFINITE);

    GetExitCodeProcess(pi.hProcess, &exitCode);
    CloseHandle(pi.hProcess);

    /* Did the process crash? */
    if (exitCode == 0xC0000005) {
      /* Run child process again */
      again = TRUE;
      goto done;
    }

    if (exitCode == 1) {
      MessageBoxW(NULL, L"Dialog Canceled", L"safedlgs", MB_OK | MB_ICONINFORMATION);
    }
    else {
      WCHAR buf[2*kBufSize];
      StringCbCopyW(buf, sizeof(buf), L"File Selected = ");
      StringCbCatW(buf, sizeof(buf), last_path);
      MessageBoxW(NULL, buf, L"safedlgs", MB_OK | MB_ICONINFORMATION);
    }

  done:;
    CloseHandle(childRead);
    if (readThread)
      CloseHandle(readThread);
  }
  return 0;
}

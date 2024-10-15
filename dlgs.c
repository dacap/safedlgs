/* safedlgs - Copyright (c) 2024 David Capello

   This file is released under the terms of the MIT license.
   Read LICENSE.txt for more information.
*/

#define COBJMACROS /* We want the C-like COM interface using macros */

#include <windows.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <werapi.h>
#include <strsafe.h>

const kCrashProcessButtonID = 1001;

typedef struct FDE {
  IFileDialogEventsVtbl* vtbl1;
  IFileDialogControlEventsVtbl* vtbl2;
  ULONG ref;
} FDE;

HANDLE hStdout;

void print(LPCWSTR s)
{
  size_t size = 4096;
  HRESULT hr = StringCbLengthW(s, size, &size); /* Size in bytes */
  if (FAILED(hr))
    return;
  WriteFile(hStdout, s, size, NULL, NULL);
  WriteFile(hStdout, L"\r\n", 4, NULL, NULL);
  FlushFileBuffers(hStdout);
}

HRESULT STDMETHODCALLTYPE FDE_QueryInterface(FDE* this,
                                             REFIID riid,
                                             void** ppvObject)
{
  if (IsEqualIID(riid, &IID_IUnknown) ||
      IsEqualIID(riid, &IID_IFileDialogEvents)) {
    *ppvObject = (void*)this->vtbl1;
    ++this->ref;
    return S_OK;
  }
  else if (IsEqualIID(riid, &IID_IFileDialogControlEvents)) {
    *ppvObject = (void*)this->vtbl2;
    ++this->ref;
    return S_OK;
  }
  return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE FDE_AddRef(FDE* this)
{
  ++this->ref;
  return S_OK;
}

ULONG STDMETHODCALLTYPE FDE_Release(FDE* this)
{
  --this->ref;
  if (this->ref == 0) {
    /* Do nothing, as we use an instance in the stack */
  }
  return this->ref;
}

HRESULT STDMETHODCALLTYPE FDE_Noop(FDE* this)
{
  return S_OK;
}

/* Listen the changes in the IFileDialog to print the latest selected
   location in the STDOUT. This is used by the parent process to go
   back to the last selected location if the process crashes. */
HRESULT STDMETHODCALLTYPE FDE_OnFolderChange(FDE* this,
                                             IFileDialog* dlg)
{
  IShellItem* item = NULL;
  HRESULT hr = IFileDialog_GetFolder(dlg, &item);
  if (FAILED(hr))
    return hr;

  LPWSTR name = NULL;
  hr = IShellItem_GetDisplayName(item, SIGDN_DESKTOPABSOLUTEPARSING, &name);
  if (FAILED(hr))
    return hr;

  /* Print folder name in STDOUT */
  print(name);
  CoTaskMemFree(name);
  IShellItem_Release(item);

  return S_OK;
}

HRESULT STDMETHODCALLTYPE FDCE_OnButtonClicked(FDE* this,
                                               IFileDialogCustomize* pfdc,
                                               DWORD dwIDCtl)
{
  if (dwIDCtl == kCrashProcessButtonID) {
    *((int*)NULL) = 0;
  }
  return S_OK;
}

static IFileDialogEventsVtbl FDE_Vtbl = {
  (void*)FDE_QueryInterface,
  (void*)FDE_AddRef,
  (void*)FDE_Release,
  (void*)FDE_Noop,              /* OnFileOk */
  (void*)FDE_Noop,              /* OnFolderChanging */
  (void*)FDE_OnFolderChange,
  (void*)FDE_Noop,              /* OnSelectionChange */
  (void*)FDE_Noop,              /* OnShareViolation */
  (void*)FDE_Noop,              /* OnTypeChange */
  (void*)FDE_Noop,              /* OnOverwrite */
};

static IFileDialogControlEventsVtbl FDCE_Vtbl = {
  (void*)FDE_QueryInterface,
  (void*)FDE_AddRef,
  (void*)FDE_Release,
  (void*)FDE_Noop,              /* OnItemSelected */
  (void*)FDCE_OnButtonClicked,
  (void*)FDE_Noop,              /* OnCheckButtonToggled */
  (void*)FDE_Noop,              /* OnControlActivating */
};

int wmain(int argc, wchar_t** argv)
{
  IFileDialog* dlg = NULL;
  DWORD dwCookie = 0;
  HRESULT hr;
  FILEOPENDIALOGOPTIONS options;
  BOOL save = FALSE;
  wchar_t* title = L"Select File";
  wchar_t* folder = NULL;
  FDE fde = { &FDE_Vtbl, &FDCE_Vtbl, 1 };
  IFileDialogCustomize* customize = NULL;
  int i;

  hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hStdout == INVALID_HANDLE_VALUE)
    return 1;

  CoInitialize(NULL);

  /* Avoid showing the "dlgs.exe has stopped working" dialog when this crashes */
  SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  WerSetFlags(WER_FAULT_REPORTING_NO_UI);

  /* Process command line arguments */
  for (i=1; i<argc; ++i) {
    if (StrCmpW(argv[i], L"-save") == 0) {
      save = TRUE;
    }
    else if (StrCmpW(argv[i], L"-title") == 0 && i+1 < argc) {
      title = argv[++i];
    }
    else if (StrCmpW(argv[i], L"-folder") == 0 && i+1 < argc) {
      folder = argv[++i];
    }
  }

  options =
    FOS_NOCHANGEDIR |
    FOS_PATHMUSTEXIST |
    FOS_FORCEFILESYSTEM;
  if (save)
    options |= FOS_OVERWRITEPROMPT;
  else
    options |= FOS_FILEMUSTEXIST;

  hr = CoCreateInstance(
    (save ? &CLSID_FileSaveDialog:
            &CLSID_FileOpenDialog),
    NULL, CLSCTX_INPROC_SERVER, &IID_IFileDialog, &dlg);
  if (FAILED(hr))
    goto exit;

  hr = IFileDialog_SetOptions(dlg, options);
  if (FAILED(hr))
    goto exit;

  hr = IFileDialog_SetTitle(dlg, title);
  if (FAILED(hr))
    goto exit;

  if (folder) {
    IShellItem* item = NULL;
    SHCreateItemFromParsingName(folder, NULL, &IID_IShellItem, &item);
    if (item) {
      IFileDialog_SetFolder(dlg, item);
      IShellItem_Release(item);
    }
  }

  hr = IFileDialog_Advise(dlg, (IFileDialogEvents*)&fde, &dwCookie);
  if (FAILED(hr))
    goto exit;

  /* We use IFileDialogCustomize to add a button to simulate a crash */
  hr = IFileDialog_QueryInterface(dlg, &IID_IFileDialogCustomize, &customize);
  if (FAILED(hr))
    goto exit;

  hr = IFileDialogCustomize_AddPushButton(
    customize, kCrashProcessButtonID, L"Crash Process");

  hr = IFileDialog_Show(dlg, NULL);
  if (FAILED(hr)) {
    /* The Cancel button was pressed */
    hr = 1;
    goto exit;
  }

  /* Print the selected file */
  {
    IShellItem* item;
    LPWSTR fn;

    hr = IFileDialog_GetResult(dlg, &item);
    if (FAILED(hr))
      goto exit;

    hr = IShellItem_GetDisplayName(item, SIGDN_DESKTOPABSOLUTEPARSING, &fn);
    if (FAILED(hr))
      return hr;

    print(fn);
    hr = 0;
  }

exit:;
  CoUninitialize();

  if (dlg) {
    if (customize)
      IFileDialogCustomize_Release(customize);
    if (dwCookie)
      IFileDialog_Unadvise(dlg, dwCookie);
    IFileDialog_Release(dlg);
  }

  return hr;
}

# Crashes on IFileDialog::Show()

All these crashes happen when we call
[IFileDialog::Show()](https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-imodalwindow-show)
method of a [IFileDialog](https://learn.microsoft.com/en-us/windows/win32/shell/common-file-dialog).

The first one looks like a native *100% Microsoft* crash. Other ones
are third-party IMEs.

## comctl32.dll

A buggy `comctl32.dll` was found on Windows 10.0.17763, 10.0.18363,
10.0.19045, 10.0.22631, and 10.0.22631 so far. Not sure if Microsoft
fixed this on Windows 10.0.26100. It looks like this crash happens
when we want to copy some text from the file name field:

```
Fatal Error: EXCEPTION_ACCESS_VIOLATION_READ / 0x0

Thread Crashed:
0   msvcrt.dll                      memcpy
1   comctl32.DLL                    Edit_Copy
2   comctl32.DLL                    Edit_WndProc
3   USER32.dll                      UserCallWinProcCheckWow
4   USER32.dll                      CallWindowProcW
5   comctl32.DLL                    CallNextSubclassProc
6   comctl32.DLL                    DefSubclassProc
7   SHELL32.dll                     DefSubclassProc
8   SHELL32.dll                     CAutoComplete::_EditWndProc
9   comctl32.DLL                    CallNextSubclassProc
10  comctl32.DLL                    MasterSubclassProc
11  USER32.dll                      UserCallWinProcCheckWow
12  USER32.dll                      SendMessageWorker
13  USER32.dll                      SendMessageW
14  comctl32.DLL                    EditSL_Char
15  comctl32.DLL                    EditSL_WndProc
16  comctl32.DLL                    Edit_WndProc
17  USER32.dll                      UserCallWinProcCheckWow
18  USER32.dll                      CallWindowProcW
19  comctl32.DLL                    CallNextSubclassProc
20  comctl32.DLL                    DefSubclassProc
21  SHELL32.dll                     DefSubclassProc
22  SHELL32.dll                     CAutoComplete::_OnChar
23  SHELL32.dll                     CAutoComplete::_EditWndProc
24  comctl32.DLL                    CallNextSubclassProc
25  comctl32.DLL                    MasterSubclassProc
26  USER32.dll                      UserCallWinProcCheckWow
27  USER32.dll                      DispatchMessageWorker
28  USER32.dll                      IsDialogMessageW
29  USER32.dll                      DialogBox2
30  USER32.dll                      InternalDialogBox
31  USER32.dll                      DialogBoxIndirectParamAorW
32  USER32.dll                      DialogBoxIndirectParamW
33  COMDLG32.dll                    <lambda>::operator()
34  COMDLG32.dll                    CFileOpenSave::Show
```

## imjpapi.dll

```
Fatal Error: STATUS_HEAP_CORRUPTION

Thread Crashed:
0   ntdll.dll                       RtlReportFatalFailure
1   ntdll.dll                       RtlReportCriticalFailure
2   ntdll.dll                       RtlpHeapHandleError
3   ntdll.dll                       RtlpHpHeapHandleError
4   ntdll.dll                       RtlpLogHeapFailure
5   ntdll.dll                       RtlpProbeUserBufferSafe
6   ntdll.dll                       RtlGetUserInfoHeap
7   KERNEL32.DLL                    GlobalHandle
8   imjpapi.dll                     FreeUserBuffers
9   imjpapi.dll                     KnlTerm
10  imjpapi.dll                     destructor'
11  imjpapi.dll                     IJConv::Release
12  imjpapi.dll                     DestroyIConvInstance
13  imjpapi.dll                     IImeConvert::releaseObjects
14  imjpapi.dll                     IImeConvert::Release
15  imjpapi.dll                     CIImeIPoint::Release
16  imjpapi.dll                     CImeCommonAPI_JPN_Desktop_V1::~CImeCommonAPI_JPN_Desktop_V1
17  imjpapi.dll                     destructor'
18  imjpapi.dll                     ATL::CComPolyObject<T>::Release
19  imjptip.dll                     destructor'
20  imjptip.dll                     Imeapiutil::CImeShortcutMenuLaunchRequest::Release
21  imjptip.dll                     destructor'
22  imjptip.dll                     destructor'
23  imjptip.dll                     CTipFnCommandUIObjectMgr::Release
24  imetip.dll                      destructor'
25  imetip.dll                      CTipContextEditor::Uninitialize
26  imetip.dll                      CTipContextEditorMgr::UninitContextEditor
27  imetip.dll                      CTipContextEditorMgr::OnIcEvent
28  imetip.dll                      CTipContextEditorMgr::_OnIcEvent
29  imetip.dll                      Tsfutil::CTfThreadMgrEventSink::OnPopContext
30  MSCTF.dll                       CThreadInputMgr::OnContextUninitialized
31  textinputframework.dll          CDocumentInputManager::_Pop
32  textinputframework.dll          CDocumentInputManager::Pop
33  MsftEdit.dll                    CUIM::Uninit
34  MsftEdit.dll                    CTextMsgFilter::TurnOffUIM
35  MsftEdit.dll                    CTextMsgFilter::~CTextMsgFilter
36  MsftEdit.dll                    CTextMsgFilter::Release
37  MsftEdit.dll                    CTxtEdit::~CTxtEdit
38  MsftEdit.dll                    destructor'
39  MsftEdit.dll                    CCallMgrCenter::ExitContext
40  MsftEdit.dll                    CTxtEdit::CUnknown::Release
41  explorerframe.dll               CSearchEditBoxBase::OnDestroy
42  DUI70.dll                       DirectUI::Element::_DisplayNodeCallback
43  DUser.dll                       GPCB::xwInvokeDirect
44  DUser.dll                       DuVisual::xwBeginDestroy
45  DUser.dll                       DuVisual::xwDestroy
46  DUser.dll                       DuVisual::xwBeginDestroy
47  DUser.dll                       DuRootGadget::xwDestroy
48  DUser.dll                       BaseObject::xwUnlock
49  DUser.dll                       DeleteHandle
50  DUI70.dll                       DirectUI::Element::Destroy
51  explorerframe.dll               CSearchBox::_CleanUpForDestroy
52  explorerframe.dll               CSearchBox::SetSite
53  shcore.dll                      IUnknown_SetSite
54  explorerframe.dll               CUniversalSearchBand::_CleanupUniversalSearchBand
55  explorerframe.dll               CUniversalSearchBand::SetSite
56  shcore.dll                      IUnknown_SetSite
57  explorerframe.dll               CBandSite::RemoveBand
58  explorerframe.dll               CNavBar::SetSite
59  shcore.dll                      IUnknown_SetSite
60  COMDLG32.dll                    CFileOpenSave::_OnDestroyDialog
61  COMDLG32.dll                    CFileOpenSave::s_OpenSaveDlgProc
62  USER32.dll                      UserCallDlgProcCheckWow
63  USER32.dll                      DefDlgProcWorker
64  USER32.dll                      DefDlgProcW
65  USER32.dll                      UserCallWinProcCheckWow
66  USER32.dll                      DispatchClientMessage
67  USER32.dll                      _fnDWORD
68  ntdll.dll                       KiUserCallbackDispatch
69  win32u.dll                      NtUserDestroyWindow
70  USER32.dll                      DialogBox2
71  USER32.dll                      InternalDialogBox
72  USER32.dll                      DialogBoxIndirectParamAorW
73  USER32.dll                      DialogBoxIndirectParamW
74  COMDLG32.dll                    <lambda>::operator()
75  COMDLG32.dll                    CFileOpenSave::Show
```

## SogouPY.ime

```
Fatal Error: STATUS_HEAP_CORRUPTION

Thread Crashed:
0   ntdll.dll                       RtlReportFatalFailure
1   ntdll.dll                       RtlReportCriticalFailure
2   ntdll.dll                       RtlpHeapHandleError
3   ntdll.dll                       RtlpHpHeapHandleError
4   ntdll.dll                       RtlpLogHeapFailure
5   ntdll.dll                       RtlGetUserInfoHeap
6   KERNEL32.DLL                    LocalSize
7   SogouPY.ime                     <unknown>
8   SogouPY.ime                     <unknown>
9   CoreUIComponents.dll            Cn::Engine::Heap::Leave
10  ntdll.dll                       LdrpPreprocessDllName
11  ntdll.dll                       LdrpResolveProcedureAddress
12  ntdll.dll                       LdrGetProcedureAddressForCaller
13  KERNELBASE.dll                  GetProcAddressForCaller
14  SogouPY.ime                     <unknown>
15  SogouTSF.ime                    <unknown>
16  MSCTF.dll                       TF_CreateDisplayAttributeMgr
17  SogouTSF.ime                    <unknown>
18  SogouTSF.ime                    <unknown>
19  SogouPY.ime                     <unknown>
20  SogouPY.ime                     <unknown>
21  SogouTSF.ime                    <unknown>
22  MSCTF.dll                       CThreadMgrEventSink::OnSetFocus
23  MSCTF.dll                       CThreadInputMgr::_NotifyCallbacks
24  MSCTF.dll                       CThreadInputMgr::_SetFocus
25  MSCTF.dll                       CThreadInputMgr::OnInputFocusEvent
26  MSCTF.dll                       CThreadInputMgr::OnCiceroEvent
27  MSCTF.dll                       WinEventProc
28  USER32.dll                      _ClientCallWinEventProc
29  ntdll.dll                       KiUserCallbackDispatch
30  win32u.dll                      ZwUserSetFocus
31  comctl32.DLL                    ComboBox_WndProc
32  USER32.dll                      UserCallWinProcCheckWow
33  USER32.dll                      CallWindowProcW
34  DUser.dll                       WndBridge::RawWndProc
35  atlthunk.dll                    AtlThunk_0x07
36  USER32.dll                      UserCallWinProcCheckWow
37  USER32.dll                      DispatchClientMessage
38  USER32.dll                      _fnDWORD
39  ntdll.dll                       KiUserCallbackDispatch
40  win32u.dll                      ZwUserSetFocus
41  SHELL32.dll                     CSaveControlsModuleInner::_CreateControls
42  SHELL32.dll                     CSaveControlsModuleInner::SetSite
43  SHELL32.dll                     CSaveControlsModule::LayoutInitialized
44  SHELL32.dll                     DUI_SendInitializationToUnknown
45  SHELL32.dll                     DUI_WalkIUnknownElements
46  SHELL32.dll                     [inlined] DUI_WalkIUnknownElements
47  SHELL32.dll                     [inlined] DUI_WalkIUnknownElements
48  SHELL32.dll                     [inlined] DUI_WalkIUnknownElements
49  SHELL32.dll                     [inlined] DUI_WalkIUnknownElements
50  SHELL32.dll                     DUI_WalkIUnknownElements
51  SHELL32.dll                     CDUIViewFrame::SetLayoutDefinition
52  SHELL32.dll                     CExplorerBrowser::_SwitchView
53  SHELL32.dll                     CExplorerBrowser::_BrowseToView
54  SHELL32.dll                     CExplorerBrowser::_BrowseObjectInternal
55  SHELL32.dll                     CExplorerBrowser::_OnBrowseObject
56  SHELL32.dll                     CExplorerBrowser::BrowseObject
57  COMDLG32.dll                    CFileOpenSave::_JumpToInitialLocation
58  COMDLG32.dll                    CFileOpenSave::_InitOpenSaveDialog
59  COMDLG32.dll                    CFileOpenSave::s_OpenSaveDlgProc
60  USER32.dll                      UserCallDlgProcCheckWow
61  USER32.dll                      DefDlgProcWorker
62  USER32.dll                      DefDlgProcW
63  USER32.dll                      UserCallWinProcCheckWow
64  USER32.dll                      SendMessageWorker
65  USER32.dll                      InternalCreateDialog
66  USER32.dll                      InternalDialogBox
67  USER32.dll                      DialogBoxIndirectParamAorW
68  USER32.dll                      DialogBoxIndirectParamW
69  COMDLG32.dll                    <lambda>::operator()
70  COMDLG32.dll                    CFileOpenSave::Show
```

## QQPinyinTSF.dll

```
Fatal Error: EXCEPTION_ACCESS_VIOLATION_READ / 0xffffffffffffffff

Thread Crashed:
0   MSCTF.dll                       CicBridge::Notify
1   MSCTF.dll                       NotifyIME
2   IMM32.DLL                       MakeIMENotify
3   IMM32.DLL                       ImmSetConversionStatus
4   MSCTF.dll                       CInputModeEventSink::Callback
5   MSCTF.dll                       CCompartment::InternalSetValue
6   MSCTF.dll                       CCompartment::SetValue
7   QQPinyinTSF.dll                 <unknown>
8   QQPinyinTSF.dll                 <unknown>
9   QQPinyinTSF.dll                 <unknown>
10  MSCTF.dll                       CCompartment::InternalSetValue
11  MSCTF.dll                       CCompartment::SetValue
12  QQPinyinTSF.dll                 <unknown>
13  QQPinyinTSF.dll                 <unknown>
14  QQPinyinTSF.dll                 <unknown>
15  MSCTF.dll                       CThreadInputMgr::_NotifyCallbacks
16  MSCTF.dll                       CThreadInputMgr::_SetFocus
17  MSCTF.dll                       CThreadInputMgr::OnInputFocusEvent
18  MSCTF.dll                       CThreadInputMgr::OnCiceroEvent
19  MSCTF.dll                       WinEventProc
20  USER32.dll                      _ClientCallWinEventProc
21  ntdll.dll                       KiUserCallbackDispatch
22  win32u.dll                      ZwUserSetFocus
23  DUser.dll                       DuRootGadget::xdUpdateKeyboardFocus
24  DUser.dll                       SetGadgetFocus
25  DUI70.dll                       DirectUI::Element::SetKeyFocus
26  explorerframe.dll               UIItemsView::OnInput
27  DUI70.dll                       DirectUI::Element::_DisplayNodeCallback
28  DUser.dll                       GPCB::xwInvokeRoute
29  DUser.dll                       GPCB::xwInvokeFull
30  DUser.dll                       BaseMsgQ::xwProcessNL
31  DUser.dll                       DeferredMsgQueueLock::~DeferredMsgQueueLock
32  DUser.dll                       HWndContainer::xdHandleMessage
33  DUser.dll                       ExtraInfoWndProc
34  USER32.dll                      UserCallWinProcCheckWow
35  USER32.dll                      CallWindowProcW
36  comctl32.DLL                    CallNextSubclassProc
37  comctl32.DLL                    DefSubclassProc
38  explorerframe.dll               UIItemsView::_UIItemsViewSubclassProc
39  explorerframe.dll               UIItemsView::s_UIItemsViewSubclassProc
40  comctl32.DLL                    CallNextSubclassProc
41  comctl32.DLL                    DefSubclassProc
42  explorerframe.dll               CToolTipManager::_PropertyToolTipSubclassProc
43  explorerframe.dll               CToolTipManager::s_PropertyToolTipSubclassProc
44  comctl32.DLL                    CallNextSubclassProc
45  comctl32.DLL                    TTSubclassProc
46  comctl32.DLL                    CallNextSubclassProc
47  comctl32.DLL                    MasterSubclassProc
48  USER32.dll                      UserCallWinProcCheckWow
49  USER32.dll                      DispatchMessageWorker
50  USER32.dll                      IsDialogMessageW
51  USER32.dll                      DialogBox2
52  USER32.dll                      InternalDialogBox
53  USER32.dll                      DialogBoxIndirectParamAorW
54  USER32.dll                      DialogBoxIndirectParamW
55  COMDLG32.dll                    <lambda>::operator()
56  COMDLG32.dll                    CFileOpenSave::Show
```

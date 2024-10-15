# safedlgs

> This is a proof of concept. Use this code as [you wish](LICENSE.txt).

There is no safe way to use the standard file dialog to open/save
files on Windows. What does this mean? It looks like there are several
`comctl32.dll` implementations, and some third-party Windows IME
(Input Method Editors) that can crash your application just by using
the standard file dialog from Windows.

As the file dialog contains a `EDIT` control, it opens a full range of
possible bugs/crashes when a third-party IME is configured. Which
means that just opening the file selector is a potential source of
crashes for your application.

One possible way to fix this is using a background process just to
open the file dialog. If the process crashes, we can reopen it, even
using the last location where the crash was produced.

## Demo

This project includes the following programs:

* `dlgs.exe`: Shows the native Windows file dialog using the
  [IFileDialog](https://learn.microsoft.com/en-us/windows/win32/shell/common-file-dialog)
  interface. This process can crash. Each time the user changes to
  other folder, it prints that folder to STDOUT, so we can re-open the
  file dialog in the last location.
* `test.exe`: Wrapper that (re-)executes `dlgs.exe`. This process
  shouldn't crash, if it detects that `dlgs.exe` crashed, it just
  re-open it in the last visited location.

The `dlgs.exe` will show the native file dialog with an extra "Crash
Process" button just to test how it works in case of crash:

![Screen Shot](https://private-user-images.githubusercontent.com/39654/376804575-b6712452-8988-456b-a12f-9f6c39ec796e.png?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3MjkwMjc1NDMsIm5iZiI6MTcyOTAyNzI0MywicGF0aCI6Ii8zOTY1NC8zNzY4MDQ1NzUtYjY3MTI0NTItODk4OC00NTZiLWExMmYtOWY2YzM5ZWM3OTZlLnBuZz9YLUFtei1BbGdvcml0aG09QVdTNC1ITUFDLVNIQTI1NiZYLUFtei1DcmVkZW50aWFsPUFLSUFWQ09EWUxTQTUzUFFLNFpBJTJGMjAyNDEwMTUlMkZ1cy1lYXN0LTElMkZzMyUyRmF3czRfcmVxdWVzdCZYLUFtei1EYXRlPTIwMjQxMDE1VDIxMjA0M1omWC1BbXotRXhwaXJlcz0zMDAmWC1BbXotU2lnbmF0dXJlPTJlZDEzMWM4OWY4Yzk3ZDM0NGI1OWJmMDU0MmYxNjVmOWJmYzllMmY4YzdhYTA4MWRkYmIxMjVmYTE5MzQwNmImWC1BbXotU2lnbmVkSGVhZGVycz1ob3N0In0.QDQr10iqt3sOtGWAnh7JI3BR5UsFUc7TZZn_iAHdnFg)

## Crashes

You can find some stack traces in the [CRASHES](CRASHES.md) file
about real world Windows and IME bugs out there.

## License

This project is released under the terms of the MIT license. Read
[LICENSE](LICENSE.txt) for more information.

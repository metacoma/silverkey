#NoEnv  ; Recommended for performance and compatibility with future AutoHotkey releases.
#Warn  ; Enable warnings to assist with detecting common errors.
SendMode Input  ; Recommended for new scripts due to its superior speed and reliability.
SetWorkingDir %A_ScriptDir%  ; Ensures a consistent starting directory.
#SingleInstance Force
#WinActivateForce
DetectHiddenWindows, On

Run Notepad,,,NotepadPID
Sleep 500
Run ../../bin/silverkey-qt.exe,,,SilverKeyPID
if SilverKeyPID
{
    Sleep 500
    WinGetTitle, SilverKeyTitle, ahk_pid %SilverKeyPID% ; Title will contain the processe's first window's title
    SilverKeyWinID = WinExist(SilverKeyTitle)


    WinActivate ahk_id %SilverKeyWinID%
    SendMessage, 0x0312, -2, 2, , %SilverKeyTitle%
    Sleep 100
    Send /aaab
    Sleep 100
    Send {Down}
    Sleep 100
    Send {Enter}
    Sleep 500
    Send ^s
    Sleep 100
    Send, {Alt Down}{Alt Up}test.txt{Enter}
    Sleep 1000
    Process, Close, %SilverKeyPID%
    Process, Close, %NotepadPID%
    return
}


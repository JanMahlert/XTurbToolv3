#include "XTurbRunner.h"
#include "Logger.h"
#include <fstream>
#include <filesystem>

XTurbRunner::XTurbRunner(const std::wstring& exePath) : exePath(exePath) {}

bool XTurbRunner::run(const std::wstring& inputFilePath) {
    std::filesystem::path exeFsPath(exePath);
    std::wstring exeDir = exeFsPath.parent_path().wstring();
    std::wstring batFilePath = exeDir + L"\\run_xturb_temp.bat";

    // Create a .bat file to run XTurb with input redirection
    std::wofstream batFile(batFilePath);
    if (!batFile.is_open()) {
        Logger::logError(L"Failed to create batch file.");
        return false;
    }

    // Write the batch contents
    batFile << L"@echo off\n";
    batFile << L"cd /d \"" << exeDir << L"\"\n";
    batFile << L"\"" << exePath << L"\" < \"" << inputFilePath << L"\"\n";
    batFile.close();

    // Command line to run the batch
    std::wstring commandLine = L"cmd.exe /C \"" + batFilePath + L"\"";

    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };

    BOOL success = CreateProcessW(
        nullptr,
        &commandLine[0],
        nullptr,
        nullptr,
        FALSE,
        CREATE_NO_WINDOW,
        nullptr,
        nullptr,
        &si,
        &pi
    );

    if (!success) {
        Logger::logError(L"Failed to run batch file. Error code: " + std::to_wstring(GetLastError()));
        return false;
    }

    // Wait for the process to complete
    DWORD waitResult = WaitForSingleObject(pi.hProcess, 30000); // wait max 30s
    if (waitResult != WAIT_OBJECT_0) {
        Logger::logError(L"XTurb process timed out or failed to complete.");
        TerminateProcess(pi.hProcess, 1);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return false;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // Cleanup Process; Important, otherwise program freezes.
    std::filesystem::remove(batFilePath);

    Logger::logError(L"XTurb completed successfully.");
    return true;
}

std::wstring XTurbRunner::getExePath() const {
    return exePath;
}


#include "logger.h"
#include <Windows.h>

namespace logger {

    static const char* LOG_PATH = "C:\\Delusive\\Rust\\Debug.md";

    bool initialize() {
        AllocConsole();
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        freopen_s((FILE**)stdin, "CONIN$", "r", stdin);

        CreateDirectoryA("C:\\Delusive\\Rust\\", nullptr);

        HANDLE hFile = CreateFileA(
            LOG_PATH,
            GENERIC_WRITE,
            FILE_SHARE_READ,
            nullptr,
            CREATE_ALWAYS,          
            FILE_ATTRIBUTE_NORMAL,
            nullptr
        );

        if (hFile == INVALID_HANDLE_VALUE) return false;

        SYSTEMTIME st;
        GetLocalTime(&st);

        char header[512];
        int  header_len = snprintf(header, sizeof(header),
            "# Delusive \xE2\x80\x94 Debug Log\r\n"         
            "**Session:** %04d-%02d-%02d %02d:%02d:%02d  \r\n"
            "\r\n"
            "---\r\n"
            "\r\n",
            st.wYear, st.wMonth, st.wDay,
            st.wHour, st.wMinute, st.wSecond
        );

        DWORD written = 0;
        WriteFile(hFile, header, (DWORD)header_len, &written, nullptr);
        CloseHandle(hFile);

        return true;
    }

    void write_to_file(const char* text) {
        if (!text || !*text) return;

        HANDLE hFile = CreateFileA(
            LOG_PATH,
            FILE_APPEND_DATA,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            OPEN_ALWAYS,           
            FILE_ATTRIBUTE_NORMAL,
            nullptr
        );

        if (hFile == INVALID_HANDLE_VALUE) return;

        DWORD written = 0;
        WriteFile(hFile, text, (DWORD)strlen(text), &written, nullptr);
        CloseHandle(hFile);
    }

} // namespace logger
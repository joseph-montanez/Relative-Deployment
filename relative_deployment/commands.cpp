
#include "stdafx.h"
#include "commands.h"
#include <iostream>
#include <winsock2.h>
#include <stdlib.h>
#include <fstream>
#include <windows.h>
#include "WinBase.h"
#include "picojson.h"
#include <crtdbg.h>
#include <stdarg.h>
#include <direct.h>
#include <stdio.h>
#include <string>

int get_random_port() {
	int port_range = max_port - min_port;
	return min_port + (rand() % port_range + 1);
}

bool port_is_available(int port, const char *address) {
    struct sockaddr_in client;         
    SOCKET sock = INVALID_SOCKET;
	WSADATA stack_info; 
	int error;

	error = WSAStartup(MAKEWORD(2,0), &stack_info); 

	if (error != 0) {
		std::cout << "Failed to startup winsock" << std::endl;
		return false;
	}

    client.sin_family = AF_INET;  
    client.sin_port = htons(port);  
    client.sin_addr.S_un.S_addr = inet_addr(address);

    sock = static_cast<int>(socket(AF_INET, SOCK_STREAM, 0)); 

	if (sock == INVALID_SOCKET) {
		std::cout << "Failed to startup a socket" << std::endl;
		return false;
	}

    int result = connect(sock, reinterpret_cast<struct sockaddr *>(&client), sizeof(client)); 
	bool unused = (result == SOCKET_ERROR);

	closesocket(sock);
	WSACleanup();

    if (unused) {
		return true;
	}
    else {
		return false;
	}
}

int get_available_port() {
	bool avail = false;
	int avail_port = 0;
	do {
		int port = get_random_port();
		if (port_is_available(port, "127.0.0.1")) {
			avail = true;
			avail_port = port;
		}
	} while(!avail);

	return avail_port;
}

std::string read_file(const char *filename) {
	std::string data(
		std::istreambuf_iterator<char>(
			(std::ifstream(filename).rdbuf())
		),
		(std::istreambuf_iterator<char>())
	);

	return data;
}


void run_command(const wchar_t *cmd)
{

	LPTSTR szCmdline = _tcsdup(cmd);
	STARTUPINFO info={sizeof(info)};
	PROCESS_INFORMATION processInfo;
	int created = CreateProcess(
		NULL,        // _In_opt_     LPCTSTR lpApplicationName,
		szCmdline,   // _Inout_opt_  LPTSTR lpCommandLine,
		NULL,        // _In_opt_     LPSECURITY_ATTRIBUTES lpProcessAttributes,
		NULL,        // _In_opt_     LPSECURITY_ATTRIBUTES lpThreadAttributes,
		NULL,        // _In_         BOOL bInheritHandles,
		NULL,        // _In_         DWORD dwCreationFlags,
		NULL,        // _In_opt_     LPVOID lpEnvironment,
		NULL,        // _In_opt_     LPCTSTR lpCurrentDirectory,
		&info,       // _In_         LPSTARTUPINFO lpStartupInfo,
		&processInfo // _Out_        LPPROCESS_INFORMATION lpProcessInformation
	);
}

void open_browser(const wchar_t *url)
{
	ShellExecute(NULL, TEXT("open"), url, NULL, NULL, SW_SHOWNORMAL);
}

void TRACE(const char *message, ...) {
	char message_buffer[1024];
	va_list args;
	va_start(args, message);

	std::string message_line(message);
	message_line.append("\n");

	_vsnprintf_s(message_buffer, 1024, message_line.c_str(), args);
	std::cout << message_buffer;
	_RPT0(_CRT_WARN, message_buffer);
}

void cwd()
{
	char* buffer;

	// Get the current working directory: 
	if((buffer = _getcwd( NULL, 0 )) == NULL) {
		perror( "_getcwd error" );
	}
	else {
		TRACE("%s \nLength: %d\n", reinterpret_cast<const char *>(buffer), strlen(buffer));
		free(buffer);
	}
}
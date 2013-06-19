#pragma once
#include "stdafx.h"
#include <string>

const int min_port = 49152;
const int max_port = 65535;

int get_random_port();
bool port_is_available(int port, const char *address);
int get_available_port();
std::string read_file(const char *filename);
void run_command(const wchar_t *cmd);
void open_browser(const wchar_t *url);
void TRACE(const char *message, ...);
void cwd();
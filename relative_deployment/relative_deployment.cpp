// relative_deployer.cpp : Defines the entry point for the console application.
//
#pragma comment(lib, "ws2_32.lib")

#include "stdafx.h"
#include "picojson.h"
#include "commands.h"
#include <stdio.h>
#include <time.h>
#include <string>
#include <iostream>
#include <map>

int _tmain(int argc, _TCHAR* argv[])
{
	srand(static_cast<unsigned int>(time(NULL)));

	TRACE("Reading configuration file");
	cwd();

	std::string config_data(read_file("config.json"));
	std::map <std::string, std::string>ports;

	if (config_data.length() > 0) {
		TRACE("Parsing configuration file");
		picojson::value root;

		//-- Parse
		std::string error("");
		picojson::parse(root, config_data.begin(), config_data.end(), &error);
		if (error.length() > 0) {
			TRACE("Unable to parse configuration file: ");
			exit(1);
		}

		//-- Get commands
		if (root.contains("commands")) {
			TRACE("commands is found, checking if it is an array");
			picojson::value commands = root.get("commands");
			if (commands.is<picojson::array>()) {
				picojson::array arr = commands.get<picojson::array>();
				for (picojson::array::iterator it = arr.begin(); it != arr.end(); it++) {
					if (!it->is<picojson::object>()) {
						TRACE("Every command must be an object");
						exit(1);
					}
					picojson::object obj = it->get<picojson::object>();

					//-- Get the name
					picojson::value json_name = obj["name"];
					std::string name(json_name.to_str());
					if (name.length() > 0) {
						TRACE("name: %s", name.c_str());
					} else {
						TRACE("Every commands needs a pseudonym, a \"name\"");
						exit(1);
					}

					//-- Get the type
					picojson::value json_type = obj["type"];
					std::string type(json_type.to_str());
					if (type.length() == 0) {
						type = "process";
					}
					TRACE("type: %s", type.c_str());

					//-- Get the cmd
					picojson::value json_cmd = obj["cmd"];
					std::string cmd(json_cmd.to_str());
					if (cmd.length() > 0) {
						TRACE("cmd: %s", cmd.c_str());
					} else {
						TRACE("Every commands needs a command line, a \"cmd\"");
						exit(1);
					}

					//-- Get the port
					picojson::value json_port = obj["port"];
					std::string port(json_port.to_str());
					if (port.length() > 0) {
						TRACE("port: %s", port.c_str());
						bool existing_port = std::string::npos != port.find(":");
						//-- Get a random available port
						if (port.compare("random") == 0) {
							int avail_port = get_available_port();
							port = "";
							port.append(std::to_string(avail_port));
							port;
						}
						//-- Pull a port from an existing command
						else if (existing_port) {
							std::string port_name = port.substr(1, port.length());
							TRACE("find the port \"%s\" in the ports map", port_name.c_str());
							if (ports.find(port_name) != ports.end()) {
								port = ports[port_name];
							} else {
								TRACE("Cannot find existing port named \"%s\" are you sure its defined before this command?", port_name.c_str());
								exit(1);
							}
						}
						//-- See if port is in use
						else {
							size_t avail_port = std::stoi(port);
							if (!port_is_available(avail_port, "localhost")) {
								TRACE("port %s is already used", port.c_str());
								exit(1);
							}
						}
						ports[name] = port;
					}

					//-- Replace the random port
					std::size_t port_index = cmd.find("$port");
					if (std::string::npos != port_index) {
						cmd.replace(port_index, 5, port);
					}

					for (auto it = ports.begin(); it != ports.end(); ++it) {
						std::string mapped_port_name = it->first;
						std::string mapped_port = it->second;
						std::size_t mapped_port_index;
						do {
							mapped_port_index = cmd.find("$:" + mapped_port_name);
							if (std::string::npos != mapped_port_index) {
								cmd.replace(mapped_port_index, mapped_port_name.length() + 2, mapped_port);
							}
						} while(std::string::npos != mapped_port_index);
					}

					TRACE("RUNNING CMD: %s", cmd.c_str());
					
					std::wstring wcmd(cmd.begin(), cmd.end());
					if (type.compare("process") == 0) {
						run_command(wcmd.c_str());
					}
					else if (type.compare("webpage") == 0) {
						open_browser(wcmd.c_str());
					}
					
					TRACE("cmd after: %s", cmd.c_str());
				}
			} else {
				TRACE("commands needs to be an array");
				exit(1);
			}
		}
	} else {
		TRACE("Unable read configuration file");
		exit(1);
	}
	return 0;
}


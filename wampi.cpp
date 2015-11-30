/**
 *
 * How to compile: g++ -O3 -std=c++0x -o wampi wampi.cpp aprs.cpp
 */

#include "aprs.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>

using namespace std;

void show_help(char *s);

int main(int argc, char **argv)
{
	int option_char;
	string to_call, from_call, message = "";
	string path = "WIDE1-1";

	if(argc == 1) {
		show_help(argv[0]);
		exit(1);
	}

	while ((option_char = getopt(argc, argv, "s:d:p:m:")) != -1) {
		switch(option_char) {
			case 's':
				from_call = optarg;
				transform(from_call.begin(), from_call.end(), from_call.begin(), ::toupper);
				break;
			case 'd':
				to_call = optarg;
				transform(to_call.begin(), to_call.end(), to_call.begin(), ::toupper);
				break;
			case 'p':
				path = optarg;
				transform(path.begin(), path.end(), path.begin(), ::toupper);
				if (path.empty()) {
					path = "WIDE1-1";
				}
				break;
			case 'm':
				message = optarg;
				break;
			case '?':
				show_help(argv[0]);
				break;
			case ':':
				cout << "Missing required parameter!" << endl;
				show_help(argv[0]);
				exit(1);
		}
	}

	// show summary
	cout << left;
	cout << setfill(' ') << setw(15) << "Source:" << from_call << endl;
	cout << setfill(' ') << setw(15) << "Destination:" << to_call << endl;
	cout << setfill(' ') << setw(15) << "Path:" << path << endl;
	cout << setfill(' ') << setw(15) << "Message:" << message << endl;
	cout << endl;

	Aprs* a = new Aprs("APRS-0", "AB3Y-1", "WIDE1-1", "Test");
	a->setGps("0000.00N", "0000.00W", "0");
	a->buildPacket();
	a->displayPacket(1);
	a->displayBitStream();

	return 1;
}

void show_help(char *s)
{
	cout << "Usage: " << s << " -d <to callsign> -s <from callsign> [-p <path>] [-m <message>]" << endl;
	cout << "Options: -d destination callsign with optional SSID (e.g. APRS or APRS-0)" << endl;
	cout << "         -s source callsign with optional SSID (e.g. AB3Y or AB3Y-1) " << endl;
	cout << "         -p optional path (default is WIDE1-1)" << endl;
	cout << "         -m option message to be attached to the APRS packet" << endl;
}

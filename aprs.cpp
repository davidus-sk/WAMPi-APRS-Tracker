#include <iostream>
#include <string>
#include <vector>
#include "aprs.h"

using namespace std;

Aprs::Aprs(string fromCallsign, string toCallsign, string path, string message)
{
	s_fromCallsign = fromCallsign;
	s_toCallsign = toCallsign;
	s_path = path;
	s_message = message;
}

void Aprs::setGps(string latitude, string longitude, string altitude)
{
	s_lat = latitude;
	s_lon = longitude;
	s_alt = altitude;
}

void Aprs::buildPacket()
{
	bool hasSsid;
	size_t len;
	size_t l;
	string callsign;
	string ssid;
	int id;

	// -- destination callsign

	// determine length
	hasSsid = s_toCallsign.find_first_of("-") == std::string::npos;
	len = hasSsid ? s_toCallsign.length() : s_toCallsign.find_first_of("-");

	// pad if needed
	callsign = s_toCallsign;
	if (len < 6) {
		l = 6 - len;
		callsign = s_toCallsign.substr(0, len);
		callsign.append(l, ' ');
	}

	// insert into buffer
	for (int i = 0; i < 6; i++) {
		char ch = (char)callsign.at(i);

		// shift each byte by one
		ch = ch << 1;

		vc_buffer.push_back(ch);
	}

	// insert SSID
	ssid = hasSsid ? "" : s_toCallsign.substr(len + 1);

	if (!ssid.empty()) {
		id = std::stoi(ssid, nullptr, 0);
	} else {
		id = 0;
	}

	vc_buffer.push_back(0xe0 + (id << 1));

	// -- source callsign

	// determine length
	hasSsid = s_fromCallsign.find_first_of("-") == std::string::npos;
	len = hasSsid ? s_fromCallsign.length() : s_fromCallsign.find_first_of("-");

	// pad if needed
	callsign = s_fromCallsign;
	if (len < 6) {
		l = 6 - len;
		callsign = s_fromCallsign.substr(0, len);
		callsign.append(l, ' ');
	}

	// insert into buffer
	for (int i = 0; i < 6; i++) {
		char ch = (char)callsign.at(i);

		// shift each byte by one
		ch = ch << 1;

		vc_buffer.push_back(ch);
	}

	// insert SSID
	ssid = hasSsid ? "" : s_fromCallsign.substr(len + 1);

	if (!ssid.empty()) {
		id = std::stoi(ssid, nullptr, 0);
	} else {
		id = 0;
	}

	vc_buffer.push_back(0xe0 + (id << 1));

	// -- path
	len = s_path.length();
	for (int i = 0; i < len; i++) {
		char ch = (char)s_path.at(i);

		// shift each byte by one
		ch = ch << 1;

		// last byte is ORed with 1 to mark end
		if (i == (len - 1)) {
			ch = ch | 1;
		}

		vc_buffer.push_back(ch);
	}

	// -- control field
	vc_buffer.push_back(0x03);

	// -- protocol id
	vc_buffer.push_back(0xf0);

	// -- mesage

	vc_buffer.push_back('!');

	// gps first
	if (!s_lat.empty() && !s_lon.empty()) {
		string location = s_lat + "/" + s_lon + ">";
		std::copy(location.begin(), location.end(), std::back_inserter(vc_buffer));
	}

	// message
	if (!s_message.empty()) {
		std::copy(s_message.begin(), s_message.end(), std::back_inserter(vc_buffer));
	}

	// calculate CRC of the packet
	calculateFcs();

	// create bit stream
	buildBitStream();
}

void Aprs::calculateFcs()
{
	unsigned short crc = 0xffff;
	unsigned short poly_table[] = {
		0x0000, 0x1081, 0x2102, 0x3183,
		0x4204, 0x5285, 0x6306, 0x7387,
		0x8408, 0x9489, 0xa50a, 0xb58b,
		0xc60c, 0xd68d, 0xe70e, 0xf78f
	};

	for (char ch : vc_buffer) {
		crc = ( crc >> 4 ) ^ poly_table[(crc & 0xf) ^ (ch & 0xf)];
		crc = ( crc >> 4 ) ^ poly_table[(crc & 0xf) ^ (ch >> 4)];
	}

	// load low byte first
	vc_buffer.push_back(crc & 0xff);
	vc_buffer.push_back(crc >> 8 & 0xff);
}

void Aprs::addToBitStream(char byte)
{
	for (int i = 1; i <= 128; i = i + i) {
		char bit = (byte & i) == i ? '1' : '0';
		vc_bitStream.push_back(bit);
	}
}

void Aprs::buildBitStream()
{
	short one_counter = 0;

	// add start flag
	addToBitStream(0x7e);

	// add the whole message
	for (char ch : vc_buffer) {
		// go from LSB to MSB
		for (int i = 1; i <= 128; i = i + i) {
			char bit = (ch & i) == i ? '1' : '0';

			if (bit == '1') {
				one_counter++;
			} else {
				one_counter = 0;
			}

			vc_bitStream.push_back(bit);

			// if we encounter 5 1s in a row, put 0 after the 5th
			if (one_counter == 5) {
				vc_bitStream.push_back('0');
				one_counter = 0;
			}
		}
	}

	// add stop flag
	addToBitStream(0x7e);
}

void Aprs::displayBitStream()
{
	cout << "Printing APRS bit stream:" << endl << endl;

	for (char ch : vc_bitStream) {
		cout << ch;
	}

	 cout << endl << endl;
}

void Aprs::displayPacket(bool ascii = 0)
{
	cout << "Printing AX.25 APRS packet in " << (ascii ? "ASCII" : "HEX") << ":" << endl << endl;

	if (!ascii) {
		cout << std::hex << std::uppercase;
	}

	for (char ch : vc_buffer) {
		if (ascii) {
			cout << ch << " ";
		} else {
			cout << (int)ch << " ";
		}
	}

	cout << endl << endl;
}

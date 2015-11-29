#include <string>
#include <vector>

using namespace std;

#ifndef APRS_H
#define APRS_H

class Aprs
{
	private:
		vector<char> vc_buffer;
		vector<char> vc_bitStream;

		string s_fromCallsign;
		string s_toCallsign;
		string s_path;
		string s_message;

		string s_lat;
		string s_lon;
		string s_alt;

		void calculateFcs();
		void addToBitStream(char byte);
		void buildBitStream();

	public:
		Aprs(string fromCallsign, string toCallsign, string path, string message);
		void setGps(string latitude, string longitude, string altitude);
		void buildPacket();
		void displayPacket(bool ascii);
		void displayBitStream();
};

#endif

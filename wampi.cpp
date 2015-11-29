/**
 *
 * How to compile: g++ -O3 -std=c++0x -o wampi wampi.cpp aprs.cpp
 */

#include "aprs.h"

int main() {
	Aprs* a = new Aprs("APRS-0", "AB3Y-1", "WIDE1-1", "Test");
	a->setGps("0000.00N", "0000.00W", "0");
	a->buildPacket();
	a->displayPacket(1);
	a->displayBitStream();

	return 1;
}

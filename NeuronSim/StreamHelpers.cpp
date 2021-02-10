#include "StreamHelpers.h"

using namespace std;

// Write a string, and its length, to an output stream
void writeString(const string & str, ofstream & ofs)
{
	uint32_t size = uint32_t(str.size());
	ofs.write(reinterpret_cast<char *>(&size), sizeof(size));
	ofs.write(&str[0], size);
}

// Read a string, and its length, from an input stream
void readString(string & str, ifstream & ifs)
{
	uint32_t length;
	ifs.read(reinterpret_cast<char *>(&length), sizeof(length));
	str.resize(length);
	ifs.read(&str[0], length);
}


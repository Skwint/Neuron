#ifndef STREAM_HELPERS_H
#define STREAM_HELPERS_H

#include <string>
#include <fstream>

// Write POD data to an output stream
template <typename T>
void writePod(T data, std::ofstream & ofs)
{
	ofs.write(reinterpret_cast<char *>(&data), sizeof(T));
}

// Read PDO data from an input stream
template <typename T>
void readPod(T & data, std::ifstream & ifs)
{
	ifs.read(reinterpret_cast<char *>(&data), sizeof(T));
}

// Write a string, and its length, to an output stream
void writeString(const std::string & str, std::ofstream & ofs);

// Read a string, and its length, from an input stream
void readString(std::string & str, std::ifstream & ifs);


#endif

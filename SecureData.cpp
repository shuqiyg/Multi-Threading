/* Workshop 9 - Multi-Threading
   Name: Shuqi Yang
   Std ID: 132162207
   Email: syang136@myseneca.ca
   Date: 07-28-2021
   -------------------------------------------------------
   I have done all the coding by myself and only copied the
   code that my professor provided to complete my workshops
   and assignments.
 */
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <functional>
#include <chrono>
#include "SecureData.h"

using namespace std;
using namespace std::placeholders;

namespace w9 {

	void converter(char* t, char key, int n, const Cryptor& c) {
		for (int i = 0; i < n; i++)
			t[i] = c(t[i], key);
	}

	SecureData::SecureData(const char* file, char key, ostream* pOfs)
	{
		ofs = pOfs;

		// open text file
		fstream input(file, std::ios::in);
		if (!input)
			throw string("\n***Failed to open file ") +
			string(file) + string(" ***\n");

		// copy from file into memory
		input.seekg(0, std::ios::end);
		nbytes = (int)input.tellg() + 1;

		text = new char[nbytes];

		input.seekg(ios::beg);
		int i = 0;
		input >> noskipws;
		while (input.good())
			input >> text[i++];
		text[nbytes - 1] = '\0';
		*ofs << "\n" << nbytes - 1 << " bytes copied from file "
			<< file << " into memory (null byte added)\n";
		encoded = false;

		// encode using key
		code(key);
		*ofs << "Data encrypted in memory\n";
	}

	SecureData::~SecureData() {
		delete[] text;
	}

	void SecureData::display(std::ostream& os) const {
		if (text && !encoded)
			os << text << std::endl;
		else if (encoded)
			throw std::string("\n***Data is encoded***\n");
		else
			throw std::string("\n***No data stored***\n");
	}

	void SecureData::code(char key)
	{
		char* text_p0 = text;
		char* text_p1 = text + nbytes / 4;
		char* text_p2 = text_p1 + nbytes / 4;
		char* text_p3 = text_p2 + nbytes / 4;
	
		auto fn_conv = std::bind(converter, _1, key, nbytes/ 4 , Cryptor());
		std::thread t1(fn_conv, text_p0);
		std::thread t2(fn_conv, text_p1);
		std::thread t3(fn_conv, text_p2);
		std::thread t4(fn_conv, text_p3);

		t1.join();
		t2.join();
		t3.join();
		t4.join();
		encoded = !encoded;
	}

	void SecureData::backup(const char* file) {
		if (!text)
			throw std::string("\n***No data stored***\n");
		else if (!encoded)
			throw std::string("\n***Data is not encoded***\n");
		else
		{
			std::ofstream fw(file, std::ios::out | std::ios::binary|std::ios::trunc);
			if (!fw) throw string("Fail to open file!");
			fw.write(text, nbytes);
			fw.close();
		}
	}

	void SecureData::restore(const char* file, char key) {
		std::ifstream fr(file, std::ios::in | std::ios::binary);
		if (!fr) throw string("Fail to open file!");

		fr.seekg(0, std::ios::end);
		nbytes = (int)fr.tellg() + 1;
		delete []text;
		text = new char[nbytes];

		fr.seekg(ios::beg);
		int i = 0;
		fr >> noskipws;
		while (fr.good())
			fr >> text[i++];
		text[nbytes - 1] = '\0';


		*ofs << "\n" << nbytes - 1 << " bytes copied from binary file "
			<< file << " into memory.\n";

		encoded = true;

		// decode using key
		code(key);

		*ofs << "Data decrypted in memory\n\n";
	}

	std::ostream& operator<<(std::ostream& os, const SecureData& sd) {
		sd.display(os);
		return os;
	}
}

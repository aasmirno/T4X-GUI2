#pragma once
#include <iostream>
#include <fstream>
#include <string>

/*
	class Logger:
		barebones logging using stlib
		Methods:
			log
			clear	
*/

class Logger {
private:
	std::string output_file = " ";
public:
	/*
		Create a logger with given output file
	*/
	Logger(std::string file_name) {
		output_file = file_name;
	}
	Logger() {}

	/*
		Set or change the output file string
	*/
	void setOutput(std::string new_out) {
		output_file = new_out;
	}

	/*
		Log a message to the file
	*/
	int log(std::string message) {
		//Check that the out file has been set correctly
		if (output_file == " ") {
			return -1;
		}

		std::ofstream fout;
		fout.open(output_file);
		if (fout.fail()) {	//check file opening error
			return -1;
		}
		fout << message;
		fout.close();

		return 0;
	}
};


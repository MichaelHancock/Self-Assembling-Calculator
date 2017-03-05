#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <array>
#include <ctime>
#include <set>

#include "DanaGrammer.h"
#include "DanaVariable.h"

//Function prototypes
std::string readDataFile(std::string);
void writeDataFile(std::string, std::string);
std::string getRequiredInterfaces();
std::string composeFunction(std::string, std::string, int);
std::string runSystemCommand(std::string);
bool compileFunction(std::string);
bool testFunction(std::string, std::string, std::string);
int testFunction(std::string path, std::string test);

int main() {
	//Read input and target data
	std::cout << " - Input Data - " << std::endl;
	const std::string input = readDataFile("Input file not found, try again");
	std::cout << "\n - Output Data - " << std::endl;
	const std::string target = readDataFile("Target file not found, try again");
	
	//Get required interfaces
	std::cout << "\n - Required Interfaces - \nInput required interfaces (seperate by <enter_key>), when complete type -d" << std::endl;
	const std::string requires = getRequiredInterfaces();

	//Loop until solution can be found
	const std::string outputPath = "Resources/calculator.dn";
	const std::string compilePath = "Resources/calculator.o";
	std::set<std::string> failedAttempts;
	bool complete = false;
	bool didCompile = false;
	bool solutionFound = false;

	//Time implementation
	clock_t begin = clock();

	while (!complete) {
		//Generate code
		const std::string function = composeFunction(outputPath, requires, 6);
		const std::set<std::string>::iterator it = failedAttempts.find(function);

		//Check that composed function has not been tried before
		if (it == failedAttempts.end()) {
			//Compile code 
			if (compileFunction(outputPath))
				solutionFound = testFunction(compilePath, input, target);

			if (!solutionFound)
				failedAttempts.insert(function);

			//End loop if solution is found
			complete = solutionFound;
		}
	}

	//Calculate time taken to run 
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

	//Output result to console
	std::cout << "\n\nFunction generated successfully in " << elapsed_secs << " CPU seconds" << std::endl;
	std::cout << "Number of failed attempts: " << failedAttempts.size() << std::endl;
	std::cout << "Result of  " << input << ": " << testFunction(compilePath, input) << std::endl;
	
	//Output general solution result
	srand((unsigned int)time(0));
	for (int i = 0; i < 10; i++) {
		std::string nextTest = std::to_string(rand() % 10) + " + " + std::to_string(rand() % 10);
		std::cout << "Result of  " << nextTest << ": " << testFunction(compilePath, nextTest) << std::endl;
	}
	
	//Check if program should be run again
	std::string runAgain = "";
	std::cout << "\Run again (y/n)?  ";
	std::cin >> runAgain;
	if (runAgain == "Y" || runAgain == "y" || runAgain == "yes") {
		std::cout << "\n-------------------------------------\n" << std::endl;
		main();
	}

	return 0;
}

std::string readDataFile(std::string errorMessage) {
	//Read file and return result in string
	bool didRead = false;
	std::string filename = "";
	std::string result = "";
	std::string buffer = "";

	while (!didRead) {
		//Loop until file has been read
		std::cout << "Enter filename: ";
		std::cin >> filename;
		std::ifstream fileToRead(filename);

		if (fileToRead.is_open()) {
			//Read all lines
			while (fileToRead >> buffer) {

				//Add spaces where needed
				if (result.size() > 0)
					buffer = " " + buffer;

				result += buffer;
			}

			fileToRead.close();
			didRead = true;
		}
		else {
			//File not found
			std::cout << errorMessage << " " << std::endl;
		}
	}

	return result;
}

void writeDataFile(std::string toWrite, std::string path) {
	//Output generated file
	std::ofstream generatedFile(path);
	generatedFile << toWrite;
	generatedFile.close();
}

std::string getRequiredInterfaces() {
	//Get all required interfaces for Dana program
	std::string requires = "";
	std::string read;
	bool complete = false;

	//Read input until -d command is input
	while (!complete) {
		std::cout << "Required: ";
		std::cin.ignore(std::cin.rdbuf()->in_avail(), '\n');
		std::getline(std::cin, read);

		//Check for termination character
		if (read != "-d")
			requires = requires + " " + read + ",";
		else {
			//Remove trailing character and end loop
			requires.pop_back();
			complete = true;
		}
	}

	return requires;
}

std::string composeFunction(std::string outputPath, std::string requiredInterfaces, int solutionLength) {
	//Compose known variables
	DanaVariable a = DanaVariable("a", "string", 5);
	std::vector <DanaVariable> variables = { a };

	//Initialise DanaGrammer class
	DanaGrammer functionGenerator = DanaGrammer(requiredInterfaces, variables);
	
	//Generate code to find solution to target txt file
	std::vector<std::string> lines = {"component provides App requires" + requiredInterfaces + " {"};
	lines.push_back("int App:main(AppParam params[]) {");
	lines.push_back("char a[] = params[0].string");

	//Loop until solution of correct length has be generated
	for (int i = 0; i < solutionLength; i++) {
		lines.push_back(functionGenerator.nextLine(i, solutionLength));
	}

	lines.push_back("}");
	lines.push_back("}");

	//Collapse vector into string, append line spaces to lines of code
	std::string result = "";
	for (size_t i = 0; i < lines.size(); i++) {
		result = result + lines.at(i) + "\n";
	}
	
	//Write file to resources
	writeDataFile(result, outputPath);
	return result;
}

std::string runSystemCommand(std::string command) {
	//Execute system command and return result string
	const std::shared_ptr<FILE> pipe(_popen(command.c_str(), "r"), _pclose);
	std::array<char, 128> buffer;
	std::string result;
	
	//Check for early command fail
	if (!pipe)
		throw std::runtime_error("popen() failed!");

	//Read data from pipe
	while (!feof(pipe.get())) {
		if (fgets(buffer.data(), 128, pipe.get()) != NULL)
			result = result + buffer.data();
	}
	
	return result;
}

bool compileFunction(std::string path) {
	//Try to compile new function
	const std::string command = "dnc " + path;
	const int response = system(command.c_str());

	return response == 0;
}

bool testFunction(std::string path, std::string test, std::string expectedResult) {
	//Test new function and compare with target
	const std::string inputWithFormatting = "\"" + test + "\"";
	const std::string command = "dana " + path + " " + inputWithFormatting;
	const int response = system(command.c_str());

	return response == std::stoi(expectedResult);
}

int testFunction(std::string path, std::string test) {
	//Test function and return output
	const std::string inputWithFormatting = "\"" + test + "\"";
	const std::string command = "dana " + path + " " + inputWithFormatting;
	const int response = system(command.c_str());

	return response;
}
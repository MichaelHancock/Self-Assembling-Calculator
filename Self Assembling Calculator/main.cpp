#include <array>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "DanaLineSet.h"
#include "GeneticTransform.h"

//Function prototypes
std::string readDataFile(std::string);
std::string getRequiredInterfaces();
void appendDataFile(std::string, std::string);
int testFunction(std::string, std::string);
bool shouldRunAgain();

int main() {
	//Read input and target data
	std::cout << " - Input Data - " << std::endl;
	const std::string input = readDataFile("Input file not found, try again");
	std::cout << "\n - Output Data - " << std::endl;
	const std::string target = readDataFile("Target file not found, try again");

	//Get required interfaces
	std::cout << "\n - Required Interfaces - \nInput required interfaces (seperate by <enter_key>), when complete type -d" << std::endl;
	const std::string requires = getRequiredInterfaces();

	//Initialise functions needed for generator
	std::vector<DanaFunction> newFunctions = {};
	newFunctions.push_back(DanaFunction("adder.add", "int", { "int", "int" }));
	newFunctions.push_back(DanaFunction("uInt.intFromString", "int", { "char" }));

	//Generate primary line of function - including assign from functions params 
	DanaFunction primaryFunctionCall = DanaFunction(DanaVariable("params", "char", 5), "string");
	primaryFunctionCall.setMatchedParam(DanaVariable("x", "int", std::to_string(0)), 0);
	DanaLine primaryLine = DanaLine(0, DanaVariable("a", "char", 5), primaryFunctionCall);

	//Initialise the primary population
	std::vector<DanaLineSet> population;
	for (int i = 0; i < 10; i++) {
		DanaLineSet newLineSet;
		newLineSet.insertLine(primaryLine);
		population.push_back(newLineSet);
	}

	//Loop until solution can be found
	const std::string outputPath = "Resources/calculator.dn";
	const std::string compilePath = "Resources/calculator.o";
	std::string resultFunction = "";
	std::string function;
	bool solutionFound = false;

	//Start thread to generate new functions
	clock_t begin = clock();

	GeneticTransform GA = GeneticTransform(population, newFunctions, requires, input, target);
	function = GA.cycleGeneration();
	std::vector<std::pair<DanaLineSet, double>> resultPopulation = GA.getPopulation();

	//Calculate time taken to run 
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

	//Output result to console
	std::cout << "\n\nFunction generated successfully in " << elapsed_secs << " CPU seconds" << std::endl;
	std::cout << "Result of  " << input << ": " << testFunction(compilePath, input) << std::endl;

	//Output general solution result
	srand((unsigned int)time(0));
	for (int i = 0; i < 10; i++) {
		std::string nextTest = std::to_string(rand() % 10) + " + " + std::to_string(rand() % 10);
		std::cout << "Result of  " << nextTest << ": " << testFunction(compilePath, nextTest) << std::endl;
	}

	//Store output of process as a text file 
	std::string processOutput = "\n";
	processOutput += "Time taken: " + std::to_string(elapsed_secs) + " CPU seconds\n";
	processOutput += function;
	appendDataFile(processOutput, "Resources/diagnostics.txt");

	if (shouldRunAgain())
		main();

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

void appendDataFile(std::string toWrite, std::string path) {
	//Append string to file 
	std::ofstream outfile;
	outfile.open(path, std::ios_base::app);
	outfile << toWrite;
	outfile.close();
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

int testFunction(std::string path, std::string test) {
	//Test function and return output
	const std::string inputWithFormatting = "\"" + test + "\"";
	const std::string command = "dana " + path + " " + inputWithFormatting;
	const int response = system(command.c_str());

	return response;
}

bool shouldRunAgain() {
	//Check if program should be run again
	std::string runAgain = "";
	std::cout << "\nRun again (y/n)?  ";
	std::cin >> runAgain;
	if (runAgain == "Y" || runAgain == "y" || runAgain == "yes") {
		std::cout << "\n-------------------------------------\n" << std::endl;
		return true;
	}

	return false;
}
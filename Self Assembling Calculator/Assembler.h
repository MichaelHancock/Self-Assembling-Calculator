#pragma once
#include "LineStore.h"
#include <assert.h>   

typedef std::vector<std::string> ListOfStrings;

class Assembler {
private:
	LineStore store;
	ListOfStrings functionHeader;
	std::string Assembler::collpaseVector(ListOfStrings lines) {
		std::string result = "";

		//	Collapse vector into string, add lines spaces
		for (int i = 0; i < lines.size(); i++) {
			result += lines.at(i) + "\n";
		}

		return result;
	}

	ListOfStrings Assembler::outputTwo(std::string variableToReturn) {
		ListOfStrings generatedFunctions;

		for (int i = 0; i < store.getList(0).size(); i++) {
			for (int j = 0; j < store.getList(1).size(); j++) {
				ListOfStrings output = functionHeader;
				output.push_back("\t\t" + store.getList(0).at(i));
				output.push_back("\t\t" + store.getList(1).at(j));
				output.push_back("\t\treturn " + variableToReturn);
				output.push_back("\t}");
				output.push_back("}");

				generatedFunctions.push_back(collpaseVector(output));
			}
		}

		return generatedFunctions;
	}

	ListOfStrings Assembler::outputThree(std::string variableToReturn) {
		ListOfStrings generatedFunctions;

		for (int i = 0; i < store.getList(0).size(); i++) {
			for (int j = 0; j < store.getList(1).size(); j++) {
				for (int k = 0; k < store.getList(2).size(); k++) {
					ListOfStrings output = functionHeader;
					output.push_back("\t\t" + store.getList(0).at(i));
					output.push_back("\t\t" + store.getList(1).at(j));
					output.push_back("\t\t" + store.getList(2).at(k));
					output.push_back("\t\treturn " + variableToReturn);
					output.push_back("\t}");
					output.push_back("}");

					generatedFunctions.push_back(collpaseVector(output));
				}
			}
		}

		return generatedFunctions;
	}

	ListOfStrings Assembler::outputFour(std::string variableToReturn) {
		ListOfStrings generatedFunctions;

		for (int i = 0; i < store.getList(0).size(); i++) {
			for (int j = 0; j < store.getList(1).size(); j++) {
				for (int k = 0; k < store.getList(2).size(); k++) {
					for (int l = 0; l < store.getList(3).size(); l++) {
						ListOfStrings output = functionHeader;
						output.push_back("\t\t" + store.getList(0).at(i));
						output.push_back("\t\t" + store.getList(1).at(j));
						output.push_back("\t\t" + store.getList(2).at(k));
						output.push_back("\t\t" + store.getList(3).at(l));
						output.push_back("\t\treturn " + variableToReturn);
						output.push_back("\t}");
						output.push_back("}");

						generatedFunctions.push_back(collpaseVector(output));
					}
				}
			}
		}

		return generatedFunctions;
	}

	ListOfStrings Assembler::outputFive(std::string variableToReturn) {
		ListOfStrings generatedFunctions;

		for (int i = 0; i < store.getList(0).size(); i++) {
			for (int j = 0; j < store.getList(1).size(); j++) {
				for (int k = 0; k < store.getList(2).size(); k++) {
					for (int l = 0; l < store.getList(3).size(); l++) {
						for (int m = 0; m < store.getList(4).size(); m++) {
							ListOfStrings output = functionHeader;
							output.push_back("\t\t" + store.getList(0).at(i));
							output.push_back("\t\t" + store.getList(1).at(j));
							output.push_back("\t\t" + store.getList(2).at(k));
							output.push_back("\t\t" + store.getList(3).at(l));
							output.push_back("\t\t" + store.getList(4).at(m));
							output.push_back("\t\treturn " + variableToReturn);
							output.push_back("\t}");
							output.push_back("}");

							generatedFunctions.push_back(collpaseVector(output));
						}
					}
				}
			}
		}

		return generatedFunctions;
	}

	ListOfStrings Assembler::outputSix(std::string variableToReturn) {
		ListOfStrings generatedFunctions;

		for (int i = 0; i < store.getList(0).size(); i++) {
			for (int j = 0; j < store.getList(1).size(); j++) {
				for (int k = 0; k < store.getList(2).size(); k++) {
					for (int l = 0; l < store.getList(3).size(); l++) {
						for (int m = 0; m < store.getList(4).size(); m++) {
							for (int n = 0; n < store.getList(5).size(); n++) {
								ListOfStrings output = functionHeader;
								output.push_back("\t\t" + store.getList(0).at(i));
								output.push_back("\t\t" + store.getList(1).at(j));
								output.push_back("\t\t" + store.getList(2).at(k));
								output.push_back("\t\t" + store.getList(3).at(l));
								output.push_back("\t\t" + store.getList(4).at(m));
								output.push_back("\t\t" + store.getList(5).at(n));
								output.push_back("\t\treturn " + variableToReturn);
								output.push_back("\t}");
								output.push_back("}");

								generatedFunctions.push_back(collpaseVector(output));
							}
						}
					}
				}
			}
		}

		return generatedFunctions;
	}

	std::string Assembler::getVariableName(int lineNumber) {
		assert(lineNumber >= 1 && lineNumber <= 26);
		return std::string(1, "abcdefghijklmnopqrstuvwxyz"[lineNumber - 1]);
	}

public:
	Assembler() {}

	Assembler(LineStore ls, ListOfStrings functionHead) {
		store = ls;
		functionHeader = functionHead;
	}

	ListOfStrings Assembler::make() {
		switch (store.size()) {
		case 2:
			return outputTwo(getVariableName(2));
		case 3:
			return outputThree(getVariableName(3));
		case 4:
			return outputFour(getVariableName(4));
		case 5:
			return outputFive(getVariableName(5));
		case 6:
			return outputSix(getVariableName(6));
		}
	}
};
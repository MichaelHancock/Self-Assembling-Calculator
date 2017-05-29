#include "Assembler.h"

/* --- Constructor --- */

Assembler::Assembler(LineStore ls, ListOfStrings functionHead) {
	store = ls;
	functionHeader = functionHead;
}

/* --- Public Member Functions --- */

ListOfStrings Assembler::make() {
	//	Populate vector of pairs - used to iterate through function variations
	std::vector<std::pair<int, int>> iter = {};
	for (int i = 0; i < 6; i++)
		iter.push_back(std::make_pair(0, 0));
	for (int i = 0; i < store.size(); i++)
		iter.at(i).second = store.getList(i).size();

	ListOfStrings result = output(iter);
	iter.clear();
	return result;
}

/* --- Private Member Functions --- */

std::string Assembler::collpaseVector(ListOfStrings lines) {
	std::string result = "";

	//	Collapse vector into string, add lines spaces
	for (int i = 0; i < lines.size(); i++) {
		result += lines.at(i) + "\n";
	}

	return result;
}

std::string Assembler::getVariableName(int lineNumber) {
	assert(lineNumber >= 1 && lineNumber <= 26);
	return std::string(1, "abcdefghijklmnopqrstuvwxyz"[lineNumber - 1]);
}

ListOfStrings Assembler::output(std::vector<std::pair<int, int>> iter) {
	ListOfStrings generatedFunctions = {};
	std::string variableToReturn = getVariableName(store.size());
	bool allVariationsFound = false;

	while (!allVariationsFound) {
		//	Compose lines of new function
		ListOfStrings newOutput = functionHeader;
		newOutput.push_back("\t\t" + store.getList(0).at(iter.at(0).first));
		newOutput.push_back("\t\t" + store.getList(1).at(iter.at(1).first));

		//	Compose further lines if store contains enough potential lines
		store.size() > 2 ? newOutput.push_back("\t\t" + store.getList(2).at(iter.at(2).first)) : NULL;
		store.size() > 3 ? newOutput.push_back("\t\t" + store.getList(3).at(iter.at(3).first)) : NULL;
		store.size() > 4 ? newOutput.push_back("\t\t" + store.getList(4).at(iter.at(4).first)) : NULL;
		store.size() > 5 ? newOutput.push_back("\t\t" + store.getList(5).at(iter.at(5).first)) : NULL;

		//	Add default function footer. Return variable created on the last line
		newOutput.push_back("\t\treturn " + variableToReturn);
		newOutput.push_back("\t}");
		newOutput.push_back("}");

		//	Collapse vector of lines into a string - append this to list of generatedFunctions
		generatedFunctions.push_back(collpaseVector(newOutput));
		newOutput.clear();

		//	Increment values of iter - used to compose above lines
		for (int i = iter.size() - 1; i > -1; i--) {
			if (iter.at(i).first < iter.at(i).second - 1) {
				iter.at(i).first++;
				break;
			}
			else {
				if (iter.at(i).first == iter.at(i).second - 1) {
					if (i - 1 == 0 && iter.at(i - 1).second == 1) {
						allVariationsFound = true;
						break;
					}

					iter.at(i).first = 0;
				}
				else {
					//	Check if iter is fully iterated 
					bool allComplete = true;
					for (int k = 0; k < iter.size(); k++) {
						if (!(iter.at(k).first == iter.at(k).second - 1)) {
							if (!(iter.at(k).first == 0 && iter.at(k).second == 1))
								allComplete = false;
						}
					}

					allVariationsFound = allComplete;
				}

				//	Escape loop if complete 
				if (allVariationsFound)
					break;
			}
		}
	}

	return generatedFunctions;
}

/* --- Deconstructor --- */

Assembler::~Assembler() {
	store.clear();
	functionHeader.clear();
}
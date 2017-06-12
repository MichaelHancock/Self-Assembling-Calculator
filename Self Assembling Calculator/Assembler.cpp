#include "Assembler.h"

/* --- Constructor --- */

Assembler::Assembler(LineStore &ls) {
	store = ls;
}

Assembler::Assembler(LineStore &ls, ListOfStrings functionHead) {
	store = ls;
	functionHeader = functionHead;
}

/* --- Public Member Functions --- */

ListOfStrings Assembler::makeFunctions() {
	std::vector<std::pair<int, int>> iter = primeIterator();
	ListOfStrings generatedFunctions = {};
	std::string variableToReturn = getVariableName(store.size());
	bool allVariationsFound = false;

	while (!allVariationsFound) {
		//	Test Line variation
		DanaLineSet testFunction = DanaLineSet();
		for (int i = 0; i < store.size(); i++)
			store.size() > i ? testFunction.insertLine(store.getList(i).at(iter.at(i).first)) : NULL;

		//	Compose lines of new function
		ListOfStrings newOutput = functionHeader;
		for (int i = 0; i < store.size(); i++)
			store.size() > i ? newOutput.push_back("\t\t" + store.getList(i).at(iter.at(i).first).composeLine()) : NULL;

		//	Add default function footer. Return variable created on the last line
		newOutput.push_back("\t\treturn " + variableToReturn);
		newOutput.push_back("\t}");
		newOutput.push_back("}");

		//	Collapse vector of lines into a string - append this to list of generatedFunctions
		testFunction.validateLines() ? generatedFunctions.push_back(collpaseVector(newOutput)) : NULL;
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

std::vector<DanaLineSet> Assembler::makeOnlyLines() {
	std::vector<std::pair<int, int>> iter = primeIterator();
	std::vector<DanaLineSet> generatedFunctions = {};
	std::string variableToReturn = getVariableName(store.size());
	bool allVariationsFound = false;

	while (!allVariationsFound) {
		//	Test Line variation
		DanaLineSet testFunction = DanaLineSet();
		for (int i = 0; i < store.size(); i++)
			store.size() > i ? testFunction.insertLine(store.getList(i).at(iter.at(i).first)) : NULL;

		//	If function is successful - add it to generatedFunctions
		testFunction.validateLines() ? generatedFunctions.push_back(testFunction) : NULL;

		//	Return early if only one variation is found on line 1 - and store is of size 1
		if (store.size() == 1 && store.getList(0).size() == 1) 
			return generatedFunctions;

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

/* --- Private Member Functions --- */

std::vector<std::pair<int, int>> Assembler::primeIterator() {
	//	Populate vector of pairs - used to iterate through function variations
	std::vector<std::pair<int, int>> iter = {};
	for (int i = 0; i < 6; i++)
		iter.push_back(std::make_pair(0, 0));
	for (int i = 0; i < store.size(); i++)
		iter.at(i).second = store.getList(i).size();

	return iter;
}

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

/* --- Deconstructor --- */

Assembler::~Assembler() {
	store.clear();
	functionHeader.clear();
}
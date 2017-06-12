#pragma once
#include "DanaVariable.h"
#include "DanaLineSet.h"
#include "LineStore.h"
#include <assert.h>   

typedef std::vector<std::string> ListOfStrings;
typedef std::vector<DanaLine> ListOfLines;

class Assembler {
private:
	LineStore store;
	ListOfStrings functionHeader;
	DanaVariable functionParams;
	std::vector<std::pair<int, int>> primeIterator();
	std::string Assembler::collpaseVector(ListOfStrings);
	std::string Assembler::getVariableName(int);

public:
	Assembler(LineStore&);
	Assembler(LineStore&, ListOfStrings);
	ListOfStrings Assembler::makeFunctions();
	std::vector<DanaLineSet> makeOnlyLines();
	~Assembler();
};
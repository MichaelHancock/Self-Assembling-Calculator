#pragma once
#include "LineStore.h"
#include <assert.h>   

typedef std::vector<std::string> ListOfStrings;

class Assembler {
private:
	LineStore store;
	ListOfStrings functionHeader;
	std::string Assembler::collpaseVector(ListOfStrings);
	std::string Assembler::getVariableName(int);
	ListOfStrings Assembler::output(std::vector<std::pair<int, int>>);

public:
	Assembler(LineStore, ListOfStrings);
	ListOfStrings Assembler::make();
	~Assembler();
};
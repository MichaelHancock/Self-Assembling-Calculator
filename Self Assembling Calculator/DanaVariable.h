#pragma once
#include <string>

/* --- Class to store type and name information for a Dana variable --- */

class DanaVariable {
public:
	std::string name;
	std::string type;
	int size;
	int value;

	DanaVariable() {
	}

	DanaVariable(std::string variableName, std::string variableType) {
		name = variableName;
		type = variableType;
		size = NULL;
		value = NULL;
	}

	DanaVariable(std::string variableName, std::string variableType, int variableSize) {
		name = variableName;
		type = variableType;
		value = NULL;

		if (variableSize > 0)
			size = variableSize;
	}

	bool DanaVariable::operator==(const DanaVariable& dv) {
		return name == dv.name && type == dv.type;
	}
};


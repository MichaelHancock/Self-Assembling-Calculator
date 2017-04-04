#pragma once
#include <string>

/* --- Class to store type and name information for a Dana variable --- */

class DanaVariable {
public:
	std::string name;
	std::string type;
	int size;

	DanaVariable() {
	}

	DanaVariable(std::string variableName, std::string variableType) {
		name = variableName;
		type = variableType;
		size = NULL;
	}

	DanaVariable(std::string variableName, std::string variableType, int variableSize) {
		name = variableName;
		type = variableType;

		if (variableSize > 0)
			size = variableSize;
	}
};


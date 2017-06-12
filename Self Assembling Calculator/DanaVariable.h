#pragma once
#include <string>

/* --- Class to store type and name information for a Dana variable --- */

class DanaVariable {
private:
	bool array;
	int arraySize;
	std::string value = "";

public:
	std::string name;
	std::string type;

	DanaVariable() {}

	DanaVariable(std::string variableName, std::string variableType) {
		name = variableName;
		type = variableType;
		array = false;
		arraySize = NULL;
	}

	DanaVariable(std::string variableName, std::string variableType, std::string storedValue) {
		name = variableName;
		type = variableType;
		array = false;
		arraySize = NULL;
		value = storedValue;
	}

	DanaVariable(std::string variableName, std::string variableType, int sizeOfArray) {
		name = variableName;
		type = variableType;
		arraySize = sizeOfArray;
		array = true;
	}

	void setValue(std::string newValue) {
		value = newValue;
	}

	std::string getValue() {
		return value;
	}

	std::string composeVariable() {
		if (array)
			return type + " " + name + " [] ";

		return type + " " + name;
	}

	int size() {
		return arraySize;
	}

	bool isArray() {
		return array;
	}

	bool DanaVariable::operator==(const DanaVariable& dv) {
		return name == dv.name && type == dv.type;
	}
};


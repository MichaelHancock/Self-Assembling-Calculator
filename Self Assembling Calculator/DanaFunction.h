#pragma once
#include <string>
#include <vector>

/* --- Class to store return type, name and parameter information for a Dana function --- */

class DanaFunction {
private:
	std::vector<std::string> parameters;

public:
	std::string name;
	std::string returnType;
	bool isArrayAssign;
	DanaVariable functionObject;

	DanaFunction(std::string functionName, std::string functionReturnType, std::vector<std::string> listOfParamTypes) {
		name = functionName;
		returnType = functionReturnType;
		parameters = listOfParamTypes;
		isArrayAssign = false;
	}

	DanaFunction(DanaVariable danaArray) {
		parameters = { "int" };
		isArrayAssign = true;
		functionObject = danaArray;

		if (danaArray.type == "string")
			returnType = "char";
		else
			returnType = danaArray.type;
	}

	std::string getParameter(int index) {
		return parameters.at(index);
	}

	int numberOfParameters() {
		return parameters.size();
	}
};


#pragma once
#include <exception>
#include <string>
#include "DanaFunction.h"
#include "DanaVariable.h"

class DanaLine {
private:
	int lineNumber;
	DanaVariable declaredVariable;
	DanaFunction functionCalled;

public:
	DanaLine() {}

	DanaLine(int line, DanaVariable newVariable, DanaFunction newFunction) {
		if (newFunction.returnType != newVariable.type) {
			std::string errorMessage = "Type mismatch between provided variable type '" + newVariable.type;
			errorMessage += "' and " + newFunction.name;
			errorMessage += "'s return type '" + newFunction.returnType;
			errorMessage += "'";

			throw std::exception(errorMessage.c_str());
		}

		lineNumber = line;
		declaredVariable = newVariable;
		functionCalled = newFunction;
	}

	int getLineNumber() {
		return lineNumber;
	}

	DanaVariable getDeclaredVariable() {
		return declaredVariable;
	}

	DanaFunction getFunctionCalled() {
		return functionCalled;
	}

	std::vector<DanaVariable> getFunctionParameters() {
		std::vector<DanaVariable> result;

		for (int i = 0; i < functionCalled.numberOfParameters(); i++) 
			result.push_back(functionCalled.getMatchedParameter(i));

		return result;
	}

	std::string composeLine() {
		return declaredVariable.composeVariable() + " = " + functionCalled.composeFunctionCall();
	}
};


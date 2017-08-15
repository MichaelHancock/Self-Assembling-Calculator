#pragma once
#include <exception>
#include <string>
#include "DanaFunction.h"
#include "DanaVariable.h"

/* --- Class to store declared variable and function called for a Dana line --- */

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

	int getLineNumber() const {
		return lineNumber;
	}

	void setLineNumber(int newNumber) {
		lineNumber = newNumber;
	}

	DanaVariable getDeclaredVariable() const {
		return declaredVariable;
	}

	DanaFunction getFunctionCalled() const {
		return functionCalled;
	}

	std::vector<DanaVariable> getFunctionParameters() const {
		std::vector<DanaVariable> result;

		for (int i = 0; i < functionCalled.numberOfParameters(); i++) 
			result.push_back(functionCalled.getMatchedParameter(i));

		return result;
	}

	std::string composeLine() const {
		return getDeclaredVariable().composeVariable() + " = " + functionCalled.composeFunctionCall();
	}

	bool operator==(const DanaLine& dl) const {
		return dl.composeLine() == composeLine();
	}
};


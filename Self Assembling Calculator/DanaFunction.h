#pragma once
#include <exception>
#include <string>
#include <vector>
#include "DanaVariable.h"

/* --- Class to store return type, name and parameter information for a Dana function --- */

class DanaFunction {
private:
	std::vector<std::string> requiredParams;
	std::vector<DanaVariable> matchedParams;
	bool arrayAssign;

public:
	std::string name;
	std::string returnType;

	DanaFunction() {}

	DanaFunction(std::string functionName, std::string functionReturnType, std::vector<std::string> listOfParamTypes) {
		name = functionName;
		returnType = functionReturnType;
		requiredParams = listOfParamTypes;
		arrayAssign = false;
		
		for (auto i : requiredParams)
			matchedParams.push_back(DanaVariable());
	}

	DanaFunction(DanaVariable danaArray) {
		requiredParams = { "int" };
		matchedParams = { DanaVariable() };
		arrayAssign = true;

		if (danaArray.type == "string")
			returnType = "char";
		else
			returnType = danaArray.type;
	}

	DanaVariable getMatchedParameter(int index) {
		return matchedParams.at(index);
	}

	std::string getParameterType(int index) {
		return requiredParams.at(index);
	}

	void setMatchedParam(DanaVariable param, int index) {
		if (param.type == getParameterType(index))
			matchedParams.at(index) = param;
		else {
			std::string errorMessage = "Type mismatch between provided param type '" + param.type;
			errorMessage += "' and " + name;
			errorMessage += "'s parameter '" + getParameterType(index);
			errorMessage += "'";

			throw std::exception(errorMessage.c_str());
		}
	}

	void clearMatchedParams() {
		for (auto& i : matchedParams) 
			i = DanaVariable();
	}

	std::string composeFunctionCall() {
		if (isArrayAssign()) {
			std::string paramValue = matchedParams.at(0).getValue();
			std::string outVal = paramValue.size() > 0 ? paramValue : matchedParams.at(0).name;
			return "[ " + outVal + " ]";
		}

		std::string call = name + " ( ";
		for (int i = 0; i < numberOfParameters(); i++) {
			if (i != 0)
				call += ", ";

			call += getMatchedParameter(i).name;
		}

		return call += " )";
	}

	int numberOfParameters() {
		return requiredParams.size();
	}

	bool isArrayAssign() {
		return arrayAssign;
	}

	bool operator==(const DanaFunction& df) {
		return (name == df.name && returnType == df.returnType && arrayAssign == df.arrayAssign);
	}
};
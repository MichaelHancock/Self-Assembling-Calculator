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
	std::string assignmentModifier;
	DanaVariable functionObject;
	bool arrayAssign;
	bool infix;

public:
	std::string name;
	std::string returnType;

	DanaFunction() {}

	DanaFunction(std::string functionName, std::string functionReturnType, std::vector<std::string> listOfParamTypes) {
		name = functionName;
		returnType = functionReturnType;
		requiredParams = listOfParamTypes;
		arrayAssign = false;
		infix = false;

		for (auto i : requiredParams)
			matchedParams.push_back(DanaVariable());
	}

	DanaFunction(std::string functionName, std::string functionReturnType, std::vector<std::string> listOfParamTypes, bool isInfix) {
		name = functionName;
		returnType = functionReturnType;
		requiredParams = listOfParamTypes;
		arrayAssign = false;
		infix = isInfix;

		if (infix && requiredParams.size() != 2) {
			std::string errorMessage = "Infix function has the wrong number of required parameters: ";
			errorMessage += std::to_string(requiredParams.size());

			throw std::exception(errorMessage.c_str());
		}
	}

	DanaFunction(DanaVariable danaArray) {
		requiredParams = { "int" };
		matchedParams = { DanaVariable() };
		arrayAssign = true;
		functionObject = danaArray;

		if (danaArray.type == "string")
			returnType = "char";
		else
			returnType = danaArray.type;
	}

	DanaFunction(DanaVariable danaArray, std::string modifier){
		requiredParams = { "int" };
		matchedParams = { DanaVariable() };
		arrayAssign = true;
		assignmentModifier = "." + modifier;
		returnType = danaArray.type;
		functionObject = danaArray;
	}

	DanaVariable getMatchedParameter(int index) {
		return matchedParams.at(index);
	}

	std::string getParameterType(int index) {
		return requiredParams.at(index);
	}

	DanaVariable getFunctionObject() {
		return functionObject;
	}

	void setMatchedParam(DanaVariable param, int index) {
		if (param.type == getParameterType(index)) {
			if (matchedParams.size() <= index || matchedParams.size() == 0) 
				matchedParams.push_back(param);
			else 
				matchedParams.at(index) = param;
		}
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
			return functionObject.name + " [ " + outVal + " ]" + assignmentModifier;
		}
		else if (isInfix()) {
			std::string call = matchedParams.at(0).name + " " + name + " " + matchedParams.at(1).name;
			return call;
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
		return (int)requiredParams.size();
	}

	bool isInfix() {
		return infix;
	}

	bool isArrayAssign() {
		return arrayAssign;
	}

	bool operator==(const DanaFunction& df) {
		return (name == df.name && returnType == df.returnType && arrayAssign == df.arrayAssign);
	}

	~DanaFunction() {
		matchedParams.clear();
		requiredParams.clear();
	}
};
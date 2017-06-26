#pragma once
#include <exception>
#include <vector>
#include "DanaLine.h"
#include "DanaFunction.h"
#include "DanaVariable.h"

class DanaLineSet {
private:
	std::vector<DanaLine> lines;

public:
	DanaLineSet() {}

	int numberOfLines() {
		return (int)lines.size();
	}

	void insertLine(DanaLine newLine) {
		const int lineIndex = newLine.getLineNumber();

		if (lineIndex < 0 || lineIndex > numberOfLines()) {
			const std::string errorMessage = "Invalid line number " + lineIndex;
			throw std::exception(errorMessage.c_str());
		}

		if (lineIndex == numberOfLines()) {
			lines.push_back(newLine);
			return;
		}

		lines.at(lineIndex) = newLine;
	}

	void deleteLine(int index) {
		lines.erase(lines.begin() + index);
		for (int i = index; i < lines.size(); i++) 
			lines.at(i).setLineNumber(lines.at(i).getLineNumber() - 1);
	}

	void safeDelete(std::vector<int> linesToDelete) {
		std::vector<int> nextLinesToDelete;

		for (auto index : linesToDelete) {
			//	Get line (and declared var) to delete from vector of lines
			DanaLine toDelete;
			DanaVariable varToDelete;
			for (auto checkLine : lines) {
				if (checkLine.getLineNumber() == index) {
					toDelete = checkLine;
					varToDelete = checkLine.getDeclaredVariable();
				}
			}			

			//	Check for dependencies on other lines
			std::vector<int> usages = findVariableUsages(varToDelete);
			for (auto thisUsage : usages) {
				if (!(std::find(nextLinesToDelete.begin(), nextLinesToDelete.end(),
					thisUsage) != nextLinesToDelete.end())) {
					nextLinesToDelete.push_back(thisUsage);
				}
			}

			//	Remove line
			lines.erase(std::remove(lines.begin(), lines.end(), toDelete), lines.end());
		}

		if (nextLinesToDelete.size() == 0) {
			//	Reset line numbers after delete
			for (int i = 0; i < lines.size(); i++) 
				lines.at(i).setLineNumber(i);

			return;
		}
		
		//	There are more lines to delete - make recurisve call
		safeDelete(nextLinesToDelete);
	}

	DanaLine getLine(int lineNumber) {
		if (lineNumber < 0 || lineNumber > (numberOfLines() - 1)) {
			const std::string errorMessage = "Invalid line number " + lineNumber;
			throw std::exception(errorMessage.c_str());
		}

		return lines.at(lineNumber);
	}

	std::vector<DanaVariable> getAllVariables() {
		std::vector<DanaVariable> result;

		for (auto i : lines)
			result.push_back(i.getDeclaredVariable());

		return result;
	}

	std::vector<DanaVariable> variablesInScope(int line) {
		std::vector<DanaVariable> result = {};

		for (int i = 0; i < line; i++) 
			result.push_back(lines.at(i).getDeclaredVariable());
		
		return result;
	}

	std::vector<int> findVariableUsages(DanaVariable var) {
		std::vector<int> result = {};

		for (auto i : lines) {
			const std::vector<DanaVariable> paramsUsedAtLine = i.getFunctionParameters();
			bool varUsedAtThisLine = false;
			DanaFunction funcAtThisLine = i.getFunctionCalled();

			if (funcAtThisLine.isArrayAssign() && funcAtThisLine.getFunctionObject() == var) {
				result.push_back(i.getLineNumber());
				continue;
			}

			for (auto j : paramsUsedAtLine) {
				if (j.name == var.name) 
					varUsedAtThisLine = true;
			}

			if (varUsedAtThisLine)
				result.push_back(i.getLineNumber());
		}

		return result;
	}

	std::vector<int> findFunctionInstances(DanaFunction func) {
		std::vector<int> result = {};

		for (auto i : lines) {
			DanaFunction functionUsedAtLine = i.getFunctionCalled();
	
			if (functionUsedAtLine == func)
				result.push_back(i.getLineNumber());
		}

		return result;
	}

	bool validateLines() {
		bool linesAreValid = true;
		std::vector<DanaVariable> allVariables = getAllVariables();

		for (int i = 0; i < numberOfLines(); i++) {

			std::vector<DanaVariable> currentlyInScope = variablesInScope(i);
			std::vector<DanaVariable> parametersUsed = lines.at(i).getFunctionParameters();
			DanaVariable declaredOnThisLine = lines.at(i).getDeclaredVariable();

			if (std::count(allVariables.begin(), allVariables.end(), declaredOnThisLine) > 1)
				linesAreValid = false;

			for (auto j : parametersUsed) {
				if (!(std::find(currentlyInScope.begin(), currentlyInScope.end(), j) 
					!= currentlyInScope.end()) && j.getValue() == "") {
					linesAreValid = false;
				}
			}
		}

		return linesAreValid;
	}
	
	std::string composeLines() {
		std::string result;

		for (auto i : lines)
			result = result + i.composeLine() + "\n";

		return result;
	}
};


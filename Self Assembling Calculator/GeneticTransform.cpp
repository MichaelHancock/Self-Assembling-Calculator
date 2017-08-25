#include "GeneticTransform.h"

/* - Constructor - */

GeneticTransform::GeneticTransform(std::vector<DanaLineSet> &initialPopulation, std::vector<DanaFunction> &newFunctions, 
	std::string requires, std::string cmdInput, std::string cmdTarget) : input(cmdInput), target(cmdTarget) {
	srand((unsigned int)time(0));
	knownFunctions = newFunctions;
	for (auto i : initialPopulation)
		populaton.push_back(std::make_pair(i, 0));

	//	Check if initial population includes an array
	bool includedArrayAssign = false;
	for (auto func : initialPopulation) {
		std::vector<DanaVariable> declared = func.getAllVariables();

		for (auto var : declared) {
			
			//	Add array assign to knownFunctions if initial population includes an array variable
			if (var.isArray() && !includedArrayAssign) {
				includedArrayAssign = true;
				knownFunctions.push_back(DanaFunction(var));
			}
		}
	}

	//	Add infix operators to functions
	knownFunctions.push_back(DanaFunction("+", "int", { "int", "int" }, true));

	//	Compose function header - including required functions
	functionHeader.push_back("component provides App requires" + requires + " {");
	functionHeader.push_back("\tint App:main(AppParam params[]) {");
}

/* - Private member functions - */

int GeneticTransform::getRandomNumber(int low, int high) {
	if (low == high || high + low == 0)
		return low;

	high = (high - low) + 1;
	return rand() % high + low;
}

std::string GeneticTransform::getVariableName(int lineNumber) {
	assert(lineNumber >= 1 && lineNumber <= 26);
	return std::string(1, "abcdefghijklmnopqrstuvwxyz"[lineNumber - 1]);
}

std::vector<DanaLine> GeneticTransform::getAllLineVariations(int lineNumber, const std::vector<DanaVariable> &knownVariables) {
	const std::vector<DanaVariable> vars = knownVariables;
	std::vector<DanaLine> potentialLines = {};
	std::vector<DanaFunction> validFunctions = {};

	//	Look for functions whos parameters we can match
	for (auto thisFunc : knownFunctions) {

		//	If array in availableVariables, add array assign function to validFunctions
		if (thisFunc.isArrayAssign()) {
			bool arrayFound = false;

			for (auto thisVar : vars) {
				if (thisVar.isArray())
					arrayFound = true;
			}

			if (arrayFound) {
				validFunctions.push_back(thisFunc);
				continue;
			}
		}

		//	Iterate through function params - searching for matches in vars 
		int numberOfParamsMatched = 0;
		for (int i = 0; i < thisFunc.numberOfParameters(); i++) {
			const std::string currentParamType = thisFunc.getParameterType(i);

			for (auto thisVar : vars) {
				if (thisVar.type == currentParamType && !thisVar.isArray()) {
					numberOfParamsMatched++;
					break;
				}
			}
		}

		//	If matching vars are found, add to validFunctions
		if (numberOfParamsMatched == thisFunc.numberOfParameters())
			validFunctions.push_back(thisFunc);
	}

	//	Find all parameter variations for validFunctions 
	for (auto thisFunc : validFunctions) {
		if (thisFunc.isArrayAssign()) {
			DanaVariable arr;

			//	Get array from vars
			for (auto thisVar : vars) {
				if (thisVar.isArray())
					arr = thisVar;
			}

			//	Generate a new potential line foreach possible array index
			for (int i = 0; i < arr.size() - 1; i++) {
				thisFunc.setMatchedParam(DanaVariable("x", "int", std::to_string(i)), 0);
				const DanaLine newLine = DanaLine(lineNumber - 1,
					DanaVariable(getVariableName(lineNumber), arr.type), thisFunc);
				potentialLines.push_back(newLine);
			}
		}
		else if (thisFunc.numberOfParameters() == 1) {
			const std::string param = thisFunc.getParameterType(0);
			std::vector<DanaVariable> filteredVariables = vars;

			//	Filter variables which cannot be used to populate this functions parameters
			filteredVariables.erase(std::remove_if(filteredVariables.begin(), filteredVariables.end(),
				[param](DanaVariable v) { return v.type != param || v.isArray(); }), filteredVariables.end());

			for (auto var : filteredVariables) {
				thisFunc.setMatchedParam(var, 0);
				const DanaLine newLine = DanaLine(lineNumber - 1,
					DanaVariable(getVariableName(lineNumber), thisFunc.returnType), thisFunc);
				potentialLines.push_back(newLine);
			}
		}
		else if (thisFunc.numberOfParameters() == 2 && thisFunc.getParameterType(0) == thisFunc.getParameterType(1)) {
			const std::string param = thisFunc.getParameterType(0);
			std::vector<DanaVariable> filteredVariables = vars;

			//	Filter variables which cannot be used to populate this functions parameters
			filteredVariables.erase(std::remove_if(filteredVariables.begin(), filteredVariables.end(),
				[param](DanaVariable v) { return v.type != param; }), filteredVariables.end());

			for (auto var1 : filteredVariables) {
				for (auto var2 : filteredVariables) {
					thisFunc.setMatchedParam(var1, 0);
					thisFunc.setMatchedParam(var2, 1);
					const DanaLine newLine = DanaLine(lineNumber - 1,
						DanaVariable(getVariableName(lineNumber), thisFunc.returnType), thisFunc);
					potentialLines.push_back(newLine);
				}
			}
		}
	}

	return potentialLines;
}

DanaLineSet GeneticTransform::correctVariableNames(DanaLineSet lnSet) {
	DanaLineSet newLineSet = lnSet;

	for (int i = 0; i < newLineSet.numberOfLines(); i++) {
		//	Store information about this current line
		DanaLine currentLine = newLineSet.getLine(i);
		const DanaVariable currentLineVar = currentLine.getDeclaredVariable();
		const DanaFunction currrentLineFunc = currentLine.getFunctionCalled();
		const std::string correctNameForThisLine = getVariableName(i + 1);
		const std::string currentName = currentLineVar.name;

		//	Check if var declared on this line has the right name
		if (correctNameForThisLine == currentName)
			continue;

		//	If not - replace the declaration on this line
		DanaVariable replaceLineVar = currentLineVar;
		replaceLineVar.name = correctNameForThisLine;
		newLineSet.insertLine(DanaLine(i, replaceLineVar, currrentLineFunc));
		
		//	If not - find and replace all instances
		for (int j = i; j < newLineSet.numberOfLines(); j++) {
			const DanaLine checkLine = newLineSet.getLine(j);
			const DanaVariable checkVar = checkLine.getDeclaredVariable();
			const DanaFunction checkFunc = checkLine.getFunctionCalled();

			//	Rename variable usage instances - for array assign function calls
			if (checkFunc.isArrayAssign()) {
				DanaVariable arr = checkFunc.getFunctionObject();

				if (arr.name == currentName) {
					arr.name = correctNameForThisLine;
					DanaFunction replaceFunc = DanaFunction(arr);
					replaceFunc.setMatchedParam(checkFunc.getMatchedParameter(0), 0);
					newLineSet.insertLine(DanaLine(j, checkVar, replaceFunc));
				}
			}

			//	Rename variable usage instances - for use as parameter in a functtion call
			if (!checkFunc.isArrayAssign()) {
				std::vector<DanaVariable> params;
				DanaFunction replaceFunc = checkFunc;
				for (int k = 0; k < checkFunc.numberOfParameters(); k++)
					params.push_back(checkFunc.getMatchedParameter(k));

				for (int k = 0; k < params.size(); k++) {
					if (params.at(k).name == currentName) {
						DanaVariable replaceVar = params.at(k);
						replaceVar.name = correctNameForThisLine;
						replaceFunc.setMatchedParam(replaceVar, k);
					}
				}

				newLineSet.insertLine(DanaLine(j, checkVar, replaceFunc));
			}
		}
	}

	return newLineSet;
}

std::string GeneticTransform::runSystemCommand(std::string command) {
	//	Execute system command and return result string
	const std::shared_ptr<FILE> pipe(_popen(command.c_str(), "r"), _pclose);
	std::array<char, 128> buffer;
	std::string result;

	//	Check for early command fail
	if (!pipe)
		throw std::runtime_error("popen() failed!");

	//	Read data from pipe
	while (!feof(pipe.get())) {
		if (fgets(buffer.data(), 128, pipe.get()) != NULL)
			result = result + buffer.data();
	}

	return result;
}

bool GeneticTransform::compileFunction(std::string path) {
	//	Try to compile new function
	const std::string command = "dnc " + path;
	const int response = system(command.c_str());

	return response == 0;
}

bool GeneticTransform::testFunction(std::string path, std::string test, std::string expectedResult) {
	//	Test new function and compare with target
	const std::string inputWithFormatting = "\"" + test + "\"";
	const std::string command = "dana " + path + " " + inputWithFormatting;
	const int response = system(command.c_str());

	return response == std::stoi(expectedResult);
}

void GeneticTransform::writeDataFile(std::string toWrite, std::string path) {
	//	Output generated file
	std::ofstream generatedFile(path);
	generatedFile << toWrite;
	generatedFile.close();
}

std::string inline GeneticTransform::funcToString(DanaLineSet lnSet, std::string varToReturn) {
	const DanaLineSet currentFunction = lnSet;
	const int numberOfLines = currentFunction.numberOfLines();

	//	Compose function
	std::vector<std::string> outFunction = functionHeader;
	for (int i = 0; i < numberOfLines; i++) {
		const DanaLine currentLine = currentFunction.getLine(i);
		const DanaVariable declaredVar = currentLine.getDeclaredVariable();
		const DanaFunction calledFunction = currentLine.getFunctionCalled();

		outFunction.push_back("\t\t" + currentLine.composeLine());
	}

	//	Add function footer
	outFunction.push_back("\t\treturn " + varToReturn);
	outFunction.push_back("\t}");
	outFunction.push_back("}");

	//	Collapse vector into string, add lines spaces
	std::string result = "";
	for (int i = 0; i < outFunction.size(); i++) 
		result += outFunction.at(i) + "\n";

	return result;
}

DanaVariable GeneticTransform::findReturnVariable(DanaLineSet lnSet) {
	const DanaLineSet currentFunction = lnSet;
	const int numberOfLines = currentFunction.numberOfLines();

	//	Look for the last defined integer to return
	DanaVariable varToReturn = currentFunction.getLine(numberOfLines - 1).getDeclaredVariable();
	const std::vector<DanaVariable> vars = currentFunction.getAllVariables();
	for (int j = (int)vars.size() - 1; j > -1; j--) {
		if (vars.at(j).type == "int") {
			varToReturn = vars.at(j);
			break;
		}
	}

	return varToReturn;
}

/* - Public member functions - */

DanaLineSet GeneticTransform::insertLine(DanaLineSet lnSet) {
	//	Get the variables of lnSet and select a newLineNumber for the line to be inserted
	DanaLineSet newLineSet = lnSet;
	const std::vector<DanaVariable> scopedVars = newLineSet.getAllVariables();
	const int newLineNumber = newLineSet.numberOfLines() + 1;

	//	Generate all possible new lines - select one randomly to add to the lineset
	std::vector<DanaLine> possibleLines = getAllLineVariations(newLineNumber, scopedVars);
	const int randomIndex = getRandomNumber(0, (int)(possibleLines.size() - 1));
	newLineSet.insertLine(possibleLines.at(randomIndex));

	return newLineSet;
}

DanaLineSet GeneticTransform::modifyLine(DanaLineSet lnSet) {
	if (lnSet.numberOfLines() < 2)
		return lnSet;

	//	Select a random line to modify
	const int randomIndex = getRandomNumber(1, lnSet.numberOfLines() - 1);
	DanaLineSet newLineSet = lnSet;
	DanaLine lineToModify = newLineSet.getLine(randomIndex);
	const int lineNumber = lineToModify.getLineNumber();
	
	//	Get function called on this line
	DanaFunction functionCalled = lineToModify.getFunctionCalled();
	std::vector<std::string> paramTypes;
	for (int i = 0; i < functionCalled.numberOfParameters(); i++) 
		paramTypes.push_back(functionCalled.getParameterType(i));

	//	Change index if array assign
	if (functionCalled.isArrayAssign()) {
		const DanaVariable declaredVariable = lineToModify.getDeclaredVariable();
		const DanaVariable arrayInCall = functionCalled.getFunctionObject();
		DanaVariable index = functionCalled.getMatchedParameter(0);
		index.setValue(std::to_string(getRandomNumber(0, arrayInCall.size() - 1)));
		functionCalled.setMatchedParam(index, 0);

		newLineSet.insertLine(DanaLine(lineNumber, declaredVariable, functionCalled));
		return newLineSet;
	}

	//	Change params for other inscope variables
	const std::vector<DanaVariable> inscope = newLineSet.variablesInScope(lineToModify.getLineNumber());
	for (int i = 0; i < paramTypes.size(); i++) {
		const std::string param = paramTypes.at(i);
		std::vector<DanaVariable> filteredParams = inscope;
		filteredParams.erase(std::remove_if(filteredParams.begin(), filteredParams.end(),
			[param](DanaVariable v) { return v.type != param; }), filteredParams.end());

		functionCalled.setMatchedParam(filteredParams.at(getRandomNumber(0, (int)filteredParams.size() - 1)), i);
	}

	newLineSet.insertLine(DanaLine(lineNumber, lineToModify.getDeclaredVariable(), functionCalled));
	return newLineSet;
}

DanaLineSet GeneticTransform::deleteLine(DanaLineSet lnSet) {
	if (lnSet.numberOfLines() == 0)
		return lnSet;
	
	//	Randomly select a line to delete
	DanaLineSet newLineSet = lnSet;
	const int deleteIndex = getRandomNumber(0, (int)(lnSet.numberOfLines() - 1));

	if (deleteIndex == 0)
		return lnSet;

	const DanaLine lineToDelete = newLineSet.getLine(deleteIndex);
	const DanaVariable variableToRemove = lineToDelete.getDeclaredVariable();
	std::vector<int> usagesOfVariable = newLineSet.findVariableUsages(variableToRemove);
	
	//	If var declared on the deleted line is used nowhere else 
	if (usagesOfVariable.size() == 0) {
		newLineSet.deleteLine(deleteIndex);
		newLineSet = correctVariableNames(newLineSet);
		return newLineSet;
	}

	//	If var declared on the deleted line is used elsewhere - do safe delete
	newLineSet.safeDelete({ deleteIndex });
	newLineSet = correctVariableNames(newLineSet);
	return newLineSet;
}

DanaLineSet GeneticTransform::crossover(const DanaLineSet &one, const DanaLineSet &other) {
	//	Store passed class info
	DanaLineSet current = DanaLineSet(one);
	const DanaLineSet passedLineSet = DanaLineSet(other);

	//	If there are to few lines for a crossover - do an early return and force GA to insert a line
	if (passedLineSet.numberOfLines() < 2 || current.numberOfLines() < 2) 
		return insertLine(current);
	
	//	Select a random line from the other lineset
	const int passedNumberOfLines = passedLineSet.numberOfLines();
	const int randomPassedLineIndex = getRandomNumber(1, passedNumberOfLines - 1);
	const DanaLine passedLine = passedLineSet.getLine(randomPassedLineIndex);

	const DanaFunction calledFunc = passedLine.getFunctionCalled();
	const DanaVariable declaredVar = passedLine.getDeclaredVariable();

	//	If is array assign - copy index that other lineset used
	if (calledFunc.isArrayAssign()) {
		const DanaVariable index = calledFunc.getMatchedParameter(0);

		//	Find the last array assign in current - replace the index used 
		for (int i = current.numberOfLines() - 1; i > -1; i--) {
			if (current.getLine(i).getFunctionCalled().isArrayAssign() && i > 0) {
				DanaFunction replaceFunc = DanaFunction(current.getLine(i).getFunctionCalled().getFunctionObject());
				replaceFunc.setMatchedParam(index, 0);
				current.insertLine(DanaLine(i, current.getLine(i).getDeclaredVariable(), replaceFunc));
				return current;
			}
		}

		//	If no array assign is found - return current
		return current;
	}

	return current;
}

DanaLineSet GeneticTransform::removeUnnecessaryLines(DanaLineSet lnSet, DanaVariable returnVar, bool repeatCheck) {
	DanaLineSet newLineSet = lnSet;
	const DanaVariable varToReturn = returnVar;

	//	Look for lines whom declare vars that are used nowhere else
	for (int i = 0; i < newLineSet.numberOfLines(); i++) {
		const DanaLine currentLine = newLineSet.getLine(i);
		const DanaVariable declaredVar = currentLine.getDeclaredVariable();
		const int varUsages = (int)newLineSet.findVariableUsages(declaredVar).size();

		//	If var is used nowhere else and is not the return var - Delete the line
		if (varUsages == 0 && declaredVar.name != varToReturn.name) {
			newLineSet.safeDelete({ i });
			i--;
		}
	}

	if (repeatCheck)
		newLineSet = removeUnnecessaryLines(newLineSet, varToReturn, false);

	return correctVariableNames(newLineSet);
}

void GeneticTransform::test() {	
	for (int i = 0; i < populaton.size(); i++) {
		//	Store current function and set score to 0 initially 
		const DanaLineSet currentFunction = populaton.at(i).first;
		double score = 0;
		bool returnInt = false;

		const int numberOfLines = currentFunction.numberOfLines();
		if (numberOfLines == 0) {
			populaton.at(i).second = 0;
			continue;
		}

		//	Does the function return an integer
		if (currentFunction.getLine(currentFunction.numberOfLines() - 1).getDeclaredVariable().type == "int") {
			returnInt = true;
			score++;
		}

		//	How many lines does the function have
		const double distance = abs(targetComponentLength - numberOfLines);
		score = distance == 0 ? score += 2 : score += ((double)1 / distance);

		//	How many functions does the function use
		const int numberOfFunctions = (int)knownFunctions.size();
		std::vector<std::string> usedFunctions;
		for (int j = 0; j < numberOfLines; j++) {
			const DanaFunction funcUsedOnThisLine = currentFunction.getLine(j).getFunctionCalled();
			if (std::find(usedFunctions.begin(), usedFunctions.end(),
				funcUsedOnThisLine.name + std::to_string(funcUsedOnThisLine.isArrayAssign())) == usedFunctions.end()) {
				usedFunctions.push_back(funcUsedOnThisLine.name + std::to_string(funcUsedOnThisLine.isArrayAssign()));
			}
		}
		score += (double)usedFunctions.size() / (double)numberOfFunctions;

		//	if function does not return an int, there is no point testing it - so skip the following stages of testing
		if (!returnInt) {
			populaton.at(i).second = score;
			continue;
		}

		//	Find the variable to return from the function
		const DanaVariable varToReturn = findReturnVariable(currentFunction);

		//	Compile and test function
		const std::string function = funcToString(currentFunction, varToReturn.name);
		writeDataFile(function, outputPath);
		if (compileFunction(outputPath)) {
			if (testFunction(compilePath, input, target)) {
				score += 5; 
				resultFunction = populaton.at(i);
			}
		}

		//	Set score to the population func / score pair
		populaton.at(i).second = score;
	}
}

void GeneticTransform::rank() {
	//	Use selection sort to rank population
	for (int i = 0; i < populaton.size() - 1; i++) {
		for (int j = i + 1; j < populaton.size(); j++) {
			if (populaton[i].second > populaton[j].second) {
				std::swap(populaton[i], populaton[j]);
			}
		}
	}
}

std::string GeneticTransform::cycleGeneration() {
	int totalNumberOfGenerations = 0;
	int totalNumberOfInserts = 0;
	int totalNumberOfModifies = 0;
	int totalNumberOfDeletes = 0;

	//	Loop until a result function is found
	while (resultFunction.first.numberOfLines() == 0) {
		if (totalNumberOfGenerations > 0) {
			test();
			rank();
			
			//	Crossover the top line sets with the bottom ones
			const int topIndex = (int)populaton.size() - 1;
			for (int i = 0; i < populaton.size() / 2; i++) 
				populaton.at(i).first = crossover(populaton.at(i).first, populaton.at(topIndex - i).first);
		}
		
		//	For each member of the population - perform a random muation
		for (int i = 0; i < populaton.size(); i++) {
			const int whichMutation = getRandomNumber(0, 2);
			switch (getRandomNumber(0, 2)) {
			case 0:
				totalNumberOfInserts++;
				populaton.at(i).first.numberOfInserts++;
				populaton.at(i).first = insertLine(populaton.at(i).first);
				break;
			case 1:
				totalNumberOfDeletes++;
				populaton.at(i).first.numberOfDeletes++;
				populaton.at(i).first = deleteLine(populaton.at(i).first);
				break;
			default:
				totalNumberOfModifies++;
				populaton.at(i).first.numberOfModifies++;
				populaton.at(i).first = modifyLine(populaton.at(i).first);
				break;
			}
		}

		totalNumberOfGenerations++;
	}

	//	Prepare function for output
	const DanaLineSet prunedFunc = removeUnnecessaryLines(resultFunction.first, findReturnVariable(resultFunction.first), true);
	const std::string func = funcToString(prunedFunc, findReturnVariable(prunedFunc).name);
	const int lengthBeforePrune = resultFunction.first.numberOfLines();

	//	Write result function to file so it can be tested by the client 
	writeDataFile(func, outputPath);
	compileFunction(outputPath);

	//	Compose stats
	stats["Length Of Result Component"] = lengthBeforePrune;
	stats["Number Of Generations"] = totalNumberOfGenerations;
	stats["Number Of Inserts"] = totalNumberOfInserts;
	stats["Number Of Deletes"] = totalNumberOfDeletes;
	stats["Number Of Modifies"] = totalNumberOfModifies;
	stats["Result Number Of Inserts"] = resultFunction.first.numberOfInserts;
	stats["Result Number Of Modifies"] = resultFunction.first.numberOfModifies;
	stats["Result Number Of Deletes"] = resultFunction.first.numberOfDeletes;

	return func;
}

std::vector<std::pair<DanaLineSet, double>> GeneticTransform::getPopulation() {
	return populaton;
} 

std::map<std::string, double> GeneticTransform::getLastCycleStats() {
	return stats;
}

void GeneticTransform::tuneFitnessFunction(const std::string aspect, double value) {
	if (aspect == "LENGTH") {
		targetComponentLength = value;
	}
}

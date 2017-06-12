#pragma once
#include <vector>
#include "DanaLine.h"

class LineStore {
private:
	std::vector<std::vector<DanaLine>> store;

public:
	LineStore() {}

	LineStore(std::vector<DanaLine> initialLines) {
		store.push_back(initialLines);
		initialLines.clear();
	}

	std::vector<DanaLine> getList(int index) {
		return store.at(index);
	}

	DanaLine getLine(int lineIndex, int variationIndex) {
		return store.at(lineIndex).at(variationIndex);
	}

	void addLines(std::vector<DanaLine> lines) {
		store.push_back(lines);
	}

	void clear() {
		for (auto i : store)
			i.clear();

		store.clear();
	}

	int size() {
		return store.size();
	}

	~LineStore() {
		clear();
	}
};


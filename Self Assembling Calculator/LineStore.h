#pragma once
#include <string>
#include <vector>
#include "DanaVariable.h"

typedef std::vector<std::string> List;

class LineStore {
private:
	std::vector<List> store;

public:
	LineStore() {}

	LineStore(List initialLines) {
		store.push_back(initialLines);
	}

	void LineStore::addNewListOfLines(List moreLines) {
		store.push_back(moreLines);
	}

	List LineStore::getList(int index) {
		return store.at(index);
	}

	void removeList(int index) {
		store.erase(store.begin() + index);
	}

	void clear() {
		for (int i = 0; i < store.size(); i++) {
			store.at(i).clear();
		}

		store.clear();
	}

	int size() {
		return store.size();
	}

	~LineStore() {
		clear();
	}
};


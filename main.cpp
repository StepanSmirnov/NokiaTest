#include "table_processor.h"
#include <iostream>
using namespace std;

void printToConsole(const table& data) {
	for (const auto& row : data) {
		for (size_t i = 0; i < row.size(); ++i) {
			std::cout << (i > 0 ? "," : "") << row[i];
		}
		std::cout << std::endl;
	}
}

int main(int argc, char* argv[])
{
	if (argc == 2) {
		TableProcessor processor;
		bool ok;
		processor.readFromFile(argv[1], ok);
		if (ok) {
			table result = processor.getResult();
			printToConsole(result);
		}
		else {
			cout << "data is invalid";
		}
	}
}

#pragma once
#include <vector>
#include <string>
#include <map>
#include <list>
#include <memory>


using table = std::vector<std::vector<std::string>>;
using address = std::pair<std::string, int>;

class TableProcessor
{
public:
	void readFromFile(const std::string& filename, bool& is_ok);
	table getResult() const;
private:
	struct graphNode {
		std::vector<std::shared_ptr<graphNode>> args;
		std::shared_ptr<graphNode> parent = nullptr;
		char op;
		address addr;
		std::string* cell = nullptr;
		bool is_expression = true;
	};

	int row(const address &addr) const {
		return row_lookup_.at(addr.second);
	}

	int col(const address& addr) const {
		return col_lookup_.at(addr.first);
	}

	void evaluateExpression(table& temp_table, const address& addr) const;

	std::map<std::string, int> col_lookup_;
	std::map<int, int> row_lookup_;
	table data_;
};


#include "table_processor.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <iostream>
#include <numeric>
#include <string>



using namespace std;

string performOperation(int arg1, int arg2, char op) {
	switch (op)
	{
	case '+':
		return to_string(arg1 + arg2);
	case '-':
		return to_string(max(arg1 - arg2, 0));//only positive values
	case '*':
		return to_string(arg1 * arg2);
	case '/':
		return arg2 != 0 ? to_string(arg1 / arg2) : "nan";
	default:
		break;
	}
	return "";
}

bool dataIsValid(const table& table)
{
	bool flag = false;
	if (table.size() >= 2) {
		flag = true;
		int len = table[0].size();
		for (const auto& row : table) {
			if (row.size() != len)
				flag = false;
		}
	}
	return flag;
}

tuple<address, address, char> parseExpression(const string& expression) {
	char op;
	address arg1;
	address arg2;
	address* current = &arg1;
	string num;
	for (const char c : expression) {
		if (c == '=')
			continue;
		else if (c >= '0' and c <= '9') {
			num += c;
		}
		else if ((c >= 'a' and c <= 'z') or(c >= 'A' and c <= 'Z')) {
			current->first += c;
		}
		else if (c == '+' or c == '-' or c == '*' or c == '/') {
			op = c;
			current->second = stoi(num);
			num.clear();
			current = &arg2;
		}
	}
	arg2.second = stoi(num);
	return make_tuple(arg1, arg2, op);
}

void TableProcessor::readFromFile(const string& filename, bool& is_ok)
{
	is_ok = false;
	data_.clear();
	ifstream in(filename);
	string line;
	while (getline(in, line))
	{
		vector<string> row;
		stringstream ss(line);
		string token;
		while (getline(ss, token, ',')) {
			row.push_back(token);
		}
		data_.push_back(row);
	}
	if (dataIsValid(data_)) {
		for (size_t i = 0; i < data_[0].size(); ++i) {
			col_lookup_[data_[0][i]] = i;
		}
		for (size_t i = 1; i < data_.size(); ++i) {
			row_lookup_[stoi(data_[i][0])] = i;
		}
		is_ok = true;
	}
}

table TableProcessor::getResult() const
{
	table result = data_;
	for (auto row = row_lookup_.begin(); row != row_lookup_.end(); ++row) {
		for (auto col = col_lookup_.begin(); col != col_lookup_.end(); ++col) {
			auto& cell = result[row->second][col->second];
			if (cell.size() > 5 && cell[0] == '=') {
				//arg1(2 chars)+op(1 char)+arg2(2 chars)
				evaluateExpression(result, { col->first, row->first });
			}
		}
	}
	return result;
}

/*
Algorithm for nested expressions evaluation
It's based on DFS
The difference is that it removes nodes that were arguments in evaluated expression and
moves in reverse direction after evaluation.
*/
void TableProcessor::evaluateExpression(table& temp_table, const address& addr) const
{
	list<shared_ptr<graphNode>> stack;
	shared_ptr<graphNode> root = make_shared<graphNode>();
	root->addr = addr;
	root->cell = &temp_table[row(addr)][col(addr)];
	//root->is_expression = root->cell->size() > 5 && root->cell->at(0) == '=';
	root->is_expression = true;
	stack.push_back(root);
	map<address, shared_ptr<graphNode>> discovered;
	//DFS
	while(!stack.empty()) {
		auto node = stack.back();
		stack.pop_back();
		if (discovered.find(node->addr) == discovered.end()) {
			discovered.insert(make_pair(node->addr, node));
			if (node->is_expression) {
				bool can_evaluate = true; //are both children numbers (not expressions)
				if (node->args.empty()) {
					//to prevent repeated actions when moving back
					auto exp = parseExpression(*node->cell);
					address& arg1 = get<0>(exp);
					address& arg2 = get<1>(exp);
					node->op = get<2>(exp);
					for (auto& child : { arg1, arg2 }) {
						auto child_ptr = std::make_shared<graphNode>();
						child_ptr->addr = child;
						auto& child_cell = temp_table[row(child)][col(child)];
						child_ptr->cell = &child_cell;
						child_ptr->parent = node;
						node->args.push_back(child_ptr);
					}
				}
				for (auto child_ptr : node->args) {
					child_ptr->is_expression = child_ptr->cell->size() > 5 && child_ptr->cell->at(0) == '=';
					if (child_ptr->is_expression || *child_ptr->cell == "nan")
						can_evaluate = false;
					stack.push_back(child_ptr);
				}
				if (can_evaluate) {
					int left = stoi(*node->args[0]->cell);
					int right = stoi(*node->args[1]->cell);
					*node->cell = performOperation(left, right, node->op);
					node->is_expression = false;
					node->args.clear();
					stack.pop_back(); stack.pop_back(); //last 2 values in stack are node->args
					if (node->parent) {
						//in the next run try to evaluate parent node
						stack.push_back(node->parent);
						auto it = discovered.find(node->parent->addr);
						discovered.erase(it);
					}
				}
			}
		}
	}
}

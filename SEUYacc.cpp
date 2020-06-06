// SEUYacc.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "Common.h"

using namespace std;

int read_yacc_file(const string& fileName, string& start, unordered_set<string>& terminal, ProducerVecStr& pro, vector<string>& funcVec);

void set_start(string a, ProducerVecStr& pro, vector<string>& startobj);

void get_no_terminal(ProducerVecStr& pro, unordered_set<string>& noter);

void get_first_functions(ProducerVecStr& pro, unordered_set<string>& noter, map<string, unordered_set<string>>& First, unordered_set<string>& terminal);

void build_pro_num(map<vector<string>, int>& Pro_num, ProducerVecStr& pro);

void construct_LR1_sets(const vector<string>& startobj, const unordered_set<string>& terminal, const unordered_set<string>& noter,
	const ProducerVecStr& pro, map<string, unordered_set<string>>& First, DFA& dfa, DFATransitionTable& LRState);

void merge_LR1_sets_into_LALR_sets(DFA& dfa, DFATransitionTable& LRState);

void generate_action_goto_map(const DFA& dfa, const unordered_set<string>& terminal, const unordered_set<string>& noter,
	const DFATransitionTable& LRState, const string start, const unordered_set<string>& Left, map<vector<string>, int>& Pro_num,
	unordered_map<string, int>& Left_level, map<int, map<string, int>>& GOTO, map<int, map<string, pair<string, int>>>& action);


int main()
{
	// 存放终结符
	unordered_set<string> terminal;
	
	// 存放非终结符
	unordered_set<string> noter;
	
	// 函数部分
	vector<string> funcVec;
	
	// 所有的产生式
	ProducerVecStr pro;
	
	// 存放运算符号
	unordered_set<string> Left;

	// 存放优先级
	unordered_map<string, int> Left_level;

	// first集合，follow集合
	map<string, unordered_set<string>> First;

	// 通过产生式得到编号，与input相反
	map<vector<string>, int> Pro_num;
	
	// DFA转换表, pair<项目编号, 终结符/非终结符>, 移进项目编号, i状态识别一个x后到达j状态.
	DFATransitionTable LRState; 

	// 项目集 规范族
	DFA dfa;

	// action表, action[项目集编号][终结符]<pair 移进/规约，项目集/产生式编号>
	map<int, map<string, pair<string, int>>> action;
	
	// GOTO表，goto[项目集编号][非终结符]=项目集编号
	map<int, map<string, int>> GOTO;

	// 存放开始产生式
	vector<string> startobj;

	// 开始符
	string start;

	read_yacc_file("yacc.y", start, terminal, pro, funcVec);
	set_start(start, pro, startobj);
	get_no_terminal(pro, noter); // 用来获得非终结符的集合，即产生式左边的字符的集合
	build_pro_num(Pro_num, pro); // 为了读入的产生式一个编号(读入顺序
	get_first_functions(pro, noter, First, terminal); // 生成first集
	cout << "Getting LR1..." << endl;
	construct_LR1_sets(startobj, terminal, noter, pro, First, dfa, LRState); // 用来生成状态之间的转换关系
	cout << "Converting..." << endl;
	merge_LR1_sets_into_LALR_sets(dfa, LRState);
	generate_action_goto_map(dfa, terminal, noter, LRState, start, Left, Pro_num, Left_level, GOTO, action); // 用来生成移进规约表

	return 0;
}
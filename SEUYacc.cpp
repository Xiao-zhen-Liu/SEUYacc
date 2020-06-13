// SEUYacc.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "Common.h"

using namespace std;

// 存放终结符
unordered_set<string> terminal;

// 存放非终结符
unordered_set<string> noter;

// 所有的产生式
ProducerVecStr pro;

// 存放运算符号
unordered_set<string> Left;

// 存放优先级
unordered_map<string, int> Left_level;

// first集合，follow集合
map<int, unordered_set<int>> First;

// 通过产生式得到编号，与input相反

// DFA转换表, pair<项目编号, 终结符/非终结符>, 移进项目编号, i状态识别一个x后到达j状态.
DFATransitionTable LRState;

// 项目集 规范族
DFA dfa;

// action表, action[项目集编号][终结符]<pair 移进/规约，项目集/产生式编号>
map<int, map<string, pair<string, int>>> action;

// GOTO表，goto[项目集编号][非终结符]=项目集编号
map<int, map<string, int>> GOTO;

// 存放开始产生式
vector<int> startobj;

// 开始符
string start, definition, code;

//以下为提供数字映射所增加的结构
unordered_map<string, int> sn_map;
unordered_map<int, string> ns_map;
unordered_set<int> terminal_num;
unordered_set<int> noter_num;
unordered_set<int> Left_num;
ProducerVec pro_num;
map < vector<int>, int > r_num;
map<int, vector<int>> wp_map;

int  divide;


int read_yacc_file(const string& fileName, string& start, unordered_set<string>& terminal, ProducerVecStr& pro, string& definition, string& code, unordered_set<string>& Left, unordered_map<string, int>& Left_level);

void set_start(string& a, ProducerVecStr& pro, vector<int>& startobj);

void add_start(string& a, vector<int>& startobj, unordered_map<string, int>& sn_map);

void get_no_terminal(ProducerVecStr& pro, unordered_set<string>& noter);

void get_terminal(ProducerVecStr& pro, unordered_set<string>& noter, unordered_set<string>& terminal);

void build_pro_num(unordered_set<string>& terminal, unordered_map<string, int>&  sn_map, unordered_map<int, string> & ns_map, unordered_set<int> & terminal_num, int & divide,
	unordered_set<string> &  noter, unordered_set<string>&  Left, unordered_set<int> & noter_num, unordered_set<int>&  Left_num, ProducerVecStr & pro, ProducerVec & pro_num,
	map<vector<int>, int>&  r_num, map<int, vector<int>> & wp_map);


void merge_LR1_sets_into_LALR_sets(DFA& dfa, DFATransitionTable& LRState);

void generate_action_goto_map(const DFA& dfa, const unordered_set<string>& terminal, const unordered_set<string>& noter,
	const DFATransitionTable& LRState, const string &start, const unordered_set<string>& Left, map<vector<int>, int>& Pro_num,
	unordered_map<string, int>& Left_level, map<int, map<string, int>>& GOTO, map<int, map<string, pair<string, int>>>& action,
	const unordered_set<int>& terminal_num, unordered_map<int, string> &ns_map, unordered_map<string, int>& sn_map, const unordered_set<int>& Left_num);

void write_parser_code(map<int, map<string, int>>& GOTO, map<int, map<string, pair<string, int>>>& action,
	const unordered_set<string>& terminal, const unordered_set<string>& noter, const ProducerVecStr pro, const string& definition, const string& code);

void construct_LR1_sets();   //用来生成集簇之间的转换关系

void first_operation();


int main(int argc, char const* argv[])
{
	string file_name;

	if (argc != 2) {
		cout << "ERROR: invalid arguments number!\n";
		return -1;
	}
	else
	{
		file_name = string(argv[1]);
	}

	

	read_yacc_file(file_name, start, terminal, pro, definition, code, Left, Left_level);
	cout << "START:" << start << endl;
	set_start(start, pro, startobj);

	get_no_terminal(pro, noter); // 用来获得非终结符的集合，即产生式左边的字符的集合
	get_terminal(pro, noter, terminal);//不是非终结符的字符都是终结符
	build_pro_num( terminal, sn_map,  ns_map,  terminal_num, divide,noter,  Left,  noter_num,  Left_num, pro, pro_num,r_num,wp_map); // 为了读入的产生式一个编号(读入顺序

	add_start(start,  startobj, sn_map);

	first_operation();

	
	cout << "一次" << endl;
	for (auto i : First)
	{
		int ch = i.first;
			cout << "左边为" << ns_map[ch] << "->";
			for (auto j : First[ch])
			{
				cout << ns_map[j] << " ";
			}
			cout << endl;
	}

	cout << "Getting LR1..." << endl;


	construct_LR1_sets(); // 用来生成状态之间的转换关系
	cout << "Converting..." << endl;
	merge_LR1_sets_into_LALR_sets(dfa, LRState);
	generate_action_goto_map(dfa, terminal, noter, LRState, start, Left, r_num, Left_level, GOTO, action,
		 terminal_num,  ns_map, sn_map,  Left_num); // 用来生成移进规约表
	write_parser_code(GOTO, action, terminal, noter, pro, definition, code);

	return 0;
}
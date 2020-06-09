#include "pch.h"
#include "Common.h"

using namespace std;

void write_parser_code(map<int, map<string, int>>& GOTO, map<int, map<string, pair<string, int>>>& action,
	const unordered_set<string>& terminal, const unordered_set<string>& noter, const ProducerVecStr pro)
{
	int termNum = terminal.size();
	int noterNum = noter.size();
	int statesNum = action.size();
	string opt, type;

	ofstream out;
	out.open("yacc.cpp", ios::out);

	out << "#include <set>\n#include <map>\n#include <stack>\n#include <string>\n#include <iostream>\n#include <fstream>\n#include <vector>\n\n";
	out << "using namespace std;\n\n";

	out << "static map<string, int> GOTO[" << statesNum << "] {\n";
	for (size_t i = 0; i < statesNum; i++)
	{
		int ct = 0;
		for (auto s : noter) {
			if (GOTO[i][s] != -1) ct++;
		}
		if (ct == 0) {
			opt += "\t{},\n";
			continue;
		}
		opt += "\t{";
		for (auto s : noter) {
			int w = GOTO[i][s];
			if (w != -1) {
				opt += "{\"" + s + "\", " + to_string(w) + "}, ";
			}
		}
		opt.pop_back(); opt.pop_back();
		opt += "},\n";
	}
	opt.pop_back(); opt.pop_back();
	opt += "\n};\n\n";
	out << opt;
	opt.clear();

	out << "static map<string, pair<unsigned, int>> ACTION[" << statesNum << "] {\n";
	for (size_t i = 0; i < statesNum; i++)
	{
		opt += "\t{";
		for (auto s : terminal) {
			auto r = action[i][s];
			opt += "{\"" + s + "\", {";
			type = r.first;
			if (type == "s") {
				opt += "0, ";
			}
			else if (type == "r") {
				opt += "1, ";
			}
			else if (type == "acc") {
				opt += "2, ";
			}
			else opt += "3, ";
			opt += to_string(r.second) + "}}, ";
		}
		opt.pop_back(); opt.pop_back();
		opt += "},\n";
	}
	opt.pop_back(); opt.pop_back();
	opt += "\n};\n\n";
	out << opt;
	opt.clear();

	out << "static pair<string, vector<string>> pvs[" << pro.size() << "] {\n";
	for (auto p : pro) {
		opt += "\t{\"" + p.first + "\", {";
		for (auto r : p.second) {
			opt += "\"" + r + "\", ";
		}
		opt.pop_back(); opt.pop_back();
		opt += "}},\n";
	}
	opt.pop_back(); opt.pop_back();
	opt += "\n};\n\n";
	out << opt;
	opt.clear();

	out << "stack<int> parser_stack;\n\nvector<string> input_buffer;\n\nvoid output_production(pair<string, vector<string>> prod);\n\n"
		<< "void yyerror();\n\nint main(int argc, char const *argv[])\n{\n\tstring file_name;\n\n\tif (argc != 2) {\n"
		<< "\t\tcout << \"ERROR: invalid arguments number!\\n\";\n\t\treturn -1;\n\t}\n\telse\n\t{\n\t\tfile_name = string(argv[1]);\n\t}"
		<<"\n\t\n\tifstream in;\n\tin.open(\"test.c\");\n\tstring ct;\n\n\twhile (!in.eof()) {\n\t\tgetline(in, ct);\n"
		<<"\t\tinput_buffer.push_back(ct);\n\t}\n\tinput_buffer.push_back(\"#\");\n\t\n\tparser_stack.push(0);\n\n\tint cursor = 0;\n"
		<<"\n\tstring a = input_buffer[cursor];\n\n\tint s, t, action_type, action_num, right_num;\n\n\tstring left;\n"
		<<"\n\tpair<string, vector<string>> production;\n\n\twhile (true)\n\t{\n\t\ts = parser_stack.top();\n\t\taction_type = ACTION[s][a].first;\n"
		<<"\t\taction_num = ACTION[s][a].second;\n\t\tif (action_type == 0) {\n\t\t\tparser_stack.push(action_num);\n"
		<<"\t\t\ta = input_buffer[++cursor];\n\t\t}\n\t\telse if (action_type == 1) {\n\t\t\tproduction = pvs[action_num];\n"
		<<"\t\t\t\n\t\t\tfor (int i = 0; i < production.second.size(); i++)\n\t\t\t{\n\t\t\t\tparser_stack.pop();\n\t\t\t}\n"
		<<"\t\t\tt = parser_stack.top();\n\t\t\tparser_stack.push(GOTO[t][production.first]);\n\t\t\toutput_production(production);\n"
		<<"\t\t}\n\t\telse if (action_type == 2) break;\n\t\telse {yyerror(); return -1;}\n\t}\n\n\tcout << \"SUCCESS!\" << endl;\n\treturn 0;\n}\n\n"
		<<"void output_production(pair<string, vector<string>> prod) {\n\tcout << prod.first << \"  ->  \";\n\tfor (auto str : prod.second) {\n"
		<<"\t\tcout << str << \"   \";\n\t}\n\tcout << endl;\n}\n\nvoid yyerror() {\n\tcout << \"Error\" << endl;\n}";

	out.close();
}
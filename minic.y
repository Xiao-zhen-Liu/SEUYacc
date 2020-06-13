%{
#include <set>
#include <map>
#include <stack>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class Node
{
public:
	int index;

	string label;

	bool isTerminal;

	Node();

	Node(int idx, string lb);

	virtual void printNode(fstream& out);
};

class TerminalNode: public Node {
public:
	TerminalNode(int idx, string lb);

	virtual void printNode(fstream& out);
};

class NonterminalNode: public Node {
public:
	vector<int> children;

	NonterminalNode(int idx, string lb, vector<int> chi);

	virtual void printNode(fstream& out);
};

// Stack
stack<int> parser_stack;

// Symbols
stack<Node*> nodes_stack;

// Input
vector<string> input_buffer;

map<int, Node*> nodes_map;

void output_production(pair<string, vector<string>> prod);

void yyerror();

void print_parse_tree();
%}

%union	{
	char*		name;
	int		value;
	T_LIST*		tlist;
	T_INFO*		type;
	SYM_INFO*	sym;
	SYM_LIST*	slist;
	}

%token	INT FLOAT NAME STRUCT IF ELSE RETURN NUMBER LPAR RPAR LBRACE RBRACE eplsion
%token	LBRACK RBRACK ASSIGN SEMICOLON COMMA DOT PLUS MINUS TIMES DIVIDE EQUAL

%type	<name>	NAME
%type	<value>	NUMBER
%type	<type>	type parameter exp lexp
%type	<tlist>	parameters more_parameters exps
%type	<sym>	field var
%type	<slist>	fields

/*	associativity and precedence: in order of increasing precedence */

%nonassoc	LOW  /* dummy token to suggest shift on ELSE */
%nonassoc	ELSE /* higher than LOW */

%nonassoc	EQUAL
%left		PLUS	MINUS
%left		TIMES	DIVIDE
%left		UMINUS	/* dummy token to use as precedence marker */
%left		DOT	LBRACK	/* C compatible precedence rules */

%%
program		: declarations
		;

declarations	: declaration declarations
		| eplsion
		;

declaration	: fun_declaration
		| var_declaration
		;

fun_declaration	: type NAME 
		  LPAR parameters RPAR 
		  block	
		;

parameters	: more_parameters	
		| eplsion
		;

more_parameters	: parameter COMMA more_parameters					
		| parameter		
		;

parameter	: type NAME
		;

block		: LBRACE		
		  var_declarations statements RBRACE
					
		;

var_declarations : var_declaration var_declarations
		| eplsion
		;

var_declaration	: type NAME SEMICOLON	
		;

type		: INT			
		| FLOAT			
		| type TIMES		
		| STRUCT LBRACE fields RBRACE 			
		;

fields		: field fields		
		| eplsion		
		;

field		: type NAME SEMICOLON	
		;

statements	: statement SEMICOLON statements
		| eplsion
		;

statement	: IF LPAR exp RPAR statement 		
		| IF LPAR exp RPAR statement ELSE statement	
		| lexp ASSIGN exp	
		| RETURN exp 
			
		| block
		;

lexp		: var			
		| lexp LBRACK exp RBRACK
		| lexp DOT NAME		
		;

exp		: exp DOT NAME		
		| exp LBRACK exp RBRACK	
		| exp PLUS exp		
		| exp MINUS exp		
		| exp TIMES exp		
		| exp DIVIDE exp	
		| exp EQUAL exp		
		| LPAR exp RPAR
		| MINUS exp 	
					
		| var			
		| NUMBER 		
		| NAME LPAR RPAR	
		| NAME LPAR exps RPAR	
		;

exps		: exp 			
		| exp COMMA exps	
		;

var		: NAME 	
                ;		
%%
int main(int argc, char const *argv[])
{
	cout << "YACC" << endl;
	string file_name;

	if (argc != 2) {
		cout << "ERROR: invalid arguments number!\n";
		return -1;
	}
	else
	{
		file_name = string(argv[1]);
	}
	
	ifstream in;
	in.open("test.c");
	string ct;

	while (!in.eof()) {
		getline(in, ct);
		input_buffer.push_back(ct);
	}
	input_buffer.push_back("#");
	
	parser_stack.push(0);

	int cursor = 0;

	string a = input_buffer[cursor];

	int s, t, action_type, action_num, right_num;

	int node_num = 0;

	pair<string, vector<string>> production;

	while (true)
	{
		s = parser_stack.top();
		action_type = ACTION[s][a].first;
		action_num = ACTION[s][a].second;
		if (action_type == 0) {
			parser_stack.push(action_num);
			TerminalNode* tm = new TerminalNode(node_num++, a);
			nodes_stack.push(tm);
			a = input_buffer[++cursor];
		}
		else if (action_type == 1) {
			production = pvs[action_num];
			vector<int> children;

			for (int i = 0; i < production.second.size(); i++)
			{
				Node *top = nodes_stack.top();
				children.insert(children.begin(), top->index);
				nodes_map[top->index] = top;
				nodes_stack.pop();

				parser_stack.pop();
			}
			
			t = parser_stack.top();
			parser_stack.push(GOTO[t][production.first]);
			
			NonterminalNode *no = new NonterminalNode(node_num++, production.first, children);
			nodes_stack.push(no);

			output_production(production);
		}
		else if (action_type == 2) {
			Node *top = nodes_stack.top();
			nodes_map[top->index] = top;
			break;
		}
		else {yyerror(); return -1;}
	}
	print_parse_tree();
	cout << "SUCCESS!" << endl;
	return 0;
}

Node::Node() {

}

Node::Node(int idx, string lb)
{
	index = idx;
	label = lb;
	isTerminal = true;
}

void Node::printNode(fstream& out) {
	out << "node" << index << "[" << "label=\"" << label << "\"];" << endl;
}

TerminalNode::TerminalNode(int idx, string lb) : Node(idx, lb) {
	isTerminal = true;
}

void TerminalNode::printNode(fstream& out) {
	Node::printNode(out);
}

NonterminalNode::NonterminalNode(int idx, string lb, vector<int> chi) : Node(idx, lb) {
	isTerminal = false;
	children = chi;
}

void NonterminalNode::printNode(fstream& out) {
	Node::printNode(out);
	for (auto c : children) {
		out << "node" << index << "->" << "node" << c << ";" << endl;
	}
}

void output_production(pair<string, vector<string>> prod) {
	cout << prod.first << "  ->  ";
	for (auto str : prod.second) {
		cout << str << "   ";
	}
	cout << endl;
}

void yyerror() {
	cout << "Error" << endl;
}

void print_parse_tree() {
	fstream out;
	out.open("parseTree.dot", ios::out);
	out << "digraph G{" << endl;
    out << "node [shape=rectangle];" << endl;

	for (auto i_n : nodes_map) {
		Node *n = i_n.second;
		n->printNode(out);
	}

	out << "}";
	out.close();
}

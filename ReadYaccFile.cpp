#include "pch.h"
#include "Common.h"

using namespace std;

// 用来读入.y文件
int read_yacc_file(const string& fileName, string& start, unordered_set<string>& terminal, ProducerVecStr& pro,  string& definition, string& code, unordered_set<string>& Left, unordered_map<string, int>& Left_level) {
	
	int ambiguity = 0;//用来判断yacc格式类型 0为
	int lines = 0;  //标识正读到的行数
	int opLevel = 0; //标识操作符优先级

	ifstream in;//读入myyacc文件
	//cout << "fa";
	in.open(fileName, ios::in);
	if (!in)
	{
		cout << "ERROR: Can't open Yacc file!" << endl;
		return 1;
	}
	string str;
	int i = 0;//表示statement中第几行

	getline(in, str);
	if (str == "%{") {
		getline(in, str);
		while (str != "%}" && !in.eof())
		{
			definition += str + "\n";
			getline(in, str);
		}
		if (in.eof()) {
			cout << "ERROR: incomplete code definition area." << endl;
			return 0;
		}
	}

	do
	{
		in >> str;
	} while (str != "%token");

	/*将所有token纳入token中*/
	in >> str;
	do
	{
		if (str == "%token")
		{
			in >> str;
			// cout << str << endl;
		}
		//cout << str << endl;
		terminal.insert(str);
		in >> str;
	} while (str != "%left"&&str != "%type"&&str != "%%"&&str != "%start");  //读入 token结束



	do
	{

		if (str == "%start")
		{
			cout << "---------------------------------------------------------------" << endl;
			ambiguity = 2; //如果有%start则自动生成，没有的话则把产生式第一个当做start
			break;
		}

		in >> str;
		if (str == "%left")
		{
			ambiguity = 1; //有%left说明本文法是二义性文法，需要读入符号优先级
			cout << "bi" << endl;
			break;
		}



		if (str == "%%")
		{
			ambiguity = 0; //没有%left说明本文法不是二义性文法，不需要读入符号优先级
			break;
		}
	} while (1);           //清除中间的无用文本



	if (ambiguity == 1)
	{

		cout << "fa1" << endl;
		cout << str << endl;

		do
		{
			//cout << str<<endl;
			if (str == "%left")
			{
				in >> str;
				opLevel++;
			}

			if (str == "/*")
			{
				in >> str;
				while (str != "*/")
					in >> str;
				if (str != "*/")
				{
					in >> str;
				}

				if (str == "%left")
				{
					in >> str;
					opLevel++;
				}
			}

			if (str != "*/")
			{
				Left.insert(str);
				Left_level.insert(make_pair(str, opLevel));

			}

			in >> str;

		} while (str != "%start"&&str != "%%");

	}


	if (ambiguity == 2)
	{

		cout << "第二种" << endl;
		cout << str << endl;

		do
		{
			//cout << str<<endl;
			if (str == "%start")
			{
				in >> str;
			}

			if (str == "/*")
			{
				in >> str;
				while (str != "*/")
					in >> str;
				if (str != "*/")
				{
					in >> str;
				}

				if (str == "%left")
				{
					in >> str;
					opLevel++;
				}
			}

			if (str != "*/")
			{
				start = str;
			}

			in >> str;

		} while (str != "%%");

	}


	bool flat_start = true;

	//开始读入产生式
	in >> str;
	int counter = 0;
	do {
		cout << "fa" << endl;
		cout << str << endl;
		pair<string, vector<string> > p;

		if (ambiguity == 1 && flat_start) {
			start = str; //没有start的，第一个当产生式
			flat_start = false;

			//	cout << "start:" << start << endl;
		}
		p.first = str;  //左边
		in >> str;   //全是:

		if (str != ":")
		{
			cout << str << endl;
			cout << "ERROR: NO \":\" " << endl;
			return -1;
		}
		in >> str;  //第一个冒号后 空格后的东西

		do {
			if (str == "|")
			{
				pro.push_back(p);
				p.second.clear();
				in >> str;
			}

			while (str != "|"&&str != ";")
			{
				cout << str << endl;
				p.second.push_back(str);    //右侧的集合
				in >> str;  //空格后的东西

			}
		} while (str != ";");
		pro.push_back(p);
		in >> str;  //每段：左边的单词

	} while (str != "%%");
	//读入函数部分
	do {
		getline(in, str);
		code += str + "\n";
	} while (!in.eof());

	terminal.insert("#");//给终结符添加#

	//cout << "标记end" << endl;
	return 1;

}
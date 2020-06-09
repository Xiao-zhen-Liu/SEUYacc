#include "pch.h"
#include "Common.h"

using namespace std;

// 用来读入.y文件
int read_yacc_file(const string& fileName, string& start, unordered_set<string>& terminal, ProducerVecStr& pro, vector<string>& funcVec) {
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
	} while (str != "%left" && str != "%type" && str != "%%" && str != "%start");  // 读入token结束

	if (str == "%start") {
		in >> str;
		start = str;
	}

	do
	{
		in >> str;

	} while (str != "%left" && str != "%%");           // 清除中间的无用文本


	//开始读入产生式
	in >> str;
	int counter = 0;
	do {
		/*cout << str << endl;*/
		pair<string, vector<string>> p;

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

			while (str != "|" && str != ";")
			{
				/*cout << str << endl;*/
				p.second.push_back(str);    //右侧的集合
				in >> str;  //空格后的东西

			}
		} while (str != ";");
		pro.push_back(p);
		in >> str;  //每段：左边的单词

	} while (str != "%%");

	//读入函数部分
	do {
		/*cout << str << endl;*/
		getline(in, str);

		funcVec.push_back(str);

	} while (!in.eof());

	terminal.insert("#");//给终结符添加#


	//cout << "标记end" << endl;
	return 1;

}
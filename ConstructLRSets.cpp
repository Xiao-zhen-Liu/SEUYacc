#include "pch.h"
#include "Common.h"

using namespace std;

// 定义开始符，可根据不同的.y文件调整
void set_start(string a, ProducerVecStr& pro, vector<string>& startobj)
{
	pair<string, vector<string> > start2;

	start2.first = "start";
	start2.second.push_back(a);    //右侧的集合
	pro.insert(pro.begin(), start2);

	string temp2 = "start";  //产生用于第一次内扩展的产生式
	startobj.push_back(temp2);
	temp2 = a;
	startobj.push_back(temp2);
}

// 用来获得非终结符的集合，即产生式左边的字符的集合
void get_no_terminal(ProducerVecStr& pro, unordered_set<string>& noter)
{
	string s;
	for (int i = 0; i < pro.size(); i++)
	{
		noter.insert(pro[i].first);
	}
}

// 将temp_first存入产生式为s字符串的first，之后想查s的first去temp_first中找
void build_temp_first(unordered_set<string>& temp_first, map<string, unordered_set<string>>& First, vector<string> s)
{
	temp_first.clear();//临时使用，每次清空
	bool flag = 1; //标记空元素
	for (int i = 0; i < s.size(); i++)
	{
		flag = 0;
		for (auto j : First[s[i]])
		{
			if (j != kong) temp_first.insert(j);
			else flag = 1;
		}
		if (flag == 0) break;
	}
	if (flag == 1)  temp_first.insert(kong);
}

// 生成first集
void get_first_functions(ProducerVecStr& pro, unordered_set<string>& noter, map<string, unordered_set<string>>& First, unordered_set<string>& terminal)
{
	bool flag = 1;//是否改变
	while (flag)
	{
		flag = 0;
		for (auto no : noter)
		{
			for (int i = 0; i < pro.size(); i++)
			{
				if (pro[i].first == no)
				{
					vector<string> j = pro[i].second;
					bool flag = 0; //标记空元素
					for (auto ch : j)
					{
						if (noter.find(ch) == noter.end()) // A terminal
						{
							if (First[no].insert(ch).second) flag = 1; // An insertion is made
							flag = 0; // ?
						}
						else // Is a nonterminal
						{
							for (auto k : First[ch])
							{
								if (k != kong)
								{
									if (First[no].insert(k).second) flag = 1;
									flag = 0;
								}
								else flag = 1;
							}

						}
						if (flag == 0) break;

					}
					if (flag == 1 && First[no].insert(kong).second) flag = 1;
				}
			}
			for (auto i : terminal) First[i].insert(i);
		}

	}

	/*以下为打印测试部分
	cout << "一次" << endl;
		for (auto i : First)
		{
			string ch = i.first;
			if (noter.find(ch) != noter.end())
			{
			cout << "左边为" << ch << "->";

			for (auto j : First[ch])
			{
					cout << j << " ";
			}
			cout << endl;
			}
		}
		*/
}

// 为了读入的产生式一个编号，方便之后生成规约表。编号就是序号(读入顺序）
void build_pro_num(map<vector<string>, int>& Pro_num, ProducerVecStr& pro)
{
	for (int i = 0; i < pro.size(); i++)
	{
		vector<string> temp = pro[i].second;   //产生式右向
		temp.insert(temp.begin(), pro[i].first);
		Pro_num.insert(make_pair(temp, i));

		cout << "编号: " << i << "  ";
		cout << pro[i].first << "->";
		for (int j = 0; j < pro[i].second.size(); j++)
			cout << pro[i].second[j] << " ";
		cout << endl;
	}
}

// 项目闭包的内部扩展
DFAState inter_extend(LRitem x, const unordered_set<string>& noter, const ProducerVecStr& pro, map<string, unordered_set<string>>& First, const unordered_set<string>& terminal)
{
	DFAState extended;  //需要返回的闭包
	extended.insert(x);
	bool f = 1;
	unordered_set<string> temp_first;

	while (f)  // 直到集合不再增大为止
	{
		f = 0;  //如果集合增大，f=1
		for (auto lritem1 : extended)
		{
			auto item1 = lritem1.first;          //x是其中一个项目
			if (item1.second >= item1.first.size())  //点的位置等于产生式的符号数量  
			{
				f = 0;   //成功规约，没有增大
				/*
				cout << "kai" << endl;
				for(int w=0;w<x.first.size();w++)
				cout << x.first[w] << endl;
				cout << x.second << endl;
				cout << "guan" << endl;
				*/
				break;
			}


			string s = item1.first[item1.second];    //得到了点后面的符号

			if (noter.find(s) != noter.end())    //如果点后面的是非终结符
			{

				for (int i = 0; i < pro.size(); i++)    //遍历所有的产生式
				{

					if (pro[i].first == s)    //找出这个非终结符的产生式
					{
						vector<string> temp = pro[i].second;   //产生式右向
						temp.insert(temp.begin(), s);
						//将产生式左部符号放入开头

						vector<string> xx;
						//若项目[A->x·by,a]属于闭包ans,B->R是产生式
						//对first（Ra) 中的每个字符串b，若[B->r，b]不在ans中，加入
						xx.assign(item1.first.begin() + item1.second + 1, item1.first.end());
						xx.push_back(lritem1.second);
						build_temp_first(temp_first, First, xx);
						for (auto b : temp_first)//getfirst 计算得到的结果在se中  
							if (extended.insert(make_pair(make_pair(temp, 1), b)).second)  //如果插入成功
							{
								f = 1;   //如果插入成功，f=1，集合增大了
							}

						// temp.insert(temp.begin(), s);
					}
				}
			}
			else if (terminal.find(s) == terminal.end()) cout << s << endl;
		}
	}

	//for (auto LRitem2 : extended)   //j为一个item
	//{
	//	auto item2 = LRitem2.first;
	//	vector<string> temp = item2.first;
	//	cout << temp[0] << "->";
	//	for (int k = 1; k < temp.size(); k++)
	//	{
	//		if (k == item2.second) cout << "点 ";
	//		cout << temp[k] << " ";
	//	}
	//	if (item2.second == temp.size()) cout << "点 ";
	//	cout << "," << LRitem2.second << endl;
	//}
	//cout << endl;

	return extended;

}

// 尝试每一个项目集合每一个文法符号，看是否可能得到一个新项目, 如果可以就加入到集簇中。pos是项目编号， xx为所要尝试的在后面添加的终结符/非终结符
void outer_extend(int pos, string xx, DFA& DFA, DFATransitionTable& LRState, const unordered_set<string>& noter, const ProducerVecStr& pro, map<string, unordered_set<string>>& First, const unordered_set<string>& terminal)
{
	DFAState ans;
	for (auto state1 : DFA[pos])    //x是一个OBJ  first是产生式  second是·的位置
	{
		auto LRitem1 = state1.first;
		if (LRitem1.first.size() == LRitem1.second) continue;
		//如果已经到了最后一个位置了，说明是规约项目，不用后移

		if (LRitem1.first[LRitem1.second] == xx)
		{
			DFAState temp = inter_extend(make_pair(make_pair(LRitem1.first, LRitem1.second + 1), state1.second), noter, pro, First, terminal);//计算点向后移动一位的闭包
			ans.insert(temp.begin(), temp.end());//set可以自动去重
		}
	}
	if (ans.empty()) return;  //得到的是空的，那么直接返回

	int loc = find(DFA.begin(), DFA.end(), ans) - DFA.begin(); //find返回在数组中的位置，减去起始位置得到编号
	if (loc != DFA.size())
	{
		LRState[make_pair(pos, xx)] = loc;   //找到了已经存在的项目，那么移进
	}
	else
	{
		DFA.push_back(ans);     //找到的不是已经存在的项目，那么加入DFA集合中
		LRState[make_pair(pos, xx)] = DFA.size() - 1;        //移进到新的集簇中
	}

}

// 用来生成集簇之间的转换关系
void construct_LR1_sets(const vector<string> &startobj, const unordered_set<string>& terminal, const unordered_set<string>& noter, 
	const ProducerVecStr& pro, map<string, unordered_set<string>>& First, DFA& dfa, DFATransitionTable& LRState)
{
	//vector<string> temp3;
	//temp3.push_back("#");
	DFAState statu = inter_extend(make_pair(make_pair(startobj, 1), "#"), noter, pro, First, terminal); //初始项目的闭包，作为一个项目集合
	dfa.push_back(statu);  // 存入DFA中，得到一个编号为0，因为位置为0号

	for (int i = 0; i < dfa.size(); i++)  //遍历每一个，直到DFA不再增大为止  push back 会自动放在数组最后一个
	{
		for (auto t : terminal) outer_extend(i, t, dfa, LRState, noter, pro, First, terminal);
		for (auto n : noter) outer_extend(i, n, dfa, LRState, noter, pro, First, terminal);   //试探所有的符号
		cout << dfa.size() << endl;
	}

	//下面为打印集簇部分
	//for (int i = 0; i < dfa.size(); i++)
	//{
	//	cout << i << endl;
	//	for (auto state1 : dfa[i])   //j为一个obj
	//	{
	//		auto LRitem1 = state1.first;
	//		vector<string> temp = LRitem1.first;
	//		cout << temp[0] << "->";
	//		for (int k = 1; k < temp.size(); k++)
	//		{
	//			if (k == LRitem1.second) cout << "点 ";
	//			cout << temp[k] << " ";
	//		}
	//		if (LRitem1.second == temp.size()) cout << "点 ";
	//		cout << "," << state1.second << endl;
	//	}
	//	cout << endl;
	//}

	//下面为打印移进部分     
	//for (auto i : LRState)
	//{
	//	cout << i.first.first << "遇到" << i.first.second << "移进到" << i.second << endl;
	//}

}


StateCore get_core(const DFAState& LRset) {
	StateCore rst;
	for (auto i : LRset) {
		rst.insert(i.first);
	}
	return rst;
}

// 合并同心项
void merge_LR1_sets_into_LALR_sets(DFA& dfa, DFATransitionTable& LRState) {
	DFA newDFA;
	DFATransitionTable newLRState;

	map<StateCore, set<int>> core2StatesNum;
	vector<StateCore> ordered;

	for (int i = 0; i < dfa.size(); i++) {
		auto state = dfa[i];
		auto core = get_core(state);
		if (core2StatesNum.find(core) == core2StatesNum.end()) { // 新建
			set<int> newSet = { i };
			core2StatesNum[core] = newSet;
			ordered.push_back(core);
		}
		else {
			core2StatesNum[core].insert(i);
		}
	}

	map<int, set<int>> newDFA2OldDFA; // 一对多
	map<int, int> oldDFA2NewDFA; // 多对一

	int newIdx = 0;

	// 合并，但暂不处理转换表，先记录新旧状态的映射关系
	for (auto c : ordered) {
		DFAState newState;
		set<int> sns = core2StatesNum[c];
		newDFA2OldDFA[newIdx] = sns;
		for (auto i : sns) {
			oldDFA2NewDFA[i] = newIdx;
			DFAState oldState = dfa[i];
			newState.insert(oldState.begin(), oldState.end());
		}
		newDFA.push_back(newState);
		newIdx++;
	}

	for (auto m : LRState) {
		auto oldFrom = m.first;
		auto oldTo = m.second;
		
		auto newFrom = make_pair(oldDFA2NewDFA[oldFrom.first], oldFrom.second);
		auto newTo = oldDFA2NewDFA[oldTo];
		newLRState.insert({ newFrom, newTo });
	}
	dfa = newDFA;
	LRState = newLRState;
}
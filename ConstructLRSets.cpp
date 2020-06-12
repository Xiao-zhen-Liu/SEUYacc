#include "pch.h"
#include "Common.h"

using namespace std;

// 定义开始符，可根据不同的.y文件调整
void set_start(string& a, ProducerVecStr& pro, vector<int>& startobj)
{
	pair<string, vector<string> > start2;

	start2.first = "start";
	start2.second.push_back(a);    //右侧的集合
	pro.insert(pro.begin(), start2);

}

void add_start(string& a, vector<int>& startobj, unordered_map<string, int> &sn_map)
{

	string temp2 = "start";  //产生用于第一次内扩展的产生式
	startobj.push_back(sn_map[temp2]);
	temp2 = a;
	startobj.push_back(sn_map[temp2]);

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


//不是非终结符的字符都是终结符
void get_terminal(ProducerVecStr& pro, unordered_set<string>& noter, unordered_set<string>& terminal)
{

	for (int i = 0; i < pro.size(); i++)
	{

		for (int j = 0; j < pro[i].second.size(); j++)
			if (noter.find(pro[i].second[j]) == noter.end())
				terminal.insert(pro[i].second[j]);
	}
}




// 为了读入的产生式一个编号，方便之后生成规约表。编号就是序号(读入顺序）
void build_pro_num(unordered_set<string> & terminal, unordered_map<string, int>&  sn_map, unordered_map<int, string> & ns_map, unordered_set<int> & terminal_num, int & divide,
	unordered_set<string> &  noter, unordered_set<string>&  Left, unordered_set<int>&  noter_num, unordered_set<int>&  Left_num, ProducerVecStr & pro, ProducerVec  & pro_num,
	map<vector<int>, int> & r_num ,map<int, vector<int>> & wp_map) //为了读入的产生式一个编号，方便之后生成规约表。编号就是序号(读入顺序）
{
	int count = 0;

	for (auto st : terminal)
	{
		sn_map.insert(make_pair(st, count));
		ns_map.insert(make_pair(count, st));
		terminal_num.insert(sn_map[st]);
		count++;
	}

	divide = count;

	cout << "divide: --------------  " << divide << endl;

	for (auto st : noter)
	{
		sn_map.insert(make_pair(st, count));
		ns_map.insert(make_pair(count, st));
		noter_num.insert(sn_map[st]);
		count++;

	}

	for (auto st : Left)
	{
		Left_num.insert(sn_map[st]);
	}

	for (auto st : sn_map)
	{
		cout << st.first << " -+-" << st.second << endl;
	}




	//创造pro_num

	for (auto temp_pro : pro)
	{
		vector<int> temp_vect;
		temp_vect.clear();
		for (auto temp_pair : temp_pro.second)
		{
			temp_vect.push_back(sn_map[temp_pair]);

		}
		pro_num.push_back(make_pair(sn_map[temp_pro.first], temp_vect));
	}


	for (int i = 0; i < pro_num.size(); i++)
	{
		vector<int> temp = pro_num[i].second;   //产生式右向
		temp.insert(temp.begin(), pro_num[i].first);
		r_num.insert(make_pair(temp, i));

		cout << "编号: " << i << "  ";
		cout << ns_map[pro_num[i].first] << "->";
		for (int j = 0; j < pro_num[i].second.size(); j++)
			cout << ns_map[pro_num[i].second[j]] << " ";
		cout << endl;
	}


	for (auto s : noter_num)
	{

		for (int i = 0; i < pro_num.size(); i++)    //遍历所有的产生式
		{
			if (pro_num[i].first == s)    //找出这个非终结符的产生式
			{
				wp_map[s].push_back(i);
			}
		}

	}





}


// 将temp_first存入产生式为s字符串的first，之后想查s的first去temp_first中找
void build_temp_first(unordered_set<int>& temp_first, map<int, unordered_set<int>>& First, vector<int>&  s, unordered_map<string, int> & sn_map)
{
	temp_first.clear();//临时使用，每次清空
	bool flag = 1; //标记空元素
	for (int i = 0; i < s.size(); i++)
	{
		flag = 0;
		for (auto j : First[s[i]])
		{
			if (j != sn_map[kong]) temp_first.insert(j);
			else flag = 1;
		}
		if (flag == 0) break;
	}
	if (flag == 1)  temp_first.insert(sn_map[kong]);
}

// 生成first集
void get_first_functions(ProducerVec& pro_num, unordered_set<int>& noter_num, map<int, unordered_set<int>>& First, 
	unordered_set<int>& terminal_num, unordered_map<string, int>&  sn_map)
{
	bool flag = 1;//是否改变
	while (flag)
	{
		flag = 0;
		for (auto no : noter_num)
		{
			for (int i = 0; i < pro_num.size(); i++)
			{
				if (pro_num[i].first == no)
				{
					vector<int> j = pro_num[i].second;
					bool flag = 0; //标记空元素
					for (auto ch : j)
					{
						if (noter_num.find(ch) == noter_num.end()) // A terminal
						{
							if (First[no].insert(ch).second) flag = 1; // An insertion is made
							flag = 0; // ?
						}
						else // Is a nonterminal
						{
							for (auto k : First[ch])
							{
								if (k != sn_map[kong])
								{
									if (First[no].insert(k).second) flag = 1;
									flag = 0;
								}
								else flag = 1;
							}

						}
						if (flag == 0) break;

					}
					if (flag == 1 && First[no].insert(sn_map[kong]).second) flag = 1;
				}
			}
			for (auto i : terminal_num) First[i].insert(i);
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

//项目闭包的内部扩展
unordered_set<LRitem> inter_extend(unordered_set <LRitem> & x,int &divide,
	map<int, vector<int>>&  wp_map, ProducerVec & pro_num,
	unordered_map<string, int>&  sn_map, map<int, unordered_set<int>>& First, unordered_map<int, string> & ns_map)
{
	unordered_set<LRitem> extended;
	queue<LRitem> queue;
	unordered_set<int> temp_first;


	for (auto temp : x) {//产生式先都入栈

		queue.push(temp);
		extended.insert(temp);
	}

	Item item1;
	LRitem lritem1;

	while (!queue.empty())  // 直到集合不再增大为止
	{

		
		lritem1 = queue.front();
		item1 = queue.front().first;
		if (item1.second >= item1.first.size())
		{//点在末尾，处理下一个产生式
			queue.pop();

			continue;
		}

		else {

			int s = item1.first[item1.second];    //得到了点后面的符号





			if (s < divide)//是终结符，没有闭包，处理下一个
			{
				queue.pop();
				//cout << "gan《  " << ns_map[s] << " ";
				continue;
			}


			queue.pop();
			//cout << "gan《  " << ns_map[s] << " ";





			for (auto i : wp_map[s])    //找出这个非终结符的产生式
			{
			

				vector<int> temp = pro_num[i].second;   //产生式右向


				temp.insert(temp.begin(), s);


				vector<int> xx;
				//若项目[A->x·by,a]属于闭包ans,B->R是产生式
				//对first（Ra) 中的每个字符串b，若[B->r，b]不在ans中，加入
				xx.assign(item1.first.begin() + item1.second + 1, item1.first.end());


				build_temp_first(temp_first, First, xx, sn_map);
			

				if (temp_first.find(sn_map[kong]) != temp_first.end()) {//有epsilon
					temp_first.erase(sn_map[kong]);
					temp_first.insert(lritem1.second);
				}

				LRitem TEMP;



				for (auto b : temp_first)//getfirst 计算得到的结果在se中  
				{

					TEMP.second = b;
					TEMP.first.first = temp;
					TEMP.first.second = 1;



					auto findIt = extended.find(TEMP);
					if (findIt != extended.end()) //存在一样的了， 就不再处理
						continue;

				

					queue.push(TEMP);
					extended.insert(TEMP);
				}
				//cout << "一次结束" << endl;

				/*
					if (queue.push(make_pair(make_pair(temp, 1), b)).second)  //如果插入成功
					{
						f = 1;   //如果插入成功，f=1，集合增大了
					}
				*/
				// temp.insert(temp.begin(), s);
			}



		}
	}

	//cout <<"-";

	/*
	cout << "内" << endl;
	for (auto LRitem2 : extended)   //j为一个item
	{
		auto item2 = LRitem2.first;
		vector<int> temp = item2.first;

		cout << ns_map[temp[0]]<< "->";

		for (int k = 1; k < temp.size(); k++)
		{
			if (k == item2.second) cout << "点 ";
			cout << ns_map[temp[k]] << " ";
		}
		if (item2.second == temp.size()) cout << "点 ";
		cout << "," << ns_map[LRitem2.second ]<< endl;
		

	}
	cout << endl;


	if (extended.size() > 1000)
	{
		cout << extended.size() << "内部项目" << endl;
	}

	*/

	return extended;

}



void outer_extend(int &pos, int& xx, DFA& dfa, DFATransitionTable& LRState,
	const unordered_set<string>& noter, const ProducerVecStr& pro,
	map<int, unordered_set<int>>& First, const unordered_set<string>& terminal,
	int &divide, map<int, vector<int>> &wp_map, ProducerVec & pro_num,
	unordered_map<string, int> & sn_map, unordered_map<int, string> & ns_map)
//尝试每一个项目集合每一个文法符号，看是否可能得到一个新项目
//如果可以就加入到集簇中   pos是项目编号， xx为所要尝试的在后面添加的终结符/非终结符
{
	unordered_set<LRitem> ans;

	for (auto state1 : dfa[pos])    //x是一个OBJ  first是产生式  second是·的位置
	{
		auto LRitem1 = state1.first;
		if (LRitem1.first.size() == LRitem1.second) continue;
		//如果已经到了最后一个位置了，说明是规约项目，不用后移

		unordered_set<LRitem> outer_temp;
		if (LRitem1.first[LRitem1.second] == xx)
		{

			outer_temp.insert(make_pair(make_pair(LRitem1.first, LRitem1.second + 1), state1.second));

		}
		unordered_set<LRitem> temp = inter_extend(outer_temp, divide,wp_map, pro_num,  sn_map,  First,ns_map);//计算点向后移动一位的闭包
		ans.insert(temp.begin(), temp.end());//set可以自动去重
	}

	if (ans.empty()) return;  //得到的是空的，那么直接返回

	int loc = find(dfa.begin(), dfa.end(), ans) - dfa.begin(); //find返回在数组中的位置，减去起始位置得到编号
	if (loc != dfa.size())
	{
		LRState[make_pair(pos, xx)] = loc;   //找到了已经存在的项目，那么移进

	}
	else
	{
		dfa.push_back(ans);     //找到的不是已经存在的项目，那么加入DFA集合中
		LRState[make_pair(pos, xx)] = dfa.size() - 1;        //移进到新的集簇中
	}

}





// 用来生成集簇之间的转换关系
void construct_LR1_sets(const vector<int> &startobj, const unordered_set<string>& terminal, const unordered_set<string>& noter,
	const ProducerVecStr& pro, map<int, unordered_set<int>>& First, DFA& dfa, DFATransitionTable& LRState, unordered_map<string, int> &sn_map,
	int& divide, map<int, vector<int>> &wp_map, ProducerVec &pro_num ,unordered_map<int, string>& ns_map)
{

	unordered_set<LRitem> statu;

	statu.insert(make_pair(make_pair(startobj, 1), sn_map["#"]));

	statu = inter_extend(statu, divide, wp_map, pro_num, sn_map, First, ns_map); //初始项目的闭包，作为一个项目集合
	


	//set<LRitem> statu = inter_extend(make_pair(make_pair(startobj, 1), sn_map["#"])); //初始项目的闭包，作为一个项目集合
	unordered_set<int> after_set;

	dfa.push_back(statu);  // 存入DFA中，得到一个编号为0，因为位置为0号

	for (int i = 0; i < dfa.size(); i++)  //遍历每一个，直到DFA不再增大为止  push back 会自动放在数组最后一个
	{
		cout << dfa.size() << endl;

		after_set.clear();

		for (auto state1 : dfa[i])    //x是一个OBJ  first是产生式  second是·的位置
		{

			auto LRitem1 = state1.first;

			if (LRitem1.first.size() == LRitem1.second) continue;

			after_set.insert(LRitem1.first[LRitem1.second]);
		}

		for (auto t : after_set) outer_extend(i, t,dfa,  LRState,
			 noter,  pro,
			 First, terminal,
			 divide,wp_map,  pro_num,
			 sn_map,ns_map);
	}

	
	//下面为打印集簇部分
	/*
	for (int i = 0; i < dfa.size(); i++)
	{
		cout << i << endl;
		for (auto state1 : dfa[i])   //j为一个obj
		{
			auto LRitem1 = state1.first;
			vector<int> temp = LRitem1.first;
			cout << ns_map[temp[0]] << "->";
			for (int k = 1; k < temp.size(); k++)
			{
				if (k == LRitem1.second) cout << "点 ";
				cout << ns_map[temp[k]] << " ";
			}
			if (LRitem1.second == temp.size()) cout << "点 ";
			cout << "," << ns_map[state1.second] << endl;
		}
		cout << endl;
	}

	//下面为打印移进部分     
	for (auto i : LRState)
	{
		cout << i.first.first << "遇到" << ns_map[i.first.second] << "移进到" << i.second << endl;
	}

	*/
}



StateCore get_core(const DFAState& LRset) {
	StateCore rst;
	for (auto i : LRset) {
		rst.insert(i.first);
	}
	return rst;
}

// 合并同心项
void merge_LR1_sets_into_LALR_sets( DFA& dfa, DFATransitionTable& LRState) {
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
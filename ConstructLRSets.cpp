#include "pch.h"
#include "Common.h"

using namespace std;


extern map<int, unordered_set<int>> First;
extern ProducerVec pro_num;
extern int  divide;
extern unordered_set<int> terminal_num;
extern unordered_set<int> noter_num;

// DFA转换表, pair<项目编号, 终结符/非终结符>, 移进项目编号, i状态识别一个x后到达j状态.
extern DFATransitionTable LRState;

// 项目集 规范族
extern DFA dfa;

extern vector<int> startobj;

// 开始符
extern string start, definition, code;

extern unordered_map<string, int> sn_map;
extern unordered_map<int, string> ns_map;

extern map < vector<int>, int > r_num;
extern map<int, vector<int>> wp_map;

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

	/*
	string temp2 = "declarations";  //产生用于第一次内扩展的产生式
	startobj.push_back(sn_map[temp2]);
	temp2 = "declaration";
	startobj.push_back(sn_map[temp2]);

	temp2 = "declarations";
	startobj.push_back(sn_map[temp2]);

	*/

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

	//cout << "divide: --------------  " << divide << endl;

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

	//for (auto st : sn_map)
	//{
	//	cout << st.first << " -+-" << st.second << endl;
	//}




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

		//cout << "编号: " << i << "  ";
		//cout << ns_map[pro_num[i].first] << "->";
		//for (int j = 0; j < pro_num[i].second.size(); j++)
		//	cout << ns_map[pro_num[i].second[j]] << " ";
		//cout << endl;
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






void intersection(unordered_set<int>& leftSet, const unordered_set<int>& rightSet) {
	leftSet.insert(rightSet.cbegin(), rightSet.cend());
}



//两个set为空
void first_symbol(unordered_set<int>& inputSet, const int& symbol, unordered_set<int >& handlingSymbol) {
	if (First.find(symbol) != First.end()) {//之前求过了，直接加入，算是优化，不用做后面的事了
		intersection(inputSet, First[symbol]);
		return;
	}

	if (symbol == -1) return;                       //如果是-1那不求空集
	if (symbol < divide) {//symol是终结符
		inputSet.insert(symbol);
		return;
	}                 //直接加入
	else {//symbol是非终结符
		handlingSymbol.insert(symbol);    //这个已经处理过了
		for (int i = 0; i < pro_num.size(); i++)
		{
			if (pro_num[i].first == symbol)
			{            //从第first开始，之后又second个它的产生式
							   //对于所有symbol在左边的产生式
				auto producer = pro_num[i];   //是一个产生式
				if (producer.second.size() == 1 && producer.second[0] == -1) {//symbol->epsilon
					inputSet.insert(-1);   //返回空串
					continue;
				}
				//遍历产生式右边所有项
				int j = 1;
				unordered_set<int> tempSet;
				while (j <= producer.second.size()) {//不是最后一项，且其中有epsilon
					tempSet.clear();
					if (handlingSymbol.find(producer.second[j - 1]) != handlingSymbol.end()) {//防止左递归
						break;
					}
					first_symbol(tempSet, producer.second[j - 1], handlingSymbol);
					if (tempSet.find(-1) == tempSet.end()) {
						intersection(inputSet, tempSet);
						break;
					}
					if (j == producer.second.size()) {//最后一项还有epsilon
						inputSet.insert(-1);

						break;
					}
					tempSet.erase(-1);
					intersection(inputSet, tempSet);
					++j;
				}
			}
		}
	}
}


void first_operation() {
	unordered_set<int> firstSet, stackSet;

	for (int symbol : terminal_num) {
		firstSet.clear();
		stackSet.clear();
		first_symbol(firstSet, symbol, stackSet); //求单个first
		First.emplace(symbol, firstSet);  // symbol和他的first集
	}

	for (int symbol : noter_num) {
		firstSet.clear();
		stackSet.clear();
		first_symbol(firstSet, symbol, stackSet); //求单个first
		First.emplace(symbol, firstSet);  // symbol和他的first集
	}



}

void first_string(unordered_set<int>& inputSet, const vector<int>& symbolsVec) {


	int i = 1;
	if (symbolsVec.size() == 0) {
		inputSet.insert(-1);
		return;
	}
	while (i <= symbolsVec.size()) {//不是最后一项，且其中有epsilon
		auto & tempRef = First[symbolsVec[i - 1]];
		if (tempRef.find(-1) == tempRef.end()) {
			intersection(inputSet, tempRef);
			break;
		}
		if (i == symbolsVec.size()) {//最后一项还有epsilon
			inputSet.insert(-1);
			break;
		}
		tempRef.erase(-1);
		intersection(inputSet, tempRef);
		++i;
	}
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
				continue;
			}

			queue.pop();

			for (auto i : wp_map[s])    //找出这个非终结符的产生式
			{

				vector<int> temp = pro_num[i].second;   //产生式右向
				temp.insert(temp.begin(), s);
				vector<int> xx;
				//若项目[A->x·by,a]属于闭包ans,B->R是产生式
				//对first（Ra) 中的每个字符串b，若[B->r，b]不在ans中，加入
				xx.assign(item1.first.begin() + item1.second + 1, item1.first.end());
				//xx.push_back(lritem1.second);

				temp_first.clear();

				first_string(temp_first, xx);
				
				if (temp_first.find(-1) != temp_first.end()) {//有epsilon
					temp_first.erase(-1);
					temp_first.insert(lritem1.second);
				}
				
				/*
				cout <<"预测"<< endl;
				for (auto LRitem2 : temp_first)   //j为一个item
				{
						cout << ns_map[LRitem2] << " ";
				}
				cout << endl;
				*/
				

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




void outer_extend(int & pos, map<int, unordered_set<LRitem> >& newStateMap)
//尝试每一个项目集合每一个文法符号，看是否可能得到一个新项目
//如果可以就加入到集簇中   pos是项目编号， xx为所要尝试的在后面添加的终结符/非终结符
{
	unordered_set<LRitem> ans;
	LRitem newItem;
	int edge;
	for (const auto &state1 : dfa[pos])    //x是一个OBJ  first是产生式  second是·的位置
	{
		auto LRitem1 = state1.first;
		if (LRitem1.first.size() == LRitem1.second) continue;
		//如果已经到了最后一个位置了，说明是规约项目，不用后移

		edge = LRitem1.first[LRitem1.second];  //边上的符号

		newItem.first = LRitem1;
		newItem.first.second++;

		newItem.second = state1.second;

		auto findIt = newStateMap.find(edge);
		if (findIt == newStateMap.end()) {//没有遍历过该边，新建一个集合先
			unordered_set<LRitem> newState;
			newState.insert(newItem);//把这个新的加到集合里
			newStateMap.emplace(edge, newState);
		}
		else {//如果遍历过该边，把新的lritem加到已有的集合中
			findIt->second.insert(newItem);
		}
	}

	/*
	for (auto state1 : newStateMap)   //j为一个obj
	{
		auto LRitem1 = state1.first;
		cout << "符号是" << ns_map[state1.first] << endl;

		for (auto state1 : state1.second)   //j为一个obj
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
	}
	cout << "一个扩展的初始结束了" << endl;
	//system("pause");
	*/

}

void construct_LR1_sets()   //用来生成集簇之间的转换关系
{


	//vector<string> temp3;
	//temp3.push_back("#");

	//unordered_set<LRitem> statu = inter_extend(make_pair(make_pair(startobj, 1), sn_map["#"])); //初始项目的闭包，作为一个项目集合
	unordered_set<LRitem> statu;
	statu.insert(make_pair(make_pair(startobj,1), sn_map["#"]));
	statu = inter_extend(statu, divide, wp_map, pro_num, sn_map, First, ns_map); //初始项目的闭包，作为一个项目集合


	queue<int> unhandledStates;    //有一个没处理的状态
	unhandledStates.push(0);

	dfa.push_back(statu);  // 存入DFA中，得到一个编号为0，因为位置为0号

	//cout << "状态号------------: " << dfa.size() << endl;
	map<int, unordered_set<LRitem> > newStateMap;

	int stateNumCounter = 1;

	while (!unhandledStates.empty()) {

		
		int i = unhandledStates.front();
		unhandledStates.pop();

		newStateMap.clear();
		/*
		cout << "状态号: " << dfa.size() << endl;
		if (dfa.size() > 200) system("pause");
		*/

		outer_extend(i, newStateMap);
		//after_set.clear();
		//for (auto state1 : DFA[i])    //x是一个OBJ  first是产生式  second是·的位置
		//{
		//	auto LRitem1 = state1.first;
		//	if (LRitem1.first.size() == LRitem1.second) continue;
			//如果已经到了最后一个位置了，说明是规约项目，不用后移
			//after_set.insert(LRitem1.first[LRitem1.second]);
		//for (auto t : after_set)

		for (auto & p : newStateMap) {

			p.second=inter_extend(p.second, divide, wp_map, pro_num, sn_map, First, ns_map);
			int edgeToInt = -1;
			// 检查是否存在相同的状态
			for (const auto & s : dfa) {
				if (s == p.second ) {
					edgeToInt = find(dfa.begin(), dfa.end(), s) - dfa.begin(); //find返回在数组中的位置，减去起始位置得到编号  s.numberInt;
					
					/*
					for (auto state1 : s)   //j为一个obj
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
					cout << "===================" << endl;
					for (auto state1 : p.second)   //j为一个obj
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
					

					cout << "我是状态数" << edgeToInt << endl;
					*/
					break;


				}
			}
			if (edgeToInt == -1)//不存在要新建状态了
			{

				edgeToInt = stateNumCounter;
				unhandledStates.push(stateNumCounter);
				//cout << "我也是状态数" << edgeToInt << endl;
				stateNumCounter++;
				dfa.push_back(p.second);     //找到的不是已经存在的项目，那么加入DFA集合中
			}
			LRState[make_pair(i, p.first)] = edgeToInt;
			/*cout << dfa.size() << endl;*/
		}
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
#pragma once
#include "pch.h"

#ifndef COMMON_H
#define COMMON_H

// 原来的产生式（产生式左部，产生式右部)的序列
typedef std::vector<std::pair<std::string, std::vector<std::string>>> ProducerVecStr;

typedef std::vector<std::pair<int, std::vector<int> > > ProducerVec;

// 产生式以及·的位置，定义为一个项目, 注意vector的第一个元素为产生式左边符号
typedef std::pair<std::vector<int>, int> Item;

// LR(1)/LALR项目
typedef std::pair<Item, int> LRitem;

typedef std::unordered_set<LRitem> DFAState;

typedef std::vector<std::unordered_set<LRitem>> DFA;

typedef std::set<Item> StateCore;

typedef std::map<std::pair<int, int>, int> DFATransitionTable;


// 用来定义空串，可根据不同的.y文件调整
const std::string kong = "epsilon";



namespace std {

	template<>
	struct hash<LRitem> {
		inline size_t operator()(const LRitem& s) const {
			return  hash<int>()(s.second);
		}
	};
}



#endif // !COMMON_H

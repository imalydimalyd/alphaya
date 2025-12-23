#pragma once

#include "agent.hpp"

#include <istream>
#include <ostream>
#include <iostream>//patch
#include <random>
#include<vector>

namespace AlphaYa
{
	template <typename StateType>
	class RandomAgent : public Agent<StateType>
	{
	public:
		typedef StateType State;
		typedef typename State::Action Action;
		typedef typename State::Data Data;
		typedef std::mt19937::result_type SeedType;

	std::mt19937 rd;

	RandomAgent(SeedType seed):rd(seed){
		std::cout<<"seed "<<seed<<std::endl;//patch
	}
	
		Action move(const State &state,std::istream &in, std::ostream &out) 
		{
			std::vector<Action> actions=state.generateActions();
			return actions[std::uniform_int_distribution<IndexType>(0,actions.size()-1)(rd)];
		}
	};
};
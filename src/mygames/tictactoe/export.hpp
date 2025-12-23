#pragma once

#include "../../game/game.hpp"
#include "../../agent/agent.hpp"
#include "../../agent/agent_input.hpp"
#include "../../agent/agent_random.hpp"
#include "../../agent/agent_mcts.hpp"
#include "game.hpp"

#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace AlphaYaExport
{
	using AlphaYa::IndexType;
	using AlphaYa::ScoreType;
	// Game data
	typedef AlphaYa::TicTacToe::TicTacToeData Data;
	// Game state
	typedef AlphaYa::TicTacToe::TicTacToeState State;
	// Game action
	typedef AlphaYa::TicTacToe::TicTacToeAction Action;
	typedef AlphaYa::Agent<State> Agent;
	typedef AlphaYa::InputAgent<State> InputAgent;
	typedef AlphaYa::RandomAgent<State> RandomAgent;
	typedef AlphaYa::MCTSAgent<State> MCTSAgent;

	constexpr IndexType players = State::players;

	class AgentConstructor
	{
	public:
		std::string name;
		std::string description;
		std::function<std::unique_ptr<Agent>(const std::string &)> constructor;
		bool need_config;
		AgentConstructor(
			const std::string &n,
			const std::string &d,
			const std::function<std::unique_ptr<Agent>(const std::string &)> &c,
			bool nc = false) : name(n), description(d), constructor(c), need_config(nc) {}
	};

	/*
	Input agent: read action from input
	*/
	std::unique_ptr<Agent> input_agent(const std::string &config)
	{
		return std::make_unique<InputAgent>();
	}

	/*
	Random agent: randomly move
	*/
	std::unique_ptr<Agent> random_agent(const std::string &config)
	{
		RandomAgent::SeedType seed = 42;
		std::string argument;
		for (std::istringstream cfin(config);;)
		{
			cfin >> argument;
			if (cfin.fail())
			{
				break;
			}
			if (argument == "seed")
			{
				cfin >> seed;
				continue;
			}
		}
		return std::make_unique<RandomAgent>(seed);
	}

	/*
	MCTS agent: use MCTS algorithm
	*/
	std::unique_ptr<Agent> mcts_agent(const std::string &config)
	{
		MCTSAgent::SeedType seed = 42;
		MCTSAgent::EvalType c = 1.0;
		IndexType simulate_count = 10;
		std::string argument;
		for (std::istringstream cfin(config);;)
		{
			cfin >> argument;
			if (cfin.fail())
			{
				break;
			}
			if (argument == "seed")
			{
				cfin >> seed;
				continue;
			}
			if (argument == "c")
			{
				cfin >> c;
				continue;
			}
			if (argument == "scount")
			{
				cfin >> simulate_count;
				continue;
			}
		}
		return std::make_unique<MCTSAgent>(seed, c, simulate_count);
	}

	/*
	Agent constructors
	Format: AgentConstructor("name", "description", constructor, needconfig(bool))
	*/
	const AgentConstructor agent_constructors[] = {
		AgentConstructor("human", "You", input_agent),
		AgentConstructor("random", "Randomly moving bot", random_agent, true),
		AgentConstructor("ai", "AI using MCTS algorithm", mcts_agent, true),
	};

	/*
	Names of players
	*/
	const std::string player_names[players] = {"X", "O"};

	/*
	If allow_customize_state = true, user would be able to input a string as the initial state
	*/
	constexpr bool allow_customize_state = false;

	/*
	If allow_customize_state = true, default_state is the default initial state
	If allow_customize_state = false, default_state is the only possible initial state
	*/
	const std::string default_state = "";

	/*
	Controls whether agent for each player can be customized
	*/
	const bool allow_customize_agents[players] = {true, true};

	/*
	The default agent for each player
	*/
	const std::string default_agents[players] = {"ai", "human"};

	/*
	The default agent config for each player
	*/
	const std::string default_agent_config[players] = {"", ""};

	/*
	Help string
	This string will be printed when user executes ".help" command.
	Please put the game's name, rule author, programmer and rule into this string.
	Please use English if possible.
	*/
	const std::string help = "Tic-Tac-Toe\n"
							 "Code: Ya\n"
							 "\n"
							 "Classic tic-tac-toe game.\n";

	/*
	Record file prefix
	Record files will be saved as records/<prefix>_<time>.txt
	*/
	const std::string record_prefix = "tictactoe";
};
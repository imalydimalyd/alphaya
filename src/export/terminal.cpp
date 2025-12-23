#ifdef GAME_TIC_TAC_TOE
#include "../mygames/tictactoe/export.hpp"
#endif

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

//patch
void dbg(const AlphaYaExport::State &s){
for(size_t i=0;i<AlphaYaExport::State::byte_count;++i)std::cout<<(int)s.data.bytes[i]<<' ';
std::cout<<std::endl;
}

int main()
{
	using AlphaYaExport::Action;
	using AlphaYaExport::Agent;
	using AlphaYaExport::AgentConstructor;
	using AlphaYaExport::IndexType;
	using AlphaYaExport::ScoreType;
	using AlphaYaExport::State;

	using AlphaYaExport::agent_constructors;
	using AlphaYaExport::allow_customize_agents;
	using AlphaYaExport::allow_customize_state;
	using AlphaYaExport::default_agent_config;
	using AlphaYaExport::default_agents;
	using AlphaYaExport::default_state;
	using AlphaYaExport::player_names;
	using AlphaYaExport::players;
	using AlphaYaExport::record_prefix;

	std::istream &in = std::cin;
	std::ostream &out = std::cout;

	const auto input_agent = [&](IndexType player,std::string &name,std::string &config)
	{
		out << std::endl;
		out << "Available agents are:" << std::endl;
		for (const AgentConstructor &agent_constructor : agent_constructors)
		{
			out << agent_constructor.name;
			if (!agent_constructor.description.empty())
			{
				out << " (" << agent_constructor.description << ")";
			}
			out << std::endl;
		}
		bool customize = allow_customize_agents[player];
		for (;;)
		{
			if (customize)
			{
				out << "Please choose agent for " << player_names[player];
				if (!default_agents[player].empty())
				{
					out << " (default: " << default_agents[player] << ")";
				}
				out << ": ";
				std::getline(in, name);
				if (name.empty())
				{
					name = default_agents[player];
				}
			}
			else
			{
				name = default_agents[player];
				customize = true;
			}
			for (const AgentConstructor &agent_constructor : agent_constructors)
			{
				if (agent_constructor.name == name)
				{
					config = "";
					if (agent_constructor.need_config)
					{
						out << "Please input config for " << name;
						if (!default_agent_config[player].empty())
						{
							out << " (default: " << default_agent_config[player] << ")";
						}
						out << ": ";
						std::getline(in, config);
						if (config.empty())
						{
							config = default_agent_config[player];
						}
					}
					return agent_constructor.constructor(config);
				}
			}
			out << "Unknown agent: " << name << std::endl;
		}
	};

	out << "\033[0;32mOK! Input \".play\" and press Enter to play tic-tac-toe\033[0m" << std::endl; // patch
	for (std::string cmd;;)
	{
		std::getline(in, cmd);

		if (cmd == ".play")
		{
			std::string game_record_filename;
			{
				const std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    const std::time_t nowt = std::chrono::system_clock::to_time_t(now);
	std::ostringstream sout;
	sout<<"records/"<<record_prefix<<"_"<<std::put_time(std::localtime(&nowt),"%Y%m%d_%H_%M_%S")<<".txt";
	game_record_filename=sout.str();
			}
			std::ofstream fout(game_record_filename);
			fout<<"GAME"<<std::endl;
			fout<<record_prefix<<std::endl;
			fout<<"PLAYERS"<<std::endl;
			fout<<players<<std::endl;
			if(fout.fail()){
				out<<"WARNING: folder \"records\" not found, game will not be saved"<<std::endl;
				for(std::string confirm;;){
				out<<"Input \"confirm\" and press Enter to confirm this warning: ";
				std::getline(in,confirm);
				if(confirm=="confirm"){
					break;
				}
				}
			}

			std::string state_string;
			if (allow_customize_state)
			{
				out << "Please input game state";
				if (!default_state.empty())
				{
					out << " (default: " << default_state << ")";
				}
				out << ": ";
				std::getline(in, state_string);
				if (state_string.empty())
				{
					state_string = default_state;
				}
			}
			else
			{
				state_string = default_state;
			}

			std::vector<std::unique_ptr<Agent>> agents;
			agents.reserve(players);
			for (IndexType player = 0; player < players; ++player)
			{
				std::string name,config;
				agents.push_back(input_agent(player,name,config));
			fout<<"PLAYER"<<std::endl;
			fout<<player<<std::endl;
			fout<<name<<std::endl;
			fout<<config<<std::endl;
			}

			State state;
			state.init(state_string);
			out << std::endl;
			out << "Game start!" << std::endl;
			out << std::endl;
			state.output(out, "terminal");dbg(state);//patch

			fout<<"INIT"<<std::endl;
			state.output(fout,"");
			fout<<std::endl;

			ScoreType scores[players];
			for (; !state.calculateScore(scores);)
			{
				const IndexType player = state.toMove();
				//patch
				//agents[player]->setState(state);
				out << std::endl;
				out << "It is " << player_names[player] << "'s turn" << std::endl;
				const Action action = agents[player]->move(state,in, out);
				state.move(action);
				out << std::endl;
				state.output(out, "terminal");dbg(state);//patch

			fout<<"STEP"<<std::endl;
			fout<<player<<std::endl;
			action.output(fout);
			fout<<std::endl;
			state.output(fout,"");
			fout<<std::endl;
			}
			out << std::endl;
			out << "Game over! Score:" << std::endl;
			for (IndexType player = 0; player < players; ++player)
			{
				out << "Score of " << player_names[player] << ": " << scores[player] << std::endl;
			}
			out << std::endl;
			fout<<"SCORE"<<std::endl;
			for (IndexType player = 0; player < players; ++player)
			{
				fout <<  scores[player] << std::endl;
			}
			if(!fout.fail()){
				out<<"Game record saved to "<<game_record_filename<<std::endl;
			}
			fout.close();
			continue;
		}

		if (cmd == ".help")
		{
			using AlphaYaExport::help;
			out << std::endl;
			out << help << std::endl;
			out << "Players: " << players << std::endl;
			for (IndexType player = 0; player < players; ++player)
			{
				out << (player + 1) << ". " << player_names[player] << std::endl;
			}
			out << std::endl;
			continue;
		}

		if (cmd == ".exit")
		{
			break;
		}

		if (cmd == "..ping")
		{
			out << "pong" << std::endl;
			continue;
		}

		if (cmd == "..size")
		{
			out << sizeof(State) * 8 << std::endl;
			continue;
		}

		out << std::endl;
		out << "Unknown command: " << cmd << std::endl;
		out << "Available commands are: .play .help .exit" << std::endl;
		out << "Please read README.md for more information" << std::endl;
		out << std::endl;
	}
	return 0;
}
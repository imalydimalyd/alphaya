#pragma once

#include "../../game/game.hpp"

#include <cstdint>
#include <vector>

namespace AlphaYa
{
	namespace TicTacToe
	{
		/*
		All data of game state
		It is recommended to use multiple bitboards to represent the board,
		and to represent integers in unary numeral system
		(0b0, 0b1, 0b11, 0b111, ...).
		See: https://www.chessprogramming.org/Bitboard_Board-Definition
		*/
		class TicTacToeData
		{
		public:
			std::uint16_t bitboard0, bitboard1;
			std::uint8_t side;
		};

		/*
		Game action
		*/
		class TicTacToeAction : public Action
		{
		public:
			std::uint8_t position;
			TicTacToeAction(std::uint8_t p = 0) : position(p) {}

			/*
			Output action as a short string to out, which does not contain endl
			*/
			void output(std::ostream &out) const
			{
				out << (char)('a' + position % 3) << (char)('1' + position / 3);
			}

			/*
			Check if two actions are equal
			*/
			bool operator==(const TicTacToeAction &o) const
			{
				return position==o.position;
			}
		};

		static const std::uint16_t lines[] = {
			0b000000111,
			0b000111000,
			0b111000000,

			0b001001001,
			0b010010010,
			0b100100100,

			0b001010100,
			0b100010001,
		};

		/*
		Game state
		Template arguments are: <# of players, data type, action type>.
		*/
		class TicTacToeState : public State<2, TicTacToeData, TicTacToeAction>
		{
		public:
		/*
		Returns id of the current player
		*/
			IndexType toMove() const
			{
				const TicTacToeData &data=getData();
				return data.side;
			}

			/*
			Returns actions that the current can make
			*/
			std::vector<TicTacToeAction> generateActions() const
			{
				const TicTacToeData &data=getData();
				const std::uint16_t bitboard = data.bitboard0 | data.bitboard1;
				std::vector<TicTacToeAction> actions;
				actions.reserve(9);
				for (IndexType position = 0; position < 9; ++position)
				{
					if (!(bitboard >> position & 1))
					{
						actions.emplace_back(position);
					}
				}
				return actions;
			}

			/*
			Modifies the data according to action
			*/
			void move(const TicTacToeAction &action)
			{
				 TicTacToeData &data=getData();
				const std::uint16_t mask = (((std::uint16_t)1) << action.position);
				data.side ? (data.bitboard1 |= mask) : (data.bitboard0 |= mask);
				data.side ^= 1;
			}

			/*
			Calculate score if game over
			If game is over, write the score of each player into scores, and return true.
			If game is not over, do not modify scores, and return false.
			*/
			bool calculateScore(ScoreType scores[2]) const
			{
				const TicTacToeData &data=getData();
				for (const std::uint16_t line : lines)
				{
					if ((data.bitboard0 & line) == line)
					{
						scores[0] = 1;
						scores[1] = -1;
						return true;
					}
				}
				for (const std::uint16_t line : lines)
				{
					if ((data.bitboard1 & line) == line)
					{
						scores[0] = -1;
						scores[1] = 1;
						return true;
					}
				}
				if (((data.bitboard0 | data.bitboard1) & 511) == 511)
				{
					scores[0] = 0;
					scores[1] = 0;
					return true;
				}
				return false;
			}

			/*
			Init the game state from ANY string
			*/
			void init(const std::string &state_string)
			{
				 TicTacToeData &data=getData();
				data.bitboard0 = 0;
				data.bitboard1 = 0;
				IndexType count0 = 0, count1 = 0;
				for (IndexType i = 0; i < 9; ++i)
				{
					if (i >= state_string.length())
					{
						break;
					}
					if (state_string[i] == 'X')
					{
						data.bitboard0 |= (((std::uint16_t)1) << i);
						++count0;
					}
					else if (state_string[i] == 'O')
					{
						data.bitboard1 |= (((std::uint16_t)1) << i);
						++count1;
					}
				}
				data.side = (count1 < count0) ? 1 : 0;
			}

			/*
			Output the game state to out using method
			The possible methods are:
			terminal: output pretty-printed board
			(other value): output a short string, which does not contain endl, and can be read by init(std::string)
			*/
			void output(std::ostream &out, const std::string &method) const
			{
				const TicTacToeData &data=getData();
				if (method == "terminal")
				{
					out << "\033[0;90m    a   b   c \033[0m" << std::endl;
					out << "              " << std::endl;
					for (IndexType i = 0; i < 3; ++i)
					{
						if (i)
						{
							out << "   ---+---+---" << std::endl;
						}
						out << "\033[0;90m" << (i + 1) << "\033[0m  ";
						for (IndexType j = 0; j < 3; ++j)
						{
							if (j)
							{
								out << "|";
							}
							IndexType p = i * 3 + j;
							if (data.bitboard0 >> p & 1)
							{
								out << " \033[31mX\033[0m ";
							}
							else if (data.bitboard1 >> p & 1)
							{
								out << " \033[36mO\033[0m ";
							}
							else
							{
								out << "   ";
							}
						}
						out << std::endl;
					}
					return;
				}
				for (IndexType i = 0; i < 9; ++i)
				{
					if (data.bitboard0 >> i & 1)
					{
						out << "X";
					}
					else if (data.bitboard1 >> i & 1)
					{
						out << "O";
					}
					else
					{
						out << ".";
					}
				}
			}
		};
	};
};
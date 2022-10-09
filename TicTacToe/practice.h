#pragma once
#define DOCTEST_CONFIG_IMPLEMENT

#include "doctest.h"
#include <vector>
#include <algorithm>
#include <functional>
#include <numeric>
#include <optional>

using namespace std;
using namespace std::placeholders;

namespace tictactoe {
	/*
		clone coding
			: 
			examples in Hands-On Functional Programming with C++ (Alexandru Bolboaca)
	*/

	static int test(int argc, char **argv) {
		doctest::Context context;

		// defaults
		context.addFilter("test-case-exclude", "*math*"); // exclude test cases with "math" in the name
		context.setOption("rand-seed", 324);              // if order-by is set to "rand" use this seed
		context.setOption("order-by", "file");            // sort the test cases by file and line

		context.applyCommandLine(argc, argv);

		// overrides
		context.setOption("no-breaks", true); // don't break in the debugger when assertions fail

		int res = context.run(); // run queries, or run tests unless --no-run is specified

		if (context.shouldExit()) // important - query flags (and --exit) rely on the user doing this
			return res;          // propagate the result of the tests

		context.clearFilters(); // removes all filters added up to this point

		return 0;
	}

	TEST_SUITE("practice_tictactoe_clone_coding")
	{
		auto trueForAll = [](auto x) { return true; };
		auto falseForAll = [](auto x) { return false; };
		auto EqualsCharA = [](auto x) { return x == 'a'; };

		TEST_CASE("all_of") {
			vector<char> abc{ 'a', 'b', 'c' };

			CHECK(all_of(abc.begin(), abc.end(), trueForAll));
			CHECK(!all_of(abc.begin(), abc.end(), falseForAll));
			CHECK(!all_of(abc.begin(), abc.end(), EqualsCharA));
		}

		auto all_of_collection = [](const auto& collection, auto lambda) {
			return all_of(collection.begin(), collection.end(), lambda);
		};

		TEST_CASE("all_of_collection") {
			vector<char> abc{ 'a', 'b', 'c' };

			CHECK(all_of_collection(abc, trueForAll));
			CHECK(!all_of_collection(abc, falseForAll));
			CHECK(!all_of_collection(abc, EqualsCharA));
		}

		auto lineFilledWithX = [](const auto& line) {
			return all_of_collection(line, [](const auto& token) { return token == 'X'; });
		};

		TEST_CASE("Line filled with X") {
			vector<char> line{ 'X', 'X', 'X' };

			CHECK(lineFilledWithX(line));
		}

		using Line = vector<char>;

		TEST_CASE("Line not filled with X") {
			CHECK(!lineFilledWithX(Line{ 'X', 'O', 'X' }));
			CHECK(!lineFilledWithX(Line{ 'X', ' ', 'X' }));
		}

		auto lineFilledWith = [](const auto line, const auto targetToken) {
			return all_of_collection(line, [&targetToken](const auto token) {
				return token == targetToken;
				}
			);
		};

	/*	auto lineFilledWithX = bind(lineFilledWith, _1, 'X');
		auto lineFilledWithY = bind(lineFilledWith, _1, 'O');*/

		TEST_CASE("transform") {
			vector<char> abc{ 'a', 'b', 'c' };

			vector<char> aaa(3);
			transform(abc.begin(), abc.end(), aaa.begin(), [](auto element) { return 'a'; });
			CHECK_EQ(vector<char>{'a', 'a', 'a'}, aaa);
		}

		TEST_CASE("transform-fixed") {
			const auto abc = { 'a', 'b', 'c' };
			vector<char> aaa;
			aaa.reserve(abc.size());
			transform(abc.begin(), abc.end(), back_inserter(aaa), [](const char elem) { return 'a'; });
			CHECK_EQ(vector<char>{ 'a', 'a', 'a' }, aaa);
		}

		template<typename Destination>
		auto transformAll = [](const auto source, auto lambda) {
			Destination result;
			result.reserve(source.size());
			transform(source.begin(), source.end(), back_inserter(result), lambda);
			return result;
		};

		auto turnAllToa = [](auto x) { return 'a'; };

		TEST_CASE("Transform All") {
			vector<char> abc{ 'a', 'b', 'c' };
			CHECK_EQ(vector<char>({ 'a', 'a', 'a' }), transformAll<vector<char>>(abc, turnAllToa));
			CHECK_EQ("aaa", transformAll<string>(abc, turnAllToa));
		}

		auto makeCaps = [](auto x) { return toupper(x); };

		TEST_CASE("transform all") {
			vector<char> abc = { 'a', 'b', 'c' };

			CHECK_EQ("ABC", transformAll<string>(abc, makeCaps));
		}

		auto toNumber = [](auto x) { return (int)x - 'a' + 1; };

		TEST_CASE("transform all") {
			vector<char> abc = { 'a','b','c' };
			vector<int> expected = { 1,2,3 };

			CHECK_EQ(expected, transformAll<vector<int>>(abc, toNumber));
		}

		TEST_CASE("any_of") {
			vector<char> abc = { 'a','b','c' };

			CHECK(any_of(abc.begin(), abc.end(), trueForAll));
			CHECK(!any_of(abc.begin(), abc.end(), falseForAll));
			CHECK(any_of(abc.begin(), abc.end(), EqualsCharA));
		}

		auto any_of_collection = [](const auto& collection, const auto& fn) {
			return any_of(collection.begin(), collection.end(), fn);
		};

		TEST_CASE("any_of_collection") {
			vector<char> abc = { 'a','b','c' };

			CHECK(any_of_collection(abc, trueForAll));
			CHECK(!any_of_collection(abc, falseForAll));
			CHECK(any_of_collection(abc, EqualsCharA));
		}
	}

	TEST_SUITE("practice_tictactoe_board") {
		template<typename Destination>
		auto transformAll = [](const auto source, auto lambda) {
			Destination result;
			result.reserve(source.size());
			transform(source.begin(), source.end(), back_inserter(result), lambda);
			return result;
		};

		using Coordinate = pair<int, int>;
		using Line = vector<char>;
		using Board = vector<Line>;

		auto accessAtCoordinates = [](const auto& board, const Coordinate& coordinate) {
			return board[coordinate.first][coordinate.second];
		};

		auto projectCoordinates = [](const auto& board, const auto& coordinates) {
			auto boardElementFromCoordinates = bind(accessAtCoordinates, board, _1);
			return transformAll<Line>(coordinates, boardElementFromCoordinates);
		};

		auto lineCoordinates = [](const auto board, auto lineIndex) {
			vector<int> range{ 0, 1, 2 };
			return transformAll<vector<Coordinate>>(range, [lineIndex](auto index) {
				return make_pair(lineIndex, index);
			});
		};

		auto toRange = [](auto const collection) {
			vector<int> range(collection.size());
			iota(begin(range), end(range), 0);
			return range;
		};

		auto line = [](auto board, int lineIndex) {
			return projectCoordinates(board, lineCoordinates(board, lineIndex));
		};

		TEST_CASE("lines") {
			Board board{
				{'X', 'X', 'X'},
				{' ', 'O', ' '},
				{' ', ' ', 'O'}
			};

			Line expectedLine0 = { 'X', 'X', 'X' };
			CHECK_EQ(expectedLine0, line(board, 0));
			Line expectedLine1 = { ' ', 'O', ' ' };
			CHECK_EQ(expectedLine1, line(board, 1));
			Line expectedLine2 = { ' ', ' ', 'O' };
			CHECK_EQ(expectedLine2, line(board, 2));
		}

		auto columnCoordinates = [](const auto& board, const auto columnIndex) {
			auto range = toRange(board);
			return transformAll<vector<Coordinate>>(range, [columnIndex](const auto index) {
				return make_pair(index, columnIndex);
			});
		};

		auto column = [](auto board, auto columnIndex) {
			return projectCoordinates(board, columnCoordinates(board, columnIndex));
		};

		TEST_CASE("all columns") {
			Board board{
				{'X' ,'X', 'X'},
				{' ', 'O', ' '},
				{' ', ' ', 'O'}
			};

			Line expectedColumn0 = { 'X', ' ', ' ' };
			CHECK_EQ(expectedColumn0, column(board, 0));
			Line expectedColumn1 = { 'X', 'O', ' ' };
			CHECK_EQ(expectedColumn1, column(board, 1));
			Line expectedColumn2 = { 'X', ' ', 'O' };
			CHECK_EQ(expectedColumn2, column(board, 2));
		}

		auto mainDiagonalCoordinates = [](const auto board) {
			auto range = toRange(board);
			return transformAll<vector<Coordinate>>(range, [](auto index) {
				return make_pair(index, index);
			});
		};

		auto mainDiagonal = [](const auto board) {
			return projectCoordinates(board, mainDiagonalCoordinates(board));
		};

		TEST_CASE("main diagonal") {
			Board board{
				{'X', 'X', 'X'},
				{' ', 'O', ' '},
				{' ', ' ', 'O'}
			};

			Line expectedDiagonal = { 'X', 'O', 'O' };

			CHECK_EQ(expectedDiagonal, mainDiagonal(board));
		}

		auto secondaryDiagonalCoordinates = [](const auto board) {
			auto range = toRange(board);
			return transformAll<vector<Coordinate>>(range, [board](auto index) {
				return make_pair(index, board.size() - index - 1);
			});
		};

		auto secondaryDiagonal = [](const auto board) {
			return projectCoordinates(board, secondaryDiagonalCoordinates(board));
		};

		TEST_CASE("secondary diagonal") {
			Board board{
				{'X', 'X', 'X'},
				{' ', 'O', ' '},
				{' ', ' ', 'O'}
			};

			Line expectedDiagonal = {'X', 'O', ' '};

			CHECK_EQ(expectedDiagonal, secondaryDiagonal(board));
		}

		using Lines = vector<Line>;

		auto allLines = [](auto board) {
			auto range = toRange(board);
			return transformAll<Lines>(range, [board](auto index) {
				return line(board, index);
			});
		};

		auto allColumns = [](auto board) {
			auto range = toRange(board);
			return transformAll<Lines>(range, [board](auto index) {
				return column(board, index);
			});
		};

		auto allDiagonals = [](auto board) -> Lines {
			return { mainDiagonal(board), secondaryDiagonal(board) };
		};

		auto concatenate = [](auto first, const auto second) {
			auto result(first);
			result.insert(result.end(), std::make_move_iterator(second.begin()), std::make_move_iterator(second.end()));
			return result;
		};

		auto concatenate3 = [](auto first, auto const second, auto const third) {
			return concatenate(concatenate(first, second), third);
		};

		auto allLinesColumnsAndDiagonals = [](const auto board) {
			return concatenate3(allLines(board), allColumns(board), allDiagonals(board));
		};

		TEST_CASE("all lines, columns, and diagonals") {
			Board board{
				{'X', 'X', 'X'},
				{' ', 'O', ' '},
				{' ', ' ', 'O'}
			};

			Lines expected{
				{'X', 'X', 'X'},
				{' ', 'O', ' '},
				{' ', ' ', 'O'},
				{'X', ' ', ' '},
				{'X', 'O', ' '},
				{'X', ' ', 'O'},
				{'X', 'O', 'O'},
				{'X', 'O', ' '}
			};

			auto all = allLinesColumnsAndDiagonals(board);
			CHECK_EQ(expected, all);
		}

		auto all_of_collection = [](const auto& collection, auto lambda) {
			return all_of(collection.begin(), collection.end(), lambda);
		};

		auto lineFilledWith = [](const auto line, const auto targetToken) {
			return all_of_collection(line, [&targetToken](const auto token) {
				return token == targetToken;
				}
			);
		};

		auto any_of_collection = [](const auto& collection, const auto& fn) {
			return any_of(collection.begin(), collection.end(), fn);
		};

		auto lineFilledWithX = bind(lineFilledWith, _1, 'X');
		auto lineFilledWithO = bind(lineFilledWith, _1, 'O');

		auto xWins = [](const auto& board) {
			return any_of_collection(allLinesColumnsAndDiagonals(board), lineFilledWithX);
		};

		TEST_CASE("X wins") {
			Board board{
				{'X', 'X', 'X'},
				{' ', 'O', ' '},
				{' ', ' ', 'O'},
			};

			CHECK(xWins(board));
		}

		auto lineToString = [](const auto& line) {
			return transformAll<string>(line, [](const auto token) -> char {
				return token;
				});
		};

		TEST_CASE("line to string") {
			Line line{ ' ', 'X', 'O' };

			CHECK_EQ(" XO", lineToString(line));
		}

		auto boardToLinesString = [](const auto board) {
			return transformAll<vector<string>>(board, lineToString);
		};

		TEST_CASE("board to string") {
			Board board{
				{'X', 'X', 'X'},
				{' ', 'O', ' '},
				{' ', ' ', 'O'}
			};

			vector<string> expected{
				"XXX",
				" O ",
				"  O"
			};

			CHECK_EQ(expected, boardToLinesString(board));
		}

		TEST_CASE("accumulate") {
			vector<int> values = { 1, 22, 23, 45 };

			auto add = [](int first, int second) { return first + second; };
			int result = accumulate(values.begin(), values.end(), 0, add);
			CHECK_EQ(1 + 22 + 23 + 45, result);
		}

		TEST_CASE("accumulate string") {
			vector<string> strings{ "Alex", "is", "here" };
			auto concatenate = [](const string& first, const string& second) -> string {
				return first + second;
			};
			string concatenated = accumulate(strings.begin(), strings.end(), string(), concatenate);
			CHECK_EQ("Alexishere", concatenated);
		}

		auto accumulateAll = [](auto source, auto lambda) {
			return accumulate(source.begin(), source.end(), typename decltype(source)::value_type(), lambda);
		};

		auto boardToString = [](const auto board) {
			auto linesAsString = boardToLinesString(board);
			return accumulateAll(linesAsString, [](string current, string lineAsString) {
				return current + lineAsString + "\n";
			});
		};

		TEST_CASE("board to lines string") {
			Board board{
				{'X', 'X', 'X'},
				{' ', 'O', ' '},
				{' ',' ','O'}
			};
			string expected = "XXX\n O \n  O\n";
			CHECK_EQ(expected, boardToString(board));
		}


		auto equals1 = [](auto value) { return value == 1; };
		auto greaterThan11 = [](auto value) { return value > 11; };
		auto greaterThan50 = [](auto value) { return value > 50; };

		TEST_CASE("fid if") {
			vector<int> values{ 1, 12, 23, 45 };

			auto result1 = find_if(values.begin(), values.end(), equals1);
			CHECK_EQ(*result1, 1);

			auto result12 = find_if(values.begin(), values.end(), greaterThan11);
			CHECK_EQ(*result12, 12);

			auto resultNotFound = find_if(values.begin(), values.end(), greaterThan50);
			CHECK_EQ(resultNotFound, values.end());
		}

		auto findInCollection = [](const auto& collection, auto fn) {
			auto result = find_if(collection.begin(), collection.end(), fn);
			return (result == collection.end()) ? nullopt : optional(*result);
		};

		TEST_CASE("find in collection") {
			vector<int> values{ 1,12,23,45 };

			auto result1 = findInCollection(values, equals1);
			CHECK_EQ(result1, 1);

			auto result12 = findInCollection(values, greaterThan11);
			CHECK_EQ(result12, 12);

			auto resultNotFound = findInCollection(values, greaterThan50);
			CHECK(!resultNotFound.has_value());
		}

		auto howDidXWin = [](const auto& board) {

			map<string, Line> linesWithDescription{
				{"first line", line(board, 0) },
				{"second line", line(board, 1) },
				{"last line", line(board, 2) },
				{"first column", column(board, 0) },
				{"second column", column(board, 1) },
				{"last column", column(board, 2) },
				{"main diagonal", mainDiagonal(board) },
				{"secondary diagonal", secondaryDiagonal(board) },
			};

			auto found = findInCollection(linesWithDescription, [](auto value) {
				return lineFilledWithX(value.second);
			});

			return found.has_value() ? found->first : "X did not win";
		};

		auto oWins = [](const auto& board) {
			return any_of_collection(allLinesColumnsAndDiagonals(board), lineFilledWithO);
		};

		TEST_CASE("O wins") {
			Board board{
				{'X', 'O', 'X'},
				{' ', 'O', ' '},
				{' ', 'O', 'X'}
			};

			CHECK(oWins(board));
		}

		auto noneOf = [](const auto& collection, auto fn) {
			return none_of(collection.begin(), collection.end(), fn);
		};

		auto isEmpty = [](const auto token) { return token == ' '; };
		auto fullLine = [](const auto& line) {
			return noneOf(line, isEmpty);
		};

		auto full = [](const auto& board) {
			return all_of_collection(board, fullLine);
		};

		auto draw = [](const auto& board) {
			return full(board) && !xWins(board) && !oWins(board);
		};

		TEST_CASE("draw") {
			Board board{
				{'X','O','X'},
				{'O','O','X'},
				{'X','X','O'}
			};

			CHECK(draw(board));
		}

		auto inProgress = [](const auto& board) {
			return !full(board) && !xWins(board) && !oWins(board);
		};

		TEST_CASE("in progress") {
			Board board{
				{'X','O','X'},
				{'O',' ','X'},
				{'X','X','O'}
			};

			CHECK(inProgress(board));
		}

		auto findInCollectionWithDefault = [](auto collection, auto defaultResult, auto lambda) {
			auto result = findInCollection(collection, lambda);
			return result.has_value() ? (*result) : defaultResult;
		};

		auto howDidXWin2 = [](const auto board) {

			map<string, Line> linesWithDescription{
				{"first line", line(board, 0) },
				{"second line", line(board, 1) },
				{"last line", line(board, 2) },
				{"first column", column(board, 0) },
				{"second column", column(board, 1) },
				{"last column", column(board, 2) },
				{"main diagonal", mainDiagonal(board) },
				{"secondary diagonal", secondaryDiagonal(board) },
			};

			auto xDidNotWin = make_pair("X did not win", Line());
			auto xWon = [](auto value) {
				return lineFilledWithX(value.second);
			};

			return findInCollectionWithDefault(linesWithDescription, xDidNotWin, xWon).first;
		};
 	}

	TEST_SUITE("remove overlapping-and-similiarity") {
		template<typename Destination>
		auto transformAll = [](const auto source, auto lambda) {
			Destination result;
			result.reserve(source.size());
			transform(source.begin(), source.end(), back_inserter(result), lambda);
			return result;
		};

		using Coordinate = pair<int, int>;
		using Line = vector<char>;
		using Board = vector<Line>;
		using Lines = vector<Line>;

		auto allLines = [](auto board) {
			auto range = toRange(board);
			return transformAll<Lines>(range, [board](auto index) {
				return line(board, index);
				});
		};

		auto allColumns = [](auto board) {
			auto range = toRange(board);
			return transformAll<Lines>(range, [board](auto index) {
				return column(board, index);
				});
		};

		auto allDiagonals = [](auto board) -> Lines {
			return { mainDiagonal(board), secondaryDiagonal(board) };
		};

		auto concatenate = [](auto first, const auto second) {
			auto result(first);
			result.insert(result.end(), std::make_move_iterator(second.begin()), std::make_move_iterator(second.end()));
			return result;
		};

		auto concatenate3 = [](auto first, auto const second, auto const third) {
			return concatenate(concatenate(first, second), third);
		};

		auto allLinesColumnsAndDiagonals = [](const auto board) {
			return concatenate3(allLines(board), allColumns(board), allDiagonals(board));
		};

		auto any_of_collection = [](const auto& collection, const auto& fn) {
			return any_of(collection.begin(), collection.end(), fn);
		};

		auto lineFilledWith = [](const auto& line, const auto tokenToCheck) {
			return all_of_collection(line, [&tokenToCheck](const auto token) {
				return token == tokenToCheck;
			});
		};

		// use lambda? 
		auto lineFilledWithXLambda = [](const auto& line) {
			return lineFilledWith(line, 'X');
		};

		// partial application
		auto lineFilledWithX = bind(lineFilledWith, _1, 'X');
		auto lineFilledWithO = bind(lineFilledWith, _1, 'O');

		auto tokenWins = [](const auto& board, const auto& token) {
			return any_of_collection(
				allLinesColumnsAndDiagonals(board),
				[token](auto line) {
					return lineFilledWith(line, token);
				});
		};

		auto xWins = [](auto const board) {
			return tokenWins(board, 'X');
		};

		auto oWins = [](auto const board) {
			return tokenWins(board, 'O');
		};

		auto xWinsApp = bind(tokenWins, _1, 'X');
		auto oWinsApp = bind(tokenWins, _1, 'O');

		auto tokenWinsApp = [](const auto& board, const auto& token) {
			return any_of_collection(
				allLinesColumnsAndDiagonals(board),
				bind(lineFilledWith, _1, token)
			);
		};

		template<typename F, typename G, typename H>
		auto foo(F f, G g, H h) {
			return [=](auto first, auto second) {
				return f(g(first), bind(h, _1, second));
			};
		}

		auto tokenWinsFoo = foo(any_of_collection, allLinesColumnsAndDiagonals, lineFilledWith);

		template<typename CollectionBooleanOperation, typename CollectionProvider, typename Predicate>
		auto booleanOperationOnProvidedCollection(
			CollectionBooleanOperation collectionBooleanOperation,
			CollectionProvider collectionProvider,
			Predicate predicate) {
			return [=](auto collectionProviderSeed, auto predicateFirstParameter) {
				return collectionBooleanOperation(collectionProvider(collectionProviderSeed),
					bind(predicate, _1, predicateFirstParameter));
			};
		}

		enum Result {
			XWins,
			OWins,
			GameNotOverYet,
			Draw
		};

		enum Token {
			X,
			O,
			Blank
		};

		using CLine = vector<Token>;

		class CBoard {
		private:
			const vector<CLine> _board;

		public:
			CBoard() : _board{ CLine(3, Token::Blank), CLine(3,Token::Blank), CLine(3, Token::Blank) }
			{}
			
			CBoard(const vector<CLine>& initial) : _board(initial) {}

			bool anyLineFilledWith(const Token& token) const {
				for (int i = 0; i < 3; ++i) {
					if (_board[i][0] == token && _board[i][1] == token && _board[i][2] == token) {
						return true;
					}
				}

				return false;
			}

			bool anyColumnFilledWith(const Token& token) const {
				for (int i = 0; i < 3; ++i) {
					if (_board[0][i] == token && _board[1][i] == token && _board[2][i] == token) {
						return true;
					}
				}
				
				return false;
			}

			bool anyDiagonalFilledWith(const Token& token) const {
				
				bool diagonal = true;
				for (int i = 0; i < 3; ++i) {
					if (_board[i][i] != token) {
						diagonal = false;
						break;
					}
				}

				if (diagonal) {
					return true;
				}

				for (int i = 0; i < 3; ++i) {
					if (_board[i][3 - i] != token) {
						return false;
					}
				}
				return true;
			}

			bool notFilledYet() const {
				for (int i = 0; i < 3; ++i) {
					for (int j = 0; j < 3; ++j) {
						if (_board[i][j] == Token::Blank) {
							return true;
						}
					}
				}
				return false;
			}
		};

		auto tokenWinsDelegate = [](const auto& board, const auto& token) {
			return board.anyLineFilledWith(token) ||
				board.anyColumnFilledWith(token) ||
				board.anyDiagonalFilledWith(token);
		};

		auto xWinsDelegate = bind(tokenWinsDelegate, _1, Token::X);
		auto oWinsDelegate = bind(tokenWinsDelegate, _1, Token::O);
		auto gameNotOverYetDelegate = [](const auto& board) {
			return board.notFilledYet();
		};

		auto True = []() {
			return true;
		};

		using Rule = pair<function<bool()>, Result>;

		auto condition = [](auto rule) {
			return rule.first();
		};

		auto result = [](auto rule) {
			return rule.second;
		};

		auto findTheRule = [](const auto& rules) {
			return *find_if(rules.begin(), rules.end(), [](auto rule) {
				return condition(rule);
			});
		};

		auto resultForfirstRuleThatApplies = [](auto rules) {
			return result(findTheRule(rules));
		};

		Result winner(const CBoard& board) {
			auto gameNotOverYetOnBoard = bind(gameNotOverYetDelegate, board);
			auto xWinsOnBoard = bind(xWinsDelegate, board);
			auto oWinsOnBoard = bind(oWinsDelegate, board);
			
			vector<pair<function<bool()>, Result>> rules = {
				{xWinsOnBoard, XWins},
				{oWinsOnBoard, OWins},
				{gameNotOverYetOnBoard, GameNotOverYet},
				{True, Draw}
			};

			auto theRule = find_if(rules.begin(), rules.end(), [](auto pair) {
				return pair.first();
			});

			return theRule->second;
		}

		Result winner2(const CBoard& board) {
			auto gameNotOverYetOnBoard = bind(gameNotOverYetDelegate, board);
			auto xWinsOnBoard = bind(xWinsDelegate, board);
			auto oWinsOnBoard = bind(oWinsDelegate, board);

			vector<Rule> rules = {
				{xWinsOnBoard, XWins},
				{oWinsOnBoard, OWins},
				{gameNotOverYetOnBoard, GameNotOverYet},
				{True, Draw}
			};

			auto theRule = find_if(rules.begin(), rules.end(), [](auto pair) {
				return pair.first();
			});

			return resultForfirstRuleThatApplies(rules);
		}
	}
}
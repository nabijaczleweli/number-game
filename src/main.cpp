// The MIT License (MIT)

// Copyright (c) 2015 nabijaczleweli

//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.


#include <sqlite3.h>
#include <functional>
#include <iostream>
#include <cstdlib>
#include <random>
#include <string>
#include <vector>
#include <mutex>


using namespace std;


struct db_t {
	sqlite3 * db;

	db_t() : db(nullptr) {
		sqlite3_open("number-game.db", &db);
		sqlite3_exec(db, "CREATE TABLE scores(name string, score int);", nullptr, nullptr, nullptr);
	}

	~db_t() {
		sqlite3_close(db);
	}

	operator sqlite3 *() {
		return db;
	}
};


void play_game(db_t & db) {
	int desired        = 420;
	int guessed        = desired - 1;
	unsigned int tries = 0;
	cout << desired << '\n';

	while(guessed != desired) {
		++tries;

		bool valid = false;
		while(!valid) {
			cout << "Enter the number: ";
			valid = static_cast<bool>(cin >> guessed);
			cin.clear();
			cin.ignore();
			if(!valid)
				cout << "That ain't no number!\n";
		}
		if(guessed != desired) {
			cout << "Incorrect! Your number is too ";
			if(guessed < desired)
				cout << "small";
			else if(guessed > desired)
				cout << "big";
			cout << '\n';
		}
	}

	cout << "Congratulations! You guessed the number in " << tries << " tr" << ((tries > 1) ? "ies" : +"y") << "!\n"
	     << "Enter your name: ";
	string name;
	getline(cin, name);

	auto statement = sqlite3_mprintf("INSERT INTO scores VALUES(%Q, %u);", name.c_str(), tries);
	sqlite3_exec(db, statement, nullptr, nullptr, nullptr);
	sqlite3_free(statement);
}

void display_highscores(db_t & db) {
	static const auto outputscore = [](char ** values) { cout << values[1] << ":\t" << values[0] << '\n'; };

	once_flag header_printed;
	sqlite3_exec(db, "SELECT * FROM scores ORDER BY score;", [](void * data, int, char ** values, char ** names) {
		call_once(*static_cast<once_flag *>(data), [&]() {
			cout << "Entries have the format:\n";
			outputscore(names);
		});
		outputscore(values);
		return 0;
	}, &header_printed, nullptr);
}


int main() {
	static const vector<pair<function<void(db_t &)>, string>> menu({{play_game, "Play game"}, {display_highscores, "Show highscores"}, {[](auto &) {}, "Quit"}});


	bool valid = false;
	while(!valid) {
		cout << "What do you want to do?\n";
		size_t idx = 1;
		for(const auto & item : menu)
			cout << "\t" << idx++ << ". " << item.second << '\n';
		cout << "Enter one of the above numbers: ";

		valid = static_cast<bool>(cin >> idx);
		cin.clear();
		cin.ignore();
		if(!valid || idx > menu.size()) {
			cout << '\n';
			continue;
		}

		cout << '\n';
		db_t db;
		menu[idx - 1].first(db);
	}
}

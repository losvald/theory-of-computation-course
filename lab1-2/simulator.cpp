/*
 * simulator.cpp
 * 
 * Copyright (C) 2010 Leo Osvald <leo.osvald@gmail.com>
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); 
 * you may not use this file except in compliance with the License.
 *  
 * You may obtain a copy of the License at 
 * 		http://www.apache.org/licenses/LICENSE-2.0
 *  
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
 * See the License for the specific language governing permissions and 
 * limitations under the License. 
 */

#include <cstdio>
#include <string>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <vector>

#include "include/FSAbuilder.h"
#include "include/util.h"
#include "include/delimiters.h"
#include "include/labos1_types.h"

using namespace std;


class MySimulator {
private:
	static const int MAX_BUFF = 1<<16;
	MyDFA* dfa_;
public:

	MySimulator(MyDFA* dfa) {
		dfa_ = dfa;
	}

	bool simulate(const std::vector<std::string>& input_sequence) {
		dfa_->reset();
		return dfa_->process_sequence(input_sequence);
	}

	bool simulate_from_file(const std::string& input_filename) {

		static const string DELIMITERS = " ";
		static char buff[MAX_BUFF];
		FILE *f = fopen(input_filename.c_str(), "r");
		if(f == NULL) {
			fprintf(stderr, "Ne postoji datoteka \"%s\".",
					input_filename.c_str());
			return false;
		}
		printf("Simulacija zapocela\n");
		while(fgets(buff, sizeof(buff), f)) {
			buff[strlen(buff)-1] = '\0';
			//printf("Simuliram za ulazni niz: %s\n", buff);
			const string& s = buff;
			const vector<string>& tokens = StringUtils::split(s, DELIMITERS);
			/*
			if(simulate(tokens)) {
				printf("Niz \"%s\" se prihvaca\n", buff);
			}
			else {
				printf("Niz \"%s\" se ne prihvaca\n", buff);
			}
			printf("-------------------\n");
			*/
			printf("%d", simulate(tokens));
		}
		fclose(f);
		printf("Simulation zavrsena\n");
		return true;
	}
protected:
	void init() {
	}
};

bool MyDFA::process(const string& input) {
	bool ret = DFA<void*, string>::process(input);
	if(ret) {
		//printf("Presao u stanje:\n%s\n", current_state().get_name().c_str());
	}
	else {
		//printf("Usao u nepostojece stanje\n");
	}
	return ret;
}

void MyDFA::reset() {
	set_current_state(start_state_);
}

void MyDFA::transition_occured(const DFATransitionDomain& domain,
		const DFATransitionCodomain& codomain,
		const DFAInputType& input) const {
//	printf("Bla");
}

#include <iostream>

bool MyDFA::build_from_file(const string& definition_filename) {
	static const int MAX_BUFF = 4096;
	static std::map<int, std::string> label_map;
	static int cur_id = 0;

	label_map.clear();
	cur_id = 0;
	char buff[MAX_BUFF], label[MAX_BUFF];
	FILE *f = fopen(definition_filename.c_str(), "r");
	if(f == NULL) {
		fprintf(stderr, "Ne postoji datoteka \"%s\".", definition_filename.c_str());
		return false;
	}
	printf("Gradim PA...");
	bool first = true;
	while(fgets(buff, sizeof(buff), f)) {
		int len;
		buff[len = strlen(buff)-1] = '\0';
		if(len <= 1)
			continue;

		if(buff[0] == STATE_DEFINITION_TAG) {
			int id;
			sscanf(buff, "#%d %[^\n]", &id, label);
//			printf("[%d %s]\n", id, label);
			bool accept = false;
			if(label[0] == STATE_ACCEPT_PREFIX) {
				accept = true;
			}
			State<void*> state(id, NULL, accept);
			state.set_name(label+accept);
			add_state(state, first);
			label_map[state.get_id()] = state.get_name();
			first = false;
		}
		else {
			int from, to;
			sscanf(buff, "%d%d %[^\n]", &from, &to, label);

			DFATransitionDomain trans_from(label, get_state(from));
			add_transition(trans_from, get_state(to));

//			printf("GET(%d): %s\n", from, get_state(from)->get_name().c_str());
		}
	}
	fclose(f);
//	FORC(it, label_map) {
//		int id = it->first;
//		State<void*> *s = get_state(id);
//		cout << id << ":" << s << endl;
//	}

	printf("DONE\n");
	return true;
}

#include "include/pushdown_automaton.h"
#include "include/triple.h"

int main(int argc, char **argv) {
	/* XXX
	if(argc <= 2) {
		printf("Usage: (definition file) (simulation file)\n");
		return 1;
	}
	*/
	MyDFA dfa;
	dfa.build_from_file("res/def"); //XXX
	//dfa.build_from_file(argv[1]);
	MySimulator mySim(&dfa);
	//mySim.simulate_from_file(argv[2]);
	mySim.simulate_from_file("res/sim.in");

	PA<void*, string, string> pa("$");

	return 0;
}



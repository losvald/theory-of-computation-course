/*
 * FSA.h
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

#ifndef FSA_H_INCLUDED
#define FSA_H_INCLUDED

#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <cstdio>
#include "util.h"


template<typename T>
class FSA;

/**
 * Stanje konacnog automata.
 */
template<typename T>
class State {
private:
    int id_;
    bool accept_;
    T data_;
    std::string name_;
    std::map<std::string, State<T>*> transition_;
public:
    static const int DEFAULT_ID = 0;

//javne metode
    State(int id, T data, bool accept = false);
    State() { }
    int get_id() const;
    State<T>* get_next(const std::string& input);
    bool is_accept() const;
    T data() const;
    void set_name(std::string name);
    std::string get_name() const;
    std::vector<State<T>*> get_transitions();
    void add_transition(std::string input, State<T>* to);

    friend std::ostream& operator<<(std::ostream& os, const State<T>& s) {
    	os << "adr=" << (int)&s << "; id= " << s.id_ << "; name= " << s.name_
    			<< "; susjedi = [";
    	FORC(it, s.transition_)
			os << it->first << "->" << it->second->get_id() << " "
			<< it->second->get_name() << "(" << (int)&it->second << "|";
    	os << "]";
    	return os;
    }

    friend std::ostream& operator<<(std::ostream& os, State<T>* s) {
    	return operator<<(os, *s);
    }

    friend class FSA<T>;
//    friend class FSAbuilder<T>;

protected:
    void set_id(int id);

};

/**
 * Konacni automat (Finit State Machine)
 */
template<typename T>
class FSA {
	template<typename TT> friend class FSAbuilder;
protected:
    State<T>* start_state_;
    std::map< int, State<T> > states_;
    std::vector<State<T>*> accept_states_;
    int auto_id;
public:
    static const int AUTO_ID_FROM = 0;

    //metode
public:
    FSA();
    const State<T>& get_start_state() const;
    const std::vector<State<T>*>& get_accept_states() const;
    State<T>* get_state(int id);
    const std::vector< const State<T>* > get_states();

    virtual bool process(const std::string& input) = 0;

    virtual bool process_sequence(const std::vector<std::string>& input_sequence) {
    	FORC(input, input_sequence) {
    		if(input->empty())
    			continue;
    		if (!process(*input))
    			return false;
    	}
    	return true;
    }

    virtual void reset() = 0;

protected:
    void set_start_state(State<T>* state);
    void add_state(State<T>& state, bool start);
    void add_transition(int from_id, int to_id, std::string input);
};


template<typename T>
State<T>::State(int id, T data, bool accept)
    		: id_(id), accept_(accept), data_(data) { }

template<typename T>
State<T>* State<T>::get_next(const std::string& input) {
	if ( transition_.count(input) )
		return transition_[input];
	return NULL;
}

template<typename T>
bool State<T>::is_accept() const {
	return accept_;
}

template<typename T>
T State<T>::data() const {
	return data_;
}

template<typename T>
std::string State<T>::get_name() const {
	return name_;
}

//FIXME CUDAN ERROR ako je protected
template<typename T>
void State<T>::set_name(std::string name) {
	name_ = name;
}

template<typename T>
void State<T>::set_id(int id) {
	id_ = id;
}

template<typename T>
int State<T>::get_id() const {
	return id_;
}

template<typename T>
std::vector<State<T>*> State<T>::get_transitions() {
	std::vector<State<T>*> ret;
	std::vector<int> v;
	FORC(it, transition_)
		ret.push_back(it->second);
	return ret;
}

template<typename T>
void State<T>::add_transition(std::string input, State<T>* to) {
	transition_.insert(make_pair(input, to));
}

template<typename T>
FSA<T>::FSA() : auto_id(AUTO_ID_FROM) {
}

template<typename T>
const State<T>& FSA<T>::get_start_state() const {
	return *start_state_;
}

template<typename T>
const std::vector<State<T>*>& FSA<T>::get_accept_states() const {
	return accept_states_;
}

template<typename T>
const std::vector< const State<T>* > FSA<T>::get_states() {
	std::vector<State<T>*> ret;
	FORC(s, states_) {
		ret.push_back(&s->first);
	}
	return ret;
}

template<typename T>
State<T>* FSA<T>::get_state(int id) {
	return &states_[id];
}

template<typename T>
void FSA<T>::add_transition(int from_id, int to_id, std::string input) {
	states_[from_id].add_transition(input, &states_[to_id]);
}

template<typename T>
void FSA<T>::set_start_state(State<T>* state) {
	start_state_ = state;
}

template<typename T>
void FSA<T>::add_state(State<T>& state, bool start = false) {
//	state->id_ = auto_id++;
	states_[state.get_id()] = state;
	State<T>& s = states_[state.get_id()];
	if (s.is_accept()) {
		accept_states_.push_back(&s);
	}
//	printf("Added state: %d %s\n", s.get_id(), s.get_name().c_str());
	if(start) {
		set_start_state(&s);
//		printf("Set start state: %s\n", s.get_name().c_str());
	}
}

#endif // FSA_H_INCLUDED

#pragma once

#include <iostream>
#include <queue>
#include <thread>
#include <vector>
#include <condition_variable>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <memory>
#include <ctime>
#include <random>
#include <future>
#include <cassert>

using namespace std;

namespace raft {
	enum RaftStatus {
		Follower,
		Candidate,
		Leader,
		Dead,
	};
	
	static int random_election_timeout() {
		static random_device device;
		static mt19937 rng(device());
		static uniform_int_distribution<int> rnd(3, 10);

		return rnd(rng);
	}

	struct RaftStateNode {
		int term;
		RaftStatus status;
		int election_timeout;
		int votes;
		int last_voted_term;
		int hearbeat_count;
		std::string tag;

		RaftStateNode() : term(0), status(Follower), election_timeout(0), votes(0), last_voted_term(0), hearbeat_count(0) {}
		RaftStateNode(const std::string& _tag) : RaftStateNode() { tag = _tag; };
		
		int next_term() { return ++term; }
		void set_status(RaftStatus _status) { status = _status; }
		void set_new_election_time_out() { election_timeout = random_election_timeout(); }
		void set_election_time_out_max() { election_timeout = -1; }
	};
}
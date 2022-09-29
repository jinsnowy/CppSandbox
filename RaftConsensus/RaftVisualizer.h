#pragma once
#include "Singleton.h"
#include "RaftState.h"
#include "Format.h"

#include <vector>
#include <map>
#include <string>
#include <mutex>
#include <deque>

namespace raft {
	class RaftNode;
	class RaftVisualizer : public CSingleton<RaftVisualizer>
	{
	private:
		std::mutex _mtx;
		std::map<std::string, raft::RaftStateNode> _current_states;
		std::deque<std::string> _logs;

	public:
		void add_logs(string log);

		void poll(raft::RaftNode* node);

		void spin_once();
	};
}

#define ADD_LOG(fmt, ...)\
raft::RaftVisualizer::getInstance()->add_logs(Format::format(fmt, ##__VA_ARGS__));
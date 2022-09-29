#include "RaftVisualizer.h"
#include "RaftConsensus.h"

const int log_line_num = 10;

const char* get_status_str(raft::RaftStatus status) {
	switch (status)
	{
	case raft::Follower:
		return "Follower";
	case raft::Candidate:
		return "Candidate";
	case raft::Leader:
		return "Leader";
	case raft::Dead:
		return "Dead";
	default:
		break;
	}
	return "None";
}

void raft::RaftVisualizer::add_logs(std::string log)
{
	std::lock_guard<std::mutex> lk(_mtx);
	_logs.push_back(std::move(log));
	while (_logs.size() > log_line_num) {
		_logs.pop_front();
	}
}

void raft::RaftVisualizer::poll(raft::RaftNode* node)
{
	std::lock_guard<std::mutex> lk(_mtx);
	_current_states[node->get_tag()] = node->get_state();
}

void raft::RaftVisualizer::spin_once()
{
	std::lock_guard<std::mutex> lk(_mtx);
	
	system("cls");
	int log_num = (int)_logs.size();
	for (int i = 0; i < log_num; ++i) {
		printf("%s\n", _logs[i].c_str());
	}
	for (int i = 0; i < log_line_num - log_num; ++i) {
		printf("\n");
	}
	
	for (const auto& pair : _current_states) {
		printf("%s : state [%s], term (%d), votes(%d), election_timeout(%d) heartbeat(%d)\n", 
				pair.first.c_str(),
				get_status_str(pair.second.status), pair.second.term, pair.second.votes, pair.second.election_timeout, pair.second.hearbeat_count);
	}
}

#pragma once

#include "RaftMessage.h"

namespace raft {
	class RaftNode;

	class RaftRouter {
	private:	
		std::mutex mtx;
		std::vector<RaftNode*> nodes;
	
	public:
		~RaftRouter();

		void add_node(RaftNode* node);

		void start();

		void send_votes_request(const std::string& source, const RaftStateNode& status);

		void send_votes_response(const std::string& target);

		void send_heartbeat_request(int term, const std::string& source);

		void send_heartbeat_response(const std::string& target);
	
		bool is_enough_quorum(int n);

		void set_dead(const std::string& target);

		void set_restart(const std::string& target);

		std::vector<RaftNode*> get_all_nodes() { return nodes; }

		raft::RaftNode* get_random_node() const;

	private:
		std::vector<RaftNode*> shuffled_nodes();
	};
}
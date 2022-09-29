#pragma once
#include "RaftMessage.h"

namespace raft {
	class RaftNode;
	class MessageProcessor {
	private:
		RaftNode* _node;

	public:
		MessageProcessor(RaftNode* node) : _node(node) {}

		void process(RaftMessage&& message);

	private:
		void on_votes_request(VotesRequestMessage* message);
		void on_votes_response(VotesResponseMessage* message);
		void on_heartbeat_request(HeartbeatRequestMessage* message);
		void on_heartbeat_response(HeartbeatResponseMessage* message);
		void on_set_dead(SetDeadMessage* message);
		void on_set_restart(SetRestartMessage* message);
	};
}


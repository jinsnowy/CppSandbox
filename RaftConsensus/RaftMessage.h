#pragma once

#include "RaftState.h"

namespace raft {
	enum message_type {
		HeartbeatRequest,
		HeartbeatResponse,
		VotesRequest,
		VotesResponse,
		SetDead,
		SetRestart,
	};

	class RaftNode;

	struct BaseMessage {
		message_type type;
		RaftStateNode node_state;

		BaseMessage(message_type type_in) : type(type_in), node_state{} {}
		BaseMessage(message_type type_in, const RaftStateNode& node_in) : type(type_in), node_state(node_in) {}
	};

	struct HeartbeatRequestMessage : BaseMessage {
		int term;
		std::string target;
		HeartbeatRequestMessage(int term_in, const std::string& target_in) 
			: 
			BaseMessage(HeartbeatRequest),
			term(term_in),
			target(target_in) 
		{}
	};

	struct HeartbeatResponseMessage : BaseMessage {
		HeartbeatResponseMessage() : BaseMessage(HeartbeatResponse) {}
	};

	struct VotesRequestMessage : BaseMessage {
		VotesRequestMessage(RaftStateNode _source) : BaseMessage(VotesRequest, _source)
		{}
	};

	struct VotesResponseMessage : BaseMessage {
		VotesResponseMessage() : BaseMessage(VotesResponse)
		{}
	};

	struct SetDeadMessage : BaseMessage {
		SetDeadMessage() : BaseMessage(SetDead)
		{}
	};

	struct SetRestartMessage : BaseMessage {
		SetRestartMessage() : BaseMessage(SetRestart)
		{}
	};

	using RaftMessage = std::unique_ptr<BaseMessage>;
}

#include "RaftMessageProcessor.h"
#include "RaftConsensus.h"
#include "RaftVisualizer.h"
#include "Format.h"

void raft::MessageProcessor::process(RaftMessage&& message) {
	if (message->type == SetRestart) {
		on_set_restart((SetRestartMessage*)message.get());
		return;
	}
	else if (!_node->is_dead()) {
		switch (message->type) {
		case VotesRequest:
			on_votes_request((VotesRequestMessage*)message.get());
			break;
		case VotesResponse:
			on_votes_response((VotesResponseMessage*)message.get());
			break;
		case HeartbeatRequest:
			on_heartbeat_request((HeartbeatRequestMessage*)message.get());
			break;
		case HeartbeatResponse:
			on_heartbeat_response((HeartbeatResponseMessage*)message.get());
			break;
		case SetDead:
			on_set_dead((SetDeadMessage*)message.get());
			break;
		}
	}
}

void raft::MessageProcessor::on_votes_request(raft::VotesRequestMessage* message)
{
	if (_node->_inner_state.last_voted_term < message->node_state.term) {
		_node->_inner_state.last_voted_term = message->node_state.term;
		_node->_inner_state.set_new_election_time_out();
		_node->get_router()->send_votes_response(message->node_state.tag);

		ADD_LOG("node %s votes for %s in term %d", _node->get_tag().c_str(), message->node_state.tag.c_str(), message->node_state.term);
	}
}

void raft::MessageProcessor::on_votes_response(raft::VotesResponseMessage* message)
{
	if (_node->_inner_state.status == Candidate) {
		int cur_votes = ++(_node->_inner_state.votes);
		if (_node->get_router()->is_enough_quorum(cur_votes)) {
			_node->_inner_state.status = Leader;
			_node->_inner_state.set_election_time_out_max();
			_node->create_heartbeater();
		}
	}
}

void raft::MessageProcessor::on_heartbeat_request(raft::HeartbeatRequestMessage* message)
{
	if (_node->_inner_state.status == Candidate) {
		if (_node->_inner_state.term != message->term) {
			_node->_inner_state.hearbeat_count = 0;
		}
		_node->_inner_state.term = message->term;
		_node->_inner_state.hearbeat_count++;
		_node->_inner_state.set_status(Follower);
		_node->_inner_state.set_new_election_time_out();
		_node->get_router()->send_heartbeat_response(message->target);
	}
	else if (_node->_inner_state.status == Follower) {
		if (_node->_inner_state.term != message->term) {
			_node->_inner_state.hearbeat_count = 0;
		}
		_node->_inner_state.term = message->term;
		_node->_inner_state.hearbeat_count++;
		_node->_inner_state.set_new_election_time_out();
		_node->get_router()->send_heartbeat_response(message->target);
	}
}

void raft::MessageProcessor::on_heartbeat_response(HeartbeatResponseMessage* message) {
}

void raft::MessageProcessor::on_set_dead(SetDeadMessage*) {
	_node->set_dead();
}

void raft::MessageProcessor::on_set_restart(SetRestartMessage*) {
	_node->set_restart();
}

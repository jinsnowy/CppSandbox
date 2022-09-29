#include "RaftRouter.h"
#include "RaftConsensus.h"

static void delayed_send() {
	std::this_thread::sleep_for(std::chrono::seconds(2));
}

raft::RaftRouter::~RaftRouter()
{
	for (auto& node : nodes) {
		delete node;
	}
	nodes.clear();
}

void raft::RaftRouter::add_node(RaftNode* node)
{
	nodes.push_back(node);
}

void raft::RaftRouter::start()
{
	for (auto& node : nodes) {
		node->start();
	}
}

void raft::RaftRouter::send_votes_request(const std::string& source, const RaftStateNode& status)
{
	delayed_send();
	auto nodes = shuffled_nodes();
	for (auto& node : nodes) {
		if (node->is_dead())
			continue;

		if (!node->equal(source)) {
			node->push_message(std::make_unique<VotesRequestMessage>(status));
		}
	}
}

void raft::RaftRouter::send_votes_response(const std::string& target)
{
	delayed_send();
	for (auto& node : nodes) {
		if (node->is_dead())
			continue;

		if (node->equal(target)) {
			node->push_message(std::make_unique<VotesResponseMessage>());
			break;
		}
	}
}

void raft::RaftRouter::send_heartbeat_request(int term, const std::string& source)
{
	auto nodes = shuffled_nodes();
	for (auto& node : nodes) {
		if (node->is_dead())
			continue;

		if (!node->equal(source)) {
			node->push_message(std::make_unique<HeartbeatRequestMessage>(term, source));
		}
	}
}

void raft::RaftRouter::send_heartbeat_response(const std::string& target)
{
	for (auto& node : nodes) {
		if (node->is_dead())
			continue;

		if (node->equal(target)) {
			node->push_message(std::make_unique<HeartbeatResponseMessage>());
			break;
		}
	}
}

bool raft::RaftRouter::is_enough_quorum(int n)
{
	return n >= ((int)nodes.size() + 1) / 2;
}

void raft::RaftRouter::set_dead(const std::string& target)
{
	for (auto& node : nodes) {
		if (node->equal(target)) {
			node->push_message(std::make_unique<SetDeadMessage>());
			break;
		}
	}
}

void raft::RaftRouter::set_restart(const std::string& target)
{
	for (auto& node : nodes) {
		if (node->equal(target)) {
			node->push_message(std::make_unique<SetRestartMessage>());
			break;
		}
	}
}

std::vector<raft::RaftNode*> raft::RaftRouter::shuffled_nodes()
{
	static std::random_device rd{};
	static std::default_random_engine rng{ rd() };

	std::vector<raft::RaftNode*> result = nodes;
	std::shuffle(result.begin(), result.end(), rng);

	return result;
}

raft::RaftNode* raft::RaftRouter::get_random_node() const
{
	int node_num = (int)nodes.size();
	std::random_device rd{};
	std::default_random_engine rng{ rd() };
	std::uniform_int_distribution<int> dist(0, node_num - 1);

	return nodes[dist(rng)];
}
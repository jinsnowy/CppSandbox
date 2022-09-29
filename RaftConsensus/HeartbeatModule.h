#pragma once
#include <thread>
#include <mutex>

namespace raft {
	class RaftRouter;
	class RaftNode;
	class HeartbeatModule {
	private:
		bool finished;
		std::mutex mtx;
		std::condition_variable cv;
		std::thread worker;

		RaftNode* owner;
	public:
		HeartbeatModule(RaftNode* owner_node)
			:
			finished(false),
			owner(owner_node)
		{
			start();
		}

		~HeartbeatModule() {
			stop();
		}
		
	private:
		void start();

		void stop();
	};
}
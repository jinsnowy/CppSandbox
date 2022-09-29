#include "HeartbeatModule.h"
#include "RaftConsensus.h"

void raft::HeartbeatModule::start()
{
	owner->get_router()->send_heartbeat_request(owner->get_term(), owner->get_tag());

	worker = std::thread([this]() {
		while (!finished) {
			std::unique_lock<std::mutex> lk(mtx);
			cv.wait_for(lk, std::chrono::milliseconds(1000), [this]() { return finished; });

			if (finished) {
				return;
			}

			owner->get_router()->send_heartbeat_request(owner->get_term(), owner->get_tag());
		}
	});
}

void raft::HeartbeatModule::stop()
{
	{
		std::lock_guard<std::mutex> lk(mtx);
		finished = true;
	}

	cv.notify_one();

	if (worker.joinable()) {
		worker.join();
	}
}

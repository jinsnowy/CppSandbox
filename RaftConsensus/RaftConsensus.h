#pragma once

#include "RaftMessageProcessor.h"
#include "RaftRouter.h"
#include "RaftState.h"
#include "RaftVisualizer.h"
#include "HeartbeatModule.h"

using namespace std;

namespace raft {
	class RaftNode {
		friend class MessageProcessor;
	private:
		RaftRouter* _router;
		MessageProcessor* _processor;
		RaftStateNode _inner_state;

		string _tag;
		bool   _finished;
		std::thread _work;
		std::mutex _mtx;
		std::condition_variable _cv;
		std::queue<RaftMessage> _que;
		std::unique_ptr<HeartbeatModule> _heartbeater;
		
		std::promise<void> _init_signal;
		std::future<void> _init;
	public:
		RaftNode(RaftRouter* router, string tag)
			:
			_router(router),
			_processor(new MessageProcessor(this)),
			_inner_state{tag},
			_tag(std::move(tag)),
			_finished(false),
			_init_signal{},
			_init(_init_signal.get_future())
		{
			_work = std::thread([this]() {
				on_work();
			});
		}

		~RaftNode() {
			stop();
			delete _processor; _processor = nullptr;
		}

		void start() {
			_init_signal.set_value();
		}
		
		void push_message(RaftMessage&& message) {
			unique_lock<mutex> lk(_mtx);
			_que.push(std::move(message));
			lk.unlock();
			_cv.notify_one();
		}

		bool is_dead() const {
			return _inner_state.status == Dead;
		}

		bool equal(const string& name) {
			return _tag == name;
		}

		RaftRouter* get_router() const {
			return _router;	
		}

		int get_term() const {
			return _inner_state.term;
		}

		const string& get_tag() const {
			return _tag;
		}

		const RaftStateNode& get_state() const {
			return _inner_state;
		}

		void set_dead() {
			if (_inner_state.status != Dead) {
				_inner_state.status = Dead;

				release_heartbeater();
			
				ADD_LOG("node %s is dead", _tag.c_str());
			}
		}

		void set_restart() {
			if (_inner_state.status == Dead) {
				_inner_state.set_new_election_time_out();
				_inner_state.set_status(Follower);

				ADD_LOG("node %s restarts", _tag.c_str());
			}
		}

	private:
		void stop() {
			unique_lock<mutex> lk(_mtx);
			_finished = true;
			lk.unlock();
			_cv.notify_one();

			if (_work.joinable()) {
				_work.join();
			}
		}

		void on_work() {
			_init.wait();
			_inner_state.set_new_election_time_out();

			assert(_inner_state.term == 0);

			RaftVisualizer::getInstance()->poll(this);

			while (!_finished) {
				unique_lock<mutex> lk(_mtx);

				if (_inner_state.election_timeout == -1 || is_dead()) {
					_cv.wait(lk, [this]() { return !_que.empty() || _finished; });
				}
				else {
					_cv.wait_for(lk, std::chrono::seconds(_inner_state.election_timeout), [this]() { return !_que.empty() || _finished; });
				}

				if (_finished) {
					return;
				}

				vector<RaftMessage> messages;
				while (!_que.empty()) {
					auto msg = std::move(_que.front()); _que.pop();
					messages.push_back(std::move(msg));
				}
				lk.unlock();

				if (!messages.empty()) {
					for (auto& msg : messages) {
						_processor->process(std::move(msg));
					}
				}
				else {
					_inner_state.votes = 1;
					_inner_state.set_status(Candidate);
					_inner_state.set_new_election_time_out();
					_inner_state.last_voted_term = _inner_state.next_term();
					_router->send_votes_request(_tag, _inner_state);
				}

				RaftVisualizer::getInstance()->poll(this);
			}
		}

		void create_heartbeater() {
			_heartbeater.reset(new raft::HeartbeatModule(this));
		}

		void release_heartbeater() {
			_heartbeater.reset();
		}
	};
}

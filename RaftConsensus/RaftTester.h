#pragma once
#include "RaftConsensus.h"
#include "RaftVisualizer.h"

namespace raft {
	static RaftRouter* router = nullptr;

	class RaftTester{
	private:
		struct KeyboardEvent {
			virtual void operator()() = 0;
		};

		struct NodeDead : KeyboardEvent {
			string node_str;
			virtual void operator()() override {
				router->set_dead(node_str);
			}
		};

		struct NodeRestart : KeyboardEvent {
			string node_str;
			virtual void operator()() override {
				router->set_restart(node_str);
			}
		};

		mutex mtx;
		condition_variable cv;
		queue<KeyboardEvent*> eventQue;

		bool is_quit = false;
		queue<string> keyboard_input;

	public:
		RaftTester() {
			router = new RaftRouter();
		}

		~RaftTester() {
			if (router) {
				delete router;
				router = nullptr;
			}
		}

		void test() {
			RaftVisualizer* visualizer = RaftVisualizer::getInstance();

			RaftNode* n1 = new RaftNode(router, "n1");
			RaftNode* n2 = new RaftNode(router, "n2");
			RaftNode* n3 = new RaftNode(router, "n3");
			RaftNode* n4 = new RaftNode(router, "n4");
			RaftNode* n5 = new RaftNode(router, "n5");

			router->add_node(n1);
			router->add_node(n2);
			router->add_node(n3);
			router->add_node(n4);
			router->add_node(n5);
			router->start();

			std::thread t([this]() { keyboard_listen(); });
			t.detach();

			while (!is_quit)
			{
				std::unique_lock<std::mutex> lk(mtx);
				cv.wait_for(lk, std::chrono::milliseconds(500), [this]() {
					return !eventQue.empty() || is_quit;
				});

				while (!eventQue.empty()) {
					auto item = eventQue.front();
					eventQue.pop();
					(*item)();
					delete item;
				}
				lk.unlock();

				visualizer->spin_once();

				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}
		}

	private:
		void keyboard_listen()
		{
			while (1) {
				string buf;
				buf.resize(256);

				cin.getline((char*)buf.data(), 256, '\n');
				if (buf.empty())
					continue;

				char cmd = buf[0];
				switch (cmd)
				{
				case 'q':
				{
					{
						std::lock_guard<std::mutex> lk(mtx);
						is_quit = true;
					}
					cv.notify_one();

					return;
				}break;
				case 'd':
				{
					string sub = buf.substr(1);
					Format::trim(sub);
					sub.resize(strlen(sub.c_str()));
					{
						std::lock_guard<std::mutex> lk(mtx);
						auto node_dead_cmd = new NodeDead();
						node_dead_cmd->node_str = sub;
						eventQue.push(node_dead_cmd);
					}

					cv.notify_one();
				}break;
				case 'r':
				{
					string sub = buf.substr(1);
					Format::trim(sub);
					sub.resize(strlen(sub.c_str()));
					{
						std::lock_guard<std::mutex> lk(mtx);
						auto node_restart_cmd = new NodeRestart();
						node_restart_cmd->node_str = sub;
						eventQue.push(node_restart_cmd);
					}

					cv.notify_one();
				}break;
				default:
					break;
				}
			}
		}
	};
}

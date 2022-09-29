#pragma once
#include <memory>
#include <atomic>

template <typename T>
class CSingleton
{
public:
	CSingleton() = default;
	virtual ~CSingleton() = default;

	CSingleton(const CSingleton&) = delete;
	CSingleton& operator=(const CSingleton&) = delete;

	CSingleton(CSingleton&&) = delete;
	CSingleton& operator=(CSingleton&&) = delete;

public:
	static T* getInstance()
	{
		static T inst;

		return &inst;
	};
};
#pragma once
#include <chrono>
#include <string>

class Perfometer
{
	using clock = std::chrono::high_resolution_clock;
	Perfometer(Perfometer&) = delete;
	Perfometer(Perfometer&&) = delete;
	Perfometer operator = (const Perfometer&) = delete;
	Perfometer operator = (Perfometer&&) = delete;

public:
	explicit Perfometer(const std::string& name);
	~Perfometer();

	void time(const std::string& name) const;
private:
	clock::time_point m_start;
	std::string m_name;
};

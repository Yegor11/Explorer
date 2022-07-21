#include "perfometer.h"
#include<iostream>

Perfometer::Perfometer(const std::string& name)
	: m_start(clock::now())
	, m_name(name)
{

}

Perfometer::~Perfometer()
{
	time(m_name);
}

void Perfometer::time(const std::string& name) const
{
	const auto stop = clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - m_start);
	std::cout << "Time for " << name << " taken " << duration.count() << " milliseconds\n";
}

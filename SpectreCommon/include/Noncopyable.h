#pragma once 

class Noncopyable
{
public:
	Noncopyable(const Noncopyable&) = delete;
	Noncopyable(Noncopyable&&) = delete;
	Noncopyable& operator = (const Noncopyable&) = delete;
	Noncopyable& operator = (Noncopyable&&) = delete;
protected:
	Noncopyable() = default;
	~Noncopyable() = default;
};
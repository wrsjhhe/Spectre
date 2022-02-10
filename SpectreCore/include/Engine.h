#pragma once

BEGIN_NAMESPACE_SPECTRE

class Engine : public Noncopyable
{
public:
	bool Init();

private:
	Engine();
};

END_NAMESPACE_SPECTRE
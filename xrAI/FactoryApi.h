#pragma once

class CSE_Abstract;

namespace Factory
{
	CSE_Abstract* CreateEntity(LPCSTR section);
	void DestroyEntity(CSE_Abstract*& i);
};

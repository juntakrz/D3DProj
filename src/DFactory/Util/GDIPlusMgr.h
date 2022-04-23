#pragma once

#include "../D3DMgr/D3DMgr.h"

class GDIPlusMgr
{
	static ULONG_PTR m_token;
	static int32_t m_refCount;
public:
	GDIPlusMgr();
	~GDIPlusMgr();
};
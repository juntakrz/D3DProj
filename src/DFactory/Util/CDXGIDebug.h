#pragma once

class CDXGIDebug
{
#ifdef _DEBUG
	uint64_t m_CurrentMsgIndex = 0u;
	COMPTR<IDXGIInfoQueue> pDXGIInfoQueue;
#endif
public:
	CDXGIDebug();
#ifdef _DEBUG
	~CDXGIDebug() = default;
	CDXGIDebug(const CDXGIDebug&) = delete;
	CDXGIDebug& operator=(const CDXGIDebug&) = delete;

	//skips previous messages so only future ones will be processed
	void Set() noexcept;
	std::vector<std::string> GetMessages() const;
#endif
};
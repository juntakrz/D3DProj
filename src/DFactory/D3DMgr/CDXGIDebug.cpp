#include "../../pch.h"
#include "../WndMgr/WndMgr.h"
#include "CDXGIDebug.h"

CDXGIDebug::CDXGIDebug()
{
#ifdef _DEBUG
	//define function signature of DXGIGetDebugInterface
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);
	
	//load a DLL required by the function
	const auto hDXGIDebug = LoadLibraryExA("dxgidebug.dll", NULL, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
	if (!hDXGIDebug)
	{
		throw CWND_LASTEXCEPT();
	}

	//get address of DXGIGetDebugInterface from the DLL
	const DXGIGetDebugInterface DXGIGetDebugIface = reinterpret_cast<DXGIGetDebugInterface>(reinterpret_cast<void*>(GetProcAddress(hDXGIDebug, "DXGIGetDebugInterface")));
	if (!DXGIGetDebugIface)
	{
		throw CWND_LASTEXCEPT();
	}

	HRESULT hr;
	D3D_THROW_NOMSG(DXGIGetDebugIface(__uuidof(IDXGIInfoQueue), &pDXGIInfoQueue));
#endif
}

#ifdef _DEBUG
void CDXGIDebug::Set() noexcept
{
	m_CurrentMsgIndex = pDXGIInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string> CDXGIDebug::GetMessages() const
{
	HRESULT hr;
	std::stringstream sstr;
	std::vector<std::string> messages;
	const auto lastMsgIndex = pDXGIInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);

	//loop through all current messages
	for (auto i = m_CurrentMsgIndex; i < lastMsgIndex; i++)
	{
		SIZE_T msgLength = 0u;

		//get the size of the message in bytes
		D3D_THROW_NOMSG(pDXGIInfoQueue->GetMessageW(DXGI_DEBUG_ALL, i, nullptr, &msgLength));

		//allocate memory for message and get a pointer to it
		std::unique_ptr<byte[]> bytes = std::make_unique<byte[]>(msgLength);
		auto pMsg = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());

		//now we can finally put it into the proper container
		D3D_THROW_NOMSG(pDXGIInfoQueue->GetMessageW(DXGI_DEBUG_ALL, i, pMsg, &msgLength));

		sstr.str("");
		sstr.clear();
		sstr << pMsg->pDescription;
		messages.emplace_back(sstr.str());
	}

	return messages;
}

#endif
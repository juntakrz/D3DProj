#include "DFactory.h"

void DFactory::CProc::Init() noexcept
{
	pWnd = pDF->pWndMgr.get();
	pKbd = &pDF->pWndMgr->kbd;
	pMouse = &pDF->pWndMgr->mouse;
}

void DFactory::CProc::ProcessCamera() noexcept
{
	m_camDir = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_camRot.x = 0.0f;
	m_camRot.y = 0.0f;

	pKbd->IsKeyDown('W') ? m_camDir.z += 0.01f : 0.0f;
	pKbd->IsKeyDown('S') ? m_camDir.z += -0.01f : 0.0f;
	pKbd->IsKeyDown('A') ? m_camDir.x += -0.01f : 0.0f;
	pKbd->IsKeyDown('D') ? m_camDir.x += 0.01f : 0.0f;
	pKbd->IsKeyDown('R') ? m_camDir.y += 0.01f : 0.0f;
	pKbd->IsKeyDown('F') ? m_camDir.y += -0.01f : 0.0f;

	switch (pMouse->IsCursorOn())
	{
	case true:
	{
		pKbd->IsKeyDown(VK_LEFT) ? m_camRot.y += pDF->Camera()->GetRotationDelta() : NULL;
		pKbd->IsKeyDown(VK_RIGHT) ? m_camRot.y -= pDF->Camera()->GetRotationDelta() : NULL;
		pKbd->IsKeyDown(VK_UP) ? m_camRot.x += pDF->Camera()->GetRotationDelta() : NULL;
		pKbd->IsKeyDown(VK_DOWN) ? m_camRot.x -= pDF->Camera()->GetRotationDelta() : NULL;

		GetCursorPos(&m_cursorPos);
		m_oldPosX = m_cursorPos.x;
		m_oldPosY = m_cursorPos.y;
		break;
	}
	case false:
	{
		GetCursorPos(&m_cursorPos);
		m_camRot.y = float(m_oldPosX - m_cursorPos.x) * 0.005f;
		m_camRot.x = float(m_oldPosY - m_cursorPos.y) * 0.005f;
		pMouse->RestorePos();
		pMouse->GetStoredPos(m_oldPosX, m_oldPosY);
		break;
	}
	}

	DF::D3DM->Camera()->Move(m_camDir.x, m_camDir.y, m_camDir.z);
	DF::D3DM->Camera()->RotateAdd(m_camRot.x, m_camRot.y, 0.0f);
}

void DFactory::CProc::ProcessFunctions() noexcept
{
	if (const std::optional<CKbd::Event> KEvent = pKbd->ReadKey())
	{
		if (KEvent->IsRelease()) {

			switch (KEvent->GetCode())
			{
			case 'L':
			{
				pDF->LightM->ShowPLMeshes() = !pDF->LightM->ShowPLMeshes();
				break;
			}
			case VK_SPACE:
			{
				pMouse->ToggleCursor();
				break;
			}
			case VK_ESCAPE:
			{
				exit(0);
				break;
			}
			case VK_F2:			// show wireframe
			{
				DF::D3DM->RenderWireframe(!m_RenderWireframe);
				m_RenderWireframe = !m_RenderWireframe;
				break;
			}
			case VK_F3:			// show depth buffer
			{
				DF::D3DM->SetShowDepth();
				break;
			}
			case VK_ADD:
			{
				pDF->debug.int64bit++;
				break;
			}
			case VK_SUBTRACT:
			{
				pDF->debug.int64bit--;
				break;
			}
			case '1':
			{
				DF::Engine->CameraActivate("camMain");
				break;
			}
			case '2':
			{
				DF::Engine->CameraActivate("camLight");
				break;
			}
			case '5':
			{
				pDF->ModelM->Select(4);
				pDF->ModelM->Delete();
				pDF->MatM->MatDelete(8);
				pDF->MatM->MatDelete(9);
				pDF->MatM->MatDelete(10);
			}
			}
		}
	}

	pKbd->IsKeyDown('8') ? pDF->MatM->DEBUG_ShowMaterialIndex() : void();
	pKbd->IsKeyDown('9') ? pDF->MatM->DEBUG_ShowTextureIndex() : void();
	pKbd->IsKeyDown('0') ? pDF->ModelM->DEBUG_ShowModelList() : void();
}

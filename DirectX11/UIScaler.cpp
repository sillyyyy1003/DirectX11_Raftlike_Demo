#include "UIScaler.h"

UIScaler::UIScaler():
	m_referenceWindowSize(WIN_WIDTH,WIN_HEIGHT) // Default reference size
	, m_currentWindowSize(WIN_WIDTH, WIN_HEIGHT) // Initial current size
	, m_scale(1.0f, 1.0f)
	, m_isDirty(true) // Initially dirty to force an update
{
}

void UIScaler::UpdateScaler()
{
	if(m_isDirty)
	{
		// Update the scale factors based on the current window size
		m_scale.x = m_currentWindowSize.x / m_referenceWindowSize.x;
		m_scale.y = m_currentWindowSize.y / m_referenceWindowSize.y;

		// 変更された場合のみ更新する
		m_isDirty = false;
	}
}

void UIScaler::SetWindowSize(const DirectX::XMFLOAT2& windowSize)
{
	m_currentWindowSize = windowSize;
	m_isDirty = true; // Mark as dirty to trigger an update
}

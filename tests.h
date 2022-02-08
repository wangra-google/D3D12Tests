#pragma once
#include "stdafx.h"

class D3D12UnitTests
{
public:
	D3D12UnitTests();
	void TestGetCopyableFootprints();

private:
	ComPtr<ID3D12Device> m_device;
};
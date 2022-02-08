#include "stdafx.h"
#include "tests.h"

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	D3D12UnitTests unit_tests;
	unit_tests.TestGetCopyableFootprints();
	return 0;
}
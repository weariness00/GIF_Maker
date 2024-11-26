#include "TextController.h"

TextController::TextController()
{
	widthSortFlag = DT_LEFT;
	heightSortFlag = DT_TOP;
	outputFlag = DT_NOCLIP;
}

TextController::~TextController()
{
}

void TextController::SortLeft()
{
	WidthSortClean();
	widthSortFlag |= DT_LEFT;
}

void TextController::SortMiddle()
{
	WidthSortClean();
	widthSortFlag |= DT_CENTER;
}

void TextController::OnPaint(const HDC hdc)
{
	auto worldRect = wTransform.GetWorldRect();

	SetBkMode(hdc, TRANSPARENT);

	DrawText(hdc, text.c_str(), -1, worldRect, outputFlag | widthSortFlag | heightSortFlag);

	SetBkMode(hdc, OPAQUE);
}

void TextController::WidthSortClean()
{
	widthSortFlag &= ~DT_LEFT & ~DT_RIGHT & DT_CENTER;
}

void TextController::HeightSortClean()
{
	heightSortFlag &= ~DT_TOP & ~DT_BOTTOM & DT_VCENTER;
}


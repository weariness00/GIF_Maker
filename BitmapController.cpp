#include "BitmapController.h"
#include "GDIPlusManager.h"
#include <iostream>

BitmapController::BitmapController()
{
}

BitmapController::~BitmapController()
{
}

void BitmapController::OnPaint(HDC hdc)
{
    if (!GetActive()) return;
         
	auto graphic = GDIPlusManager::Instance->GetGraphics(hdc);
	if (!graphic) return;

	if(bitmap)
	{
        auto rect = wTransform.GetWorldRect();

        int x = rect->left;
        int y = rect->top;
        int w = rect->right;
        int h = rect->bottom;

        auto status = graphic->DrawImage(bitmap, x, y, w, h);
        if(status != Ok)
        {
            std::cout << "Bitmap 그리기 실패\nObject : " + name << std::endl;
        }
	}
}
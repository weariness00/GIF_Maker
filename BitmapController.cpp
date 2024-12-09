#include "BitmapController.h"
#include "framework.h"

BitmapController::BitmapController()
{
}

BitmapController::~BitmapController()
{
    delete bitmap;
}

void BitmapController::MakeBitmap(BYTE* pData, UINT width, UINT height)
{
    delete bitmap;
    bitmap = new Bitmap(
        width, height, 
        width * 4, 
        PixelFormat32bppRGB, 
        pData);
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

void BitmapController::OnPaint(HDC hdc, int extendWidth, int extendHeight)
{
    if (!GetActive()) return;

    auto graphic = GDIPlusManager::Instance->GetGraphics(hdc);
    if (!graphic) return;

    if (bitmap)
    {
        auto rect = wTransform.GetWorldRect();

        int x = rect->left;
        int y = rect->top;
        int w = rect->right;
        int h = rect->bottom;

    }
}

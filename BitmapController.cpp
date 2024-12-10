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

        int bitmapW = bitmap->GetWidth();
        int bitmapH = bitmap->GetHeight();

        int horizontalReapeatCount = std::ceil((double)extendWidth / w);
        int verticalReapeatCount = std::ceil((double)extendHeight / h);

        for (int i = 0; i < verticalReapeatCount; i++)
        {
            for (int j = 0; j < horizontalReapeatCount; j++)
            {
                int desW = j == horizontalReapeatCount - 1? extendWidth % w : w;
                int desH = i == verticalReapeatCount - 1 ? extendHeight % h : h;
                desW = (desW == 0) ? w : desW;
                desH = (desH == 0) ? h : desH;
                int srcW = j == horizontalReapeatCount - 1 ? bitmapW * desW / w : bitmapW;
                int srcH = i == verticalReapeatCount - 1 ? bitmapH * desH / h : bitmapH;

                Rect desRect(x,y,desW,desH);
                desRect.X *= (j + 1);
                desRect.Y *= (i + 1);
                graphic->DrawImage(bitmap, desRect, 0, 0, srcW, srcH, UnitPixel);
            }
        }
    }
}

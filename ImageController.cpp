#include "ImageController.h"
#include "GDIPlusManager.h"

Image* ImageController::DefaultImage = Image::FromFile(L"Image\\Empty.png");

ImageController::ImageController()
{
    wTransform.SetRect({ 0,0,100,100 });
    isDrawArea = false;

    SetOutsideAreaColor(0, 0, 0, 1);
    SetInsideAreaColor(1, 1, 1, 1);
}

ImageController::~ImageController()
{
    delete image;
}

void ImageController::SetOutsideAreaColor(float r, float g, float b, float a)
{
    outsideColorMatrix = {
    r   ,       0.0f, 0.0f, 0.0f, 0.0f, // Red
    0.0f, g   , 0.0f, 0.0f, 0.0f, // Green
    0.0f, 0.0f, b   , 0.0f, 0.0f, // Blue
    0.0f, 0.0f, 0.0f, a   , 0.0f, // Alpha (투명도)
    0.0f, 0.0f, 0.0f, 0.0f, 1.0f  // W
	};
}

void ImageController::SetInsideAreaColor(float r, float g, float b, float a)
{
    insideColorMatrix = {
r   ,       0.0f, 0.0f, 0.0f, 0.0f, // Red
0.0f, g   , 0.0f, 0.0f, 0.0f, // Green
0.0f, 0.0f, b   , 0.0f, 0.0f, // Blue
0.0f, 0.0f, 0.0f, a   , 0.0f, // Alpha (투명도)
0.0f, 0.0f, 0.0f, 0.0f, 1.0f  // W
    };
}

void ImageController::CreateImage(std::wstring path)
{
	image = Image::FromFile(path.c_str());
}

void ImageController::OnPaint(HDC hdc)
{
    auto graphic = GDIPlusManager::Instance->GetGraphics(hdc);
    if (!graphic) return;

    if (image == nullptr) image = DefaultImage;
    auto rect = wTransform.GetWorldRect();

    int x = rect->left;
    int y = rect->top;
    int w = rect->right;
    int h = rect->bottom;

    if (isDrawArea)
    {
        ImageAttributes imageAttributes;

        // 1. 전체 이미지를 반투명하게 그리기
        imageAttributes.SetColorMatrix(&outsideColorMatrix, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);
        graphic->DrawImage(
            image,
            Gdiplus::Rect(x, y, w, h),
            0, 0, image->GetWidth(), image->GetHeight(),
            UnitPixel,
            &imageAttributes);

        // 2. 특정 영역을 원래대로 덮어쓰기
        imageAttributes.SetColorMatrix(&insideColorMatrix, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);
        graphic->DrawImage(
            image,
            drawAreaRect,
            0, 0, image->GetWidth(), image->GetHeight(),
            UnitPixel,
            &imageAttributes);
    }
    else
    {
        graphic->DrawImage(image, x, y, w, h);
    }
}
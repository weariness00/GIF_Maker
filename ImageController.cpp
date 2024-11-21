#include "ImageController.h"

ImageController::ImageController()
{
    rect = { 0,0,100,100 };
}

ImageController::~ImageController()
{
 
}

void ImageController::CreateImage(HWND _hwnd, std::wstring path)
{
    // GDI+ 관련된 어떤 함수라도 사용 전에 해당 함수를 호출해야 합니다.
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // image.png 파일을 이용하여 Image 객체를 생성합니다.
    hdc = GetDC(_hwnd);
    image = Image::FromFile(path.c_str());
    graphics = new Graphics(hdc);
}

void ImageController::Release(HWND _hwnd)
{
    delete image;
    ReleaseDC(_hwnd, hdc);
}

void ImageController::OnPaint()
{
    if (image)
    {
        graphics->DrawImage(image, rect);
    }
}

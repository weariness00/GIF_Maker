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
    // GDI+ ���õ� � �Լ��� ��� ���� �ش� �Լ��� ȣ���ؾ� �մϴ�.
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // image.png ������ �̿��Ͽ� Image ��ü�� �����մϴ�.
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

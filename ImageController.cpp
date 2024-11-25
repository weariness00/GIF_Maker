#include "ImageController.h"

std::map<HDC, std::tuple<Graphics*, int>> ImageController::graphics;

ImageController::ImageController()
{
    windowType = Empty;
    wTransform.SetRect({ 0,0,100,100 });
}

ImageController::~ImageController()
{
    delete image;
}

void ImageController::CreateImage(HDC hdc, std::wstring path)
{
    // GDI+ ���õ� � �Լ��� ��� ���� �ش� �Լ��� ȣ���ؾ� �մϴ�.
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // image.png ������ �̿��Ͽ� Image ��ü�� �����մϴ�.
    if(!graphics.contains(hdc))
    {
        graphics[hdc] = std::make_tuple(new Graphics(hdc), 1);
    }

	windowType = Window;
	image = Image::FromFile(path.c_str());
}

void ImageController::Release(HDC hdc)
{
    delete image;
    if (graphics.contains(hdc))
    {
        int refCount = std::get<1>(graphics[hdc]) - 1;
        std::get<1>(graphics[hdc]) = refCount;
        if(refCount <= 0)
        {
            graphics.erase(hdc);
        }
    }
}


void ImageController::OnPaint(HDC hdc)
{
    auto graphic = std::get<0>(graphics[hdc]);
    if (!graphic) return;

    if (image)
    {
        auto rect = wTransform.GetWorldRect();

        int x = rect->left;
        int y = rect->top;
        int w = rect->right;
        int h = rect->bottom;

        if(windowType == Window)
            graphic->DrawImage(image, x, y, w, h);
        else if(windowType == PictureControl)
            graphic->DrawImage(image,x,y);
    }
}
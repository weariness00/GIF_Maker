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
    // GDI+ 관련된 어떤 함수라도 사용 전에 해당 함수를 호출해야 합니다.
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // image.png 파일을 이용하여 Image 객체를 생성합니다.
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
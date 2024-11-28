#include "ImageController.h"
#include "GDIPlusManager.h"

ImageController::ImageController()
{
    windowType = Empty;
    wTransform.SetRect({ 0,0,100,100 });
}

ImageController::~ImageController()
{
    delete image;
}

void ImageController::CreateImage(std::wstring path)
{
	windowType = Window;
	image = Image::FromFile(path.c_str());
}

void ImageController::OnPaint(HDC hdc)
{
    auto graphic = GDIPlusManager::Instance->GetGraphics(hdc);
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
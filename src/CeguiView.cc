#include "CeguiView.hh"

#include <CEGUI/RendererModules/Ogre/Renderer.h>
#include <CEGUI/CEGUI.h>

using namespace std;

CeguiView::CeguiView() 
{
    InitCegui();
    InitRootWindow();
    InitFpsViewer();
    InitATH();
}

CeguiView::~CeguiView()
{

}

void CeguiView::InitRootWindow()
{
    Root = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "Root");
    CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(Root);

}

void CeguiView::InitFpsViewer()
{
    FpsViewer = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Label", "FpsViewers");
    FpsViewer->setText("FPS");
    FpsViewer->setSize(CEGUI::USize(CEGUI::UDim(0.10,0), CEGUI::UDim(0.05, 0)));
    FpsViewer->setPosition(CEGUI::UVector2(CEGUI::UDim(0.90,0), CEGUI::UDim(0,5)));

    AddWidgetToWindow(FpsViewer, Root);
}

void CeguiView::InitATH()
{
    ATHViewer = static_cast<CEGUI::FrameWindow*>(
                CEGUI::WindowManager::getSingleton()
                .createWindow("TaharezLook/FrameWindow", "LifeWindow"));
    ATHViewer->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, 5)));
    ATHViewer->setSize(CEGUI::USize(CEGUI::UDim(0.30, 0), CEGUI::UDim(0.05, 0)));
    ATHViewer->setText("Vie (en voxels)");

    LifeProgressBar = static_cast<CEGUI::ProgressBar*>(
                CEGUI::WindowManager::getSingleton()
                .createWindow("TaharezLook/ProgressBar", "LifeBarre"));
    LifeProgressBar->setProgress(1.0f);
    LifeProgressBar->setSize(CEGUI::USize(CEGUI::UDim(1, 0), CEGUI::UDim(0.4, 0)));

    MunitionsViewer = CEGUI::WindowManager::getSingleton()
                .createWindow("TaharezLook/Label", "MunitionsViewer");
    MunitionsViewer->setText("Munitions : ");
    MunitionsViewer->setSize(CEGUI::USize(CEGUI::UDim(1,0), CEGUI::UDim(0.4, 0)));
    MunitionsViewer->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0.5, 0)));

    AddWidgetToWindow(LifeProgressBar, ATHViewer);
    AddWidgetToWindow(MunitionsViewer, ATHViewer);
    AddWidgetToWindow(ATHViewer, Root);
}

void CeguiView::AddWidgetToWindow(CEGUI::Window *child, CEGUI::Window *parent)
{
    parent->addChild(child);
}

void CeguiView::InitCegui()
{
    CEGUI::OgreRenderer& myRenderer = CEGUI::OgreRenderer::bootstrapSystem();

    // load font and setup default if not loaded via scheme
    CEGUI::Font& defaultFont = CEGUI::FontManager::getSingleton().createFromFile("DejaVuSans-12.font");
    // Set default font for the Gui Context
    CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultFont(&defaultFont);

    // load resources and set up system defaults
    CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
    CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");
}

void CeguiView::SetFpsText(const string &text)
{
    FpsViewer->setText(text);
}

void CeguiView::SetLifeProgress(const float progress)
{
    LifeProgressBar->setProgress(progress);
}

void CeguiView::SetStockMunitions(const int chargeur, const int total)
{
    MunitionsViewer->setText("Munitions : " + to_string(chargeur) + "/" + to_string(total));
}

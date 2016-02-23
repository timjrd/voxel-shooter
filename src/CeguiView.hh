#pragma once

#include <CEGUI/CEGUI.h>
#include <string>

class CeguiView
{

private:

    CEGUI::Window *FpsViewer                     = nullptr;
    CEGUI::FrameWindow *ATHViewer                = nullptr;
    CEGUI::ProgressBar *LifeProgressBar          = nullptr;
    CEGUI::Window *MunitionsViewer               = nullptr;
    CEGUI::Window *Root                          = nullptr;

public:

    CeguiView();
    ~CeguiView();

    void InitRootWindow();
    void InitFpsViewer();
    void InitATH();
    void InitCegui();
    void AddWidgetToWindow(CEGUI::Window *, CEGUI::Window *);
    void SetFpsText(const std::string &text);
    void SetLifeProgress(const float progress);
    void SetStockMunitions(const int chargeur, const int total);

};


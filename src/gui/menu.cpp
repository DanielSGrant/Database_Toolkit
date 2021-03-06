#include "about.h"
#include "app.h"
#include "menu.h"
#include "options.h"
#include "window.h"


enum
{
    // macOS does not let a menu item start at 0
    ID_OPTIONS = 1,
    ID_RELOAD,
    ID_EXIT,
    ID_ABOUT
};

wxBEGIN_EVENT_TABLE(Menu, wxMenuBar)
    EVT_MENU(ID_RELOAD, Menu::OnReload)
    EVT_MENU(ID_EXIT, Menu::OnExit)
wxEND_EVENT_TABLE()

Menu::Menu() : wxMenuBar()
{
    menu1 = new wxMenu();
    menu2 = new wxMenu();
    
    wxMenuItem *menuitem1;
    wxMenuItem *menuitem2;
    wxMenuItem *menuitem3;
    wxMenuItem *menuitem4;
    
    menuitem1 = new wxMenuItem(
        menu1,
        ID_OPTIONS,
        wxString(wxT("Options")),
        wxEmptyString,
        wxITEM_NORMAL
    );
    
    menuitem2 = new wxMenuItem(
        menu1,
        ID_RELOAD,
        wxString(wxT("Reload")),
        wxEmptyString,
        wxITEM_NORMAL
    );
    
    menuitem3 = new wxMenuItem(
        menu1,
        ID_EXIT,
        wxString(wxT("Exit")),
        wxEmptyString,
        wxITEM_NORMAL
    );
    
    menuitem4 = new wxMenuItem(
        menu2,
        ID_ABOUT,
        wxString(wxT("About")),
        wxEmptyString,
        wxITEM_NORMAL
    );
    
    menu1->Append(menuitem1);
    menu1->Append(menuitem2);
    menu1->Append(menuitem3);
    menu2->Append(menuitem4);
    
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &Menu::OnOptions, this, ID_OPTIONS);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &Menu::OnAbout, this, ID_ABOUT);
    
    this->Append(menu1, wxT("File"));
    this->Append(menu2, wxT("Help"));
}

Menu::~Menu()
{

}

void Menu::OnReload(wxCommandEvent &event)
{
    wxGetApp().SetRestart(true);
    wxGetApp().ExitMainLoop();
}

void Menu::OnExit(wxCommandEvent &event)
{
    Connect(
        ID_EXIT,
        wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(Window::OnExit)
    );
}

void Menu::ShowAbout()
{
    about = new About();
    about->Show();
    about->Bind(wxEVT_CLOSE_WINDOW, &Menu::OnAboutClose, this);
}

void Menu::OnAboutClose(wxCloseEvent &event)
{
    about = nullptr;
    event.Skip();
}

void Menu::OnAbout(wxCommandEvent &event)
{
    if (about == nullptr)
    {
        ShowAbout();
    }
    else
    {
        about->Raise();
    }
}

void Menu::ShowOptions()
{
    options = new Options();
    options->Show();
    options->Bind(wxEVT_CLOSE_WINDOW, &Menu::OnOptionsClose, this);
}

void Menu::OnOptionsClose(wxCloseEvent &event)
{
    options = nullptr;
    event.Skip();
}

void Menu::OnOptions(wxCommandEvent &event)
{
    if (options == nullptr)
    {
        ShowOptions();
    }
    else
    {
        options->Raise();
    }
}

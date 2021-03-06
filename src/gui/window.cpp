// @TODO:
// - Use wxThread?
// - Open explorer/finder after parsing
// - Store the options in the application/globally?
// - Add comments

// @DEBUG:
// - wxMessageOutput::Get()->Printf(wxT("%s"), VARIABLE_HERE);

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <filesystem>
#include <thread>
#include <vector>

#include "about.h"
#include "app.h"
#include "ini.h"
#include "listbox.h"
#include "make.h"
#include "menu.h"
#include "merge.h"
#include "options.h"
#include "utilities.h"
#include "whereami.h"
#include "window.h"

namespace fs = std::filesystem;


enum
{
    ID_BROWSE,
    ID_MAKE,
    ID_MERGE
};

wxBEGIN_EVENT_TABLE(Window, wxFrame)
    EVT_MENU(wxID_ANY, Window::OnExit)
    EVT_BUTTON(ID_BROWSE, Window::OnBrowse)
    EVT_BUTTON(ID_MAKE, Window::OnMake)
    EVT_BUTTON(ID_MERGE, Window::OnMerge)
    EVT_TIMER(-1, Window::OnTimer)
    EVT_CHAR_HOOK(Window::OnKeyDown)
wxEND_EVENT_TABLE()

Window::Window() :
    wxFrame(
        nullptr,
        wxID_ANY,
        "DBgen",
        wxPoint(30, 30),
        wxSize(450, 550),
        (wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER & ~wxMAXIMIZE_BOX)
    )
{
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    timer = new wxTimer(this);

    menubar = new Menu();
    this->SetMenuBar(menubar);

    wxBoxSizer *sizer1;
    wxBoxSizer *sizer2;
    wxBoxSizer *sizer3;
    wxBoxSizer *sizer4;

    sizer1 = new wxBoxSizer(wxVERTICAL);
    sizer2 = new wxBoxSizer(wxVERTICAL);
    sizer3 = new wxBoxSizer(wxVERTICAL);
    sizer4 = new wxBoxSizer(wxHORIZONTAL);

    text = new wxStaticText(
        this,
        wxID_ANY,
        wxT("Complete!"),
        wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL
    );

    text->Wrap(-1);

    text->Hide();

    listbox = new Listbox(this);

    int count = listbox->GetCount();

    if (count > 0)
    {
        listbox->SetSelection(0);
        listbox->SetFocus();
    }

    browse_button = new wxButton(
        this,
        ID_BROWSE,
        wxT("Browse"),
        wxPoint(300, 300),
        wxSize(300, -1),
        0
    );

    make_button = new wxButton(
        this,
        ID_MAKE,
        wxT("Make"),
        wxDefaultPosition,
        wxSize(145, -1),
        0
    );

    merge_button = new wxButton(
        this,
        ID_MERGE,
        wxT("Merge"),
        wxDefaultPosition,
        wxSize(145, -1),
        0
    );

    sizer2->Add(0, 20, 0, wxEXPAND, 0);
    sizer2->Add(text, 0, wxALIGN_CENTER | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 5);
    sizer2->Add(0, 20, 0, wxEXPAND, 0);
    sizer2->Add(listbox, 0, wxALIGN_CENTER | wxALL, 5);
    sizer3->Add(browse_button, 0, wxALL, 5);
    sizer2->Add(sizer3, 1, wxALIGN_CENTER | wxALL, 5);
    sizer2->Add(0, 0, 1, wxEXPAND, 5);
    sizer4->Add(make_button, 0, wxALL, 5);
    sizer4->Add(merge_button, 0, wxALL, 5);
    sizer2->Add(sizer4, 0, wxALIGN_CENTER | wxALL, 5);
    sizer1->Add(sizer2, 1, wxEXPAND, 5);

    this->SetSizer(sizer1);
    this->Layout();
    this->Centre(wxBOTH);
}

Window::~Window()
{

}

void Window::OnExit(wxCommandEvent &event)
{
    Close(true);
    event.Skip();
}

void Window::OnBrowse(wxCommandEvent &WXUNUSED(event))
{
    std::string base = get_base_path();
    fs::path options = base / fs::path("options.ini");

    mINI::INIFile file(options.string());
    mINI::INIStructure ini;
    file.read(ini);

    wxString input = wxString(ini["directory"]["output"]);

    wxFileDialog *OpenDialog = new wxFileDialog(
        this, _("Choose a file to open"),
        input, wxEmptyString,
        _("Genbank, Fasta (*.gb;*.fasta)|*.gb;*.fasta"),
        wxFD_OPEN | wxFD_MULTIPLE, wxDefaultPosition
    );

    if (OpenDialog->ShowModal() == wxID_OK)
    {
        wxArrayString file;
        OpenDialog->GetPaths(file);

        for (int i = 0; i < file.size(); i++)
        {
            fs::path path = file[i].ToStdString();

            if (path.extension() == ".fasta" || path.extension() == ".gb")
            {
                listbox->AppendString(file[i]);
            }
        }
    }
}

void Window::OnMake(wxCommandEvent &WXUNUSED(event))
{
    wxArrayString list = listbox->GetStrings();

    if (list.IsEmpty())
    {
        wxMessageBox(
            "Please add a file.",
            "Warning",
            wxOK | wxICON_EXCLAMATION | wxCENTRE
        );
    }
    else
    {
        std::string base = get_base_path();
        fs::path options = base / fs::path("options.ini");

        mINI::INIFile file(options.string());
        mINI::INIStructure ini;
        file.read(ini);

        for (unsigned int i = 0; i < list.size(); i++)
        {
            fs::path input = list[i].ToStdString();

            if (input.extension() == ".gb")
            {
                fs::path filename = input.filename();
                fs::path output = fs::path(ini["directory"]["output"]) / fs::path(filename);

                std::thread make(DatabaseMaker, input, output);
                make.detach();
            }
            else
            {
                wxMessageBox(
                    "Please use a file with a .gb extension.",
                    "Warning",
                    wxOK | wxICON_EXCLAMATION | wxCENTRE
                );

                return;
            }
        }

        timer->StartOnce(3500);
        text->Show();
    }
}

void Window::OnMerge(wxCommandEvent &WXUNUSED(event))
{
    wxArrayString list = listbox->GetStrings();

    if (list.IsEmpty())
    {
        wxMessageBox(
            "Please add a file.",
            "Warning",
            wxOK | wxICON_EXCLAMATION | wxCENTRE
        );
    }
    else
    {
        size_t count = list.GetCount();

        if (count == 2)
        {
            std::string base = get_base_path();
            fs::path options = base / fs::path("options.ini");

            mINI::INIFile file(options.string());
            mINI::INIStructure ini;
            file.read(ini);

            fs::path input1 = list[0].ToStdString();
            fs::path input2 = list[1].ToStdString();

            if (input1.extension() == ".fasta" && input2.extension() == ".fasta")
            {
                fs::path output = fs::path(ini["directory"]["output"]);
                std::thread merge(DatabaseMerge, input1, input2, output);
                merge.detach();
            }
            else
            {
                wxMessageBox(
                    "Please use a file with a .fasta extension.",
                    "Warning",
                    wxOK | wxICON_EXCLAMATION | wxCENTRE
                );

                return;
            }

            timer->StartOnce(3500);
            text->Show();
        }
        else
        {
            wxMessageBox(
                "Please include two .fasta files.",
                "Warning",
                wxOK | wxICON_EXCLAMATION | wxCENTRE
            );
        }
    }
}

void Window::OnTimer(wxTimerEvent &event)
{
    text->Hide();
}

void Window::OnKeyDown(wxKeyEvent &event)
{
    if ((int)event.GetKeyCode() == WXK_F5)
    {
        wxGetApp().SetRestart(true);
        wxGetApp().ExitMainLoop();
    }

    event.Skip();
}

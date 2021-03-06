#pragma once

#include <wx/filepicker.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif


class Options : public wxFrame
{
    public:
        Options();
        ~Options();
        
    public:
        void OnLoad();
        void OnAutoscan(wxCommandEvent &);
        void OnSave(wxCommandEvent &);
        void OnExit(wxCommandEvent &);
        void OnTyping(wxKeyEvent &);
        
    private:
        wxPanel
        *panel1,
        *panel2;
        
        wxBoxSizer
        *sizer1,
        *sizer2,
        *sizer3,
        *sizer4,
        *sizer5;
        
        wxStaticText
        *title,
        *title2,
        *label1,
        *label2,
        *label3,
        *label4;
        
        wxDirPickerCtrl
        *directory1,
        *directory2;
        
        wxCheckBox
        *checkbox1,
        *checkbox2,
        *checkbox3;
        
        wxButton
        *save_button,
        *cancel_button;
        
        wxDECLARE_EVENT_TABLE();
};

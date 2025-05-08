#include "pch.h"
#include "OptionsDlg.h"
#include "resource.h"
#include "DataManager.h"

IMPLEMENT_DYNAMIC(COptionsDlg, CDialogEx)

//COptionsDlg::COptionsDlg(CDataManager* data_manager, CWnd* pParent)
//    : CDialogEx(IDD_OPTIONS_DIALOG, pParent)
//    , m_data_manager(data_manager)
//{
//}

COptionsDlg::COptionsDlg(CWnd* pParent /*=nullptr*/)
    : CDialog(IDD_OPTIONS_DIALOG, pParent)
{
}

COptionsDlg::~COptionsDlg()
{
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
    ON_BN_CLICKED(IDC_ADD_NOTE, &COptionsDlg::OnAddNote)
    ON_BN_CLICKED(IDC_DELETE_NOTE, &COptionsDlg::OnDeleteNote)
END_MESSAGE_MAP()

BOOL COptionsDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    //CDialogEx::OnInitDialog();

    // 初始化对话框
    UpdateNotesList();

    return TRUE;
}

void COptionsDlg::OnOK()
{
    // 保存设置
    CDialog::OnOK();
}

void COptionsDlg::OnAddNote()
{
    CString note;
    GetDlgItemText(IDC_NOTE_EDIT, note);
    if (!note.IsEmpty())
    {
        g_data.AddNote(note.GetString());
        SetDlgItemText(IDC_NOTE_EDIT, _T(""));
        UpdateNotesList();
    }
}

void COptionsDlg::OnDeleteNote()
{
    CListBox* pList = (CListBox*)GetDlgItem(IDC_NOTES_LIST);
    if (pList)
    {
        int sel = pList->GetCurSel();
        if (sel != LB_ERR)
        {
            g_data.DeleteNote(sel);
            UpdateNotesList();
        }
    }
}

void COptionsDlg::UpdateNotesList()
{
    CListBox* pList = (CListBox*)GetDlgItem(IDC_NOTES_LIST);
    if (pList)
    {
        pList->ResetContent();
        const auto& notes = g_data.GetNotes();
        if (notes.empty()) return;
        for (const auto& note : notes)
        {
            pList->AddString(note.c_str());
        }
    }
}
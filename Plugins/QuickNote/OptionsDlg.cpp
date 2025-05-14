#include "pch.h"
#include "OptionsDlg.h"
#include "resource.h"
#include "DataManager.h"
#include "AddNoteDlg.h"

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
    DDX_Control(pDX, IDC_NOTES_LIST, m_notesList);
}

BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
    ON_BN_CLICKED(IDC_ADD_NOTE, &COptionsDlg::OnAddNote)
    ON_BN_CLICKED(IDC_DELETE_NOTE, &COptionsDlg::OnDeleteNote)
    ON_NOTIFY(NM_DBLCLK, IDC_NOTES_LIST, &COptionsDlg::OnLvnItemActivateNotesList)
END_MESSAGE_MAP()

BOOL COptionsDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    //CDialogEx::OnInitDialog();
    m_notesList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_notesList.InsertColumn(0, L"内容", LVCFMT_LEFT, 200);
    m_notesList.InsertColumn(1, L"创建时间", LVCFMT_LEFT, 150);
    m_notesList.InsertColumn(2, L"修改时间", LVCFMT_LEFT, 150);

    // 初始化对话框
    UpdateNotesList();

    return TRUE;
}

void COptionsDlg::OnOK()
{
    // 保存设置
    CDialog::OnOK();
}


// 然后实现 OnAddNote
void COptionsDlg::OnAddNote()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CAddNoteDlg dlg(this);
    if (dlg.DoModal() == IDOK)
    {
        if (!dlg.m_strNote.IsEmpty())
        {
            g_data.AddNote(dlg.m_strNote.GetString());
            UpdateNotesList();
        }
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

void COptionsDlg::OnLvnItemActivateNotesList(NMHDR* pNMHDR, LRESULT* pResult)
{
    int sel = m_notesList.GetNextItem(-1, LVNI_SELECTED);
    if (sel >= 0)
    {
        const auto& notes = g_data.GetNotes();
        if (sel >= notes.size()) return;

        const NoteData& note = notes[sel];

        // 打开编辑对话框
        CAddNoteDlg dlg(this);
        dlg.m_strNote = note.text.c_str(); // 填入原始笔记内容

        if (dlg.DoModal() == IDOK)
        {
            // 用户点击了确定，更新数据库
            g_data.UpdateNoteTextById(note.id, dlg.m_strNote.GetString());

            // 刷新界面显示
            UpdateNotesList();
        }
    }

    *pResult = 0;
}

void COptionsDlg::UpdateNotesList()
{
    m_notesList.DeleteAllItems();

    const auto& notes = g_data.GetNotes();
    if (notes.empty()) return;

    int index = 0;
    for (const auto& note : notes)
    {
        m_notesList.InsertItem(index, note.text.c_str()); // 第1列：内容
        m_notesList.SetItemText(index, 1, note.createTime.c_str()); // 第2列：创建时间
        m_notesList.SetItemText(index, 2, note.updateTime.c_str()); // 第3列：修改时间
        ++index;
    }
}
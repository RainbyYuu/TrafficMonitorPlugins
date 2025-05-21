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
    m_notesList.InsertColumn(0, L"����", LVCFMT_LEFT, 200);
    m_notesList.InsertColumn(1, L"����ʱ��", LVCFMT_LEFT, 150);
    m_notesList.InsertColumn(2, L"�޸�ʱ��", LVCFMT_LEFT, 150);

    // ��ʼ���Ի���
    UpdateNotesList();

    return TRUE;
}

void COptionsDlg::OnOK()
{
    // ��������
    CDialog::OnOK();
}


// Ȼ��ʵ�� OnAddNote
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
    CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_NOTES_LIST);
    POSITION pos = pList->GetFirstSelectedItemPosition();
    if (pos)
    {
        int sel = pList->GetNextSelectedItem(pos);
        //CString str = pList->GetItemText(sel, 0);  // ȡ��һ���ı�
        //CString msg;
        //msg.Format(L"��ѡ���˵� %d ��: %s", sel + 1, str);
        //MessageBox(msg, L"������Ϣ");
        if (pList)
        {
            if (sel != -1)
            {
                // ����ȷ�϶Ի���
                int result = MessageBox(L"ȷ��Ҫɾ�������ʼ���", L"��ʾ", MB_YESNO | MB_ICONQUESTION);
                if (result == IDYES)
                {
                    g_data.DeleteNote(sel);
                    UpdateNotesList();
                }
                // �������κβ���
            }
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

        // �򿪱༭�Ի���
        CAddNoteDlg dlg(this);

        dlg.m_strNote = note.text.c_str(); // ����ԭʼ�ʼ�����

        if (dlg.DoModal() == IDOK)
        {
            // �û������ȷ�����������ݿ�
            g_data.UpdateNoteTextById(note.id, dlg.m_strNote.GetString());

            // ˢ�½�����ʾ
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
        m_notesList.InsertItem(index, note.summary.c_str()); // ��ʾժҪ��������
        m_notesList.SetItemText(index, 1, note.createTime.c_str());
        m_notesList.SetItemText(index, 2, note.updateTime.c_str());
        ++index;
    }
}

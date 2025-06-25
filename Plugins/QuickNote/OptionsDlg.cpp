#include "pch.h"
#include "OptionsDlg.h"
#include "resource.h"
#include "DataManager.h"
#include "AddNoteDlg.h"
#include "QuickNoteDlg.h"

IMPLEMENT_DYNAMIC(COptionsDlg, CDialog)

COptionsDlg::COptionsDlg(CWnd* pParent /*=nullptr*/, int categoryId)
    : CDialog(IDD_OPTIONS_DIALOG, pParent), categoryId(categoryId)
{
}

COptionsDlg::~COptionsDlg()
{
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_NOTES_LIST, m_notesList);
	DDX_Control(pDX, IDC_ADD_NOTE, m_btnAddNote);
    DDX_Control(pDX, IDC_DELETE_NOTE, m_btnDeleteNote);
    DDX_Control(pDX, IDC_RETURN_MAIN, m_btnReturnMain);
    DDX_Control(pDX, IDC_BTN_BATCH_PRINT, m_btnBatchPrint);
}

BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
    ON_BN_CLICKED(IDC_ADD_NOTE, &COptionsDlg::OnAddNote)
    ON_BN_CLICKED(IDC_DELETE_NOTE, &COptionsDlg::OnDeleteNote)
    ON_NOTIFY(NM_DBLCLK, IDC_NOTES_LIST, &COptionsDlg::OnLvnItemActivateNotesList)
    ON_BN_CLICKED(IDC_RETURN_MAIN, &COptionsDlg::OnBnClickedReturnMain)
    ON_BN_CLICKED(IDC_BTN_BATCH_PRINT, &COptionsDlg::OnBnClickedBatchPrint) // ӳ���°�ť
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BOOL COptionsDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_bmpBackground.LoadBitmap(IDB_BACKGROUND);
    m_btnAddNote.LoadBitmaps(IDB_ADD_NORMAL,IDB_ADD_HOVER,IDB_ADD_DOWN);
	m_btnDeleteNote.LoadBitmaps(IDB_DEL_HOVER, IDB_DEL_HOVER, IDB_DEL_DOWN);
	m_btnReturnMain.LoadBitmaps(IDB_RETURN, IDB_RETURN, IDB_RETURN);
    m_btnBatchPrint.LoadBitmaps(IDB_PRINT_NORMAL, IDB_PRINT_HOVER, IDB_PRINT_DOWN);

    m_btnAddNote.SetButtonID(IDC_ADD_NOTE);
    m_btnDeleteNote.SetButtonID(IDC_DELETE_NOTE);

    m_notesList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);
    m_notesList.InsertColumn(0, L"����", LVCFMT_LEFT, 200);
    m_notesList.InsertColumn(1, L"����ʱ��", LVCFMT_LEFT, 150);
    m_notesList.InsertColumn(2, L"�޸�ʱ��", LVCFMT_LEFT, 150);

    vector<std::pair<int, std::wstring>>ca = g_data.m_setting_data.category;
    //���ص�ǰ������
    for (int i = 0; i < ca.size(); i++)
    {
        if (ca[i].first == categoryId)
        {
            categoryName = ca[i].second.c_str();
            break;
        }
    }

    // ��ʼ���Ի���
    UpdateNotesList();

    return TRUE;
}


// Ȼ��ʵ�� OnAddNote
void COptionsDlg::OnAddNote()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CAddNoteDlg dlg(this);

    dlg.m_strCategory = categoryName;

    if (dlg.DoModal() == IDOK)
    {
        if (!dlg.m_strNote.IsEmpty())
        {
            if (dlg.m_comboCategoryId)
            {   //���ѡ���˷��࣬��ȡ����ID
                g_data.AddNote(dlg.m_strNote.GetString(), dlg.m_comboCategoryId);
            }//���δѡ����࣬����ʼ���������
            else g_data.AddNote(dlg.m_strNote.GetString(), categoryId);
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
        if (sel >= notes.size()) return;

        const NoteData& note = notes[sel];

        CEditNoteDlg dlg(this);

        dlg.m_strNote = note.text.c_str();// ����ԭʼ�ʼ�����

        dlg.m_strCategory = categoryName;


        if (dlg.DoModal() == IDOK)
        {
            g_data.UpdateNoteTextById(note.id, dlg.m_strNote.GetString(), dlg.m_comboCategoryId);

            UpdateNotesList();
        }

    }

    *pResult = 0;
}

//�ڴ˴����±ʼ��б�����˽������notes��
void COptionsDlg::UpdateNotesList()
{
    m_notesList.DeleteAllItems();

    notes = g_data.getNotesWithCategoryId(categoryId);
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

void COptionsDlg::setCategoryId(int categoryId)
{
    this->categoryId = categoryId;
}

void COptionsDlg::OnBnClickedReturnMain()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CQuickNoteDlg dlg(this);
    EndDialog(IDOK);
    if (dlg.DoModal() == IDOK)
    {
        g_data.SaveConfig();
    }
}


BOOL COptionsDlg::OnEraseBkgnd(CDC* pDC)
{
    if (m_bmpBackground.GetSafeHandle())
    {
        // �����ڴ� DC ��ѡ��λͼ
        CDC memDC;
        memDC.CreateCompatibleDC(pDC); 
        CBitmap* pOldBmp = memDC.SelectObject(&m_bmpBackground);

        // ��ȡ�Ի����С
        CRect rect;
        GetClientRect(&rect);

        // ��ȡͼƬ��С
        BITMAP bmp = {};
        m_bmpBackground.GetBitmap(&bmp);

        // ƽ�̻����챳��
        for (int y = 0; y < rect.Height(); y += bmp.bmHeight)
        {
            for (int x = 0; x < rect.Width(); x += bmp.bmWidth)
            {
                pDC->BitBlt(x, y, bmp.bmWidth, bmp.bmHeight, &memDC, 0, 0, SRCCOPY);
            }
        }

        memDC.SelectObject(pOldBmp);
        return TRUE;  // ��ʾ�����Լ������˱�������
    }

    return CDialog::OnEraseBkgnd(pDC);
}

void COptionsDlg::OnBnClickedBatchPrint()
{

    for (int i = 0; i < m_notesList.GetItemCount(); ++i)
    {
        if (m_notesList.GetCheck(i)) // ����Ƿ�ѡ��
        {
            if (i < notes.size())
            {
                CString noteText = notes[i].text.c_str();
                PrintNote(noteText);
            }
        }
    }
}

void COptionsDlg::PrintNote(const CString& noteText)
{
    CPrintDialog printDlg(FALSE);
    if (printDlg.DoModal() == IDOK)
    {
        CDC dc;
        dc.Attach(printDlg.GetPrinterDC());
        dc.StartDoc(_T("��ӡ�ʼ�"));
        dc.StartPage();

        dc.TextOut(100, 100, noteText); // �򵥴�ӡ���ݣ��ɻ��ɻ��д���

        dc.EndPage();
        dc.EndDoc();
        dc.Detach();
        this->MessageBox(_T("��ӡ�ɹ�"), _T("��ʾ"), MB_OK | MB_ICONINFORMATION);
    }
}

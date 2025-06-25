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
    ON_BN_CLICKED(IDC_BTN_BATCH_PRINT, &COptionsDlg::OnBnClickedBatchPrint) // 映射新按钮
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
    m_notesList.InsertColumn(0, L"内容", LVCFMT_LEFT, 200);
    m_notesList.InsertColumn(1, L"创建时间", LVCFMT_LEFT, 150);
    m_notesList.InsertColumn(2, L"修改时间", LVCFMT_LEFT, 150);

    vector<std::pair<int, std::wstring>>ca = g_data.m_setting_data.category;
    //加载当前分类名
    for (int i = 0; i < ca.size(); i++)
    {
        if (ca[i].first == categoryId)
        {
            categoryName = ca[i].second.c_str();
            break;
        }
    }

    // 初始化对话框
    UpdateNotesList();

    return TRUE;
}


// 然后实现 OnAddNote
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
            {   //如果选择了分类，获取分类ID
                g_data.AddNote(dlg.m_strNote.GetString(), dlg.m_comboCategoryId);
            }//如果未选择分类，加入笔记所属分类
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
                // 弹出确认对话框
                int result = MessageBox(L"确定要删除这条笔记吗？", L"提示", MB_YESNO | MB_ICONQUESTION);
                if (result == IDYES)
                {
                    g_data.DeleteNote(sel);
                    UpdateNotesList();
                }
                // 否则不做任何操作
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

        dlg.m_strNote = note.text.c_str();// 填入原始笔记内容

        dlg.m_strCategory = categoryName;


        if (dlg.DoModal() == IDOK)
        {
            g_data.UpdateNoteTextById(note.id, dlg.m_strNote.GetString(), dlg.m_comboCategoryId);

            UpdateNotesList();
        }

    }

    *pResult = 0;
}

//在此处更新笔记列表，存入私有属性notes中
void COptionsDlg::UpdateNotesList()
{
    m_notesList.DeleteAllItems();

    notes = g_data.getNotesWithCategoryId(categoryId);
    if (notes.empty()) return;

    int index = 0;
    for (const auto& note : notes)
    {
        m_notesList.InsertItem(index, note.summary.c_str()); // 显示摘要而非正文
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
        // 创建内存 DC 并选入位图
        CDC memDC;
        memDC.CreateCompatibleDC(pDC); 
        CBitmap* pOldBmp = memDC.SelectObject(&m_bmpBackground);

        // 获取对话框大小
        CRect rect;
        GetClientRect(&rect);

        // 获取图片大小
        BITMAP bmp = {};
        m_bmpBackground.GetBitmap(&bmp);

        // 平铺或拉伸背景
        for (int y = 0; y < rect.Height(); y += bmp.bmHeight)
        {
            for (int x = 0; x < rect.Width(); x += bmp.bmWidth)
            {
                pDC->BitBlt(x, y, bmp.bmWidth, bmp.bmHeight, &memDC, 0, 0, SRCCOPY);
            }
        }

        memDC.SelectObject(pOldBmp);
        return TRUE;  // 表示我们自己处理了背景擦除
    }

    return CDialog::OnEraseBkgnd(pDC);
}

void COptionsDlg::OnBnClickedBatchPrint()
{

    for (int i = 0; i < m_notesList.GetItemCount(); ++i)
    {
        if (m_notesList.GetCheck(i)) // 检查是否选中
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
        dc.StartDoc(_T("打印笔记"));
        dc.StartPage();

        dc.TextOut(100, 100, noteText); // 简单打印内容，可换成换行处理

        dc.EndPage();
        dc.EndDoc();
        dc.Detach();
        this->MessageBox(_T("打印成功"), _T("提示"), MB_OK | MB_ICONINFORMATION);
    }
}

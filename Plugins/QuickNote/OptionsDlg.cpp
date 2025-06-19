#include "pch.h"
#include "OptionsDlg.h"
#include "resource.h"
#include "DataManager.h"
#include "AddNoteDlg.h"
#include "QuickNoteDlg.h"

IMPLEMENT_DYNAMIC(COptionsDlg, CDialog)

//COptionsDlg::COptionsDlg(CDataManager* data_manager, CWnd* pParent)
//    : CDialog(IDD_OPTIONS_DIALOG, pParent)
//    , m_data_manager(data_manager)
//{
//}

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
}

BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
    ON_BN_CLICKED(IDC_ADD_NOTE, &COptionsDlg::OnAddNote)
    ON_BN_CLICKED(IDC_DELETE_NOTE, &COptionsDlg::OnDeleteNote)
    ON_NOTIFY(NM_DBLCLK, IDC_NOTES_LIST, &COptionsDlg::OnLvnItemActivateNotesList)
    ON_BN_CLICKED(IDC_RETURN_MAIN, &COptionsDlg::OnBnClickedReturnMain)
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BOOL COptionsDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_bmpBackground.LoadBitmap(IDB_BACKGROUND);
    m_btnAddNote.LoadBitmaps(IDB_ADD_NORMAL,IDB_ADD_HOVER,IDB_ADD_DOWN);
	m_btnDeleteNote.LoadBitmaps(IDB_DEL_HOVER, IDB_DEL_HOVER, IDB_DEL_DOWN);
	m_btnReturnMain.LoadBitmaps(IDB_RETURN, IDB_RETURN, IDB_RETURN);

    m_notesList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_notesList.InsertColumn(0, L"内容", LVCFMT_LEFT, 200);
    m_notesList.InsertColumn(1, L"创建时间", LVCFMT_LEFT, 150);
    m_notesList.InsertColumn(2, L"修改时间", LVCFMT_LEFT, 150);

    // 初始化对话框
    UpdateNotesList();

    return TRUE;
}


// 然后实现 OnAddNote
void COptionsDlg::OnAddNote()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CAddNoteDlg dlg(this);
    vector<std::pair<int, std::wstring>>ca = g_data.m_setting_data.category;
    //在combobox显示本分类名
    for (int i = 0; i < ca.size(); i++)
    {
        if (ca[i].first == categoryId)
        {
            dlg.m_strCategory = ca[i].second.c_str();
            break;
        }
    }
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
        //CString str = pList->GetItemText(sel, 0);  // 取第一列文本
        //CString msg;
        //msg.Format(L"你选中了第 %d 行: %s", sel + 1, str);
        //MessageBox(msg, L"调试信息");
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

        vector<std::pair<int, std::wstring>>ca = g_data.m_setting_data.category;
        int cid = note.categoryId;
        for (int i = 0; i < ca.size(); i++)
        {
            if (ca[i].first == cid)
            {
                dlg.m_strCategory = ca[i].second.c_str();
                break;
            }
        }

        if (dlg.DoModal() == IDOK)
        {
            g_data.UpdateNoteTextById(note.id, dlg.m_strNote.GetString(), note.categoryId);

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

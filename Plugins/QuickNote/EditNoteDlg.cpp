#include "pch.h"
#include "resource.h"
#include "DataManager.h"
#include "EditNoteDlg.h"

IMPLEMENT_DYNAMIC(CEditNoteDlg, CDialog)


CEditNoteDlg::CEditNoteDlg(CWnd* pParent /*=nullptr*/)
    : CDialog(IDD_EDITNOTE_DIALOG, pParent)
{
}

void CEditNoteDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_NOTE_EDIT, m_editNote);
    DDX_Control(pDX, IDC_CATEGORY_OPTION, m_comboCategory);
    DDX_Text(pDX, IDC_NOTE_EDIT, m_strNote);
    DDX_Text(pDX, IDC_CATEGORY_OPTION, m_strCategory);
}

BEGIN_MESSAGE_MAP(CEditNoteDlg, CDialog)
    ON_CBN_SELCHANGE(IDC_CATEGORY_OPTION, &CEditNoteDlg::OnCbnSelchangeCategoryOption)
    ON_BN_CLICKED(IDC_PRINT_BUTTON, &CEditNoteDlg::OnBnClickedPrintButton)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BOOL CEditNoteDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_bmpBackground.LoadBitmap(IDB_BACKGROUND2);

    UpdateData(FALSE);

    m_data = g_data.m_setting_data;

    // 获取 ComboBox 控件指针
    CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_CATEGORY_OPTION);
    for (int i = 0; i < m_data.category.size(); i++)
    {
        pCombo->AddString(m_data.category[i].second.c_str());
    }

    return TRUE;
}

void CEditNoteDlg::OnOK()
{
    // 保存设置
    m_editNote.GetWindowText(m_strNote);
    CDialog::OnOK();
}

void CEditNoteDlg::OnCbnSelchangeCategoryOption()
{
    CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_CATEGORY_OPTION);  // 获取ComboBox控件指针
    if (pComboBox != nullptr)
    {
        int nIndex = pComboBox->GetCurSel();  // 获取当前选中的索引
        if (nIndex != CB_ERR)  // 确保有选项被选中
        {
            CString name;
            pComboBox->GetLBText(nIndex, name);  // 获取选中项的文本
            m_comboCategoryId = m_data.category[nIndex].first;//扩展 
        }
    }
}

BOOL CEditNoteDlg::OnEraseBkgnd(CDC* pDC)
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

void CEditNoteDlg::OnBnClickedPrintButton()
{
    PrintNote(m_strNote);
}

void CEditNoteDlg::PrintNote(const CString& noteText)
{
    CPrintDialog printDlg(FALSE);
    if (printDlg.DoModal() == IDOK)
    {
        CDC dc;
        dc.Attach(printDlg.GetPrinterDC());
        dc.StartDoc(_T("打印笔记"));
        dc.StartPage();

        // 设置字体
        CFont font;
        font.CreatePointFont(120, _T("宋体"), &dc);  // 12pt 字体
        CFont* pOldFont = dc.SelectObject(&font);

        // 打印内容（可自动换行）
        CRect rect(100, 100, 7000, 10000); // 页面范围（单位：0.01mm）
        dc.DrawText(noteText, &rect, DT_LEFT | DT_WORDBREAK | DT_EXPANDTABS);

        dc.SelectObject(pOldFont);
        font.DeleteObject();

        dc.EndPage();
        dc.EndDoc();
        dc.DeleteDC();
        AfxMessageBox(_T("打印成功！"));
    }
    else
    {
        AfxMessageBox(_T("打印失败！"));
    }
}




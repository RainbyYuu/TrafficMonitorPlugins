#include "pch.h"
#include "resource.h"
#include "DataManager.h"
#include "AddNoteDlg.h"

IMPLEMENT_DYNAMIC(CAddNoteDlg, CDialog)

CAddNoteDlg::CAddNoteDlg(CWnd* pParent /*=nullptr*/)
    : CDialog(IDD_ADDNOTE_DIALOG, pParent)
{
}

void CAddNoteDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_NOTE_EDIT, m_editNote);
	DDX_Control(pDX, IDC_CATEGORY_OPTION, m_comboCategory);
    DDX_Text(pDX, IDC_NOTE_EDIT, m_strNote);
    DDX_Text(pDX, IDC_CATEGORY_OPTION, m_strCategory);
}

BEGIN_MESSAGE_MAP(CAddNoteDlg, CDialog)
    ON_CBN_SELCHANGE(IDC_CATEGORY_OPTION, &CAddNoteDlg::OnCbnSelchangeCategoryOption)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BOOL CAddNoteDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_bmpBackground.LoadBitmap(IDB_BACKGROUND2);

    UpdateData(FALSE);

    m_data = g_data.m_setting_data;

    // ��ȡ ComboBox �ؼ�ָ��
    CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_CATEGORY_OPTION);
    for (int i = 0; i < m_data.category.size(); i++)
    {
        pCombo->AddString(m_data.category[i].second.c_str());
    }

    return TRUE;
}

void CAddNoteDlg::OnOK()
{
    // ��������
    m_editNote.GetWindowText(m_strNote);
    CDialog::OnOK();
}

void CAddNoteDlg::OnCbnSelchangeCategoryOption()
{
    CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_CATEGORY_OPTION);  // ��ȡComboBox�ؼ�ָ��
    if (pComboBox != nullptr)
    {
        int nIndex = pComboBox->GetCurSel();  // ��ȡ��ǰѡ�е�����
        if (nIndex != CB_ERR)  // ȷ����ѡ�ѡ��
        {
            CString name;
            pComboBox->GetLBText(nIndex, name);  // ��ȡѡ������ı�
            m_comboCategoryId = m_data.category[nIndex].first;
        }
    }
}


BOOL CAddNoteDlg::OnEraseBkgnd(CDC* pDC)
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

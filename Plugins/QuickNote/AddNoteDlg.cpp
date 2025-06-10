#include "pch.h"
#include "resource.h"
#include "DataManager.h"
#include "AddNoteDlg.h"

IMPLEMENT_DYNAMIC(CAddNoteDlg, CDialog)

//COptionsDlg::COptionsDlg(CDataManager* data_manager, CWnd* pParent)
//    : CDialog(IDD_OPTIONS_DIALOG, pParent)
//    , m_data_manager(data_manager)
//{
//}

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
END_MESSAGE_MAP()

BOOL CAddNoteDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

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

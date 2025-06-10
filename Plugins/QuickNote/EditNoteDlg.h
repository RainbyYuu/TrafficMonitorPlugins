#pragma once
#include "DataManager.h"
#include <afxdialogex.h>


class CEditNoteDlg : public CDialog
{
    DECLARE_DYNAMIC(CEditNoteDlg)
public:
    CEditNoteDlg(CWnd* pParent = nullptr);

    CString m_strNote;
    CString m_strCategory;
    CEdit m_editNote;
    CComboBox m_comboCategory;
    int m_comboCategoryId; // ���ڴ洢��������
    SettingData m_data;

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_EDITNOTE_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnCbnSelchangeCategoryOption();
    DECLARE_MESSAGE_MAP()
};

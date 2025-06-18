#pragma once
#include "DataManager.h"
#include <afxdialogex.h>


class CAddNoteDlg : public CDialog
{
    DECLARE_DYNAMIC(CAddNoteDlg)
public:
    CAddNoteDlg(CWnd* pParent = nullptr);

    CString m_strNote;
    CString m_strCategory;
    CEdit m_editNote;
	CComboBox m_comboCategory;
	int m_comboCategoryId; // 用于存储分类名称
    SettingData m_data;

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ADDNOTE_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnCbnSelchangeCategoryOption();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    DECLARE_MESSAGE_MAP()
private:
    CBitmap m_bmpBackground;
    CBrush m_brush;
};

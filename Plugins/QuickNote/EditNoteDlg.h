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
    int m_comboCategoryId; // 用于存储分类名称
    SettingData m_data;

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_EDITNOTE_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    void PrintNote(const CString& noteText);
    virtual void OnOK();
    afx_msg void OnCbnSelchangeCategoryOption();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnBnClickedPrintButton();
    DECLARE_MESSAGE_MAP()
private:
    CBitmap m_bmpBackground;
    CBrush m_brush;
};

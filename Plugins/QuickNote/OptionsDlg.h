#pragma once
#include "DataManager.h"
#include <afxdialogex.h>

class COptionsDlg : public CDialog
{
    DECLARE_DYNAMIC(COptionsDlg)

public:
    //COptionsDlg(CDataManager* data_manager, CWnd* pParent = nullptr);
    COptionsDlg(CWnd* pParent = nullptr);   // ��׼���캯��
    virtual ~COptionsDlg();

    SettingData m_data;

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_OPTIONS_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
    virtual BOOL OnInitDialog();
    virtual void OnOK();

    afx_msg void OnAddNote();
    afx_msg void OnDeleteNote();
    void UpdateNotesList();

    DECLARE_MESSAGE_MAP()

private:
    //CDataManager* m_data_manager;
};
#pragma once
#include "DataManager.h"
#include "ImageButton.h"
#include <afxdialogex.h>

class COptionsDlg : public CDialog
{
    DECLARE_DYNAMIC(COptionsDlg)

public:
    //COptionsDlg(CDataManager* data_manager, CWnd* pParent = nullptr);
    COptionsDlg(CWnd* pParent = nullptr, int categoryId = 1);   // 标准构造函数
    virtual ~COptionsDlg();
    void setCategoryId(int categoryId);

    SettingData m_data;

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_OPTIONS_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL OnInitDialog();

    afx_msg void OnAddNote();
    afx_msg void OnDeleteNote();
    afx_msg void OnLbnDblclkNotesList();
    afx_msg void OnLvnItemActivateNotesList(NMHDR* pNMHDR, LRESULT* pResult);
    void UpdateNotesList();
    afx_msg void OnBnClickedReturnMain();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnBnClickedBatchPrint(); // 新增
    void PrintNote(const CString& noteText); // 单条打印

    DECLARE_MESSAGE_MAP()

private:
	std::vector<NoteData> notes;
    CListCtrl m_notesList; // 声明与 IDC_NOTES_LIST 关联的成员变量
    int categoryId;
	CString categoryName;
    CBitmap m_bmpBackground;
    CBrush m_brush;
	CImageButton m_btnAddNote;
	CImageButton m_btnDeleteNote;
	CImageButton m_btnReturnMain;
    CImageButton m_btnBatchPrint;
};
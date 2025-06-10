// QuickNoteDlg.h
#pragma once
#include "SQLiteHelper.h"
#include "DataManager.h"
#include "EditNoteDlg.h"
#include <afxdialogex.h>
#include <vector>

class CQuickNoteDlg : public CDialog
{
    DECLARE_DYNAMIC(CQuickNoteDlg)
public:
    CQuickNoteDlg(CWnd* pParent = nullptr);
    ~CQuickNoteDlg();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_INI_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    virtual void OnOK();

protected:
    //HICON m_hIcon;

    // Controls
    CEdit m_editSearch;             // ���������
    CListBox m_listSearchResult;    // ʵʱ������������б�
    CListCtrl m_categoryList;       // �����������ʾ������+��Ŀ����

    // Event handlers
    afx_msg void OnBnClickedBtnAddCategory();
    afx_msg void OnBnClickedBtnSearch();
    afx_msg void OnEnChangeEditSearch();
    afx_msg void OnLvnItemchangedCategoryList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnLbnSelchangeListSearchResult();
    void UpdateCategoriesList();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnLvnEndlabeleditListctrl(NMHDR* pNMHDR, LRESULT* pResult);
    DECLARE_MESSAGE_MAP()

private:
    void LoadCategories();
    void PerformSearch();
    void UpdateSearchResults(const std::vector<NoteData>& notes);
    void ClearSearchResults();
    void InitializeLayout();       // ��ʼ������
    void ShowSearchResults(bool show); // ��ʾ/�����������
    void AdjustListBoxSize(int itemCount); // ����ListBox��С
    int CalculateOptimalListHeight(int itemCount); // ��������б�߶�

    // ���������
    UINT_PTR m_searchTimerId;
    static const UINT_PTR SEARCH_TIMER_ID = 1001;
    static const UINT SEARCH_DELAY = 300; // 300ms�ӳ�
    //��̬�������
    CRect m_originalListRect;      // ԭʼListBoxλ��
    CRect m_originalDialogRect;    // ԭʼ�Ի����С
    int m_maxVisibleItems;         // ���ɼ�����
    int m_itemHeight;              // ÿ��ĸ߶�
    bool m_isListBoxVisible;       // ListBox�Ƿ�ɼ�
    std::vector<NoteData> searchResults;
public:
    afx_msg void OnBnClickedBtnOpen();
    afx_msg void OnBnClickedBtnDelete();
};

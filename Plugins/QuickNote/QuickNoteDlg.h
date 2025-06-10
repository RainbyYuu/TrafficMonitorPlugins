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
    CEdit m_editSearch;             // 搜索输入框
    CListBox m_listSearchResult;    // 实时搜索结果下拉列表
    CListCtrl m_categoryList;       // 分类管理表格（显示分类名+项目数）

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
    void InitializeLayout();       // 初始化布局
    void ShowSearchResults(bool show); // 显示/隐藏搜索结果
    void AdjustListBoxSize(int itemCount); // 调整ListBox大小
    int CalculateOptimalListHeight(int itemCount); // 计算最佳列表高度

    // 防抖动相关
    UINT_PTR m_searchTimerId;
    static const UINT_PTR SEARCH_TIMER_ID = 1001;
    static const UINT SEARCH_DELAY = 300; // 300ms延迟
    //动态布局相关
    CRect m_originalListRect;      // 原始ListBox位置
    CRect m_originalDialogRect;    // 原始对话框大小
    int m_maxVisibleItems;         // 最大可见项数
    int m_itemHeight;              // 每项的高度
    bool m_isListBoxVisible;       // ListBox是否可见
    std::vector<NoteData> searchResults;
public:
    afx_msg void OnBnClickedBtnOpen();
    afx_msg void OnBnClickedBtnDelete();
};

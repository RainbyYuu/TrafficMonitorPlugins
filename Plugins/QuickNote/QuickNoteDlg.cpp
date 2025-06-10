#include "pch.h"
#include "QuickNoteDlg.h"
#include "AddNoteDlg.h"
#include "OptionsDlg.h"
#include <sstream>

IMPLEMENT_DYNAMIC(CQuickNoteDlg, CDialog)

CQuickNoteDlg::CQuickNoteDlg(CWnd* pParent)
	: CDialog(IDD_INI_DIALOG, pParent), m_searchTimerId(0)
{
}

CQuickNoteDlg::~CQuickNoteDlg()
{
	if (m_searchTimerId != 0) {
		KillTimer(m_searchTimerId);
	}
}

void CQuickNoteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CATEGORY_LIST, m_categoryList);
	DDX_Control(pDX, IDC_EDIT_SEARCH, m_editSearch);
	DDX_Control(pDX, IDC_LIST_SEARCH_RESULT, m_listSearchResult);
}

BEGIN_MESSAGE_MAP(CQuickNoteDlg, CDialog)
	//ON_BN_CLICKED(IDC_BTN_ADD_CATEGORY, &CQuickNoteDlg::OnBnClickedBtnAddCategory)
	//ON_BN_CLICKED(IDC_BTN_SEARCH, &CQuickNoteDlg::OnBnClickedBtnSearch)
	ON_EN_CHANGE(IDC_EDIT_SEARCH, &CQuickNoteDlg::OnEnChangeEditSearch)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CATEGORY_LIST, &CQuickNoteDlg::OnLvnItemchangedCategoryList)
	ON_LBN_SELCHANGE(IDC_LIST_SEARCH_RESULT, &CQuickNoteDlg::OnLbnSelchangeListSearchResult)
	ON_WM_TIMER()
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_CATEGORY_LIST, &CQuickNoteDlg::OnLvnEndlabeleditListctrl)
	ON_BN_CLICKED(IDC_BTN_OPEN, &CQuickNoteDlg::OnBnClickedBtnOpen)
	ON_BN_CLICKED(IDC_BTN_DELETE, &CQuickNoteDlg::OnBnClickedBtnDelete)
END_MESSAGE_MAP()


void CQuickNoteDlg::UpdateCategoriesList()
{
	m_categoryList.DeleteAllItems();

	int index = 0;
	for (const auto& category : g_data.m_setting_data.category)
	{
		CString name = category.second.c_str(); // 分类名称
		CString count;
		count.Format(L"%d", g_data.searchByCategory(category.first)); // 笔记数

		int item = m_categoryList.InsertItem(index, name); // 插入第0列
		m_categoryList.SetItemText(item, 1, count);        // 设置第1列（笔记数）
		index++;
	}
}

void CQuickNoteDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == SEARCH_TIMER_ID) {
		KillTimer(m_searchTimerId);
		m_searchTimerId = 0;

		// 执行搜索
		PerformSearch();
	}

	CDialog::OnTimer(nIDEvent);
}

void CQuickNoteDlg::LoadCategories()
{
	auto categories = g_data.m_setting_data.category;
}

void CQuickNoteDlg::PerformSearch()
{
	CString searchText;
	m_editSearch.GetWindowText(searchText);

	if (searchText.IsEmpty()) {
		ClearSearchResults();
		return;
	}

	// 执行搜索
	searchResults = g_data.SearchNotes(searchText.GetString());

	// 更新搜索结果显示
	UpdateSearchResults(searchResults);
}

void CQuickNoteDlg::UpdateSearchResults(const std::vector<NoteData>& notes)
{
	m_listSearchResult.ResetContent();

	if (notes.empty()) {
		// 没有搜索结果，隐藏ListBox
		ShowSearchResults(false);
		return;
	}

	// 添加搜索结果
	for (const auto& note : notes) {
		std::wstring displayText = note.summary;
		if (displayText.length() > 50) {
			displayText = displayText.substr(0, 50) + L"...";
		}
		displayText += L" (" + note.updateTime + L")";

		int index = m_listSearchResult.AddString(displayText.c_str());
		m_listSearchResult.SetItemData(index, note.id);
	}

	// 调整ListBox大小
	AdjustListBoxSize(static_cast<int>(notes.size()));
}

void CQuickNoteDlg::ClearSearchResults()
{
	m_listSearchResult.ResetContent();
	ShowSearchResults(false); // 隐藏ListBox
}

void CQuickNoteDlg::InitializeLayout()
{
	// 保存原始对话框大小
	GetWindowRect(&m_originalDialogRect);

	// 保存原始ListBox位置和大小
	m_listSearchResult.GetWindowRect(&m_originalListRect);
	ScreenToClient(&m_originalListRect);

	// 获取ListBox的项高度
	m_itemHeight = m_listSearchResult.GetItemHeight(0);
	if (m_itemHeight <= 0) {
		m_itemHeight = 16; // 默认高度
	}

	// 设置最大可见项数（可根据需要调整）
	m_maxVisibleItems = 8;

	// 初始状态：隐藏ListBox
	m_listSearchResult.ShowWindow(SW_HIDE);
}

void CQuickNoteDlg::ShowSearchResults(bool show)
{
	if (m_isListBoxVisible == show) {
		return; // 状态没有改变，不需要调整
	}

	m_isListBoxVisible = show;

	if (show) {
		// 显示ListBox
		m_listSearchResult.ShowWindow(SW_SHOW);
	}
	else {
		// 隐藏ListBox并调整对话框大小
		m_listSearchResult.ShowWindow(SW_HIDE);
	}
}

void CQuickNoteDlg::OnLvnEndlabeleditListctrl(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	if (pDispInfo->item.pszText != nullptr)
	{
		CString newText = pDispInfo->item.pszText;

		// 更新项文本
		m_categoryList.SetItemText(pDispInfo->item.iItem, 0, newText);

		g_data.AddCategory(newText.GetString());

		int index = m_categoryList.InsertItem(m_categoryList.GetItemCount(), _T("双击添加分类"));
		m_categoryList.EditLabel(index); // 立即进入编辑状态
		LoadCategories();
	}

	*pResult = 0; // 1 表示接受用户修改，0 表示取消
}


void CQuickNoteDlg::AdjustListBoxSize(int itemCount)
{
	if (itemCount <= 0) {
		ShowSearchResults(false);
		return;
	}

	// 显示ListBox
	ShowSearchResults(true);

	// 计算最佳高度
	int optimalHeight = CalculateOptimalListHeight(itemCount);

	// 调整ListBox大小
	CRect newListRect = m_originalListRect;
	newListRect.bottom = newListRect.top + optimalHeight;

	m_listSearchResult.MoveWindow(&newListRect);
}

int CQuickNoteDlg::CalculateOptimalListHeight(int itemCount)
{
	// 限制显示的项数
	int visibleItems = min(itemCount, m_maxVisibleItems);

	// 计算高度：项数 * 项高度 + 边框
	int height = visibleItems * m_itemHeight + 4; // +4 为边框高度

	// 最小高度
	const int minHeight = m_itemHeight + 4;
	height = max(height, minHeight);

	return height;
}

BOOL CQuickNoteDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_categoryList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_categoryList.InsertColumn(0, L"分类", LVCFMT_LEFT, 150);
	m_categoryList.InsertColumn(1, L"笔记数", LVCFMT_LEFT, 150);
	m_categoryList.ModifyStyle(0, LVS_REPORT | LVS_EDITLABELS);


	// 初始化对话框
	UpdateCategoriesList();

	int index = m_categoryList.InsertItem(m_categoryList.GetItemCount(), _T("双击添加分类"));
	m_categoryList.EditLabel(index); // 立即进入编辑状态

	InitializeLayout();

	return TRUE;
}

void CQuickNoteDlg::OnOK()
{
	CDialog::OnOK();
}

void CQuickNoteDlg::OnEnChangeEditSearch()
{
	// 如果已有定时器在运行，先取消它
	if (m_searchTimerId != 0) {
		KillTimer(m_searchTimerId);
		m_searchTimerId = 0;
	}

	// 获取搜索文本
	CString searchText;
	m_editSearch.GetWindowText(searchText);

	// 如果搜索框为空，清空搜索结果
	if (searchText.IsEmpty()) {
		ClearSearchResults();
		return;
	}

	// 设置延迟搜索定时器（防抖动）
	m_searchTimerId = SetTimer(SEARCH_TIMER_ID, SEARCH_DELAY, nullptr);
	if (m_searchTimerId != 0) {
		KillTimer(m_searchTimerId);
		m_searchTimerId = 0;
	}

	m_editSearch.GetWindowText(searchText);

	if (searchText.IsEmpty()) {
		ClearSearchResults();
		return;
	}

	m_searchTimerId = SetTimer(SEARCH_TIMER_ID, SEARCH_DELAY, nullptr);
}

void CQuickNoteDlg::OnLvnItemchangedCategoryList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void CQuickNoteDlg::OnLbnSelchangeListSearchResult()
{
	int selectedIndex = m_listSearchResult.GetCurSel();
	if (selectedIndex != LB_ERR) {
		// 获取选中笔记的ID
		int noteId = searchResults[selectedIndex].id;
		CEditNoteDlg dlg(this);

		dlg.m_strNote = searchResults[selectedIndex].text.c_str(); // 填入原始笔记内容

		vector<std::pair<int, std::wstring>>ca = g_data.m_setting_data.category;
		int cid = searchResults[selectedIndex].categoryId;
		for (int i = 0; i < ca.size(); i++)
		{
			if (ca[i].first == cid)
			{
				dlg.m_strCategory = ca[i].second.c_str();
				break;
			}
		}

		if (dlg.DoModal() == IDOK)
		{
			// 用户点击了确定，更新数据库
			g_data.UpdateNoteTextById(noteId, dlg.m_strNote.GetString(), searchResults[selectedIndex].categoryId);
		}
	}
}

void CQuickNoteDlg::OnBnClickedBtnOpen()
{
	int selectedIndex = m_categoryList.GetNextItem(-1, LVNI_SELECTED);
	if (selectedIndex == -1)
	{
		AfxMessageBox(_T("请先选择一个分类"));
		return;
	}

	// 获取分类ID（假设保存在ItemData中）
	int categoryId = g_data.m_setting_data.category[selectedIndex].first;

	// 打开新对话框并传递 categoryId
	COptionsDlg dlg(this, categoryId);
	EndDialog(IDOK);
	dlg.DoModal();
}

void CQuickNoteDlg::OnBnClickedBtnDelete()
{
	int selectedIndex = m_categoryList.GetNextItem(-1, LVNI_SELECTED);
	if (selectedIndex == -1)
	{
		AfxMessageBox(_T("请先选择一个分类"));
		return;
	}
	else if (selectedIndex >= g_data.m_setting_data.category.size())
	{
		AfxMessageBox(_T("选择分类无效"));
		return;
	}
	else
	{
		int result = AfxMessageBox(_T("确定删除该分类及其所属笔记吗？"), MB_YESNO | MB_ICONQUESTION);

		if (result == IDYES)
		{
			int categoryId = g_data.m_setting_data.category[selectedIndex].first;
			g_data.DeleteByCategoryId(categoryId);
			auto& categories = g_data.m_setting_data.category; // 假设category是一个std::vector<std::pair<int, std::wstring>>
			for (auto it = categories.begin(); it != categories.end(); ++it)
			{
				if (categoryId == it->first) // 检查是否找到了匹配的categoryId
				{
					categories.erase(it); // 使用迭代器来删除元素
					break; // 找到并删除后退出循环
				}
			}
			UpdateCategoriesList();
		}
		else return;
	}
	
	int index = m_categoryList.InsertItem(m_categoryList.GetItemCount(), _T("双击添加分类"));
	m_categoryList.EditLabel(index); // 立即进入编辑状态

}

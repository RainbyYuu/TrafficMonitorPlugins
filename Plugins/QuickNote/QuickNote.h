#pragma once
#include "PluginInterface.h"
#include "QuickNoteItem.h"
#include "DataManager.h"
#include <string>
#include <memory>

class CQuickNote : public ITMPlugin
{
private:
    CQuickNote();
    ~CQuickNote();

public:
    static CQuickNote& Instance();

    virtual IPluginItem* GetItem(int index) override;
    virtual const wchar_t* GetTooltipInfo() override;
    virtual void DataRequired() override;
    virtual OptionReturn ShowOptionsDialog(void* hParent) override;
    virtual const wchar_t* GetInfo(PluginInfoIndex index) override;
    virtual void OnExtenedInfo(ExtendedInfoIndex index, const wchar_t* data) override;
    virtual void* GetPluginIcon() override;
    virtual int GetCommandCount() override;
    virtual const wchar_t* GetCommandName(int command_index) override;
    virtual void* GetCommandIcon(int command_index) override;
    virtual void OnPluginCommand(int command_index, void* hWnd, void* para) override;
    //int IsCommandChecked(int command_index) override;

private:
    static CQuickNote m_instance;
    CQuickNoteItem m_item;
    std::wstring m_tooltip_info;
    //std::unique_ptr<CDataManager> m_data_manager;
    enum CommandIndex
    {
        CMD_SAVE,
        CMD_CLEAR,
    };
};

#ifdef __cplusplus
extern "C" {
#endif
    __declspec(dllexport) ITMPlugin* TMPluginGetInstance();
#ifdef __cplusplus
}
#endif
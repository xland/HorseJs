#include <pch.h>
#include <winrt/Windows.UI.Notifications.h>
#include <winrt/Windows.Data.Xml.Dom.h>
#include "Notify.h"


using namespace winrt;
using namespace winrt::Windows::UI;
using namespace winrt::Windows::Data::Xml::Dom;
namespace {
    std::unique_ptr<Notify> notify;
    static std::unordered_map<std::string, void (Notify::*)(const rapidjson::Value&, JsonResult*)> funcs{
    {"show", &Notify::show},
    };
}

Notify::Notify()
{
}

Notify::~Notify()
{
}

Notify* Notify::get()
{
    if(!notify) {
        notify = std::make_unique<Notify>();
	}
    return notify.get();
}
bool Notify::excute(std::string& methodName, const rapidjson::Value& param, JsonResult* result)
{
    auto it = funcs.find(methodName);
    if (it == funcs.end()) return false;
    (Notify::get()->*it->second)(param, result);
    return true;
}
void Notify::show(const rapidjson::Value& params, JsonResult* result)
{
    //todo： 要在这个注册表下写数据：HKEY_CURRENT_USER\Software\Classes\AppUserModelId\YourAppName

    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring appName = Util::convertToWStr(arr[0].GetString());
    std::wstring title = Util::convertToWStr(arr[1].GetString());
    std::wstring content = Util::convertToWStr(arr[2].GetString());

    Notifications::ToastNotifier notifier = Notifications::ToastNotificationManager::CreateToastNotifier(appName.data());
    auto xmlString = std::format(L"<toast><visual><binding template='ToastText02'><text id='1'>{}</text><text id='2'>{}</text></binding></visual></toast>", 
        title, content);
    hstring xmlHString(xmlString);
    XmlDocument xmlDoc;
    xmlDoc.LoadXml(xmlString);
    Notifications::ToastNotification toast(xmlDoc);
    notifier.Show(toast);
}
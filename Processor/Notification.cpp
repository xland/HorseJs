#include <pch.h>
#include <winrt/Windows.UI.Notifications.h>
#include <winrt/Windows.Data.Xml.Dom.h>
#include "Notification.h"


using namespace winrt;
using namespace winrt::Windows::UI;
using namespace winrt::Windows::Data::Xml::Dom;
namespace {
    std::unique_ptr<Notification> notification;
}

Notification::Notification()
{
}

Notification::~Notification()
{
}

Notification* Notification::get()
{
    if(!notification) {
        notification = std::make_unique<Notification>();
	}
    return notification.get();
}
void Notification::show(const rapidjson::Value& params, JsonResult* result) 
{
    Notifications::ToastNotifier notifier = Notifications::ToastNotificationManager::CreateToastNotifier(L"HorseJs");

    // 创建 XML 模板
    hstring xmlString = L"<toast>"
        L"<visual>"
        L"<binding template='ToastText02'>"
        L"<text id='1'>测试标题</text>"
        L"<text id='2'>测试内容</text>"
        L"</binding>"
        L"</visual>"
        L"</toast>";

    // 加载 XML
    XmlDocument xmlDoc;
    xmlDoc.LoadXml(xmlString);

    // 创建 Toast 通知
    Notifications::ToastNotification toast(xmlDoc);
    notifier.Show(toast);
}
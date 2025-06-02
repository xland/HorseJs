#include <pch.h>
#include "Os.h"

namespace {
    std::unique_ptr<Os> os;
}

Os::Os()
{
}

Os::~Os()
{
}

Os* Os::get()
{
    if(!os) {
        os = std::make_unique<Os>();
	}
    return os.get();
}

// 保存数据到用户凭据区
// https://zhuanlan.zhihu.com/p/679219628?share_code=NzSd6ri8efPP&utm_psn=1912465887958639333

// 获取当前用户的皮肤颜色 Theme

//阻止锁屏

//插入电源，使用电池

//getSystemIdleTime

//遍历窗口句柄
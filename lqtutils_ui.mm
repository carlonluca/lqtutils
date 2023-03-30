#include "lqtutils_ui.h"

#include <UIKit/UIKit.h>

namespace lqt {

ScreenLock::ScreenLock() :
    m_isValid(false)
{
}

ScreenLock::~ScreenLock()
{
}

double QmlUtils::safeAreaBottomInset()
{
    if (@available(iOS 11.0, *)) {
        UIWindow *window = UIApplication.sharedApplication.windows.firstObject;
        return window.safeAreaInsets.bottom;
    }

    return 0;
}

double QmlUtils::safeAreaTopInset()
{
    if (@available(iOS 11.0, *)) {
        UIWindow *window = UIApplication.sharedApplication.windows.firstObject;
        return window.safeAreaInsets.top;
    }

    return 0;
}

double QmlUtils::safeAreaLeftInset()
{
    if (@available(iOS 11.0, *)) {
        UIWindow *window = UIApplication.sharedApplication.windows.firstObject;
        return window.safeAreaInsets.left;
    }

    return 0;
}

double QmlUtils::safeAreaRightInset()
{
    if (@available(iOS 11.0, *)) {
        UIWindow *window = UIApplication.sharedApplication.windows.firstObject;
        return window.safeAreaInsets.right;
    }

    return 0;
}

}

#include "lqtutils_ui.h"

#include <UIKit/UIKit.h>

namespace lqt {

double LQTQmlUtils::safeAreaBottomInset()
{
    if (@available(iOS 11.0, *)) {
        UIWindow *window = UIApplication.sharedApplication.windows.firstObject;
        return window.safeAreaInsets.bottom;
    }

    return 0;
}

double LQTQmlUtils::safeAreaTopInset()
{
    if (@available(iOS 11.0, *)) {
        UIWindow *window = UIApplication.sharedApplication.windows.firstObject;
        return window.safeAreaInsets.top;
    }

    return 0;
}

}

#include <QUrl>

#include <UIKit/UIKit.h>

#include "lqtutils_ui.h"

namespace lqt {

ScreenLock::ScreenLock() :
    m_isValid(false)
{
    [[UIApplication sharedApplication] setIdleTimerDisabled: YES];
}

ScreenLock::~ScreenLock()
{
    [[UIApplication sharedApplication] setIdleTimerDisabled: NO];
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

bool QmlUtils::shareResource(const QUrl& resUrl, const QString& /* mimeType */, const QString& /* authority */)
{
    NSString* nsFilePath = resUrl.toLocalFile().toNSString();
    NSURL* url = [NSURL fileURLWithPath:nsFilePath];
    if (!url) {
        qWarning() << "Invalid file name:" << resUrl.toLocalFile();
        return false;
    }

    NSArray* dataToShare = @[url];

    UIActivityViewController* activityViewController = [[UIActivityViewController alloc] initWithActivityItems:dataToShare applicationActivities:nil];
    [activityViewController autorelease];

    UIView* view = [[[[UIApplication sharedApplication] keyWindow] rootViewController] view];
    activityViewController.popoverPresentationController.sourceView = view;

    [[[[UIApplication sharedApplication] keyWindow] rootViewController] presentViewController:activityViewController animated:YES completion:nil];

    return true;
}

QRectF QmlUtils::visibleDisplayFrame()
{
    qWarning() << "Not implemented";
    return QRectF();
}

}

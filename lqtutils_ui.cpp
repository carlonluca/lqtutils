/**
 * MIT License
 *
 * Copyright (c) 2021 Luca Carlon
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **/

#include <QTimer>
#include <QQuickWindow>
#include <QMutableListIterator>
#include <QGuiApplication>

#ifdef Q_OS_ANDROID
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QJniObject>
#else
#include <QtAndroidExtras/QtAndroid>
#include <QtAndroidExtras/QAndroidJniObject>
typedef QAndroidJniObject QJniObject;
#endif
#include <jni.h>
#endif

#include "lqtutils_ui.h"

namespace lqt {

FrameRateMonitor::FrameRateMonitor(QQuickWindow* w, QObject* parent) :
    FreqMeter(parent)
{
    setWindow(w);
}

void FrameRateMonitor::setWindow(QQuickWindow* w)
{
    if (!w)
        return;
    connect(w, &QQuickWindow::frameSwapped,
            this, &FrameRateMonitor::registerSample);
}

void QmlUtils::singleShot(int msec, QJSValue callback)
{
    QTimer::singleShot(msec, this, [callback] () mutable
    {
        if (callback.isCallable())
            callback.call();
    });
}

#ifndef Q_OS_IOS
#ifdef Q_OS_ANDROID
inline QJniObject get_cutout()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QJniObject activity = QNativeInterface::QAndroidApplication::context();
#else
    QJniObject activity = QtAndroid::androidActivity();
#endif
    if (!activity.isValid()) {
        qWarning() << "Could not get android context";
        return QJniObject();
    }

    QJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");
    if (!window.isValid()) {
        qWarning() << "Cannot get window";
        return QJniObject();
    }

    QJniObject decoView = window.callObjectMethod("getDecorView", "()Landroid/view/View;");
    if (!decoView.isValid()) {
        qWarning() << "Cannot get decorator view";
        return QJniObject();
    }

    QJniObject insets = decoView.callObjectMethod("getRootWindowInsets", "()Landroid/view/WindowInsets;");
    if (!insets.isValid()) {
        qWarning() << "Cannot get root window insets";
        return QJniObject();
    }

    return insets.callObjectMethod("getDisplayCutout", "()Landroid/view/DisplayCutout;");
}
#endif

double QmlUtils::safeAreaTopInset()
{
#ifdef Q_OS_ANDROID
    QJniObject cutout = get_cutout();
    if (!cutout.isValid())
        return 0;

    return cutout.callMethod<int>("getSafeInsetTop", "()I")/qApp->devicePixelRatio();
#else
    return 0;
#endif
}

double QmlUtils::safeAreaBottomInset()
{
#ifdef Q_OS_ANDROID
    QJniObject cutout = get_cutout();
    if (!cutout.isValid())
        return 0;

    return cutout.callMethod<int>("getSafeInsetBottom", "()I")/qApp->devicePixelRatio();
#else
    return 0;
#endif
}
#endif

} // namespace

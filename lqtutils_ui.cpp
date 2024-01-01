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

#include "qbuffer.h"
#include <QTimer>
#include <QQuickWindow>
#include <QMutableListIterator>
#include <QGuiApplication>
#include <QDebug>

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

#if !defined(QT_NO_DBUS) && defined(Q_OS_LINUX)
#include <QDBusInterface>
#include <QDBusReply>
#endif

#include "lqtutils_ui.h"
#include "lqtutils_qsl.h"

namespace lqt {

#ifndef Q_OS_IOS
ScreenLock::ScreenLock() :
    m_isValid(false)
{
#ifdef Q_OS_ANDROID
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QNativeInterface::QAndroidApplication::runOnAndroidMainThread([&] {
        lockScreen(true);
    }).waitForFinished();
#else
    QtAndroid::runOnAndroidThreadSync([&] {
        lockScreen(true);
    });
#endif
#endif
}

ScreenLock::~ScreenLock()
{
#ifdef Q_OS_ANDROID
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QNativeInterface::QAndroidApplication::runOnAndroidMainThread([&] {
        lockScreen(false);
    }).waitForFinished();
#else
    QtAndroid::runOnAndroidThreadSync([&] {
        lockScreen(false);
    });
#endif
#endif
}

void ScreenLock::lockScreen(bool lock)
{
#ifdef Q_OS_ANDROID
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QJniObject activity = QNativeInterface::QAndroidApplication::context();
#else
    QJniObject activity = QtAndroid::androidActivity();
#endif
    if (!activity.isValid())
        return;

    QJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");
    if (!window.isValid())
        return;

    const int FLAG_KEEP_SCREEN_ON = 128;
    window.callMethod<void>(lock ? "addFlags" : "clearFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
#else
    Q_UNUSED(lock);
#endif
}
#endif // Q_IOS_OS

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

QMetaObject::Connection enableAutoFrameUpdates(QQuickWindow& w)
{
    return QObject::connect(&w, &QQuickWindow::frameSwapped,
                            &w, &QQuickWindow::requestUpdate);
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

double QmlUtils::safeAreaRightInset()
{
#ifdef Q_OS_ANDROID
    QJniObject cutout = get_cutout();
    if (!cutout.isValid())
        return 0;

    return cutout.callMethod<int>("getSafeInsetRight", "()I")/qApp->devicePixelRatio();
#else
    return 0;
#endif
}

double QmlUtils::safeAreaLeftInset()
{
#ifdef Q_OS_ANDROID
    QJniObject cutout = get_cutout();
    if (!cutout.isValid())
        return 0;

    return cutout.callMethod<int>("getSafeInsetLeft", "()I")/qApp->devicePixelRatio();
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

void SystemNotification::send()
{
#if !defined(QT_NO_DBUS) && defined(Q_OS_LINUX)
    QDBusInterface interface(QSL("org.freedesktop.Notifications"),
                             QSL("/org/freedesktop/Notifications"),
                             QSL("org.freedesktop.Notifications"));

    const QVariantList args {
        m_appName,
        m_replacesId,
        m_icon,
        m_title,
        m_message,
        m_actions,
        m_hints,
        m_timeout
    };
    QDBusReply<uint> reply = interface.callWithArgumentList(QDBus::AutoDetect, QSL("Notify"), args);

    if (reply.isValid())
        qWarning() << "Failed to send notification:" << reply.error().message();
#elif defined(Q_OS_ANDROID)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QJniObject activity = QNativeInterface::QAndroidApplication::context();
#else
    QJniObject activity = QtAndroid::androidActivity();
#endif

#define J_CLASS_NOT         "Landroid/app/Notification;"
#define J_CLASS_NOT_BUILDER "Landroid/app/Notification$Builder;"
#define J_CLASS_BITMAP      "Landroid/graphics/Bitmap;"
#define J_CLASS_STRING      "Ljava/lang/String;"

    const QJniObject channelId = QJniObject::fromString(qApp->applicationName());
    const QJniObject channelName = QJniObject::fromString(QString("%1 notifier").arg(qApp->applicationName()));

    QJniObject notificationManager = activity.callObjectMethod("getSystemService",
                                                               "(" J_CLASS_STRING ")Ljava/lang/Object;",
                                                               QJniObject::fromString("notification").object());

    QJniObject builder;
    if (QJniObject::getStaticField<jint>("android/os/Build$VERSION", "SDK_INT") >= 26) {
        const jint importance = QJniObject::getStaticField<jint>("android/app/NotificationManager", "IMPORTANCE_DEFAULT");
        QJniObject notificationChannel("android/app/NotificationChannel",
                                       "(" J_CLASS_STRING "Ljava/lang/CharSequence;I)V",
                                       channelId.object(), channelName.object(), importance);
        notificationManager.callMethod<void>("createNotificationChannel",
                                             "(Landroid/app/NotificationChannel;)V", notificationChannel.object());
        builder = QJniObject("android/app/Notification$Builder",
                             "(Landroid/content/Context;" J_CLASS_STRING ")V", activity.object(), channelId.object());
    } else {
        builder = QJniObject("android/app/Notification$Builder",
                             "(Landroid/content/Context;)V", activity.object());
    }

    QBuffer iconBuffer;
    iconBuffer.open(QIODevice::WriteOnly);
    if (!m_icon.save(&iconBuffer, "png")) {
        qWarning() << "Failed to encode icon to png";
        return;
    }

    const QByteArray iconData = iconBuffer.buffer();

    QJniEnvironment env;
    QJniObject byteArrayObj = QJniObject::fromLocalRef(env->NewByteArray(iconData.size()));
    env->SetByteArrayRegion(byteArrayObj.object<jbyteArray>(),
                            0,
                            iconData.size(),
                            reinterpret_cast<const jbyte*>(iconData.constData()));

    QJniObject bitmapFactory = QJniObject::callStaticObjectMethod("android/graphics/BitmapFactory", "decodeByteArray",
                                                                  "([BII)" J_CLASS_BITMAP,
                                                                  byteArrayObj.object<jbyteArray>(), 0, iconData.size());
    QJniObject icon = QJniObject::callStaticObjectMethod("android/graphics/drawable/Icon", "createWithBitmap",
                                                         "(" J_CLASS_BITMAP ")Landroid/graphics/drawable/Icon;",
                                                         bitmapFactory.object());
    builder.callObjectMethod("setSmallIcon",
                             "(Landroid/graphics/drawable/Icon;)" J_CLASS_NOT_BUILDER,
                             icon.object());
    builder.callObjectMethod("setContentTitle",
                             "(Ljava/lang/CharSequence;)" J_CLASS_NOT_BUILDER,
                             QJniObject::fromString(m_title).object());
    builder.callObjectMethod("setContentText",
                             "(Ljava/lang/CharSequence;)" J_CLASS_NOT_BUILDER,
                             QJniObject::fromString(m_message).object());
    builder.callObjectMethod("setAutoCancel", "(Z)" J_CLASS_NOT_BUILDER, false);

    QJniObject notification = builder.callObjectMethod("build", "()" J_CLASS_NOT);

    const jint notificationId = 0;
    notificationManager.callMethod<void>("notify", "(I" J_CLASS_NOT ")V", notificationId, notification.object());
#endif
}

} // namespace

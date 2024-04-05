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

#include <QBuffer>
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
#include <QtCore/private/qandroidextras_p.h>
#endif

#if !defined(QT_NO_DBUS) && defined(Q_OS_LINUX)
#include <QDBusInterface>
#include <QDBusReply>
#endif

#if defined(Q_OS_WINDOWS)
#include <Windows.h>
#include <shellapi.h>
#endif

#include <functional>

#include "lqtutils_ui.h"
#include "lqtutils_qsl.h"
#include "lqtutils_misc.h"

namespace lqt {

#ifdef Q_OS_ANDROID
inline QJniObject get_activity()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return QNativeInterface::QAndroidApplication::context();
#else
    return QtAndroid::androidActivity();
#endif
}

inline QJniObject get_window()
{
    const QJniObject activity = get_activity();
    if (!activity.isValid())
        return QJniObject();

    QJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");
    if (!window.isValid()) {
        qWarning() << "Cannot get window";
        return QJniObject();
    }

    return window;
}

inline void run_activity_thread(std::function<void()> f)
{
#ifdef Q_OS_ANDROID
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QNativeInterface::QAndroidApplication::runOnAndroidMainThread(f).waitForFinished();
#else
    QtAndroid::runOnAndroidThreadSync(f);
#endif
#endif
}
#endif

#ifndef Q_OS_IOS
ScreenLock::ScreenLock() :
    m_isValid(false)
{
#ifdef Q_OS_ANDROID
    run_activity_thread([&] {
        lockScreen(true);
    });
#endif
}

ScreenLock::~ScreenLock()
{
#ifdef Q_OS_ANDROID
    run_activity_thread([&] {
        lockScreen(false);
    });
#endif
}

void ScreenLock::lockScreen(bool lock)
{
#ifdef Q_OS_ANDROID
    QJniObject window = get_window();
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
inline QJniObject get_decor_view()
{
    QJniObject activity = get_activity();
    if (!activity.isValid()) {
        qWarning() << "Could not get android context";
        return QJniObject();
    }

    QJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");
    if (!window.isValid()) {
        qWarning() << "Cannot get window";
        return QJniObject();
    }

    return window.callObjectMethod("getDecorView", "()Landroid/view/View;");
}

inline QJniObject get_cutout()
{
    QJniObject decoView = get_decor_view();
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

QRectF QmlUtils::visibleDisplayFrame()
{
#ifdef Q_OS_ANDROID
    QJniObject decoView = get_decor_view();
    if (!decoView.isValid()) {
        qWarning() << "Could not get decor view";
        return QRectF();
    }

    QJniObject rectangle("android/graphics/Rect");
    decoView.callMethod<void>("getWindowVisibleDisplayFrame", "(Landroid/graphics/Rect;)V", rectangle.object());

    const jint top = rectangle.getField<jint>("top")/qApp->devicePixelRatio();
    const jint bottom = rectangle.getField<jint>("bottom")/qApp->devicePixelRatio();
    const jint left = rectangle.getField<jint>("left")/qApp->devicePixelRatio();
    const jint right = rectangle.getField<jint>("right")/qApp->devicePixelRatio();

    return QRectF(left, top, right - left, bottom - top);
#else
    return QRectF();
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

bool QmlUtils::shareResource(const QUrl& resUrl, const QString& mimeType, const QString& authority)
{
#ifdef Q_OS_ANDROID
    const QJniObject jniPath = QJniObject::fromString(resUrl.toLocalFile());
    if (!jniPath.isValid()) {
        qWarning() << "Could not create jni url";
        return false;
    }

    const QJniObject jniFile("java/io/File", "(Ljava/lang/String;)V", jniPath.object<jstring>());
    if (!jniFile.isValid()) {
        qWarning() << "Could not instantiate file";
        return false;
    }

    const QJniObject jniUri = QJniObject::callStaticObjectMethod("androidx/core/content/FileProvider",
                                                                 "getUriForFile",
                                                                 "(Landroid/content/Context;Ljava/lang/String;Ljava/io/File;)Landroid/net/Uri;",
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
                                                                 QNativeInterface::QAndroidApplication::context().object(),
#else
                                                                 QNativeInterface::QAndroidApplication::context(),
#endif
                                                                 QJniObject::fromString(authority).object(),
                                                                 jniFile.object<jobject>());
    if (!jniUri.isValid()) {
        qWarning() << "Failed to create FilePath uri";
        return false;
    }

    const QJniObject jniParam = QJniObject::getStaticObjectField<jstring>("android/content/Intent", "ACTION_SEND");
    if(!jniParam.isValid()) {
        qWarning() << "Could not get jni intent";
        return false;
    }

    QJniObject jniIntent("android/content/Intent", "(Ljava/lang/String;)V", jniParam.object<jstring>());
    if (!jniIntent.isValid()) {
        qWarning() << "Could not instantiate intent";
        return false;
    }

    QJniObject jniType = QJniObject::fromString(mimeType);
    if (!jniType.isValid()) {
        qWarning() << "Could not create jni type";
        return false;
    }

    QJniObject jniResult = jniIntent.callObjectMethod("setType",
                                                      "(Ljava/lang/String;)Landroid/content/Intent;",
                                                      jniType.object<jstring>());
    if (!jniResult.isValid()) {
        qWarning() << "Cannot set intent type";
        return false;
    }

    const QJniObject jniExtraStream = QJniObject::getStaticObjectField<jstring>("android/content/Intent",
                                                                                "EXTRA_STREAM");
    if (!jniExtraStream.isValid()) {
        qWarning() << "Cannot read extra stream constant";
        return false;
    }

    jniResult = jniIntent.callObjectMethod("putExtra",
                                           "(Ljava/lang/String;Landroid/os/Parcelable;)Landroid/content/Intent;",
                                           jniExtraStream.object<jstring>(),
                                           jniUri.object<jobject>());
    if (!jniResult.isValid()) {
        qWarning() << "Cannot set extra";
        return false;
    }

    jint jniGrantReadUri = QJniObject::getStaticField<jint>("android/content/Intent", "FLAG_GRANT_READ_URI_PERMISSION");
    Q_ASSERT(jniGrantReadUri == 1);

    jint jniGrantWriteUri = QJniObject::getStaticField<jint>("android/content/Intent", "FLAG_GRANT_WRITE_URI_PERMISSION");
    Q_ASSERT(jniGrantWriteUri == 2);

    jniResult = jniIntent.callObjectMethod("addFlags", "(I)Landroid/content/Intent;", jniGrantReadUri);
    if (!jniResult.isValid()) {
        qWarning() << "Could not add flags to intent";
        return false;
    }

    jniResult = jniIntent.callObjectMethod("addFlags", "(I)Landroid/content/Intent;", jniGrantWriteUri);
    if (!jniResult.isValid()) {
        qWarning() << "Could not add flags to intent";
        return false;
    }

    QJniObject activity = QNativeInterface::QAndroidApplication::context();
    if (!activity.isValid()) {
        qWarning() << "Could not get android context";
        return false;
    }

    QJniObject packageManager = activity.callObjectMethod("getPackageManager",
                                                          "()Landroid/content/pm/PackageManager;");
    if (!packageManager.isValid()) {
        qWarning() << "Could not get packageManager object";
        return false;
    }

    QJniObject componentName = jniIntent.callObjectMethod("resolveActivity",
                                                          "(Landroid/content/pm/PackageManager;)Landroid/content/ComponentName;",
                                                          packageManager.object());
    if (!componentName.isValid())
        return false;

    QJniObject jniShareIntent =
        QJniObject::callStaticObjectMethod("android/content/Intent",
                                           "createChooser",
                                           "(Landroid/content/Intent;Ljava/lang/CharSequence;)Landroid/content/Intent;",
                                           jniIntent.object<jobject>(),
                                           QJniObject::fromString("Share file").object<jstring>());
    if (!jniShareIntent.isValid()) {
        qWarning() << "Could not create share intent";
        return false;
    }

    QtAndroidPrivate::startActivity(jniShareIntent, 1, nullptr);
    return true;
#else
    return false;
#endif
}

#endif

bool QmlUtils::isMobile()
{
#ifdef L_OS_MOBILE
    return true;
#else
    return false;
#endif
}

bool QmlUtils::setBarColorLight(bool light, bool fullscreen)
{
#if defined(Q_OS_ANDROID) && QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    run_activity_thread([&] {
        QJniObject decoView = get_decor_view();
        if (!decoView.isValid()) {
            qWarning() << "Could not get decor view";
            return;
        }

        if (fullscreen) {
            const jint layoutStable = QJniObject::getStaticField<jint>("android/view/View", "SYSTEM_UI_FLAG_LAYOUT_STABLE");
            const jint hideNavigation = QJniObject::getStaticField<jint>("android/view/View", "SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION");
            const jint fullscreen = QJniObject::getStaticField<jint>("android/view/View", "SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN");
            const jint uiVisibility = decoView.callMethod<jint>("getSystemUiVisibility", "()I")
                                      | layoutStable
                                      | hideNavigation
                                      | fullscreen;
            decoView.callMethod<void>("setSystemUiVisibility", "(I)V", uiVisibility);
        }

        if (QtAndroidPrivate::androidSdkVersion() < 26) {
            qWarning() << "Not supported for SDKs < 30";
            return;
        }
        else if (QtAndroidPrivate::androidSdkVersion() < 30) {
            const jint lightStatus = QJniObject::getStaticField<jint>("android/view/View", "SYSTEM_UI_FLAG_LIGHT_STATUS_BAR");
            const jint lightNav = QJniObject::getStaticField<jint>("android/view/View", "SYSTEM_UI_FLAG_LIGHT_NAVIGATION_BAR");
            jint uiVisibility = decoView.callMethod<jint>("getSystemUiVisibility", "()I");
            if (light) {
                uiVisibility |= lightStatus;
                uiVisibility |= lightNav;
            }
            else {
                uiVisibility &= ~lightStatus;
                uiVisibility &= ~lightNav;
            }
            decoView.callMethod<void>("setSystemUiVisibility", "(I)V", uiVisibility);
        }
        else {
            QJniObject insetsController = decoView.callObjectMethod("getWindowInsetsController", "()Landroid/view/WindowInsetsController;");
            if (!insetsController.isValid()) {
                qWarning() << "Could not get WindowInsetsController";
                return;
            }

            const jint lightValue = QJniObject::getStaticField<jint>("android/view/WindowInsetsController", "APPEARANCE_LIGHT_STATUS_BARS");
            insetsController.callMethod<void>("setSystemBarsAppearance", "(II)V", light ? lightValue : 0, lightValue);
        }
    });
    return true;
#else
    // Not supported.
    return false;
#endif
}

bool QmlUtils::setNavBarColor(const QColor &color)
{
#if defined(Q_OS_ANDROID) && QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    run_activity_thread([color] {
        if (QtAndroidPrivate::androidSdkVersion() < 21) {
            qWarning() << "Not supported for SDKs < 21";
            return;
        }

        QJniObject window = get_window();
        if (!window.isValid())
            return;

        window.callMethod<void>("setNavigationBarColor", "(I)V", color.rgba());
    });
    return true;
#else
    // Not supported.
    return false;
#endif
}

bool QmlUtils::setStatusBarColor(const QColor &color)
{
#if defined(Q_OS_ANDROID) && QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    run_activity_thread([color] {
        if (QtAndroidPrivate::androidSdkVersion() < 21) {
            qWarning() << "Not supported for SDKs < 21";
            return;
        }

        QJniObject window = get_window();
        if (!window.isValid())
            return;

        window.callMethod<void>("setStatusBarColor", "(I)V", color.rgba());
    });
    return true;
#else
    // Not supported.
    return false;
#endif
}

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
    qWarning() << "Please use AndroidSystemNotification";
#elif defined(Q_OS_WINDOWS)

    const std::wstring wstrTitle = m_title.toStdWString();
    const std::wstring wstrMsg = m_message.toStdWString();

    NOTIFYICONDATA notifyIconData;
    ZeroMemory(&notifyIconData, sizeof(NOTIFYICONDATA));

    notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
    notifyIconData.hWnd = nullptr;
    notifyIconData.uID = m_replacesId;
    notifyIconData.uFlags = NIF_INFO;
    notifyIconData.dwInfoFlags =  m_icon.isNull() ? NIIF_INFO : (NIIF_USER | NIIF_LARGE_ICON);
    notifyIconData.hBalloonIcon = m_icon.isNull() ? LoadIcon(nullptr, IDI_INFORMATION) : m_icon.toHICON();
    notifyIconData.uTimeout = m_timeout;

    lstrcpyn(notifyIconData.szInfoTitle, wstrTitle.c_str(), sizeof(notifyIconData.szInfoTitle)/sizeof(wchar_t));
    lstrcpyn(notifyIconData.szInfo, wstrMsg.c_str(), sizeof(notifyIconData.szInfo)/sizeof(wchar_t));

    Shell_NotifyIcon(NIM_ADD, &notifyIconData);

    DestroyIcon(notifyIconData.hBalloonIcon);

#else
    qWarning() << "Not implemented";
#endif
}

#ifdef Q_OS_ANDROID
void AndroidSystemNotification::send()
{
    const QJniObject activity = get_activity();

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

    QJniEnvironment env;
    QJniObject icon;
    if (!m_icon.isNull()) {
        QBuffer iconBuffer;
        iconBuffer.open(QIODevice::WriteOnly);
        if (!m_icon.save(&iconBuffer, "png")) {
            qWarning() << "Failed to encode icon to png";
            return;
        }

        const QByteArray iconData = iconBuffer.buffer();

        QJniObject byteArrayObj = QJniObject::fromLocalRef(env->NewByteArray(iconData.size()));
        env->SetByteArrayRegion(byteArrayObj.object<jbyteArray>(),
                                0,
                                iconData.size(),
                                reinterpret_cast<const jbyte*>(iconData.constData()));

        QJniObject bitmap = QJniObject::callStaticObjectMethod("android/graphics/BitmapFactory", "decodeByteArray",
                                                               "([BII)" J_CLASS_BITMAP,
                                                               byteArrayObj.object<jbyteArray>(), 0, iconData.size());
        icon = QJniObject::callStaticObjectMethod("android/graphics/drawable/Icon", "createWithBitmap",
                                                  "(" J_CLASS_BITMAP ")Landroid/graphics/drawable/Icon;",
                                                  bitmap.object());
    }

    const jint flags = QJniObject::getStaticField<jint>("android/content/Intent", "FLAG_ACTIVITY_NEW_TASK") |
                       QJniObject::getStaticField<jint>("android/content/Intent", "FLAG_ACTIVITY_CLEAR_TASK");
    const QJniObject openIntent("android/content/Intent",
                                "(Landroid/content/Context;Ljava/lang/Class;)V",
                                activity.object(),
                                env.findClass(m_activityClass.replace('.', '/').toLocal8Bit()));

    QJniObject pendingIntent;
    if (openIntent.isValid()) {
        openIntent.callObjectMethod("setFlags", "(I)Landroid/content/Intent;", flags);
        pendingIntent = QJniObject::callStaticObjectMethod("android/app/PendingIntent",
                                                           "getActivity",
                                                           "(Landroid/content/Context;ILandroid/content/Intent;I)Landroid/app/PendingIntent;",
                                                           activity.object(),
                                                           0,
                                                           openIntent.object(),
                                                           QJniObject::getStaticField<jint>("android/app/PendingIntent", "FLAG_IMMUTABLE"));
    }

    if (icon.isValid())
        builder.callObjectMethod("setSmallIcon",
                                 "(Landroid/graphics/drawable/Icon;)" J_CLASS_NOT_BUILDER,
                                 icon.object());
    if (pendingIntent.isValid() && m_openApp)
        builder.callObjectMethod("setContentIntent",
                                 "(Landroid/app/PendingIntent;)" J_CLASS_NOT_BUILDER,
                                 pendingIntent.object());
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
}
#endif // Q_OS_ANDROID

} // namespace

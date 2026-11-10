/**
 * MIT License
 *
 * Copyright (c) 2022 Luca Carlon
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

#ifndef LQTUTILS_NET
#define LQTUTILS_NET

#include <QObject>
#include <QUrl>
#include <QString>
#include <QNetworkReply>
#include <QThread>
#include <QFile>
#include <QBuffer>

#include "lqtutils_prop.h"
#include "lqtutils_enum.h"

L_DECLARE_ENUM(LQTDownloaderState,
               S_IDLE,
               S_DOWNLOADING,
               S_DONE,
               S_NETWORK_FAILURE,
               S_IO_FAILURE,
               S_ABORTED)

namespace lqt {

class DownloaderPriv : public QObject
{
    Q_OBJECT
    L_RO_PROP_AS(LQTDownloaderState::Value, state, LQTDownloaderState::S_IDLE)
public:
    DownloaderPriv(const QUrl& url, QIODevice* io, QObject* parent = nullptr);
    ~DownloaderPriv();

    QIODevice* ioDevice() { return m_io; }

public slots:
    void download();
    void abort();
    void write(const QByteArray& data);

signals:
    void downloadProgress(qint64 progress, qint64 total);

private:
    QNetworkAccessManager* m_manager;
    QNetworkReply* m_reply;
    QUrl m_url;
    QIODevice* m_io;
};

class Downloader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool state READ state NOTIFY stateChanged)
public:
    Downloader(const QUrl& url, const QString& filePath, QObject* parent = nullptr);
    Downloader(const QUrl& url, QByteArray* bucket, QObject* parent = nullptr);
    Downloader(const QUrl& url, QIODevice* destIo, QObject *parent);
    ~Downloader();

    void download();
    void abort();

    LQTDownloaderState::Value state() const { return m_threadContext->state(); }

signals:
#ifdef LQTUTILS_OMIT_ARG_FROM_SIGNAL
    void stateChanged();
#else
    void stateChanged(LQTDownloaderState::Value state);
#endif
    void downloadProgress(qint64 done, qint64 total);

private:
    QThread* m_thread;
    DownloaderPriv* m_threadContext;
    QUrl m_url;
    QIODevice* m_io;
};

inline
DownloaderPriv::DownloaderPriv(const QUrl& url, QIODevice* io, QObject* parent) :
    QObject(parent)
    , m_manager(new QNetworkAccessManager(this))
    , m_reply(nullptr)
    , m_url(url)
    , m_io(io) {}

inline
DownloaderPriv::~DownloaderPriv()
{
#ifdef DEBUG_LOGS
    qDebug() << Q_FUNC_INFO;
#endif
}

inline
void DownloaderPriv::download()
{
    set_state(LQTDownloaderState::S_DOWNLOADING);

    QNetworkRequest req(m_url);
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                     QNetworkRequest::NoLessSafeRedirectPolicy);

    m_reply = m_manager->get(req);
    connect(m_reply, &QNetworkReply::downloadProgress,
            this, &DownloaderPriv::downloadProgress);
    connect(m_reply, &QNetworkReply::readyRead, this, [this] {
        if (m_state != LQTDownloaderState::S_DOWNLOADING)
            return;
        write(m_reply->readAll());
    });
    connect(m_reply, &QNetworkReply::finished, this, [this] {
        if (m_state != LQTDownloaderState::S_DOWNLOADING)
            return;
        if (m_reply->error() != QNetworkReply::NoError) {
#ifdef DEBUG_LOGS
            qWarning() << "Download error:" << m_reply->error() << m_reply->errorString();
#endif
            set_state(LQTDownloaderState::S_NETWORK_FAILURE);
            return;
        }

        m_io->close();
        set_state(LQTDownloaderState::S_DONE);
    });
}

inline
void DownloaderPriv::abort()
{
    if (m_reply) {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = nullptr;
    }

    set_state(LQTDownloaderState::S_ABORTED);
}

inline
void DownloaderPriv::write(const QByteArray& data)
{
    if (m_state != LQTDownloaderState::S_DOWNLOADING)
        return;

    if (!m_io->isOpen()) {
        if (!m_io->open(QIODevice::WriteOnly)) {
            set_state(LQTDownloaderState::S_IO_FAILURE);
            return;
        }
    }

    if (m_io->write(data) != data.size())
        set_state(LQTDownloaderState::S_IO_FAILURE);
}

class LQTThread : public QThread
{
#ifdef DEBUG_LOGS
    ~LQTThread() {
        qDebug() << Q_FUNC_INFO;
    }
#endif
};

inline
Downloader::Downloader(const QUrl& url, const QString& filePath, QObject* parent) :
    Downloader(url, new QFile(filePath), parent)
{
    connect(m_thread, &QThread::finished,
            m_threadContext->ioDevice(), &QIODevice::deleteLater);
}

inline
Downloader::Downloader(const QUrl &url, QByteArray* bucket, QObject *parent) :
    Downloader(url, new QBuffer(bucket), parent)
{
    connect(m_thread, &QThread::finished,
            m_threadContext->ioDevice(), &QIODevice::deleteLater);
}

inline
Downloader::Downloader(const QUrl& url, QIODevice* destIo, QObject* parent) :
    QObject(parent)
    , m_thread(new LQTThread)
    , m_threadContext(new DownloaderPriv(url, destIo))
    , m_url(url)
    , m_io(destIo)
{
    LQTDownloaderState::registerEnum("com.luke", 1, 0);

    m_threadContext->moveToThread(m_thread);
    m_thread->start();

    connect(m_threadContext, &DownloaderPriv::stateChanged,
            this, &Downloader::stateChanged);
    connect(m_threadContext, &DownloaderPriv::downloadProgress,
            this, &Downloader::downloadProgress);
    connect(this, &Downloader::destroyed,
            m_threadContext, &QObject::deleteLater);
    connect(m_threadContext, &DownloaderPriv::destroyed,
            m_thread, &QThread::quit);
    connect(m_thread, &QThread::finished,
            m_thread, &QThread::deleteLater);
}

inline
Downloader::~Downloader()
{
#ifdef DEBUG_LOGS
    qDebug() << Q_FUNC_INFO;
#endif
    if (state() == LQTDownloaderState::S_DOWNLOADING)
        abort();
}

inline
void Downloader::download()
{
    if (m_threadContext->state() != LQTDownloaderState::S_IDLE) {
        qFatal("Do not re-use the downloader");
        return;
    }

    QMetaObject::invokeMethod(m_threadContext, "download", Qt::BlockingQueuedConnection);
}

inline
void Downloader::abort()
{
    if (m_threadContext->state() == LQTDownloaderState::S_DOWNLOADING)
        QMetaObject::invokeMethod(m_threadContext, "abort", Qt::BlockingQueuedConnection);
}

} // namespace

#endif // LQTUTILS_NET

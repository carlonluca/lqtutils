#include <QFile>

#include "lqtutils_net.h"

void LQTDownloaderPriv::write(const QByteArray& data)
{
    if (m_failed)
        return;
    if (!m_file.isOpen()) {
        if (!m_file.open(QIODevice::WriteOnly)) {
            m_failed = true;
            emit errorOccurred();
            return;
        }
    }

    if (m_file.write(data) != data.size()) {
        m_failed = true;
        emit errorOccurred();
    }
}

#include <QCoreApplication>

class LQTThread : public QThread
{
#ifdef DEBUG_LOGS
    ~LQTThread() {
        qDebug() << Q_FUNC_INFO;
    }
#endif
};

LQTDownloader::LQTDownloader(const QUrl& url, const QString& filePath, QObject* parent) :
    QObject(parent)
  , m_manager(new QNetworkAccessManager(this))
  , m_url(url)
  , m_filePath(filePath)
  , m_thread(new LQTThread)
  , m_threadContext(new LQTDownloaderPriv(filePath))
  , m_reply(nullptr)
{
    m_threadContext->moveToThread(m_thread);
    m_thread->start();

    connect(this, &LQTDownloader::destroyed,
            m_threadContext, &QObject::deleteLater);
    connect(m_threadContext, &LQTDownloaderPriv::destroyed,
            m_thread, &QThread::quit);
    connect(m_thread, &QThread::finished,
            m_thread, &QThread::deleteLater);

    connect(m_threadContext, &LQTDownloaderPriv::errorOccurred, this, [this] {
        set_state(S_FAILURE);
        if (m_reply)
            m_reply->abort();
    });
}

LQTDownloader::~LQTDownloader()
{
#ifdef DEBUG_LOGS
    qDebug() << Q_FUNC_INFO;
#endif
    if (m_state == S_DOWNLOADING)
        abort();
    m_thread = nullptr;
    m_threadContext = nullptr;
}

void LQTDownloader::download()
{
    if (m_state != S_IDLE) {
        qFatal("Do not re-use the downloader");
        return;
    }

    set_state(S_DOWNLOADING);

    m_reply = m_manager->get(QNetworkRequest(m_url));
    connect(m_reply, &QNetworkReply::downloadProgress,
            this, &LQTDownloader::downloadProgress);
    connect(m_reply, &QNetworkReply::readyRead, this, [this] {
        if (!m_threadContext || m_state != S_DOWNLOADING)
            return;
        QByteArray data = m_reply->readAll();
        QMetaObject::invokeMethod(m_threadContext, "write", Q_ARG(QByteArray, data));
    });
    connect(m_reply, &QNetworkReply::finished, this, [this] {
        if (!m_threadContext || m_state != S_DOWNLOADING)
            return;
        if (m_reply->error() != QNetworkReply::NoError) {
            set_state(S_FAILURE);
            emit networkErrorOccurred(m_reply->error());
            return;
        }

        set_state(S_DONE);
        emit downloadSucceeded(m_filePath);
    });
}

void LQTDownloader::abort()
{
    if (m_reply) {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = nullptr;
    }

    set_state(S_ABORTED);
}

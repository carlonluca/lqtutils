#include <QFile>

#include "lqtutils_net.h"

LQTDownloaderPriv::LQTDownloaderPriv(const QUrl& url, const QString &filePath, QObject* parent) :
    QObject(parent)
  , m_manager(new QNetworkAccessManager(this))
  , m_reply(nullptr)
  , m_url(url)
  , m_filePath(filePath)
  , m_file(m_filePath) {}

void LQTDownloaderPriv::download()
{
    set_state(LQTDownloaderState::S_DOWNLOADING);

    m_reply = m_manager->get(QNetworkRequest(m_url));
    connect(m_reply, &QNetworkReply::downloadProgress,
            this, &LQTDownloaderPriv::downloadProgress);
    connect(m_reply, &QNetworkReply::readyRead, this, [this] {
        if (m_state != LQTDownloaderState::S_DOWNLOADING)
            return;
        write(m_reply->readAll());
    });
    connect(m_reply, &QNetworkReply::finished, this, [this] {
        if (m_state != LQTDownloaderState::S_DOWNLOADING)
            return;
        if (m_reply->error() != QNetworkReply::NoError) {
            set_state(LQTDownloaderState::S_NETWORK_FAILURE);
            return;
        }

        set_state(LQTDownloaderState::S_DONE);
    });
}

void LQTDownloaderPriv::abort()
{
    if (m_reply) {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = nullptr;
    }

    set_state(LQTDownloaderState::S_ABORTED);
}

void LQTDownloaderPriv::write(const QByteArray& data)
{
    if (m_state != LQTDownloaderState::S_DOWNLOADING)
        return;
    if (!m_file.isOpen()) {
        if (!m_file.open(QIODevice::WriteOnly)) {
            set_state(LQTDownloaderState::S_IO_FAILURE);
            return;
        }
    }

    if (m_file.write(data) != data.size())
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

LQTDownloader::LQTDownloader(const QUrl& url, const QString& filePath, QObject* parent) :
    QObject(parent)
  , m_url(url)
  , m_filePath(filePath)
  , m_thread(new LQTThread)
  , m_threadContext(new LQTDownloaderPriv(url, filePath))
{
    LQTDownloaderState::registerEnum("com.luke", 1, 0);

    m_threadContext->moveToThread(m_thread);
    m_thread->start();

    connect(m_threadContext, &LQTDownloaderPriv::stateChanged,
            this, &LQTDownloader::stateChanged);
    connect(m_threadContext, &LQTDownloaderPriv::downloadProgress,
            this, &LQTDownloader::downloadProgress);
    connect(this, &LQTDownloader::destroyed,
            m_threadContext, &QObject::deleteLater);
    connect(m_threadContext, &LQTDownloaderPriv::destroyed,
            m_thread, &QThread::quit);
    connect(m_thread, &QThread::finished,
            m_thread, &QThread::deleteLater);
}

LQTDownloader::~LQTDownloader()
{
#ifdef DEBUG_LOGS
    qDebug() << Q_FUNC_INFO;
#endif
    if (state() == LQTDownloaderState::S_DOWNLOADING)
        abort();
}

void LQTDownloader::download()
{
    if (m_threadContext->state() != LQTDownloaderState::S_IDLE) {
        qFatal("Do not re-use the downloader");
        return;
    }

    QMetaObject::invokeMethod(m_threadContext, "download", Qt::BlockingQueuedConnection);
}

void LQTDownloader::abort()
{
    if (m_threadContext->state() == LQTDownloaderState::S_DOWNLOADING)
        QMetaObject::invokeMethod(m_threadContext, "abort", Qt::BlockingQueuedConnection);
}

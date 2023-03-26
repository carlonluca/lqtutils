#include <QFile>

#include "lqtutils_net.h"

//#define DEBUG_LOGS

namespace lqt {

DownloaderPriv::DownloaderPriv(const QUrl& url, QIODevice* io, QObject* parent) :
    QObject(parent)
  , m_manager(new QNetworkAccessManager(this))
  , m_reply(nullptr)
  , m_url(url)
  , m_io(io) {}

DownloaderPriv::~DownloaderPriv()
{
#ifdef DEBUG_LOGS
    qDebug() << Q_FUNC_INFO;
#endif
}

void DownloaderPriv::download()
{
    set_state(LQTDownloaderState::S_DOWNLOADING);

    m_reply = m_manager->get(QNetworkRequest(m_url));
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
            set_state(LQTDownloaderState::S_NETWORK_FAILURE);
            return;
        }

        m_io->close();
        set_state(LQTDownloaderState::S_DONE);
    });
}

void DownloaderPriv::abort()
{
    if (m_reply) {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = nullptr;
    }

    set_state(LQTDownloaderState::S_ABORTED);
}

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

Downloader::Downloader(const QUrl& url, const QString& filePath, QObject* parent) :
    Downloader(url, new QFile(filePath), parent)
{
    connect(m_thread, &QThread::finished,
            m_threadContext->ioDevice(), &QIODevice::deleteLater);
}

Downloader::Downloader(const QUrl& url, QIODevice* destIo, QObject* parent) :
    QObject(parent)
  , m_url(url)
  , m_io(destIo)
  , m_thread(new LQTThread)
  , m_threadContext(new DownloaderPriv(url, destIo))
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

Downloader::~Downloader()
{
#ifdef DEBUG_LOGS
    qDebug() << Q_FUNC_INFO;
#endif
    if (state() == LQTDownloaderState::S_DOWNLOADING)
        abort();
}

void Downloader::download()
{
    if (m_threadContext->state() != LQTDownloaderState::S_IDLE) {
        qFatal("Do not re-use the downloader");
        return;
    }

    QMetaObject::invokeMethod(m_threadContext, "download", Qt::BlockingQueuedConnection);
}

void Downloader::abort()
{
    if (m_threadContext->state() == LQTDownloaderState::S_DOWNLOADING)
        QMetaObject::invokeMethod(m_threadContext, "abort", Qt::BlockingQueuedConnection);
}

} // namespace

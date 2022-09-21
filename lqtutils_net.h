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

#include "lqtutils_prop.h"

#define DEBUG_LOGS

class LQTDownloaderPriv : public QObject
{
    Q_OBJECT
public:
    LQTDownloaderPriv(const QString& filePath, QObject* parent = nullptr) :
        QObject(parent)
      , m_filePath(filePath)
      , m_file(m_filePath)
      , m_failed(false) {}
#ifdef DEBUG_LOGS
    ~LQTDownloaderPriv() {
        qDebug() << Q_FUNC_INFO;
    }
#endif

public slots:
    void write(const QByteArray& data);

signals:
    void errorOccurred();

private:
    QString m_filePath;
    QFile m_file;
    bool m_failed;
};

class LQTDownloader : public QObject
{
    Q_OBJECT
public:
    enum DownloaderState {
        S_IDLE,
        S_DOWNLOADING,
        S_DONE,
        S_FAILURE,
        S_ABORTED
    };
    Q_ENUM(DownloaderState)

private:
    L_RO_PROP_AS(DownloaderState, state, DownloaderState::S_IDLE)

public:
    LQTDownloader(const QUrl& url, const QString& filePath, QObject* parent = nullptr);
    ~LQTDownloader();

    void download();
    void abort();

signals:
    void networkErrorOccurred(QNetworkReply::NetworkError err);
    void ioErrorOccurred();
    void downloadSucceeded(QString filePath);
    void downloadProgress(qint64 done, qint64 total);
    void finished();

private:
    QNetworkAccessManager* m_manager;
    QThread* m_thread;
    LQTDownloaderPriv* m_threadContext;
    QUrl m_url;
    QString m_filePath;
    QNetworkReply* m_reply;
};

#endif // LQTUTILS_NET

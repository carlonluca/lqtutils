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
#include "lqtutils_enum.h"

#define DEBUG_LOGS

L_DECLARE_ENUM(LQTDownloaderState,
               S_IDLE,
               S_DOWNLOADING,
               S_DONE,
               S_NETWORK_FAILURE,
               S_IO_FAILURE,
               S_ABORTED)

class LQTDownloaderPriv : public QObject
{
    Q_OBJECT
    L_RO_PROP_AS(LQTDownloaderState::Value, state, LQTDownloaderState::S_IDLE)
public:
    LQTDownloaderPriv(const QUrl& url, const QString& filePath, QObject* parent = nullptr);
    ~LQTDownloaderPriv() {
#ifdef DEBUG_LOGS
        qDebug() << Q_FUNC_INFO;
#endif
    }

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
    QString m_filePath;
    QFile m_file;
};

class LQTDownloader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool state READ state NOTIFY stateChanged)
public:
    LQTDownloader(const QUrl& url, const QString& filePath, QObject* parent = nullptr);
    ~LQTDownloader();

    void download();
    void abort();

    LQTDownloaderState::Value state() const { return m_threadContext->state(); }

signals:
    void stateChanged();
    void downloadProgress(qint64 done, qint64 total);

private:
    QThread* m_thread;
    LQTDownloaderPriv* m_threadContext;
    QUrl m_url;
    QString m_filePath;
};

#endif // LQTUTILS_NET

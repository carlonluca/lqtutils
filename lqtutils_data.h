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

#include <QByteArray>
#include <QString>
#include <QCryptographicHash>
#include <QFile>
#include <QRandomGenerator>
#include <QSet>
#include <QDebug>
#include <QBuffer>

#include "lqtutils_string.h"

namespace lqt {

/**
 * Computes the hash of a file.
 *
 * @brief lqt_hash
 * @param fileName
 * @param algo
 * @return
 */
QByteArray hash(const QString &fileName,
                    QCryptographicHash::Algorithm algo = QCryptographicHash::Md5)
{
    QFile f(fileName);
    if (!f.open(QFile::ReadOnly))
        return QByteArray();

    QCryptographicHash hash(algo);
    if (hash.addData(&f))
        return hash.result();

    return QByteArray();
}

/**
 * Writes random data to a device.
 *
 * @brief lqt_random_file
 * @param fileName
 * @param size
 * @return
 */
bool random_data(QIODevice* device, qint64 size)
{
    const int chunks = size/1024;
    for (int i = 0; i < chunks; i++) {
        QVector<quint32> list;
        list.resize(1024/4);
        QRandomGenerator::global()->fillRange(list.data(), list.size());
        const int size = list.size()*sizeof(quint32);
        if (device->write(reinterpret_cast<const char*>(list.data()), size) != size)
            return false;
    }

    const int tailSize = size - chunks*1024;
    for (int i = 0; i < tailSize; i++) {
        const quint32 v = QRandomGenerator::global()->generate();
        device->write(reinterpret_cast<const char*>(&v), 1);
    }

    return true;
}

/**
 * Creates a random file of a specified size.
 *
 * @brief lqt_random_file
 * @param fileName
 * @param size
 * @return
 */
bool random_file(const QString& fileName, qint64 size)
{
    QFile f(fileName);
    if (!f.open(QIODevice::WriteOnly))
        return false;

    return random_data(&f, size);
}

/**
 * Returns random data in a buffer.
 *
 * @brief random_data
 * @param size
 * @return
 */
QByteArray random_data(qint64 size)
{
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    return random_data(&buffer, size) ? buffer.buffer() : QByteArray();
}

template<typename T>
inline const T* get_any(const QSet<T>& set)
{
    if (set.isEmpty())
        return nullptr;
    typename QSet<T>::const_iterator it = set.constBegin();
    return &(*it);
}

/**
 * Reads an entire file into memory.
 *
 * @brief readAll
 * @param io
 * @return
 */
inline QByteArray read_all(QIODevice* io)
{
    if (!io)
        return QByteArray();

    if (!io->open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open I/O device";
        return QByteArray();
    }

    return io->readAll();
}

/**
 * Reads all data in a file.
 *
 * @brief read_all
 * @param filePath
 * @return
 */
inline QByteArray read_all(const QString& filePath)
{
    QFile f(filePath);
    return read_all(&f);
}

/**
 * Writes data to the device.
 *
 * @brief write_all
 * @param io
 * @param data
 * @return
 */
inline bool write_all(QIODevice* io, const QByteArray& data)
{
    if (!io)
        return false;

    if (!io->open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open I/O device";
        return false;
    }

    return io->write(data) == data.size();
}

/**
 * Writes data to the file.
 *
 * @brief write_all
 * @param filePath
 * @param data
 * @return
 */
inline bool write_all(const QString& filePath, const QByteArray& data)
{
    QFile f(filePath);
    return write_all(&f, data);
}

/**
 * Copies an entire directory to another location.
 *
 * @brief copy_path
 * @param src
 * @param dst
 * @return
 */
bool copy_path(const QString& src, const QString& dst)
{
    QDir sourceDir(src);
    if (!sourceDir.exists())
        return false;

    QDir dstDir(dst);
    if (dstDir.exists())
        dstDir = QDir(lqt::path_combine({ dst, sourceDir.dirName() }));
    if (dstDir.exists()) {
        qWarning() << "Destination directory exists";
        return false;
    }

    if (!dstDir.mkpath(".")) {
        qWarning() << "Failed to create dir" << dstDir.absolutePath();
        return false;
    }

    for (const auto& entry : QDirListing(src, QDirListing::IteratorFlag::Recursive)) {
        const QString filePath = entry.absoluteFilePath();
        const QString relativePath = sourceDir.relativeFilePath(filePath);
        const QString destPath = lqt::path_combine({dstDir.absolutePath(), relativePath});

        if (QFile(destPath).exists()) {
            qWarning() << "Destination exists:" << destPath;
            continue;
        }

        if (entry.isDir()) {
            if (!QDir(destPath).mkpath(".")) {
                qWarning() << "Failed to create dir:" << destPath;
                continue;
            }
        }
        else {
            if (!QFile(filePath).copy(destPath)) {
                qWarning() << "Failed to copy" << filePath << "->" << destPath;
                continue;
            }
        }
    }

    return true;
}

} // namespace

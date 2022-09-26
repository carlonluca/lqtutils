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

/**
 * Computes the hash of a file.
 *
 * @brief lqt_hash
 * @param fileName
 * @param algo
 * @return
 */
QByteArray lqt_hash(const QString &fileName,
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
 * Creates a random file of a specified size.
 *
 * @brief lqt_random_file
 * @param fileName
 * @param size
 * @return
 */
bool lqt_random_file(const QString& fileName, qint64 size)
{
    QFile f(fileName);
    if (!f.open(QIODevice::WriteOnly))
        return false;

    const int chunks = size/1024;
    for (int i = 0; i < chunks; i++) {
        QVector<quint32> list;
        list.resize(1024/4);
        QRandomGenerator::global()->fillRange(list.data(), list.size());
        const int size = list.size()*sizeof(quint32);
        if (f.write(reinterpret_cast<const char*>(list.data()), size) != size)
            return false;
    }

    const int tailSize = size - chunks*1024;
    for (int i = 0; i < tailSize; i++) {
        const quint32 v = QRandomGenerator::global()->generate();
        f.write(reinterpret_cast<const char*>(&v), 1);
    }

    return true;
}

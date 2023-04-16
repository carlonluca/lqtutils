/**
 * MIT License
 *
 * Copyright (c) 2023 Luca Carlon
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

#include <QFile>
#include <QString>
#include <QDebug>
#include <QRegularExpression>

#include <optional>

namespace lqt {

struct MemData
{
    qint64 totalMemBytes;
    qint64 freeMemBytes;
};

inline std::optional<MemData> read_mem_data()
{
    QFile memFile(QStringLiteral("/proc/meminfo"));
    if (!memFile.exists()) {
        qWarning() << "Meminfo file missing";
        return std::nullopt;
    }

    if (!memFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot read meminfo file";
        return std::nullopt;
    }

    const QString meminfoContent = memFile.readAll();

    static const QRegularExpression regexMemTotal(QStringLiteral("MemTotal:\\s*(\\d+)"));
    static const QRegularExpression regexMemFree(QStringLiteral("MemFree:\\s*(\\d+)"));
    static const QRegularExpression regexMemBuff(QStringLiteral("Buffers:\\s*(\\d+)"));
    static const QRegularExpression regexMemCache(QStringLiteral("Cached:\\s*(\\d+)"));

    auto readValue = [&meminfoContent] (const QRegularExpression& regex) -> qint64 {
        QRegularExpressionMatch match = regex.match(meminfoContent);
        if (!match.hasMatch() || match.lastCapturedIndex() != 1) {
            qWarning() << "Failed to parse meminfo file";
            return -1;
        }

        return match.captured(1).toLongLong();
    };

    qint64 memTotal = readValue(regexMemTotal)*1024;
    qint64 memFree = readValue(regexMemFree)*1024;
    qint64 memBuff = readValue(regexMemBuff)*1024;
    qint64 memCache = readValue(regexMemCache)*1024;
    if (memTotal < 0 || memFree < 0 || memBuff < 0 || memCache < 0)
        return std::nullopt;

    return MemData {
        memTotal,
        memFree + memBuff + memCache
    };
}

}

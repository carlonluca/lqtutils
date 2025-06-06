/**
 * MIT License
 *
 * Copyright (c) 2020 Luca Carlon
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

#ifndef LQTUTILS_STRING_H
#define LQTUTILS_STRING_H

#include <QDir>
#include <QString>
#include <QRectF>
#include <QLocale>

#ifndef QStringLiteral
#define QStringLiteral(s) s
#endif

namespace lqt {

inline QString path_combine(std::initializer_list<QString> l)
{
    QString ret;
    for (size_t i = 0; i < l.size(); i++) {
        ret += *(l.begin() + i);
        if (Q_LIKELY(i != l.size() - 1))
            ret += QDir::separator();
    }
    return ret;
}

inline int string_to_int(const QString& s, int def, bool* ok = nullptr)
{
    bool _ok;
    int ret = QLocale::system().toInt(s, &_ok);
    if (ok)
        *ok = _ok;
    return _ok ? ret : def;
}

inline qint64 string_to_int64(const QString& s, qint64 def, bool* ok = nullptr)
{
    bool _ok;
    qint64 ret = QLocale::system().toLongLong(s, &_ok);
    if (ok)
        *ok = _ok;
    return _ok ? ret : def;
}

inline qint64 string_to_uint64(const QString& s, quint64 def, bool* ok = nullptr)
{
    bool _ok;
    qint64 ret = QLocale::system().toULongLong(s, &_ok);
    if (ok)
        *ok = _ok;
    return _ok ? ret : def;
}

inline float string_to_float(const QString& s, float def, bool* ok = nullptr)
{
    bool _ok;
    float ret = QLocale::system().toFloat(s, &_ok);
    if (ok)
        *ok = _ok;
    return _ok ? ret : def;
}

inline double string_to_double(const QString& s, double def, bool* ok = nullptr)
{
    bool _ok;
    double ret = QLocale::system().toDouble(s, &_ok);
    if (ok)
        *ok = _ok;
    return _ok ? ret : def;
}

inline float string_to_float_en(const QString& s, float def, bool* ok = nullptr)
{
    bool _ok;
    QLocale locale(QLocale::English, QLocale::UnitedStates);
    float ret = locale.toFloat(s, &_ok);
    if (ok)
        *ok = _ok;
    return _ok ? ret : def;
}

inline QRectF string_to_rect(const QString& s)
{
    if (s.isEmpty())
        return QRectF();

    QStringList tokens = s.split(',');
    if (tokens.size() != 4)
        return QRectF();

    bool convOk;
    QRectF ret;
    QLocale locale(QLocale::English, QLocale::UnitedStates);
    ret.setX(locale.toDouble(tokens[0].trimmed(), &convOk));
    if (!convOk)
        return QRectF();
    ret.setY(locale.toDouble(tokens[1].trimmed(), &convOk));
    if (!convOk)
        return QRectF();
    ret.setWidth(locale.toDouble(tokens[2].trimmed(), &convOk));
    if (!convOk)
        return QRectF();
    ret.setHeight(locale.toDouble(tokens[3].trimmed(), &convOk));
    if (!convOk)
        return QRectF();

    return ret;
}

inline QString string_from_rect(const QRectF& rect)
{
    if (!rect.isValid() || rect.isNull())
        return QString();

    QLocale locale(QLocale::English, QLocale::UnitedStates);
    return QString(QStringLiteral("%1,%2,%3,%4")).arg(
            locale.toString(rect.x())
          , locale.toString(rect.y())
          , locale.toString(rect.width())
          , locale.toString(rect.height()));
}

inline QPointF string_to_point(const QString& s)
{
    if (s.isEmpty())
        return QPoint();

    QStringList tokens = s.split(',');
    if (tokens.size() != 2)
        return QPoint();

    bool convOk;
    QPoint ret;
    QLocale locale(QLocale::English, QLocale::UnitedStates);
    ret.setX(locale.toDouble(tokens[0].trimmed(), &convOk));
    if (!convOk)
        return QPointF();
    ret.setY(locale.toDouble(tokens[1].trimmed(), &convOk));
    if (!convOk)
        return QPointF();

    return ret;
}

inline QString string_from_point(const QPointF& point)
{
    if (point.isNull())
        return QString();

    QLocale locale(QLocale::English, QLocale::UnitedStates);
    return QString(QStringLiteral("%1,%2"))
        .arg(locale.toString(point.x()), locale.toString(point.y()));
}

inline QSize string_to_size(const QString& s)
{
    if (s.isEmpty())
        return QSize();

    QStringList tokens = s.split('x');
    if (tokens.size() != 2)
        return QSize();

    bool convOk;
    int width = string_to_int(tokens[0], 0, &convOk);
    if (!convOk)
        return QSize();
    int height = string_to_int(tokens[1], 0, &convOk);
    if (!convOk)
        return QSize();

    return QSize(width, height);
}

inline QString size_to_string(const QSize& size)
{
    if (!size.isValid() || size.isNull())
        return QString();

    return QString(QStringLiteral("%1x%2"))
            .arg(size.width())
            .arg(size.height());
}

} // namespace

#endif // LQTUTILS_STRING_H

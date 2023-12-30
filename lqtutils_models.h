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

#ifndef LQTUTILS_MODELS_H
#define LQTUTILS_MODELS_H

#include <QObject>
#include <QVariant>
#include <QModelIndex>
#include <QAbstractListModel>
#include <QSharedPointer>
#include <QHash>
#include <QList>

template<typename T>
class QmlSharedPointerList : public QAbstractListModel
{
public:
    QmlSharedPointerList<T>(const QList<QSharedPointer<T>>& list = QList<QSharedPointer<T>>(), QObject* parent = nullptr) :
        QAbstractListModel(parent), m_list(list) {}

    int rowCount(const QModelIndex& parent = QModelIndex()) const override {
        return static_cast<int>(m_list.count());
    }

    QVariant data(const QModelIndex& index, int role = Qt::UserRole) const override {
        if (role != Qt::UserRole)
            return QVariant();
        if (index.row() < 0 || index.row() >= m_list.size())
            return QVariant();

        return QVariant::fromValue<T*>(m_list[index.row()].data());
    }

    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles.insert(Qt::UserRole, "data");
        return roles;
    }

    void refreshModel(const QList<QSharedPointer<T>>& list) {
        beginResetModel();
        m_list = list;
        endResetModel();
    }

private:
    QList<QSharedPointer<T>> m_list;
};

#endif // LQTUTILS_MODELS_H

/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Labs Calendar module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QQUICKCALENDARMODEL_P_H
#define QQUICKCALENDARMODEL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qdatetime.h>
#include <QtQml/qqmlparserstatus.h>
#include <QtQml/qqml.h>

QT_BEGIN_NAMESPACE

class QQuickCalendarModelPrivate;

class QQuickCalendarModel : public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QDate from READ from WRITE setFrom NOTIFY fromChanged FINAL)
    Q_PROPERTY(QDate to READ to WRITE setTo NOTIFY toChanged FINAL)
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    explicit QQuickCalendarModel(QObject *parent = nullptr);

    QDate from() const;
    void setFrom(const QDate &from);

    QDate to() const;
    void setTo(const QDate &to);

    Q_INVOKABLE int monthAt(int index) const;
    Q_INVOKABLE int yearAt(int index) const;
    Q_INVOKABLE int indexOf(const QDate &date) const;
    Q_INVOKABLE int indexOf(int year, int month) const;

    enum {
        MonthRole,
        YearRole
    };

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

Q_SIGNALS:
    void fromChanged();
    void toChanged();
    void countChanged();

protected:
    void classBegin() override;
    void componentComplete() override;

private:
    Q_DISABLE_COPY(QQuickCalendarModel)
    Q_DECLARE_PRIVATE(QQuickCalendarModel)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QQuickCalendarModel)

#endif // QQUICKCALENDARMODEL_P_H

/***************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Copyright (C) 2013 BlackBerry Limited. All rights reserved.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Quick Controls 2 module of the Qt Toolkit.
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
**
**
**
****************************************************************************/

#ifndef QQUICKSTYLESELECTOR_P_H
#define QQUICKSTYLESELECTOR_P_H

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

#include <QtCore/qurl.h>
#include <QtCore/qstring.h>
#include <QtCore/qscopedpointer.h>
#include <QtQuickControls2/private/qtquickcontrols2global_p.h>

QT_BEGIN_NAMESPACE

class QQuickStyleSelectorPrivate;

class Q_QUICKCONTROLS2_PRIVATE_EXPORT QQuickStyleSelector
{
public:
    QQuickStyleSelector();
    ~QQuickStyleSelector();

    QStringList selectors() const;
    void addSelector(const QString &selector);

    QStringList paths() const;
    void setPaths(const QStringList &paths);

    QUrl select(const QString &fileName) const;

private:
    Q_DISABLE_COPY(QQuickStyleSelector)
    Q_DECLARE_PRIVATE(QQuickStyleSelector)
    QScopedPointer<QQuickStyleSelectorPrivate> d_ptr;
};

QT_END_NAMESPACE

#endif // QQUICKSTYLESELECTOR_P_H

/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Quick Templates 2 module of the Qt Toolkit.
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

#ifndef QQUICKPOPUPITEM_P_P_H
#define QQUICKPOPUPITEM_P_P_H

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

#include <QtQuickTemplates2/private/qquickpopupitem_p.h>
#include <QtQuickTemplates2/private/qquickpalette_p.h>

QT_BEGIN_NAMESPACE

class QQuickPopup;

class QQuickPopupItemPrivate : public QQuickPagePrivate
{
    Q_DECLARE_PUBLIC(QQuickPopupItem)

public:
    QQuickPopupItemPrivate(QQuickPopup *popup);

    void init();

    void implicitWidthChanged() override;
    void implicitHeightChanged() override;

    void resolveFont() override;
    void resolvePalette() override;

    QQuickItem *getContentItem() override;

    void cancelContentItem() override;
    void executeContentItem(bool complete = false) override;

    void cancelBackground() override;
    void executeBackground(bool complete = false) override;

    int backId = 0;
    int escapeId = 0;
    QQuickPopup *popup = nullptr;
};

QT_END_NAMESPACE

#endif // QQUICKPOPUPITEM_P_P_H

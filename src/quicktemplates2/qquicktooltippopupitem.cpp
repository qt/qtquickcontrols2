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

#include "qquicktooltippopupitem_p_p.h"

#include <QtQuickTemplates2/private/qquickpopup_p.h>

QT_BEGIN_NAMESPACE

QQuickToolTipPopupItemPrivate::QQuickToolTipPopupItemPrivate(QQuickPopup *popup)
    : QQuickPopupItemPrivate(popup)
{
}

qreal QQuickToolTipPopupItemPrivate::getContentWidth() const
{
    auto textItem = qobject_cast<QQuickText*>(contentItem);
    if (textItem)
        return textItem->contentWidth();

    return contentItem ? contentItem->implicitWidth() : 0;
}

void QQuickToolTipPopupItemPrivate::updateContentWidth()
{
    Q_Q(QQuickToolTipPopupItem);
    // Don't need to calculate the implicit contentWidth if an explicit one was set.
    if (hasContentWidth)
        return;

    auto textItem = qobject_cast<QQuickText*>(contentItem);
    if (!textItem) {
        // It's not a Text item, so use the base contentWidth logic (i.e. use implicitWidth).
        QQuickPopupItemPrivate::updateContentWidth();
        return;
    }

    const qreal oldContentWidth = contentWidth;
    const qreal newContentWidth = textItem->contentWidth();
    if (qFuzzyCompare(oldContentWidth, newContentWidth))
        return;

    contentWidth = newContentWidth;
    q->contentSizeChange(QSizeF(contentWidth, contentHeight), QSizeF(oldContentWidth, contentHeight));
    emit q->contentWidthChanged();
}

QQuickToolTipPopupItem::QQuickToolTipPopupItem(QQuickPopup *popup)
    : QQuickPopupItem(*(new QQuickToolTipPopupItemPrivate(popup)))
{
}

void QQuickToolTipPopupItem::contentItemChange(QQuickItem *newItem, QQuickItem *oldItem)
{
    Q_D(QQuickToolTipPopupItem);
    QQuickPopupItem::contentItemChange(newItem, oldItem);

    // Text's implicitWidth does not account for newlines and hence is too large,
    // so we need to listen to contentWidth's change signals.
    auto oldTextItem = qobject_cast<QQuickText*>(oldItem);
    if (oldTextItem) {
        QObjectPrivate::disconnect(oldTextItem, &QQuickText::contentWidthChanged,
            d, &QQuickToolTipPopupItemPrivate::updateContentWidth);
    }

    auto newTextItem = qobject_cast<QQuickText*>(newItem);
    if (newTextItem) {
        QObjectPrivate::connect(newTextItem, &QQuickText::contentWidthChanged,
            d, &QQuickToolTipPopupItemPrivate::updateContentWidth);
    }
}

QT_END_NAMESPACE

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

#include <QtQuickTemplates2/private/qquickpage_p.h>

QT_BEGIN_NAMESPACE

class QQuickPopup;
class QQuickPopupItemPrivate;
class QQuickPopupItem : public QQuickPage
{
    Q_OBJECT

public:
    explicit QQuickPopupItem(QQuickPopup *popup);

    void grabShortcut();
    void ungrabShortcut();

protected:
    void updatePolish() override;

    bool event(QEvent *event) override;
    bool childMouseEventFilter(QQuickItem *child, QEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseUngrabEvent() override;
#if QT_CONFIG(quicktemplates2_multitouch)
    void touchEvent(QTouchEvent *event) override;
    void touchUngrabEvent() override;
#endif
#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent *event) override;
#endif

    void contentItemChange(QQuickItem *newItem, QQuickItem *oldItem) override;
    void contentSizeChange(const QSizeF &newSize, const QSizeF &oldSize) override;
    void fontChange(const QFont &newFont, const QFont &oldFont) override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    void localeChange(const QLocale &newLocale, const QLocale &oldLocale) override;
    void mirrorChange() override;
    void itemChange(ItemChange change, const ItemChangeData &data) override;
    void paddingChange(const QMarginsF &newPadding, const QMarginsF &oldPadding) override;
    void paletteChange(const QPalette &newPalette, const QPalette &oldPalette) override;
    void enabledChange() override;

    QFont defaultFont() const override;
    QPalette defaultPalette() const override;

#if QT_CONFIG(accessibility)
    QAccessible::Role accessibleRole() const override;
    void accessibilityActiveChanged(bool active) override;
#endif

private:
    Q_DISABLE_COPY(QQuickPopupItem)
    Q_DECLARE_PRIVATE(QQuickPopupItem)
    friend class QQuickPopup;
};

QT_END_NAMESPACE

#endif // QQUICKPOPUPITEM_P_P_H

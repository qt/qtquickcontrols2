/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Labs Templates module of the Qt Toolkit.
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

#ifndef QQUICKPLATFORMMENUITEMGROUP_P_H
#define QQUICKPLATFORMMENUITEMGROUP_P_H

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

#include <QtCore/qobject.h>
#include <QtCore/qvector.h>
#include <QtQml/qqml.h>

QT_BEGIN_NAMESPACE

class QQuickPlatformMenuItem;
class QQuickPlatformMenuItemGroupPrivate;

class QQuickPlatformMenuItemGroup : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged FINAL)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged FINAL)
    Q_PROPERTY(bool exclusive READ isExclusive WRITE setExclusive NOTIFY exclusiveChanged FINAL)
    Q_PROPERTY(QQuickPlatformMenuItem *checkedItem READ checkedItem WRITE setCheckedItem NOTIFY checkedItemChanged FINAL)
    Q_PROPERTY(QQmlListProperty<QQuickPlatformMenuItem> items READ items NOTIFY itemsChanged FINAL)

public:
    explicit QQuickPlatformMenuItemGroup(QObject *parent = nullptr);
    ~QQuickPlatformMenuItemGroup();

    bool isEnabled() const;
    void setEnabled(bool enabled);

    bool isVisible() const;
    void setVisible(bool visible);

    bool isExclusive() const;
    void setExclusive(bool exclusive);

    QQuickPlatformMenuItem *checkedItem() const;
    void setCheckedItem(QQuickPlatformMenuItem *item);

    QQmlListProperty<QQuickPlatformMenuItem> items();

    Q_INVOKABLE void addItem(QQuickPlatformMenuItem *item);
    Q_INVOKABLE void removeItem(QQuickPlatformMenuItem *item);
    Q_INVOKABLE void clear();

Q_SIGNALS:
    void triggered(QQuickPlatformMenuItem *item);
    void hovered(QQuickPlatformMenuItem *item);

    void enabledChanged();
    void visibleChanged();
    void exclusiveChanged();
    void checkedItemChanged();
    void itemsChanged();

private:
    QQuickPlatformMenuItem *findCurrent() const;
    void updateCurrent();
    void activateItem();
    void hoverItem();

    static void items_append(QQmlListProperty<QQuickPlatformMenuItem> *prop, QQuickPlatformMenuItem *obj);
    static int items_count(QQmlListProperty<QQuickPlatformMenuItem> *prop);
    static QQuickPlatformMenuItem *items_at(QQmlListProperty<QQuickPlatformMenuItem> *prop, int index);
    static void items_clear(QQmlListProperty<QQuickPlatformMenuItem> *prop);

    bool m_enabled;
    bool m_visible;
    bool m_exclusive;
    QQuickPlatformMenuItem *m_checkedItem;
    QVector<QQuickPlatformMenuItem*> m_items;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QQuickPlatformMenuItemGroup)

#endif // QQUICKPLATFORMMENUITEMGROUP_P_H

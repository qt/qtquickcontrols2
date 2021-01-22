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

#ifndef QQUICKAPPLICATIONWINDOW_P_H
#define QQUICKAPPLICATIONWINDOW_P_H

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

#include <QtQuick/private/qquickwindowmodule_p.h>
#include <QtQuickTemplates2/private/qtquicktemplates2global_p.h>
#include <QtGui/qfont.h>
#include <QtGui/qpalette.h>
#include <QtCore/qlocale.h>

QT_BEGIN_NAMESPACE

class QQuickOverlay;
class QQuickApplicationWindowPrivate;
class QQuickApplicationWindowAttached;
class QQuickApplicationWindowAttachedPrivate;

class Q_QUICKTEMPLATES2_PRIVATE_EXPORT QQuickApplicationWindow : public QQuickWindowQmlImpl
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *background READ background WRITE setBackground NOTIFY backgroundChanged FINAL)
    Q_PROPERTY(QQuickItem *contentItem READ contentItem CONSTANT FINAL)
    Q_PRIVATE_PROPERTY(QQuickApplicationWindow::d_func(), QQmlListProperty<QObject> contentData READ contentData FINAL)
    Q_PROPERTY(QQuickItem *activeFocusControl READ activeFocusControl NOTIFY activeFocusControlChanged FINAL)
    Q_PROPERTY(QQuickItem *header READ header WRITE setHeader NOTIFY headerChanged FINAL)
    Q_PROPERTY(QQuickItem *footer READ footer WRITE setFooter NOTIFY footerChanged FINAL)
    Q_PROPERTY(QQuickOverlay *overlay READ overlay CONSTANT FINAL)
    Q_PROPERTY(QFont font READ font WRITE setFont RESET resetFont NOTIFY fontChanged FINAL)
    Q_PROPERTY(QLocale locale READ locale WRITE setLocale RESET resetLocale NOTIFY localeChanged FINAL)
    // 2.3 (Qt 5.10)
    Q_PROPERTY(QPalette palette READ palette WRITE setPalette RESET resetPalette NOTIFY paletteChanged FINAL REVISION 3)
    Q_PROPERTY(QQuickItem *menuBar READ menuBar WRITE setMenuBar NOTIFY menuBarChanged FINAL REVISION 3)
    Q_CLASSINFO("DeferredPropertyNames", "background")
    Q_CLASSINFO("DefaultProperty", "contentData")

public:
    explicit QQuickApplicationWindow(QWindow *parent = nullptr);
    ~QQuickApplicationWindow();

    static QQuickApplicationWindowAttached *qmlAttachedProperties(QObject *object);

    QQuickItem *background() const;
    void setBackground(QQuickItem *background);

    QQuickItem *contentItem() const;

    QQuickItem *activeFocusControl() const;

    QQuickItem *header() const;
    void setHeader(QQuickItem *header);

    QQuickItem *footer() const;
    void setFooter(QQuickItem *footer);

    QQuickOverlay *overlay() const;

    QFont font() const;
    void setFont(const QFont &font);
    void resetFont();

    QLocale locale() const;
    void setLocale(const QLocale &locale);
    void resetLocale();

    // 2.3 (Qt 5.10)
    QPalette palette() const;
    void setPalette(const QPalette &palette);
    void resetPalette();

    QQuickItem *menuBar() const;
    void setMenuBar(QQuickItem *menuBar);

Q_SIGNALS:
    void backgroundChanged();
    void activeFocusControlChanged();
    void headerChanged();
    void footerChanged();
    void fontChanged();
    void localeChanged();
    Q_REVISION(3) void paletteChanged();
    Q_REVISION(3) void menuBarChanged();

protected:
    bool isComponentComplete() const;
    void classBegin() override;
    void componentComplete() override;
    void resizeEvent(QResizeEvent *event) override;

private:
    Q_DISABLE_COPY(QQuickApplicationWindow)
    Q_DECLARE_PRIVATE(QQuickApplicationWindow)
    Q_PRIVATE_SLOT(d_func(), void _q_updateActiveFocus())
    QScopedPointer<QQuickApplicationWindowPrivate> d_ptr;
};

class Q_QUICKTEMPLATES2_PRIVATE_EXPORT QQuickApplicationWindowAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQuickApplicationWindow *window READ window NOTIFY windowChanged FINAL)
    Q_PROPERTY(QQuickItem *contentItem READ contentItem NOTIFY contentItemChanged FINAL)
    Q_PROPERTY(QQuickItem *activeFocusControl READ activeFocusControl NOTIFY activeFocusControlChanged FINAL)
    Q_PROPERTY(QQuickItem *header READ header NOTIFY headerChanged FINAL)
    Q_PROPERTY(QQuickItem *footer READ footer NOTIFY footerChanged FINAL)
    Q_PROPERTY(QQuickOverlay *overlay READ overlay NOTIFY overlayChanged FINAL)
    Q_PROPERTY(QQuickItem *menuBar READ menuBar NOTIFY menuBarChanged FINAL) // REVISION 3

public:
    explicit QQuickApplicationWindowAttached(QObject *parent = nullptr);

    QQuickApplicationWindow *window() const;
    QQuickItem *contentItem() const;
    QQuickItem *activeFocusControl() const;
    QQuickItem *header() const;
    QQuickItem *footer() const;
    QQuickOverlay *overlay() const;
    QQuickItem *menuBar() const;

Q_SIGNALS:
    void windowChanged();
    void contentItemChanged();
    void activeFocusControlChanged();
    void headerChanged();
    void footerChanged();
    void overlayChanged();
    // 2.3 (Qt 5.10)
    /*Q_REVISION(3)*/ void menuBarChanged();

private:
    Q_DISABLE_COPY(QQuickApplicationWindowAttached)
    Q_DECLARE_PRIVATE(QQuickApplicationWindowAttached)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QQuickApplicationWindow)
QML_DECLARE_TYPEINFO(QQuickApplicationWindow, QML_HAS_ATTACHED_PROPERTIES)

#endif // QQUICKAPPLICATIONWINDOW_P_H

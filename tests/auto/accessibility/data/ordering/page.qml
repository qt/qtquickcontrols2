import QtQuick 2.15
import QtQuick.Controls 2.15

Page {
    title: "Page"
    Accessible.role: Accessible.Pane

    header: Label {
        text: "Header"
    }

    footer: Label {
        text: "Footer"
    }

    Label {
        text: "Content item 1"
    }

    Label {
        text: "Content item 2"
    }
}

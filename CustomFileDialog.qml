import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Effects
import QtQuick.Controls.Basic

Window {
    id: root
    title: "Выбор объекта (Локально / MinIO)"
    width: 500; height: 400
//    modal: false
//    standardButtons: Dialog.Cancel | Dialog.Open

    property string currentSelectedPath: ""
    property string currentPath: "/"
    signal pathSelected(string folderPath)

    onVisibleChanged: {
        if (visible) {
            tabBar.forceActiveFocus()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        // Поле выбранного пути
        TextField {
            Layout.fillWidth: true
            text: root.currentSelectedPath
            readOnly: true
            placeholderText: "Ничего не выбрано"
        }
        // Навигационная панель (Табы)
        TabBar {
            id: tabBar
            Layout.fillWidth: true
            currentIndex: 0
            focus: true
            focusPolicy: Qt.StrongFocus
            onActiveFocusChanged: {
                if (activeFocus && currentItem) {
                    currentItem.forceActiveFocus()
                }
            }
            TabButton {
                text: "Локально"
//                focus: true
                focusPolicy:Qt.ClickFocus
                onClicked: {
                    storageModel.enterLocal("/")
                    gridView.forceActiveFocus()
                }
                onActiveFocusChanged: {
                    if (activeFocus) storageModel.enterLocal("/")
                }
                //KeyNavigation.down: gridView    //.forceActiveFocus()
                Keys.onPressed: (event) => {
                    if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                        gridView.forceActiveFocus()
                        event.accepted = true
                    }
                }
                Keys.onDownPressed: {
                    gridView.forceActiveFocus()
                }
                contentItem: Text {
                    text: parent.text
                    font.pixelSize: 15
                    font.weight: parent.checked ? Font.DemiBold : Font.Normal
                    color: parent.checked ? "#616161" : (parent.hovered ? "#555" : "#888")
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight

                    Behavior on color { ColorAnimation { duration: 200 } }
                }
                // 2. Фон и индикатор (линия под кнопкой)
                background: Rectangle {
                    anchors.fill: parent
                    color: parent.down ? "#f0f0f0" : (parent.hovered ? "#f8f8f8" : "transparent")

                    // Линия-индикатор
                    Rectangle {
                        anchors.bottom: parent.bottom
                        width: parent.width
                        height: parent.parent.checked ? 3 : 0 // Появляется только когда выбрано
                        color: "#616161"        //"#2196F3"
                        radius: 2

                        // Плавная анимация появления линии
                        Behavior on height { NumberAnimation { duration: 150 } }
                    }

                    Behavior on color { ColorAnimation { duration: 200 } }
                }
            }
            TabButton {
                text: "MinIO"
                focus: true
                focusPolicy:Qt.ClickFocus
                onClicked: {
                    storageModel.enterMinio("main-bucket")
                    gridView.forceActiveFocus()
                }
                onActiveFocusChanged: {
                    if (activeFocus) storageModel.enterMinio("main-bucket")
                }
                //KeyNavigation.down: gridView    //.forceActiveFocus()
                Keys.onPressed: (event) => {
                    if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                        gridView.forceActiveFocus()
                        event.accepted = true
                    }
                }
                Keys.onDownPressed: {
                    gridView.forceActiveFocus()
                }
                contentItem: Text {
                    text: parent.text
                    font.pixelSize: 15
                    font.weight: parent.checked ? Font.DemiBold : Font.Normal
                    color: parent.checked ? "#616161" : (parent.hovered ? "#555" : "#888")
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight

                    Behavior on color { ColorAnimation { duration: 200 } }
                }
                // 2. Фон и индикатор (линия под кнопкой)
                background: Rectangle {
                    anchors.fill: parent
                    color: parent.down ? "#f0f0f0" : (parent.hovered ? "#f8f8f8" : "transparent")

                    // Линия-индикатор
                    Rectangle {
                        anchors.bottom: parent.bottom
                        width: parent.width
                        height: parent.parent.checked ? 3 : 0 // Появляется только когда выбрано
                        color: "#616161"        //"#2196F3"
                        radius: 2

                        // Плавная анимация появления линии
                        Behavior on height { NumberAnimation { duration: 150 } }
                    }

                    Behavior on color { ColorAnimation { duration: 200 } }
                }
            }
        }

        // Список файлов и папок
//        ListView {
        GridView{
//            id: listView
            id: gridView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true // clip what is it?
            focus: true
            flow: GridView.FlowTopToBottom

            cellWidth: 140  // Ширина колонки
            cellHeight: 60 // Высота строки

            model: storageModel // Объект UnifiedStorageModel из C++
//            property real lastClickTime: 0
            // Настройка ScrollBar (Полоса прокрутки)
            ScrollBar.vertical: ScrollBar {
                active: true // Всегда видна при прокрутке
                policy: ScrollBar.AlwaysOn // Или AsNeeded
            }

            highlight: Rectangle { color: "gainsboro"; radius: 2} //; z:2 }
            highlightFollowsCurrentItem: true

            delegate: ItemDelegate {
//                width: listView.width
                //highlighted: root.currentSelectedPath === model.path
                width: gridView.cellWidth - 4
                height: gridView.cellHeight - 4
//                highlighted: GridView.isCurrentItem

                contentItem: RowLayout {
                    spacing: 10
                    Text {
                        text: model.isDir ? "📁" : "📄"
                        font.pixelSize: 18
                        Layout.alignment: Qt.AlignHCenter
                    }
                    Column {
                        //color:"blue"
                        Layout.fillWidth: true
                        Text {
                            width: parent.width
                            text: model.name; font.bold: true
                            // horizontalAlignment: Text.AlignHCenter
                            // verticalAlignment: Text.AlignLeft
                            elide: Text.ElideRight // Обрезаем длинные имена
                        }
                        Text {
                            text: model.isMinio ? "Облако MinIO" : "Локальный путь"
                            font.pixelSize: 10; color: "gray"
                        }
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        gridView.currentIndex = index
                    }
                    onDoubleClicked: {
                        acceptSelection()
                        event.accepted = true;
                    }
                }
                Keys.onPressed: (event) => {
                    if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
//                        console.log("Enter нажата!");
                        acceptSelection(); // Ваш метод подтверждения выбора
                        event.accepted = true; // Останавливаем дальнейшее распространение события
                    }
                }

                function acceptSelection() {
                    root.currentSelectedPath = model.path
                    if (model.isDir) {
                        if (model.isMinio) storageModel.enterMinio(model.name)
                        else storageModel.enterLocal(model.path)
                    } else {
                        console.log(currentSelectedPath);
                        root.pathSelected(currentSelectedPath);
                        root.close()
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            //Layout.alignment: Qt.AlignRight
            Button {
                id: btn_opn
                Layout.fillWidth: true
                Layout.preferredWidth: 1
                text: "Open"

                onClicked: {
                    root.pathSelected(currentSelectedPath);
                    root.close() }
                background: Rectangle {
                    anchors.fill: parent
                    color: parent.pressed ? "#f0f0f0" : "white"
                    radius: 10
                    border.color: "#d0d0d0"

                    // Ваш любимый эффект теперь будет работать без варнингов
                    layer.enabled: true
                    layer.effect: MultiEffect {
                        shadowEnabled: true
                        shadowOpacity: parent.pressed ? 0.2 : 0.4
                        shadowBlur: 0.5
                        shadowVerticalOffset: parent.pressed ? 1 : 3
                    }
                }
            }
            Button {
                Layout.fillWidth: true
                Layout.preferredWidth: 1
                text: "Close";
                background: Rectangle {
                    anchors.fill: parent
                    color: parent.pressed ? "#f0f0f0" : "white"
                    radius: 10
                    border.color: "#d0d0d0"

                    // Ваш любимый эффект теперь будет работать без варнингов
                    layer.enabled: true
                    layer.effect: MultiEffect {
                        shadowEnabled: true
                        shadowOpacity: parent.pressed ? 0.2 : 0.4
                        shadowBlur: 0.5
                        shadowVerticalOffset: parent.pressed ? 1 : 3
                    }
                }
                onClicked: {
                    root.close()
                }
            }
        }
    }
}

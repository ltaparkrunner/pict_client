import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Effects
import QtQuick.Controls.Basic

Window {
    id: root
    title: "Selecting an object (Locally / MinIO)"
    width: 500; height: 400
//    modal: false
//    standardButtons: Dialog.Cancel | Dialog.Open

    property string currentSelectedPath: "."
//    property string currentPath: "/"
    signal openPathSelected(string path)
    signal writePathSelected(list<string> listPath, string destPath)
    signal deletePathSelected(string path)
    property int lastSelectedTab: 0

    onVisibleChanged: {
        if (visible) {
            lastSelectedTab = 0
            tabBar.forceActiveFocus()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 2
        // TextField of selected path
        TextField {
            Layout.fillWidth: true
            text: root.currentSelectedPath
            placeholderText: "Nothing is selected"

            font.pixelSize: 15
            font.weight: parent.checked ? Font.DemiBold : Font.Normal
            color: parent.checked ? "#616161" : (parent.hovered ? "#555" : "#888")
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            background: Rectangle {
                implicitWidth: 200
                implicitHeight: 40
                color: "#f0f0f0" //parent.enabled ? "transparent" : "#353535"
                border.color: parent.activeFocus ? "#21be2b" : "#bdbebf"
                border.width: parent.activeFocus ? 2 : 1
                radius: 4
            }
            Keys.onPressed: (event) => {
                if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                    console.log("TextField Keys.onPressed Enter")
                    // gridView.forceActiveFocus()
                    // event.accepted = true
                }
            }
        }
        // Навигационная панель (Табы)
        TabBar {
            id: tabBar
            Layout.fillWidth: true
            currentIndex: root.lastSelectedTab
            focus: true
            focusPolicy: Qt.StrongFocus
            onActiveFocusChanged: {
                if (activeFocus && currentItem) {
                    currentItem.forceActiveFocus()
                }
            }
            Keys.onPressed: (event) => {
                if (event.key === Qt.Key_Left) {
                    console.log("TabBar Нажата стрелка Влево")
                }
                if (event.key === Qt.Key_Right) {
                    console.log("TabBar Нажата стрелка Вправо")
                }
            }
            spacing: 4
            TabButton {
                text: "Локально"
                id: tb_local
//                focus: true
                focusPolicy:Qt.ClickFocus
                onClicked: {
                    storageModel.enterLocal("/")
                    tb_local.forceActiveFocus()
                }
                onActiveFocusChanged: {
                    if (activeFocus) {
                        storageModel.enterLocal("/");
                        root.lastSelectedTab=0
                    }
                }
                Keys.onPressed: (event) => {
                    if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                        gridView.forceActiveFocus()
                        event.accepted = true
                    }
                    if (event.key === Qt.Key_Left) {
                        console.log("tb_local Нажата стрелка Влево")
                    }
                    if (event.key === Qt.Key_Right) {
                        console.log("tb_local Нажата стрелка Вправо")
                    }
                }
                KeyNavigation.right: tb_minio
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
                        visible: !tb_local.activeFocus
                    }
                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: -3 // Рамка чуть шире самой кнопки
                        color: tb_local.enabled ? "transparent" : "#353535"
                        border.color: tb_local.activeFocus ? "#21be2b" : "#bdbebf"
                        border.width: tb_local.activeFocus ? 2 : 1
                        radius: 4
                        visible: tb_local.activeFocus   //  tb_local.visualFocus
                    }
                    Behavior on color { ColorAnimation { duration: 200 } }
                }
            }
            TabButton {
                id: tb_minio
                text: "MinIO"
                focus: true
                focusPolicy:Qt.ClickFocus
                onClicked: {
                    storageModel.enterMinio("main-bucket")
                    tb_minio.forceActiveFocus()
                }
                onActiveFocusChanged: {
                    if (activeFocus) {
                        storageModel.enterMinio("main-bucket");
                        root.lastSelectedTab=1
                    }
                }
                Keys.onPressed: (event) => {
                    if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                        gridView.forceActiveFocus()
                        event.accepted = true
                    }
                    if (event.key === Qt.Key_Right) {
                        console.log("tb_minio Нажата стрелка Вправо")
                    }
                    if (event.key === Qt.Key_Left) {
                        console.log("tb_minio Нажата стрелка Влево")
                    }
                }
                KeyNavigation.left: tb_local
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
                        visible: !tb_minio.activeFocus
                    }
                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: -3 // Рамка чуть шире самой кнопки
                        // color: "transparent"
                        // border.color: "#2196F3"
                        // border.width: 2
                        // radius: 6
                        color: tb_minio.enabled ? "transparent" : "#353535"
                        border.color: tb_minio.activeFocus ? "#21be2b" : "#bdbebf"
                        border.width: tb_minio.activeFocus ? 2 : 1
                        radius: 4
                        visible: tb_minio.activeFocus // tb_minio.visualFocus       Виден только при фокусе
                    }
                    Behavior on color { ColorAnimation { duration: 200 } }
                }
            }
        }

        // Список файлов и папок
        GridView{
            id: gridView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true // clip what is it?
            focus: true
            flow: GridView.FlowTopToBottom

            cellWidth: 140  // Ширина колонки
            cellHeight: 60 // Высота строки

            model: storageModel // Объект UnifiedStorageModel из C++
            // Настройка ScrollBar (Полоса прокрутки)
            ScrollBar.vertical: ScrollBar {
                active: true // Всегда видна при прокрутке
                policy: ScrollBar.AlwaysOn // Или AsNeeded
            }

            highlightMoveDuration: 0
            highlight: Rectangle { color: "gainsboro"; radius: 2} //; z:2 }
            highlightFollowsCurrentItem: true

            delegate: ItemDelegate {
                width: gridView.cellWidth - 4
                height: gridView.cellHeight - 4

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
                        acceptSelection(gridView)
                        //mouse.accepted = true;
                    }
                }
                Keys.onPressed: (event) => {
                    if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                        acceptSelection(gridView); // Ваш метод подтверждения выбора
                        event.accepted = true; // Останавливаем дальнейшее распространение события
                    }
                }

                function acceptSelection(grid) {
                    root.currentSelectedPath = model.path
                    //root.
                    if (model.isDir) {
                        if (model.isMinio) storageModel.enterMinioBucket(model.name)
                        else storageModel.enterLocal(model.path)
                        //  console.log("acceptSelection: ", grid)
                        grid.currentIndex = 0
                        // parent.currentItem = 0
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
            Layout.margins: 5
            //Layout.alignment: Qt.AlignRight
            Button {
                id: btn_wrt
                Layout.fillWidth: true
                Layout.preferredWidth: 1
                text: "Write"
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
                onClicked: {
//                    var p = storageModel.data(0, path)
                    let ls = []
//                    console.log("storageModel.data(0, PathRole))", storageModel.data(0, storageModel.PathRole))
                    console.log("storageModel.get(0).path", storageModel.data(0, PathRole))
//                    ls.push(storageModel.data(0, PathRole))
                    ls.push(storageModel.data(0, PathRole))
                    console.log("after ls.append(storageModel.data(0, PathRole))")
                    root.writePathSelected(ls, currentSelectedPath);
                    root.close() }
                background: Rectangle {
                    anchors.fill: parent
                    color: parent.down ? "#f0f0f0" : (parent.hovered ? "#f8f8f8" : "#f0f0f0")   //"transparent")
                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: -3
                        // color: "transparent"
                        // border.color: "#2196F3"
                        // border.width: 2
                        // radius: 6
                        color: btn_wrt.enabled ? "transparent" : "#353535"
                        border.color: btn_wrt.activeFocus ? "#21be2b" : "#bdbebf"
                        border.width: btn_wrt.activeFocus ? 2 : 1
                        radius: 4
                        visible: btn_wrt.visualFocus // Виден только при фокусе
                    }
                }
            }
            Button {
                id: btn_dlt
                Layout.fillWidth: true
                Layout.preferredWidth: 1
                text: "Delete";
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
                background: Rectangle {
                    anchors.fill: parent
                    color: parent.down ? "#f0f0f0" : (parent.hovered ? "#f8f8f8" : "#f0f0f0")  //"transparent")
                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: -3 // Рамка чуть шире самой кнопки
                        color: btn_dlt.enabled ? "transparent" : "#353535"
                        border.color: btn_dlt.activeFocus ? "#21be2b" : "#bdbebf"
                        border.width: btn_dlt.activeFocus ? 2 : 1
                        radius: 4
                        visible: btn_dlt.visualFocus // Виден только при фокусе
                    }
                }
                onClicked: {
                    root.deletePathSelected(currentSelectedPath);
                    root.close()
                }
            }
            Button {
                id: btn_cls
                Layout.fillWidth: true
                Layout.preferredWidth: 1
                text: "Close";
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
                background: Rectangle {
                    anchors.fill: parent
                    color: parent.down ? "#f0f0f0" : (parent.hovered ? "#f8f8f8" : "#f0f0f0")  //"transparent")
                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: -3 // Рамка чуть шире самой кнопки
                        color: btn_cls.enabled ? "transparent" : "#353535"
                        border.color: btn_cls.activeFocus ? "#21be2b" : "#bdbebf"
                        border.width: btn_cls.activeFocus ? 2 : 1
                        radius: 4
                        visible: btn_cls.visualFocus // Виден только при фокусе
                    }
                }
                onClicked: {
                    root.close()
                }
            }
        }
    }
//    onActiveFocusItemChanged: console.log("Фокус сейчас на: " + activeFocusItem)
//    Component.on
}

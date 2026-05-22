import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Effects
import QtQuick.Controls.Basic
import pict_client

Window {
//Dialog {
    id: customFileDlg
    title: "Selecting an object (Locally / Network storage)"
    width: 800; height: 500
    flags: Qt.Dialog | Qt.WindowStaysOnTopHint
    modality: Qt.ApplicationModal
//    modal: false
//    standardButtons: Dialog.Cancel | Dialog.Open

    property string currentSelectedPath: "."
    property alias textFld: customFileDlg.currentSelectedPath
    // property string currentLocalPath: "/home"
    // property string currentNetworkPath: "http://minio:9000/"
//    property string currentPath: "/"

//    signal openPathsSelected(list<string> path)
    property alias currentTabIndex: tabBar.currentIndex
    signal openIndicesSelected(list<int> inds)
    signal openIndexSelected(int index)
    //signal tabSelected(int tabIndex)
    property int lastSelectedTab: 0
    property var selectedIndices: []
//    property var selectedPaths: []

    onVisibleChanged: {
        if(visible) {
            lastSelectedTab = 0
            tabBar.forceActiveFocus()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 2
        // TextField of selected path
        TextField {
            id: textf
            Layout.fillWidth: true
            text: customFileDlg.currentSelectedPath
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
            // currentIndex: customFileDlg.lastSelectedTab
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
                text: "Local"
                id: tb_local
//                focus: true
                focusPolicy:Qt.ClickFocus
                onClicked: {
                    console.log("currentLocalPath: ", currentLocalPath)
                    storageModel.enterLocal(currentLocalPath)   // TODO:
                    tb_local.forceActiveFocus()
                }
                onActiveFocusChanged: {
                    if (activeFocus) {
                        //  storageModel.enterLocal("/"); TODO: right to write
                        customFileDlg.lastSelectedTab=0
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
                KeyNavigation.right: nw_storage
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
                id: nw_storage
                text: "Network storage"
                focus: true
                focusPolicy:Qt.ClickFocus
                onClicked: {
                    storageModel.enterNetStore("main-bucket")
                    nw_storage.forceActiveFocus()
                }
                onActiveFocusChanged: {
                    // if (activeFocus) {
                    //     storageModel.enterNetStore("main-bucket");
                    //     customFileDlg.lastSelectedTab=1
                    // }
                }
                Keys.onPressed: (event) => {
                    if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                        gridView.forceActiveFocus()
                        event.accepted = true
                    }
                    if (event.key === Qt.Key_Right) {
                        console.log("nw_storage Нажата стрелка Вправо")
                    }
                    if (event.key === Qt.Key_Left) {
                        console.log("nw_storage Нажата стрелка Влево")
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
                        visible: !nw_storage.activeFocus
                    }
                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: -3 // Рамка чуть шире самой кнопки
                        // color: "transparent"
                        // border.color: "#2196F3"
                        // border.width: 2
                        // radius: 6
                        color: nw_storage.enabled ? "transparent" : "#353535"
                        border.color: nw_storage.activeFocus ? "#21be2b" : "#bdbebf"
                        border.width: nw_storage.activeFocus ? 2 : 1
                        radius: 4
                        visible: nw_storage.activeFocus // nw_storage.visualFocus       Виден только при фокусе
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
            //height: cellHeight
            clip: true // clip what is it?
            focus: true
            flow: GridView.FlowTopToBottom
            //flow: GridView.FlowLeftToRight
            cellWidth: 140  // Ширина колонки
            cellHeight: 60 // Высота строки

            model: storageModel // Объект UnifiedStorageModel из C++
            // Настройка ScrollBar (Полоса прокрутки)
            ScrollBar.horizontal: ScrollBar {
                id: horizontalScrollBar
            //ScrollBar.vertical: ScrollBar {
                active: true // Всегда видна при прокрутке
                policy: ScrollBar.AsNeeded  //AlwaysOn // Или AsNeeded
                implicitHeight: 14
                visible: size < 1.0
                contentItem: Rectangle {
                    // visible: ScrollBar.AsNeeded
                    implicitWidth: 100
                    implicitHeight: 12
                    radius: 6
                    //  Делаем цвет ярче, когда на скроллбар наводят мышь или тянут его
                    color: horizontalScrollBar.pressed ? "#1177BB" :
                           horizontalScrollBar.hovered ? "#2299EE" : "#888888"
                    //  Плавное изменение цвета при наведении
                    Behavior on color { ColorAnimation { duration: 150 } }
                }
            }

            highlightMoveDuration: 0
            highlight: Rectangle { color: "gainsboro"; radius: 2} //; z:2 }
            highlightFollowsCurrentItem: true

            delegate: ItemDelegate {
                width: gridView.cellWidth - 4
                height: gridView.cellHeight - 4
                background: Rectangle {
                    anchors.fill: parent
                    border.color: selectedIndices.indexOf(index) !== -1 ? "blue" : "transparent"
                }
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
//                    border.color: selectedIndices.indexOf(index) !== -1 ? "blue" : "transparent"
                MouseArea {
                    anchors.fill: parent
                    onClicked: (mouse) => {
                       if (mouse.modifiers & Qt.ShiftModifier && selectedIndices.length > 0) {
                           // Выбор диапазона от последнего выбранного до текущего
                           let start = selectedIndices[selectedIndices.length - 1];
                           let end = index;
                           let range = [];
                           for (let i = Math.min(start, end); i <= Math.max(start, end); i++) {
                               range.push(i);
                           }
                           selectedIndices = range;
                       } else
                        if (mouse.modifiers & Qt.ControlModifier) {
                            // Режим мультивыбора (Ctrl)
                            let temp = selectedIndices;
                            let foundAt = temp.indexOf(index);

                            if (foundAt !== -1) {
                                temp.splice(foundAt, 1); // Убрать из выбора, если уже там
                            } else {
                                temp.push(index); // Добавить в выбор
                            }
                            selectedIndices = temp; // Обновляем массив для срабатывания Binding
                        } else {
                            // Одиночный выбор (без Ctrl)
                            selectedIndices = [index];
                        }
                        // Устанавливаем currentIndex для GridView (визуальный фокус)
                        gridView.currentIndex = index;
                    }
                    onDoubleClicked: {
                        //acceptSelection(gridView)
                        customFileDlg.selectedIndices = []
                        acceptSelectionEnterFolder(index)
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
                    customFileDlg.currentSelectedPath = model.path
                    //customFileDlg.
                    if (model.isDir) {
                        if (model.isMinio) storageModel.enterMinioBucket(model.name)
                        else {
                            console.log("model.path: ", model.path)
                            storageModel.enterLocal(model.path)
                        }
                        //  console.log("acceptSelection: ", grid)
                        grid.currentIndex = 0
                        // parent.currentItem = 0
                    } else {
                        console.log(currentSelectedPath);
                        //customFileDlg.openPathsSelected([currentSelectedPath]);
                        customFileDlg.openIndicesSelected([index])
                        rootWnd.currentCustomDlgTb = tabBar.currentIndex
                        customFileDlg.selectedIndices = []
                        customFileDlg.close()
                    }
                }
                function acceptSelectionEnterFolder(index) {
                    if(!model.isMinio && model.isDir) {
                        rootWnd.currentLocalPath = model.path;
                        console.log("rootWnd.currentLocalPath = ",rootWnd.currentLocalPath);
                    }
                    customFileDlg.currentSelectedPath = model.path
                    //customFileDlg.
                    if (model.isDir) {
                        console.log("SecondCustomFileDialog function acceptSelection(index) model.isDir ")
                        storageModel.enterFolder(index)
                        GridView.view.currentIndex = 0;  // TODO:
                    } else {
                        //  console.log(currentSelectedPath);
                        customFileDlg.openIndexSelected(index)
                        rootWnd.currentCustomDlgTb = tabBar.currentIndex
                        customFileDlg.selectedIndices = []
                        customFileDlg.close()
                    }
                }
            }
            Item {
                anchors.fill: parent
                visible: ((wsClient.authConnectionState !== WebSocketClient.Authorized) && (nw_storage.activeFocus))
                Text {
                    anchors.centerIn: parent
                    text: "No network connection."
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.margins: 5
            //Layout.alignment: Qt.AlignRight
            Button {
                id: btn_opn
                Layout.fillWidth: true
                Layout.preferredWidth: 1
                text: "Open"
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
                    if(selectedIndices){
                        console.log("selectedIndices", selectedIndices)
                        // var selectedPaths = []
                        // for(let index of selectedIndices) {
                        //     console.log("storageModel.get(index).path", storageModel.get(index).path)
                        //     selectedPaths.push(storageModel.get(index).path)
                        // }
                        //  customFileDlg.openPathsSelected(selectedPaths);
                        customFileDlg.openIndicesSelected(selectedIndices);
                    }
                    else //customFileDlg.openPathsSelected([currentSelectedPath]);
                        customFileDlg.openIndicesSelected([selectedIndices]);
                    //customFileDlg.tabSelected(tabBar.currentIndex)
                    rootWnd.currentCustomDlgTb = tabBar.currentIndex
                    customFileDlg.selectedIndices = []
                    customFileDlg.close() }
                background: Rectangle {
                    anchors.fill: parent
                    color: parent.down ? "#f0f0f0" : (parent.hovered ? "#f8f8f8" : "#f0f0f0")   //"transparent")
                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: -3 // Рамка чуть шире самой кнопки

                        color: btn_opn.enabled ? "transparent" : "#353535"
                        border.color: btn_opn.activeFocus ? "#21be2b" : "#bdbebf"
                        border.width: btn_opn.activeFocus ? 2 : 1
                        radius: 4
                        visible: btn_opn.visualFocus // Виден только при фокусе
                    }
                }
            }
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
                    let li = []
                    //  console.log("gridView2.model.length: ", gridView2.model.length, "gridView2.model.rowCount()", gridView2.model.rowCount())
                    //  console.log("imageModel.rowCount(): ", imageModel.rowCount(), " ", data.cleanPath, " ")
                    for(let i=0; i<imageModel.rowCount(); i++){
                        var data = imageModel.get(i);
                        console.log("imageModel.get(",i ,")= ", data.cleanPath, " ", data.mongoId);
                        li.push( {"path":data.cleanPath, "mongoId":data.mongoId});
                    }
                    console.log("btn_wrt onClicked: ", li[0], "  ", currentSelectedPath)
                    root.writeImages(li, currentSelectedPath);
                    root.close()
                }
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
                    console.log("delete onClicked")
                    root.deletePathsSelected(selectedIndices);
                    console.log("delete indicesToPaths(selectedIndices)", selectedIndices)
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
                    //customFileDlg.tabSelected(tabBar.currentIndex)
                    rootWnd.currentCustomDlgTb = tabBar.currentIndex
                    customFileDlg.selectedIndices = []
                    customFileDlg.close()
                }
            }
        }

    }
//    onActiveFocusItemChanged: console.log("Фокус сейчас на: " + activeFocusItem)
//    Component.on
}

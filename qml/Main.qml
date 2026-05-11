import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1
import com.myapp.helpers 1.0
import QtQuick.Controls.Basic
import QtQuick.Dialogs
import QtQuick.Effects
import QtQuick.Shapes

ApplicationWindow {
    id: rootWnd
    visible: true
    width: 1000
    height: 600
    minimumWidth: 400
    minimumHeight: 300
    title: "Fixed Left, Flexible Right"

    Connections {
        target: imageModel
        function onMinioImageToQML(url) {
            mainImageSource = url;
            //console.log("Получено из C++:", url)
        }
    }
    Connections {
        target: wsClient
        function onShowLoginRequired(){
            console.log("function onShowLoginRequired()")
            loginWarn.open()
        }
    }

    WarnDialog{
        id: loginWarn
    }

    CustomFileDialog {
        id: customDialog
        onOpenPathsSelected:(paths) => {
            if(paths){
                tf.content = paths[0]
                for(var path of paths)
                    processPath(path)
            }
        }
    }

    SecondCustomFileDialog {
        id: secondCustomDialog
        // onOpenPathsSelected:(paths) => {
        //     tf.content = currentSelectedPath
        //     processPath(currentSelectedPath)
        // }

        onOpenIndexSelected:(index) => {
            console.log("onOpenIndexSelected: ", index, " rows: ", storageModel.rowCount())
            if(index>=0 && index<storageModel.rowCount()){
                let img = storageModel.get(index);
                let imgPath = img.path;
                let prefix = "file:///";
                if(!img.isMinio && !imgPath.startsWith(prefix)){
                    mainImageSource = prefix + imgPath
                }
                else mainImageSource = imgPath
                //  console.log("onOpenIndexSelected: ", imgPath);
                let data = storageModel.getData(index);
                //  console.log("onOpenIndexSelected: ", index);
                imageModel.insertImage(data);
            }
            else console.log("Путь не распознан или не существует 1");
        }
        // root.writePathSelected(ls, currentSelectedPath)
        onWritePathsSelected:(ls, paths) => {
            //  console.log("onWritePathsSelected paths: ", paths)
            storageModel.writeToFolder(ls)
        }

        onDeletePathsSelected:(indices) => {
            if(indices){
                console.log("onDeletePathsSelected:(paths)", indices[0])
                storageModel.deleteIndices(indices)
            }
            else {
                msgNothingToDo.text = "You must select a folder to save the images."
                msgNothingToDo.open()
            }
            tf.content = currentSelectedPath
            processPath(currentSelectedPath)
        }
        // Check if the files are in the target folder
        function ifFilesInFolder(ls, path){
            for(const [index, filePath] of ls.entries()){
                const dir = filePath.substring(0, filePath.lastIndexOf("/"));
                // console.log("dir = ", dir, "path = ", path)
                if(dir === path) ls.splice(index, 1);
            }
        }
    }

    UserLogin {
        id: userLogin
    }

    // LoginDialog {
    //     id: userLogin
    // }

    MessageDialog {
        id: msgNothingToDo
        title: "Nothing To Do"  // "Подтверждение"
        text: "Path is empty, or smth like this"    // "Вы уверены, что хотите удалить этот файл?"
        informativeText: "Nothing To Do"     //"Это действие нельзя будет отменить."
        buttons: MessageDialog.Ok | MessageDialog.Cancel

        onAccepted: close()     //console.log("Нажата кнопка ОК")
        onRejected: close()     //console.log("Нажата кнопка Отмена")
    }

    MessageDialog {
        id: plug
        title: "Plug"  // "Подтверждение"
        text: "Code is not ready"    // "Вы уверены, что хотите удалить этот файл?"
        informativeText: "It'll be written"     //"Это действие нельзя будет отменить."
        buttons: MessageDialog.Ok | MessageDialog.Cancel

        onAccepted: close()     //console.log("Нажата кнопка ОК")
        onRejected: close()     //console.log("Нажата кнопка Отмена")
    }

    property string mainImageSource: ""
    property list<string> myImages: ["", "", "", "", "", ""]
    property string largeImgPath: ""
    // Основной горизонтальный контейнер
    ColumnLayout{
        anchors.fill: parent
        ColumnLayout {
            Layout.fillHeight: true
            width: parent.width
            RowLayout{
                TextField {
                    id: tf
                    property string content: ""
                    placeholderText: "Open/Write file/folder"
                    Layout.fillWidth: true
                    Layout.preferredWidth: 4
                    //  text: content
                    text: content
                    background: Rectangle {
                        implicitWidth: 200
                        implicitHeight: 40
                        color: tf.enabled ? "transparent" : "#353535"
                        border.color: tf.activeFocus ? "#21be2b" : "#bdbebf"
                        border.width: tf.activeFocus ? 2 : 1
                        radius: 4
                    }
                }
                AbstractButton {
                    id: loginButton
                    Layout.preferredWidth: 25
                    Layout.preferredHeight: 25
                    Item {
                        id: userImageCliped
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        width: 25
                        height: 25

                        Image {
                            id: userImage
                            anchors.fill: parent
                            source: getCurrentUserImage()
                            visible: false

                            function getCurrentUserImage() {
                                //if (!rootWnd.loginService.loggedIn)
                                    return "../icons/user.svg";
                                // for (let i = 0; i < users.data.length; i++) {
                                //     if (users.data[i].email === loginService.user)
                                //         return users.data[i].avatar;
                                // }
                            }
                        }

                        Image {
                            id: userMask
                            source: "../icons/userMask.svg"
                            anchors.fill: userImage
                            anchors.margins: 4
                            visible: false
                        }

                        MultiEffect {
                            source: userImage
                            anchors.fill: userImage
                            maskSource: userMask
                            maskEnabled: true
                        }
                    }

                    onClicked: {
                        userLogin.open()
                        // var pos = mapToGlobal(Qt.point(x, y))
                        // pos = userMenu.parent.mapFromGlobal(pos)
                        // userMenu.x = x - userMenu.width + 25 + 3
                        // userMenu.y = y + 25 + 3
                    }

                    Shape {
                       id: bubble
                       x: -text.width - 25
                       anchors.margins: 3

                       preferredRendererType: Shape.CurveRenderer

                       // visible: !rootWnd.loginService.loggedIn

                       ShapePath {
                           strokeWidth: 0
                           fillColor: "#667085"
                           startX: 5; startY: 0
                           PathLine { x: 5 + text.width + 6; y: 0 }
                           PathArc { x: 10 + text.width + 6; y: 5; radiusX: 5; radiusY: 5}
                           // arrow
                           PathLine { x: 10 + text.width + 6; y: 8 + text.height / 2 - 6 }
                           PathLine { x: 10 + text.width + 6 + 6; y: 8 + text.height / 2 }
                           PathLine { x: 10 + text.width + 6; y: 8 + text.height / 2 + 6}
                           PathLine { x: 10 + text.width + 6; y: 5 + text.height + 6 }
                           // end arrow
                           PathArc { x: 5 + text.width + 6; y: 10 + text.height + 6 ; radiusX: 5; radiusY: 5}
                           PathLine { x: 5; y: 10 + text.height + 6 }
                           PathArc { x: 0; y: 5 + text.height + 6 ; radiusX: 5; radiusY: 5}
                           PathLine { x: 0; y: 5 }
                           PathArc { x: 5; y: 0 ; radiusX: 5; radiusY: 5}
                       }
                       Text {
                           x: 8
                           y: 8
                           id: text
                           color: "white"
                           text: qsTr("Log in to edit")
                           font.bold: true
                           horizontalAlignment: Qt.AlignHCenter
                           verticalAlignment: Qt.AlignVCenter
                       }
                   }
                }
            }
            RowLayout{
                Button {
                    text: "Open"
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    onClicked: {
                        if(tf.text && tf.text.trim().length > 0) {
                            let type = FileHelper.checkPathType(tf.text);
                            if (type === FileHelperType.LocalFile) {
                                console.log("Это локальный файл");
                            } else if (type === FileHelperType.LocalFolder) {
                                console.log("Это локальная папка");
                            } else if (type === FileHelperType.MinioBucket) {
                                console.log("Это бакет MinIO");
                            } else if (type === FileHelperType.MinioFile) {
                                console.log("Это объект (файл) в MinIO");
                            } else {
                                console.log("Путь не распознан или не существует");
                            }
                        }
                    }
                }
                Button {
                    text: "Write"
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    onClicked: {
                        FileHelper.deleteMinioBuckets(["pictures"])
                    }
                }
                Button {
                    text: "Open file/folder"
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    onClicked: {
                        wsClient.connectToServer()
                        storageModel.enterLocal("/"); // TODO:??
                        customDialog.show();
                    }
                }
                // Button {
                //     text: "Write file/folder"
                //     Layout.fillWidth: true
                //     Layout.preferredWidth: 1
                //     onClicked: {
                //         wsClient.connectToServer()
                //         storageModel.enterLocal("/");
                //         secondCustomDialog.show();
                //     }
                // }
            }
        }

        RowLayout {
           Layout.fillHeight: true
           width: parent.width
            // ЛЕВАЯ ЧАСТЬ (Поля ввода и сетка)
            ColumnLayout {
                Layout.fillHeight: true
                Layout.preferredWidth: (parent.width - 330 < 300) ? parent.width / 2 : 330
                spacing: 5//10
                Item{
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    GridView {
                        property string lastDoubleClickedPath: ""
                        id: imageGrid
                        anchors.fill: parent
                        anchors.margins: 10
                        clip: true

                        cellWidth: 100
                        cellHeight: 100

                        model: imageModel
//                        delegate: Item {
                        delegate: ItemDelegate {
                            width: imageGrid.cellWidth
                            height: imageGrid.cellHeight
                            id: imageDelegate
                            z: mouseArea.containsMouse ? 100 : 1
                            readonly property GridView parentView: GridView.view
                            readonly property int limit1: 40
                            //  focusPolicy: Qt.ClickFocus
                            Rectangle {
                                anchors.fill: parent
                                anchors.margins: 5 // Отступы между картинками
                                color: "white"
                                border.color: "silver"
                                radius: 4

                                Image {
                                    anchors.fill: parent
                                    anchors.margins: 2
                                    source: model.path // Данные из C++
                                    fillMode: Image.PreserveAspectCrop // Чтобы не искажать пропорции
                                    clip: true
                                }
                            }
                            MouseArea {
                                id: mouseArea
                                anchors.fill: parent
                                hoverEnabled: true
                                acceptedButtons: Qt.LeftButton | Qt.RightButton
                                onDoubleClicked: {
                                    let img = imageModel.get(index)
                                    mainImageSource = img.path
                                    tf.content = img.cleanPath
                                }
                                onClicked: (mouse) => {
                                    if (mouse.button === Qt.RightButton) {
                                        contextMenu.popup() // Открываем меню
                                    } else {
                                        //  dlgt.parentView.currentIndex = index;
                                        console.log("imageDelegate.y: ", imageDelegate.y,  "  imageDelegate.height: ", imageDelegate.height , "  imageGrid.height: ", imageGrid.height)
                                    }
                                }
                                onEntered: {
                                    floatingInfo.targetItem = imageDelegate;
                                    infoText.text = model.cleanPath; // Передаем путь из модели
                                    floatingInfo.updatePosition();
                                }

                                onExited: {
                                    floatingInfo.visible = false;
                                    floatingInfo.targetItem = null;
                                }
                            }
                            Menu {
                                id: contextMenu
                                MenuItem {
                                    text: "Скопировать путь"
                                    onTriggered: {
                                        let img = imageModel.get(index)
                                        imageModel.copyToClipboard(model.cleanPath)
                                        console.log("Путь скопирован: " + model.cleanPath)}
                                }
                                MenuItem {
                                    text: "Удалить"
                                    onTriggered: { imageModel.removeItem(index) }
                                }
                            }
                            function shortenPath(path, limit) {
                                if (path.length <= limit) return path;
                                let partSize = Math.floor(limit / 2) - 2;
                                return path.substring(0, partSize) + "..." + path.substring(path.length - partSize);
                            }
                        }
                        ScrollBar.vertical: ScrollBar {}
                        // onActiveFocusChanged: if (!activeFocus) tf.content = mainImageSource
                    }
                    Rectangle {
                        id: floatingInfo
                        width: imageGrid.width
                        //height: Math.min(infoText.implicitHeight + 10, infoText.lineHeight * 3 + 10)
                        height: infoText.implicitHeight + 10
                        color: "#E6000000"
                        visible: false
                        z: 999 // Всегда поверх всего

                        property var targetItem: null // Ссылка на делегат, над которым мышь

                        Text {
                            id: infoText
                            anchors.fill: parent; anchors.margins: 5
                            color: "white"; wrapMode: Text.WrapAnywhere
                            maximumLineCount: 3; elide: Text.ElideMiddle
                        }

                        // Функция динамического пересчета позиции
                        function updatePosition() {
                            if (!targetItem) return;

                            // Получаем глобальные координаты делегата относительно окна/сетки
                            let pos = targetItem.mapToItem(imageGrid, 0, 0);

                            // Решаем: сверху или снизу
                            let spaceBelow = imageGrid.height - (pos.y + targetItem.height);
                            if (spaceBelow < height + 40) {
                                y = pos.y - height + 10; // Показываем НАД
                            } else {
                                y = pos.y + targetItem.height + 10; // Показываем ПОД
                            }

                            visible = true;
                        }
                    }
                }
            }

            // ПРАВАЯ ЧАСТЬ (Большое окно изображения)
            ColumnLayout{
                Layout.preferredWidth: (parent.width - 330 < 300) ? parent.width / 2 : parent.width - 330
                Rectangle {
                    id: largeImgRect
                    property string savedString: ""
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.preferredWidth: (parent.width - 330 < 300) ? parent.width / 2 : parent.width - 330

                    color: mainImageSource === "" ? "#808080" : "#222"
                    border.color: "#222"    //"#333"

                    Image {
                        id: largeImg
                        anchors.fill: parent
                        source: mainImageSource //"https://placeholder.com" // Замените на свое фото
                        fillMode: Image.PreserveAspectFit

                        // Плавное появление картинки
                        opacity: status === Image.Ready ? 1 : 0
                        Behavior on opacity { NumberAnimation { duration: 500 } }
                    }

                    Text {
                        anchors.bottom: parent.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "No Image Loaded"//"Large Image Preview"
                        color: "white"
                        font.pixelSize: 18
                        visible: mainImageSource === ""
                    }
                    // MouseArea {
                    //     anchors.fill: parent
                    //     hoverEnabled: true
                    //     onEntered: {  largeImgRect.savedString = tf.content
                    //         tf.content = mainImageSource
                    //     }
                    // }
                }
            }
        }
    }

//        folder: StandardPaths.writableLocation(StandardPaths.PicturesLocation)

    function processPath(path){
        if(path) {
            let type = FileHelper.checkPathType(path);
            if (type === FileHelperType.LocalFile) {
                console.log("before imageGrid.model.addFilePath(path)", path)
                imageGrid.model.addImagePath(path)
                console.log("after imageGrid.model.addFilePath(path)")
                mainImageSource = imageGrid.model.resolvePath(path)
                console.log("after mainImageSource = imageGrid.model.resolvePath(path)", path)
            } else if (type === FileHelperType.LocalFolder) {
//                console.log("Это локальная папка");
                mainImageSource = imageGrid.model.addFilesFromFolder(path)
            } else if (type === FileHelperType.MinioBucket) {
//                console.log("Это бакет MinIO");
                mainImageSource = imageGrid.model.addImagesFromMinioBucket(path)
            } else if (type === FileHelperType.MinioFile) {
                console.log("Это объект (файл) в MinIO", path);
                mainImageSource = imageGrid.model.addMinioImagePath(path)
            } else {
                console.log("Путь не распознан или не существует");
            }
        }
    }

    function processWritePaths(ls, paths) {
        if(paths) {
            let type = FileHelper.checkPathType(path);
            if (type === FileHelperType.LocalFile) {
                imageGrid.model.addImagePath(path)
                mainImageSource = imageGrid.model.resolvePath(path)
            } else if (type === FileHelperType.LocalFolder) {
                mainImageSource = imageGrid.model.addFilesFromFolder(path)
            } else if (type === FileHelperType.MinioBucket) {
//                console.log("Это бакет MinIO");
                mainImageSource = imageGrid.model.addImagesFromMinioBucket(path)
            } else if (type === FileHelperType.MinioFile) {
                console.log("Это объект (файл) в MinIO", path);
                mainImageSource = imageGrid.model.addMinioImagePath(path)
            } else {
                console.log("Путь не распознан или не существует");
            }
        }
    }
    function processDeletePath(currentSelectedPath) {
        if(path) {
            let type = FileHelper.checkPathType(path);
            if (type === FileHelperType.LocalFile) {
                imageGrid.model.addImagePath(path)
                mainImageSource = imageGrid.model.resolvePath(path)
            } else if (type === FileHelperType.LocalFolder) {
                mainImageSource = imageGrid.model.addFilesFromFolder(path)
            } else if (type === FileHelperType.MinioBucket) {
                mainImageSource = imageGrid.model.addImagesFromMinioBucket(path)
            } else if (type === FileHelperType.MinioFile) {
                console.log("Это объект (файл) в MinIO", path);
                mainImageSource = imageGrid.model.addMinioImagePath(path)
            } else {
                console.log("Путь не распознан или не существует");
            }
        }
    }
}

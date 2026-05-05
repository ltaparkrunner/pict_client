import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1
import com.myapp.helpers 1.0
import QtQuick.Controls.Basic
import QtQuick.Dialogs

ApplicationWindow {
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
                console.log("onOpenIndexSelected: ", imgPath);
                // mainImageSource = "file:///C:/wrk/Qt_projs/pict_client/assets/pictures/img23.jpg"
                let data = storageModel.getData(index);
                console.log("onOpenIndexSelected: ", index);
                imageModel.insertImage(data);
            }
            else console.log("Путь не распознан или не существует 1");
        }
        // root.writePathSelected(ls, currentSelectedPath)
        onWritePathsSelected:(ls, paths) => {
            console.log("onWritePathsSelected paths: ", paths)
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
            }
            RowLayout{
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
                Button {
                    text: "Write file/folder"
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    onClicked: {
                        wsClient.connectToServer()
                        storageModel.enterLocal("/");
                        secondCustomDialog.show();
                    }
                }
            }
        }

        RowLayout {
           Layout.fillHeight: true
           width: parent.width
//           Layout.fillWidth: parent.width
//            anchors.fill: parent
            // anchors.margins: 5//10
            // spacing: 5//20

            // ЛЕВАЯ ЧАСТЬ (Поля ввода и сетка)
            ColumnLayout {
                // id: cl1
                // Рассчитываем ширину: 2 колонки по 150px + отступы (например, 10px spacing + 20px margins)
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
                        // MouseArea {
                        //     anchors.fill: parent
                        //     acceptedButtons:Qt.NoButton
                        //     hoverEnabled: true
                        //     onExited: tf.content = "forever young"
                        // }
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
                                    //  console.log("delegate onDoubleClicked: ", index, "  ", JSON.stringify(img))
                                    mainImageSource = img.path
                                    tf.content = img.cleanPath
                                    // tf.content = mainImageSource
                                    //  imageGrid.lastDoubleClickedPath = img.path
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
                            // Rectangle {
                            //     id: infoPanel
                            //     width: imageGrid.width
                            //     height: infoText.implicitHeight + 10
                            //     readonly property bool isTooCloseToBottom: {
                            //         // Получаем Y-координату нижнего края ячейки относительно ГРАНИЦ imageGrid
                            //         let mappedY = imageDelegate.mapToItem(imageGrid, 0, imageDelegate.height).y;
                            //         console.log("mappedY: ", mappedY, "imageGrid.height", imageGrid.height)
                            //         // Если от нижнего края ячейки до края окна меньше 100 пикселей
                            //         return (mappedY + 100) > imageGrid.height;
                            //     }
                            //     // readonly property bool isBottomRow: (imageDelegate.y + imageDelegate.height + 10 >= imageGrid.height)
                            //     // {
                            //     //     console.log("infoText.implicitHeight: ", infoText.implicitHeight,
                            //     //         "infoText.lineHeight: ", infoText.lineHeight, "infoText linecount", infoText.lineCount)
                            //     //     return  Math.min(infoText.implicitHeight + 10, infoText.lineHeight * 3 + 10)
                            //     // }

                            //     color: "#E6000000" // Темный полупрозрачный
                            //     visible: mouseArea.containsMouse

                            //     // Перемещаем панель так, чтобы она всегда начиналась от левого края GridView
                            //     x: -imageDelegate.x

                            //     // ЛОГИКА ПОЛОЖЕНИЯ (Сверху или Снизу):
                            //     // // Если под элементом меньше 80 пикселей — показываем над элементом
                            //     // anchors.bottom: (imageDelegate.y + imageDelegate.height + 80 > imageGrid.height) ? parent.top : undefined
                            //     // anchors.top: (anchors.bottom === undefined) ? parent.bottom : undefined


                            //     // anchors.top:parent.bottom
                            //     // anchors.bottom:(imageDelegate.y + imageDelegate.height + 10 >= imageGrid.height) ? parent.top : undefined
                            //     // //anchors.top:(imageDelegate.y + imageDelegate.height + 10 < imageGrid.height) ? parent.bottom : undefined
                            //     // anchors.top: (anchors.bottom === undefined) ? parent.bottom : undefined
                            //     anchors.top: isTooCloseToBottom ? undefined : parent.bottom
                            //     anchors.bottom: isTooCloseToBottom ? parent.top : undefined


                            //     Text {
                            //         id: infoText
                            //         anchors.fill: parent
                            //         anchors.margins: 5
                            //         color: "white"
                            //         font.pixelSize: 12

                            //         text: model.cleanPath
                            //         // Настройки текста:
                            //         wrapMode: Text.WrapAnywhere
                            //         maximumLineCount: 3        // Максимум 3 строки
                            //         elide: Text.ElideMiddle    // Сокращаем середину, если не влезло в 3 строки
                            //         horizontalAlignment: Text.AlignLeft
                            //         verticalAlignment: Text.AlignVCenter
                            //     }
                            //     Connections {
                            //         target: imageGrid
                            //         function onHeightChanged() {
                            //             // Это заставит QML перепроверить условие isTooCloseToBottom
                            //             infoPanel.visible = infoPanel.visible
                            //         }
                            //     }
                            // }
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
                        height: Math.min(infoText.implicitHeight + 10, infoText.lineHeight * 3 + 10)
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
                            if (spaceBelow < height + 20) {
                                y = pos.y - height; // Показываем НАД
                            } else {
                                y = pos.y + targetItem.height; // Показываем ПОД
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

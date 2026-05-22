import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1
import com.myapp.helpers 1.0
import QtQuick.Controls.Basic
import QtQuick.Dialogs
import QtQuick.Effects
import QtQuick.Shapes
import QtCore
import pict_client //1.0

ApplicationWindow {
    id: rootWnd
    visible: true
    width: 1000
    height: 600
    minimumWidth: 400
    minimumHeight: 300
    title: "Fixed Left, Flexible Right"

    property string currentLocalPath: Qt.platform.os === "windows" ? "C:/Users" : "/home"
    property string currentNetworkPath: "http://minio:9000/"
    property int currentCustomDlgTb: 0

    Settings {
        category: "General"
        // Связываем свойство QSettings со свойством окна
        property alias currentLocalPath: rootWnd.currentLocalPath
    }

    Connections {
        target: imageModel
        function onMinioImageToQML(url) {
            mainImageSource = url;
            //  console.log("Получено из C++:", url)
        }
    }
    // Connections {
    //     target: wsClient
    //     function onDisconnected(){
    //         //  console.log("function onShowLoginRequired()")
    //         loginWarn.open()
    //     }
    // }

    // Connections {
    //     target: authHandler
    //     function onSuccAuth(errMsg){

    //     }
    //     // function onLogoff(msg){
    //     // }
    // }

    WarnDialog{
        clientBackend: wsClient
        id: loginWarn
    }

    CustomFileDialog {
        id: customDialog
        onOpenIndexSelected:(index) => {
            //  console.log("onOpenIndexSelected: ", index, " rows: ", storageModel.rowCount())
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
        onOpenIndicesSelected:(indices) => {
            let maxindx = storageModel.rowCount()
            let arr = []
            let succ = 0
            let dir = 0
            let dirToGo
            for(let indx of indices) {
                console.log("onOpenIndicesSelected indx: ", indx)
                if(indx>=0 && indx < maxindx){
                    let img = storageModel.get(indx);
                    if(!img.isDir && !img.isMinioBucket && !img.VirtualDir) {
                        if(!succ){
                            let imgPath = img.path;
                            let prefix = "file:///";
                            if(!img.isMinio && !imgPath.startsWith(prefix)){
                                mainImageSource = prefix + imgPath
                            }
                            else mainImageSource = imgPath
                            succ = 1
                        }
                        let data = storageModel.getData(indx)
                        arr.push(data)
                    }
                    else if((dir===0) && (img.isDir ||  img.isMinioBucket)){
                        dirToGo = {indx:indx, isDir:img.isDir, isMinio:img.isMinio, isBucket:img.isMinioBucket}; dir = 1;
                    }
                }
            }
            imageModel.insertImages(arr);
            if(dirToGo.isDir && !dirToGo.isMinio)   {
                    console.log("storageModel.enterLocal(dirToGo.indx", dirToGo.indx)
                    storageModel.enterLocal(dirToGo.indx)
                }
            else if(dirToGo.isBucket)     storageModel.enterMinioBucket(dirToGo.indx)
            else if(dirToGo.isDir && dirToGo.isMinio && !dirToGo.isBucket)  storageModel.enterNetStore(dirToGo.indx)
        }
    }

    NetworkDialog {
        id: nwDialog
        // Связываем свойство диалога напрямую с C++ классом
        // Предварительно зарегистрировав MyClient в контексте QML под именем myClient
        isNetworkAvailable: wsClient.authConnectionState //isConnected
        //  clientBackend: wsClient

        onAccepted: {
            // При нажатии "Повторить" инициируем новое подключение
            wsClient.connectToServer()
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

        onWriteImages: (lf, path) => {
            //  console.log("onWritePathsSelected paths: ", path)
            storageModel.writeImagesToFolder(lf, path);
        }

        onDeletePathsSelected:(indices) => {
            if(indices){
                //  console.log("onDeletePathsSelected:(paths)", indices[0])
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

    UserLogoff {
        id: userLogoff
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
                AbstractButton {
                    id: loginButton
                    Layout.preferredWidth: 25
                    Layout.preferredHeight: 25
                    hoverEnabled: true
                    Item {
                        id: userImageCliped
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
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
//                        if(!authHandler.loggedIn) {
                        if(wsClient.authConnectionState === WebSocketClient.NotAuthorized ||
                            wsClient.authConnectionState === WebSocketClient.LoggedOut){
                            userLogin.statusTextElement.text = ""
                            userLogin.open()
                        }
                        else if(wsClient.authConnectionState === WebSocketClient.Authorized){
                            userLogoff.open()
                        }
                        else {
                            nwDialog.open()
                        }
                    }
                    Shape {
                        id: bubble
                        x: -text.width - 25
                        anchors.margins: 3
                        preferredRendererType: Shape.CurveRenderer
                        visible: wsClient.authConnectionState === WebSocketClient.NotAuthorized ||
                                 wsClient.authConnectionState === WebSocketClient.LoggedOut
                        //  visible: loginButton.hovered || (authHandler ? authHandler.loggedIn : false)
                        //  visible: !rootWnd.authHandler.loggedIn
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
                            //  text: qsTr("Log in to edit")
                            text: authHandler ? ( authHandler.loggedIn ? authHandler.username : "Log in to edit") : "Log in to edit"
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
                    text: "File dialog"
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    onClicked: {
                        wsClient.connectToServer()
                        customDialog.currentTabIndex = 0
                        storageModel.enterLocal(currentLocalPath); // TODO:??
                        customDialog.textFld = currentLocalPath
                        // else storageModel.enterNetStore("main-bucket")
                        customDialog.show();
                        // customDialog.open()
                    }
                }
                // Button {
                //     text: "Write file/folder"
                //     Layout.fillWidth: true
                //     Layout.preferredWidth: 1
                //     onClicked: {
                //         wsClient.connectToServer()
                //         storageModel.enterLocal(currentLocalPath);
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
                                    infoText.text = model ? model.cleanPath : ""; // Передаем путь из модели
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
                        //  anchors.bottom: parent.bottom
                        //  anchors.horizontalCenter: parent.horizontalCenter
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
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
    footer: ToolBar {
        id: statusBar

        // Высота панели
        height: 30

        // Меняем цвет фона панели в зависимости от состояния сети/авторизации
        background: Rectangle {
            color: {
                if (!wsClient) return "#667085" // Цвет по умолчанию (Серый)

                switch (wsClient.authConnectionState) {
                    case WebSocketClient.Authorized:
                        return "#2e7d32" // Зеленый (Успешно подключен)
                    case WebSocketClient.Connecting:
                    case WebSocketClient.Connected:
                    case WebSocketClient.Authenticating:
                    case WebSocketClient.LoggingOut:
                    case WebSocketClient.AuthorizedNoPingRespond:
                    case WebSocketClient.LoggedOutNoPingRespond:
                    case WebSocketClient.ExternalDisconnecting:
                    case WebSocketClient.UserDisconnecting:
                        return "#f57c00" // Оранжевый (В процессе)
                    case WebSocketClient.NoConnection:
                        return "#d32f2f" // Красный (Ошибка)
                    case WebSocketClient.NotAuthorized:
                    case WebSocketClient.LoggedOut: return "darkkhaki"
                    default:
                        return "#667085"
                }
            }

            // Плавный переход цвета при смене состояний
            Behavior on color { ColorAnimation { duration: 300 } }
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            spacing: 10
            // verticalAlignment: Qt.AlignVCenter

            // Иконка или индикатор состояния (маленький кружок)
            Rectangle {
                width: 10
                height: 10
                radius: 5
                Layout.alignment: Qt.AlignVCenter
                color: "white"

                // Делаем так, чтобы индикатор мигал во время подключения
                SequentialAnimation on opacity {
                    running: wsClient.authConnectionState === WebSocketClient.Connecting ||
                             wsClient.authConnectionState === WebSocketClient.Authenticating
                    loops: Animation.Infinite
                    PropertyAnimation { to: 0.2; duration: 500 }
                    PropertyAnimation { to: 1.0; duration: 500 }
                    // Если состояние стабильное, просто горит на 100%
                    onRunningChanged: if (!running) opacity = 1.0
                }
            }
            // Текстовое описание текущего статуса
            Label {
                //anchors.verticalCenter: parent.verticalCenter
                Layout.alignment: Qt.AlignVCenter
                color: "white"
                font.pixelSize: 12
                font.bold: true

                text: {
                    if (!wsClient) return "Инициализация..."
                    //  console.log("wsClient.authConnectionState: ", wsClient.authConnectionState, " wsClient.Idle: ", wsClient.Idle)
                    // for (var property in wsClient) {
                    //     try {
                    //         // Выводим имя свойства и его текущее значение
                    //         console.log(property + " : " + obj[property])
                    //     } catch (e) {
                    //         // Некоторые свойства Qt защищены от чтения в рантайме
                    //         console.log(property + " : [Ошибка чтения: " + e.message + "]")
                    //     }
                    // }
                    //  console.log(JSON.stringify(wsClient, null, 2))
                    switch (wsClient.authConnectionState) {
                        case WebSocketClient.Idle: return "Отключено"
                        case WebSocketClient.Connecting: return "Подключение по сети..."
                        case WebSocketClient.Connected: return "Сеть активна, авторизация..."
                        case WebSocketClient.Authenticating: return "Проверка токена безопасности..."
                        case WebSocketClient.Authorized: return "Подключен: " + (authHandler ? authHandler.username : "")
                        case WebSocketClient.NotAuthorized: return "Ни один пользователь не подключен."     //"Ошибка: Токен устарел или испорчен"
                        case WebSocketClient.NoConnection: return "Ошибка: Нет связи с сервером"
                        case WebSocketClient.LoggingOut: return "Пользователь отключается"
                        case WebSocketClient.LoggedOut: return "Ни один пользователь не подключен."
                        case WebSocketClient.AuthorizedNoPingRespond: return "Подключен: " + (authHandler ? authHandler.username : "") + ".  Ping отсутствует."
                        case WebSocketClient.LoggedOutNoPingRespond: return "Ни один пользователь не подключен.  Ping отсутствует."
                        case WebSocketClient.ExternalDisconnecting: return "Ошибка сервера."
                        case WebSocketClient.UserDisconnecting: return "Отключаемся."
                        default: return "Неизвестный статус"
                    }

                }
            }
        }
    }
    function processPath(path){
        if(path) {
            let type = FileHelper.checkPathType(path);
            if (type === FileHelperType.LocalFile) {
                console.log("before imageGrid.model.addFilePath(path)", path)
                imageGrid.model.addImagePath(path)
//                console.log("after imageGrid.model.addFilePath(path)")
                mainImageSource = imageGrid.model.resolvePath(path)
//                console.log("after mainImageSource = imageGrid.model.resolvePath(path)", path)
            } else if (type === FileHelperType.LocalFolder) {
//                console.log("Это локальная папка");
                mainImageSource = imageGrid.model.addFilesFromFolder(path)
            } else if (type === FileHelperType.MinioBucket) {
//                console.log("Это бакет MinIO");
                mainImageSource = imageGrid.model.addImagesFromMinioBucket(path)
            } else if (type === FileHelperType.MinioFile) {
//                console.log("Это объект (файл) в MinIO", path);
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

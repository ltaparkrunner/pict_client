import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Basic

Dialog {
    id: warnLoginDialog
    modal: true
    anchors.centerIn: parent
    width: 350

    Column {
        Text{
            text: "Login or register"
        }

        Button {
            text: "Close"
            onClicked: {
                warnLoginDialog.close()
            }
        }
    }
}

/*
    Copyright (C) 2020 Sebastian J. Wolf and other contributors

    This file is part of Fernschreiber.

    Fernschreiber is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Fernschreiber is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Fernschreiber. If not, see <http://www.gnu.org/licenses/>.
*/
import QtQuick 2.6
import Sailfish.Silica 1.0
import Nemo.Notifications 1.0
import WerkWolf.Fernschreiber 1.0
import "../components"
import "../js/twemoji.js" as Emoji
import "../js/functions.js" as Functions
import "../js/debug.js" as Debug

Page {
    id: topicsPage
    allowedOrientations: Orientation.All

    property var chatId;
    property var groupInfo;
    property var supergroupInformation;
    property var chatPicture;

    Component.onCompleted: {
        tdLibWrapper.getForumTopics(chatId)
        console.info(JSON.stringify(supergroupInformation))
    }

    Connections {
        target: tdLibWrapper

        onTopicsReceived: {
            console.log("Topic received. Eah")
//            console.log('Groups: ', JSON.stringify(groupInfo));
//            var openAndSendStartToBot = chat["@extra"].indexOf("openAndSendStartToBot:") === 0
//            if(chat["@extra"] === "openDirectly" || openAndSendStartToBot && chat.type["@type"] === "chatTypePrivate") {
//                pageStack.pop(overviewPage, PageStackAction.Immediate)
//                // if we get a new chat (no messages?), we can not use the provided data
//                var chatinfo = tdLibWrapper.getChat(chat.id);
//                var options = { "chatInformation" : chatinfo }
//                if(openAndSendStartToBot) {
//                    options.doSendBotStartMessage = true;
//                    options.sendBotStartMessageParameter = chat["@extra"].substring(22);
//                }
//                pageStack.push(Qt.resolvedUrl("../pages/ChatPage.qml"), options);
//            }
        }
        onErrorReceived: {
            Functions.handleErrorMessage(code, message);
        }
    }

    PageHeader {
        id: pageHeader
        title: groupInfo.title
        visible: opacity > 0
        Behavior on opacity { FadeAnimation {} }

        ProfileThumbnail {
            id: chatPictureThumbnail
            replacementStringHint: groupInfo.title
            width: Theme.itemSizeMedium
            height: Theme.itemSizeMedium
            photoData: chatPicture
        }

    }

    SilicaListView {
        id: topicListView
        anchors {
            top: pageHeader.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        clip: true
//        opacity: (overviewPage.chatListCreated && !overviewPage.logoutLoading) ? 1 : 0
//        Behavior on opacity { FadeAnimation {} }
        model: topicListModel
        delegate: ChatListViewItem {
            ownUserId: overviewPage.ownUserId
            isVerified: is_verified
            onClicked: {
                pageStack.push(Qt.resolvedUrl("../pages/ChatPage.qml"), {
                    chatInformation : groupInfo,
                    chatPicture: chatPicture,
                    messageThreadId: messageThreadId
                })
            }
        }

        ViewPlaceholder {
            enabled: topicListView.count === 0
            text: topicListView.count === 0 ? qsTr("The Group doesn'thave topics") : qsTr("No matching chats found.")
//            hintText: qsTr("You can search public chats or create a new chat via the pull-down menu.")
        }

        VerticalScrollDecorator {}
    }
}

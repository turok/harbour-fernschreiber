/*
    Copyright (C) 2025 Anton Turko and other contributors

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

#ifndef TOPICLISTMODEL_H
#define TOPICLISTMODEL_H

#include <QAbstractListModel>
#include "appsettings.h"

class TDLibWrapper;
class AppSettings;

class TopicListModel : public QAbstractListModel
{
    Q_OBJECT

public:

    enum Role {
        RoleDisplay = Qt::DisplayRole,
        RoleChatId,
        RoleChatType,
        RoleGroupId,
        RoleTitle,
        RolePhotoSmall,
        RoleUnreadCount,
        RoleUnreadMentionCount,
        RoleUnreadReactionCount,
        RoleAvailableReactions,
        RoleLastReadInboxMessageId,
        RoleLastMessageSenderId,
        RoleLastMessageDate,
        RoleLastMessageText,
        RoleLastMessageStatus,
        RoleChatMemberStatus,
        RoleSecretChatState,
        RoleIsVerified,
        RoleIsChannel,
        RoleIsMarkedAsUnread,
        RoleIsPinned,
        RoleFilter,
        RoleDraftMessageText,
        RoleDraftMessageDate,
        RoleMessageThreadId
    };


    TopicListModel(TDLibWrapper *tdLibWrapper, AppSettings *appSettings);
    ~TopicListModel() override;

    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &index = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

private slots:
    void topicsReceived(const QVariantMap &topics);

private:
    class TopicData;

    TDLibWrapper *tdLibWrapper;
    AppSettings *appSettings;

    QList<TopicData*> topicList;

};

#endif //TOPICLISTMODEL_H

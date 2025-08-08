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


#include "topiclistmodel.h"

#define DEBUG_MODULE TopicListModel
#include "debuglog.h"
#include "tdlibwrapper.h"
#include "fernschreiberutils.h"

namespace {
    const QString ID("id");
    const QString DATE("date");
    const QString TEXT("text");
    const QString TYPE("type");
    const QString TITLE("title");
    const QString NAME("name");
    const QString INFO("info");
    const QString MESSAGE_THREAD_ID("message_thread_id");
    const QString PHOTO("photo");
    const QString SMALL("small");
    const QString ORDER("order");
    const QString CHAT_ID("chat_id");
    const QString CONTENT("content");
    const QString LAST_MESSAGE("last_message");
    const QString DRAFT_MESSAGE("draft_message");
    const QString SENDER_ID("sender_id");
    const QString USER_ID("user_id");
    const QString BASIC_GROUP_ID("basic_group_id");
    const QString SUPERGROUP_ID("supergroup_id");
    const QString UNREAD_COUNT("unread_count");
    const QString UNREAD_MENTION_COUNT("unread_mention_count");
    const QString UNREAD_REACTION_COUNT("unread_reaction_count");
    const QString AVAILABLE_REACTIONS("available_reactions");
    const QString NOTIFICATION_SETTINGS("notification_settings");
    const QString LAST_READ_INBOX_MESSAGE_ID("last_read_inbox_message_id");
    const QString LAST_READ_OUTBOX_MESSAGE_ID("last_read_outbox_message_id");
    const QString SENDING_STATE("sending_state");
    const QString IS_CHANNEL("is_channel");
    const QString IS_VERIFIED("is_verified");
    const QString IS_MARKED_AS_UNREAD("is_marked_as_unread");
    const QString IS_PINNED("is_pinned");
    const QString PINNED_MESSAGE_ID("pinned_message_id");
    const QString _TYPE("@type");
    const QString SECRET_CHAT_ID("secret_chat_id");
    const QString TOPICS("topics");
}

class TopicListModel::TopicData {
public:
    TopicData(TDLibWrapper *tdLibWrapper, const QVariantMap &data);
    int compareTo(const TopicData *chat) const;
    bool setOrder(const QString &order);

    const QVariant lastMessage(const QString &key) const;
    QString title() const;
    int unreadCount() const;
    int unreadMentionCount() const;
    int unreadReactionCount() const;
    QVariant availableReactions() const;
    QVariant photoSmall() const;
    qlonglong lastReadInboxMessageId() const;
    qlonglong senderUserId() const;
    qlonglong senderChatId() const;
    bool senderIsChat() const;
    qlonglong senderMessageDate() const;
    QString senderMessageText() const;
    QString senderMessageStatus() const;
    qlonglong draftMessageDate() const;
    QString draftMessageText() const;
    bool isChannel() const;
    bool isHidden() const;
    bool isMarkedAsUnread() const;
    bool isPinned() const;
    bool updateUnreadCount(int unreadCount);
    bool updateLastReadInboxMessageId(qlonglong messageId);
public:
    TDLibWrapper *tdLibWrapper;
    QVariantMap topicData;
    qlonglong chatId;
    qlonglong messageThreadId;
    qlonglong order;
    qlonglong groupId;
    bool verified;
};


TopicListModel::TopicData::TopicData(TDLibWrapper *tdLibWrapper, const QVariantMap &data):
    tdLibWrapper(tdLibWrapper),
    topicData(data),
    chatId(data.value(ID).toLongLong()),
    messageThreadId(data.value(INFO).toMap().value(MESSAGE_THREAD_ID).toLongLong()),
    order(data.value(ORDER).toLongLong()),
    groupId(data.value(SUPERGROUP_ID).toLongLong()),
    verified(false)
{
}

int TopicListModel::TopicData::compareTo(const TopicData *other) const
{
    if (order == other->order) {
        return (chatId < other->chatId) ? 1 : -1;
    } else {
        // This puts most recent ones to the top of the list
        return (order < other->order) ? 1 : -1;
    }
}

bool TopicListModel::TopicData::setOrder(const QString &newOrder)
{
    if (!newOrder.isEmpty()) {
        topicData.insert(ORDER, newOrder);
        order = newOrder.toLongLong();
        return true;
    }
    return false;
}

inline const QVariant TopicListModel::TopicData::lastMessage(const QString &key) const
{
    return topicData.value(LAST_MESSAGE).toMap().value(key);
}

QString TopicListModel::TopicData::title() const
{
    return topicData.value(INFO).toMap().value(NAME).toString();
}

int TopicListModel::TopicData::unreadCount() const
{
    return topicData.value(UNREAD_COUNT).toInt();
}

int TopicListModel::TopicData::unreadMentionCount() const
{
    return topicData.value(UNREAD_MENTION_COUNT).toInt();
}

int TopicListModel::TopicData::unreadReactionCount() const
{
    return topicData.value(UNREAD_COUNT).toInt();
}

QVariant TopicListModel::TopicData::availableReactions() const
{
    return topicData.value(AVAILABLE_REACTIONS);
}

QVariant TopicListModel::TopicData::photoSmall() const
{
    return topicData.value(PHOTO).toMap().value(SMALL);
}

qlonglong TopicListModel::TopicData::lastReadInboxMessageId() const
{
    return topicData.value(LAST_READ_INBOX_MESSAGE_ID).toLongLong();
}

qlonglong TopicListModel::TopicData::senderUserId() const
{
    return lastMessage(SENDER_ID).toMap().value(USER_ID).toLongLong();
}

qlonglong TopicListModel::TopicData::senderChatId() const
{
    return lastMessage(SENDER_ID).toMap().value(CHAT_ID).toLongLong();
}

bool TopicListModel::TopicData::senderIsChat() const
{
    return lastMessage(SENDER_ID).toMap().value(_TYPE).toString() == "messageSenderChat";
}

qlonglong TopicListModel::TopicData::senderMessageDate() const
{
    return lastMessage(DATE).toLongLong();
}

QString TopicListModel::TopicData::senderMessageText() const
{
    qlonglong myUserId = tdLibWrapper->getUserInformation().value(ID).toLongLong();
    return FernschreiberUtils::getMessageShortText(tdLibWrapper, lastMessage(CONTENT).toMap(), isChannel(), myUserId, lastMessage(SENDER_ID).toMap() );
//    return "";
}


QString TopicListModel::TopicData::senderMessageStatus() const
{
    qlonglong myUserId = tdLibWrapper->getUserInformation().value(ID).toLongLong();
    if (isChannel() || myUserId != senderUserId() || myUserId == chatId) {
        return "";
    }
    if (lastMessage(ID) == topicData.value(LAST_READ_OUTBOX_MESSAGE_ID)) {
        return "&nbsp;&nbsp;✅";
    } else {
        QVariantMap lastMessage = topicData.value(LAST_MESSAGE).toMap();
        if (lastMessage.contains(SENDING_STATE)) {
            QVariantMap sendingState = lastMessage.value(SENDING_STATE).toMap();
            if (sendingState.value(_TYPE).toString() == "messageSendingStatePending") {
                return "&nbsp;&nbsp;🕙";
            } else {
                return "&nbsp;&nbsp;❌";
            }
        } else {
            return "&nbsp;&nbsp;☑️";
        }
    }
}

qlonglong TopicListModel::TopicData::draftMessageDate() const
{
    QVariantMap draft = topicData.value(DRAFT_MESSAGE).toMap();
    if(draft.isEmpty()) {
        return qlonglong(0);
    }
    return draft.value(DATE).toLongLong();
}

QString TopicListModel::TopicData::draftMessageText() const
{
    QVariantMap draft = topicData.value(DRAFT_MESSAGE).toMap();
    if(draft.isEmpty()) {
        return QString();
    }
    return draft.value("input_message_text").toMap().value(TEXT).toMap().value(TEXT).toString();
}

bool TopicListModel::TopicData::isChannel() const
{
    return topicData.value(TYPE).toMap().value(IS_CHANNEL).toBool();
}

bool TopicListModel::TopicData::isHidden() const
{
    return false;
}

bool TopicListModel::TopicData::isMarkedAsUnread() const
{
    return topicData.value(IS_MARKED_AS_UNREAD).toBool();
}

bool TopicListModel::TopicData::isPinned() const
{
    return topicData.value(IS_PINNED).toBool();
}

bool TopicListModel::TopicData::updateUnreadCount(int count)
{
    const int prevUnreadCount(unreadCount());
    topicData.insert(UNREAD_COUNT, count);
    return prevUnreadCount != unreadCount();
}

bool TopicListModel::TopicData::updateLastReadInboxMessageId(qlonglong messageId)
{
    const qlonglong prevLastReadInboxMessageId(lastReadInboxMessageId());
    topicData.insert(LAST_READ_INBOX_MESSAGE_ID, messageId);
    return prevLastReadInboxMessageId != lastReadInboxMessageId();
}


TopicListModel::TopicListModel(TDLibWrapper *tdLibWrapper, AppSettings *appSettings)
{
    this->tdLibWrapper = tdLibWrapper;
    this->appSettings = appSettings;
    connect(tdLibWrapper, SIGNAL(topicsReceived(QVariantMap)), this, SLOT(topicsReceived(QVariantMap)));
    LOG("TopicListModel created");
}

TopicListModel::~TopicListModel()
{
    LOG("Destroying TopicListModel...");
    qDeleteAll(topicList);
}

QHash<int,QByteArray> TopicListModel::roleNames() const
{
    QHash<int,QByteArray> roles;
    roles.insert(TopicListModel::RoleDisplay, "display");
    roles.insert(TopicListModel::RoleTitle, "title");
    roles.insert(TopicListModel::RolePhotoSmall, "photo_small");
    roles.insert(TopicListModel::RoleUnreadCount, "unread_count");
    roles.insert(TopicListModel::RoleUnreadMentionCount, "unread_mention_count");
    roles.insert(TopicListModel::RoleUnreadReactionCount, "unread_reaction_count");
    roles.insert(TopicListModel::RoleAvailableReactions, "available_reactions");
    roles.insert(TopicListModel::RoleLastReadInboxMessageId, "last_read_inbox_message_id");
    roles.insert(TopicListModel::RoleLastMessageSenderId, "last_message_sender_id");
    roles.insert(TopicListModel::RoleLastMessageDate, "last_message_date");
    roles.insert(TopicListModel::RoleLastMessageText, "last_message_text");
    roles.insert(TopicListModel::RoleLastMessageStatus, "last_message_status");
    roles.insert(TopicListModel::RoleChatMemberStatus, "chat_member_status");
    roles.insert(TopicListModel::RoleSecretChatState, "secret_chat_state");
    roles.insert(TopicListModel::RoleIsVerified, "is_verified");
    roles.insert(TopicListModel::RoleIsChannel, "is_channel");
    roles.insert(TopicListModel::RoleIsMarkedAsUnread, "is_marked_as_unread");
    roles.insert(TopicListModel::RoleIsPinned, "is_pinned");
    roles.insert(TopicListModel::RoleFilter, "filter");
    roles.insert(TopicListModel::RoleDraftMessageDate, "draft_message_date");
    roles.insert(TopicListModel::RoleDraftMessageText, "draft_message_text");
    roles.insert(TopicListModel::RoleMessageThreadId, "messageThreadId");
    return roles;
}


int TopicListModel::rowCount(const QModelIndex &) const
{
    return topicList.size();
}

QVariant TopicListModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();
    if (row >= 0 && row < topicList.size()) {
        const TopicData *data = topicList.at(row);
        switch ((TopicListModel::Role)role) {
            case TopicListModel::RoleDisplay: return data->topicData;
            case TopicListModel::RoleChatId: return data->chatId;
//            case TopicListModel::RoleChatType: return data->chatType;
//            case TopicListModel::RoleGroupId: return data->groupId;
            case TopicListModel::RoleTitle: return data->title();
            case TopicListModel::RolePhotoSmall: return data->photoSmall();
            case TopicListModel::RoleUnreadCount: return data->unreadCount();
            case TopicListModel::RoleUnreadMentionCount: return data->unreadMentionCount();
            case TopicListModel::RoleAvailableReactions: return data->availableReactions();
            case TopicListModel::RoleUnreadReactionCount: return data->unreadReactionCount();
            case TopicListModel::RoleLastReadInboxMessageId: return data->lastReadInboxMessageId();
            case TopicListModel::RoleLastMessageSenderId: return data->senderUserId();
            case TopicListModel::RoleLastMessageText: return data->senderMessageText();
            case TopicListModel::RoleLastMessageDate: return data->senderMessageDate();
            case TopicListModel::RoleLastMessageStatus: return data->senderMessageStatus();
//            case TopicListModel::RoleChatMemberStatus: return data->memberStatus;
//            case TopicListModel::RoleSecretChatState: return data->secretChatState;
            case TopicListModel::RoleIsVerified: return data->verified;
            case TopicListModel::RoleIsChannel: return data->isChannel();
            case TopicListModel::RoleIsMarkedAsUnread: return data->isMarkedAsUnread();
            case TopicListModel::RoleIsPinned: return data->isPinned();
            case TopicListModel::RoleFilter: return data->title() + " " + data->senderMessageText();
            case TopicListModel::RoleDraftMessageText: return data->draftMessageText();
            case TopicListModel::RoleDraftMessageDate: return data->draftMessageDate();
            case TopicListModel::RoleMessageThreadId: return data->messageThreadId;
        }
    }
    return QVariant();
}

void TopicListModel::topicsReceived(const QVariantMap &data)
{
    WARN("Topics received...");
    const QVariantList topicsList(data.value(TOPICS).toList());
    WARN("Topicsnumbers");
    WARN(topicsList.size());
    int i = 0;
    for (auto it = topicsList.constBegin(); it != topicsList.constEnd(); ++it) {
        const QVariant &value = *it;
        if (value.canConvert<QVariantMap>()) {
            beginInsertRows(QModelIndex(), i, i);
            QVariantMap topicMap = value.toMap();
            topicList.append(new TopicData(tdLibWrapper, topicMap));
            endInsertRows();
            i++;
        }
    }
}

/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "chatsession.h"
#include "messagehandler.h"
#include "objectgenerator.h"
#include "servicemanager.h"
#include "notification.h"
#include "account.h"
#include "history.h"
#include <QBasicTimer>
#include <QDateTime>
#include <numeric>

namespace qutim_sdk_0_3
{
typedef QMap<quint64, ChatSession*> MessageHookMap;
Q_GLOBAL_STATIC(MessageHookMap, messageHookMap)

class MessageHandlerHook : public MessageHandler
{
public:
	MessageHandlerHook()
	{

	}
	
	MessageHandlerAsyncResult doHandle(Message &message) override
	{
		ChatSession *session = messageHookMap()->value(originalMessageId());
		if (session) {
			session->doAppendMessage(message);
		}
		return makeAsyncResult(Accept, QString());
	}

};

class HistoryHook : public MessageHandler
{
public:
	HistoryHook()
	{
		Config cfg(QLatin1String("appearance"));
		cfg.beginGroup(QLatin1String("chat"));
		cfg.beginGroup(QLatin1String("history"));
		m_storeMessages = cfg.value(QLatin1String("storeMessages"), true);
		m_storeServiceMessages = cfg.value(QLatin1String("storeServiceMessages"), true);
	}

	MessageHandlerAsyncResult doHandle(Message &message) override
	{

		if (m_storeMessages && message.property("store", true)
			&& (m_storeServiceMessages || !message.property("service", false))) {
			History::instance()->store(message);
		}
		return makeAsyncResult(Accept, QString());
	}

private:
	bool m_storeMessages;
	bool m_storeServiceMessages;
};

class ChatUnitSenderMessageHandler : public MessageHandler
{
public:
	MessageHandlerAsyncResult doHandle(Message &message) override
	{
		if (!message.isIncoming()
		        && !message.property("service", false)
		        && !message.property("history", false)
		        && !message.property("donotsend", false)) {
            if (!message.chatUnit()->send(message)) {
				return makeAsyncResult(Error, QString());
            }
		}
		return makeAsyncResult(Accept, QString());
	}
};

class ChatSessionPrivate
{
public:
	ChatSessionPrivate() : active(false) {}

	bool active;
	QDateTime dateOpened;
};

struct ChatLayerData
{
	ServicePointer<ChatLayer> self;
	QScopedPointer<MessageHandlerHook> handlerHook;
	QScopedPointer<ChatUnitSenderMessageHandler> senderHook;
	QScopedPointer<HistoryHook> historyHook;
};

Q_GLOBAL_STATIC(ChatLayerData, p)

ChatSession::ChatSession(ChatLayer *chat) : QObject(chat), d_ptr(new ChatSessionPrivate)
{
	Q_D(ChatSession);
	d->dateOpened = QDateTime::currentDateTime();
}

ChatSession::~ChatSession()
{
}


void ChatSession::append(const Message &originalMessage, const ChatSession::AppendHandler &handler)
{
    Message message = originalMessage;
    if (!message.chatUnit()) {
		qWarning() << "Message" << message.text() << "must have a chatUnit";
		message.setChatUnit(getUnit());
	}

    const quint64 messageId = message.id();
	messageHookMap()->insert(messageId, this);
	MessageHandler::handle(message).connect(this, [messageId, handler] (const Message &message, MessageHandler::Result result, const QString &reason) {
        messageHookMap()->remove(messageId);
        
        if (MessageHandler::Accept != result) {
            NotificationRequest request(Notification::BlockedMessage);
            request.setObject(message.chatUnit());
            request.setText(reason);
			request.send();
            if (handler)
                handler(-result, message, reason);
            return;
        }
        if (!message.property("service", false) && !message.property("autoreply", false))
            message.chatUnit()->setLastActivity(message.time());
        
        if (handler)
            handler(messageId, message, reason);
    });
}

void ChatSession::append(const qutim_sdk_0_3::Message &message)
{
    append(message, AppendHandler());
}

void ChatSession::appendMessage(const qutim_sdk_0_3::Message &message)
{
	append(message);
}

bool ChatSession::isActive()
{
	return d_func()->active;
}

QDateTime ChatSession::dateOpened() const
{
	return d_func()->dateOpened;
}

void ChatSession::setDateOpened(const QDateTime &date)
{
	Q_D(ChatSession);
	if (d->dateOpened == date)
		return;
	d->dateOpened = date;
	emit dateOpenedChanged(d->dateOpened);
}

void ChatSession::setActive(bool active)
{
	Q_D(ChatSession);
	if (active == d->active)
		return;
	doSetActive(active);
	d->active = active;
	emit activated(active);
}

void ChatSession::virtual_hook(int id, void *data)
{
	Q_UNUSED(id);
	Q_UNUSED(data);
}

class ChatLayerPrivate
{
public:
	ChatLayerPrivate() : alerted(false) {}
	bool alerted;
	QBasicTimer alertTimer;
};

ChatLayer::ChatLayer() : d_ptr(new ChatLayerPrivate)
{
	qRegisterMetaType<qutim_sdk_0_3::MessageList>("qutim_sdk_0_3::MessageList");
	p()->handlerHook.reset(new MessageHandlerHook);
	p()->senderHook.reset(new ChatUnitSenderMessageHandler);
	p()->historyHook.reset(new HistoryHook);

	MessageHandler::registerHandler(p()->handlerHook.data(),
	                                QLatin1String("HandlerHook"),
	                                MessageHandler::ChatInPriority,
	                                MessageHandler::ChatOutPriority);
	MessageHandler::registerHandler(p()->senderHook.data(),
	                                QLatin1String("SenderHook"),
	                                MessageHandler::NormalPriortity,
	                                MessageHandler::SenderPriority);
	MessageHandler::registerHandler(p()->historyHook.data(),
									QLatin1String("HistoryHook"),
									MessageHandler::HistoryPriority,
									MessageHandler::HistoryPriority);
}

ChatLayer::~ChatLayer()
{
	p()->handlerHook.reset(0);
	p()->historyHook.reset(0);
}

ChatLayer *ChatLayer::instance()
{
	return p()->self.data();
}

ChatSession *ChatLayer::getSession(Account *acc, QObject *obj, bool create)
{
	QString id = (acc && obj) ? obj->property("id").toString() : QString();
	return id.isEmpty() ? 0 : getSession(acc->getUnit(id, create));
}

ChatSession *ChatLayer::getSession(QObject *obj, bool create)
{
	Account *acc = obj->property("account").value<Account *>();
	QString id = obj->property("id").toString();
	return getSession(acc, id, create);
}

ChatSession *ChatLayer::getSession(Account *acc, const QString &id, bool create)
{
	return (acc && !id.isEmpty()) ? getSession(acc->getUnit(id, create)) : 0;
}

ChatUnit *ChatLayer::getUnitForSession(ChatUnit *unit) const
{
	Account *acc = unit ? unit->account() : 0;
	return acc ? acc->getUnitForSession(unit) : unit;
}

ChatSession* ChatLayer::get(ChatUnit* unit, bool create)
{
	return instance() ? instance()->getSession(unit,create) : 0;
}

bool ChatLayer::isAlerted() const
{
	return d_func()->alerted;
}

void ChatLayer::alert(bool on)
{
	Q_D(ChatLayer);
	if (d->alerted == on)
		return;
	d->alerted = on;
	d->alertTimer.stop();
	emit alertStatusChanged(on);
}

void ChatLayer::alert(int msecs)
{
	Q_D(ChatLayer);
	d->alertTimer.start(msecs, this);
	if (!d->alerted) {
		d->alerted = true;
		emit alertStatusChanged(true);
	}
}

bool ChatLayer::event(QEvent *ev)
{
	if (ev->type() == QEvent::Timer) {
		QTimerEvent *timerEvent = static_cast<QTimerEvent*>(ev);
		if (timerEvent->timerId() == d_func()->alertTimer.timerId()) {
			d_func()->alertTimer.stop();
			alert(false);
			return true;
		}
	}
	return QObject::event(ev);
}

void ChatLayer::virtual_hook(int id, void *data)
{
	Q_UNUSED(id);
	Q_UNUSED(data);
}
}


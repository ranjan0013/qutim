/****************************************************************************
 *  jmucmanager.h
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/
#ifndef JMUCMANAGER_H
#define JMUCMANAGER_H

#include <QObject>
#include <qutim/chatunit.h>
#include <jreen/presence.h>
#include <qutim/dataforms.h>
#include <qutim/status.h>

namespace qutim_sdk_0_3 
{
class Conference;
}

namespace jreen
{
class PrivacyItem;
}

namespace Jabber
{
class JAccount;
class JBookmarkManager;
class JMUCManagerPrivate;
class JMUCSession;

class JMUCManager : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(JMUCManager)
public:
	JMUCManager(JAccount *account, QObject *parent = 0);
	~JMUCManager();
	qutim_sdk_0_3::ChatUnit *muc(const jreen::JID &jid);
	JBookmarkManager *bookmarkManager();
	void syncBookmarks();
	void join(const QString &conference, const QString &nick = QString(), const QString &password = QString());
	void setPresenceToRooms(jreen::Presence::Type presence);
	void leave(const QString &room);
	bool event(QEvent *event);
	void appendMUCSession(JMUCSession *room);
signals:
	void conferenceCreated(qutim_sdk_0_3::Conference*);
private slots:
	//TODO rewrite on private slots
	void onListReceived(const QString &name, const QList<jreen::PrivacyItem> &items);
	void onActiveListChanged(const QString &name);
	void bookmarksChanged();
	void closeMUCSession();
protected:
	void closeMUCSession(JMUCSession *room);
private:
	QScopedPointer<JMUCManagerPrivate> d_ptr;
	Q_PRIVATE_SLOT(d_func() , void _q_status_changed(qutim_sdk_0_3::Status))
};
}

#endif // JMUCMANAGER_H

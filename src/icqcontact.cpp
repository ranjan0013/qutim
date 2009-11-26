/****************************************************************************
 *  icqcontact.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "icqcontact_p.h"
#include "icqaccount.h"
#include "roster.h"
#include "qutim/messagesession.h"

namespace Icq {

IcqContact::IcqContact(const QString &uin, IcqAccount *account) : Contact(account), p(new IcqContactPrivate)
{
	p->account = account;
	p->uin = uin;
	p->status = Offline;

	clearCapabilities();
}

QSet<QString> IcqContact::tags() const
{
	QSet<QString> group;
	QString group_name = p->group_id == not_in_list_group ? QString() : p->account->roster()->groupId2Name(p->group_id);
	if(!group_name.isNull())
		group.insert(group_name);
	return group;
}

QString IcqContact::id() const
{
	return p->uin;
}

QString IcqContact::name() const
{
	return p->name;
}

Status IcqContact::status() const
{
	return p->status;
}

bool IcqContact::isInList() const
{
	return p->group_id == not_in_list_group;
}

// TODO: Impl this fucked things)

void IcqContact::sendMessage(const Message &message)
{
	p->account->roster()->sendMessage(p->uin, message.text());
}

void IcqContact::setName(const QString &name)
{
	p->account->roster()->sendRenameContactRequest(p->uin, name);
}

void IcqContact::setTags(const QSet<QString> &tags)
{
}

void IcqContact::setInList(bool inList)
{
}

bool IcqContact::RtfSupport()
{
	return p->rtf_support;
}

bool IcqContact::TypingSupport()
{
	return p->typing_support;
}

bool IcqContact::AimChatSupport()
{
	return p->aim_chat_support;
}

bool IcqContact::AimImageSupport()
{
	return p->aim_image_support;
}

bool IcqContact::XtrazSupport()
{
	return p->xtraz_support;
}

bool IcqContact::Utf8Support()
{
	return p->utf8_support;
}

bool IcqContact::SendFileSupport()
{
	return p->sendfile_support;
}

bool IcqContact::DirectSupport()
{
	return p->direct_support;
}

bool IcqContact::IconSupport()
{
	return p->icon_support;
}

bool IcqContact::GetFileSupport()
{
	return p->getfile_support;
}

bool IcqContact::SrvRelaySupport()
{
	return p->srvrelay_support;
}

bool IcqContact::AvatarSupport()
{
	return p->avatar_support;
}

const Capabilities &IcqContact::capabilities()
{
	return p->capabilities;
}

const Capabilities &IcqContact::shortCapabilities()
{
	return p->short_capabilities;
}

const DirectConnectionInfo &IcqContact::dcInfo()
{
	return p->dc_info;
}

void IcqContact::clearCapabilities()
{
	p->rtf_support = false;
	p->typing_support = false;
	p->aim_chat_support = false;
	p->aim_image_support = false;
	p->xtraz_support = false;
	p->utf8_support = false;
	p->sendfile_support = false;
	p->direct_support = false;
	p->aim_chat_support = false;
	p->getfile_support = false;
	p->srvrelay_support = false;
	p->avatar_support = false;
	p->capabilities.clear();
}

} // namespace Icq

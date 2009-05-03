/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "collisionprotect.h"
#include "plugxmlhandler.h"
#include <QSet>
#include <QFileInfo>


CollisionProtect::CollisionProtect(const QString& prefix) {
	this->prefix = prefix;
}


bool CollisionProtect::checkPackageFiles(const QStringList& files) {
	foreach (QString value,files) {
		QString file = prefix + value;
		QFileInfo info (file);
		if (!info.isDir()&&info.exists()) 
			return false;
	}
	return true;
}

bool CollisionProtect::checkPackageName(const QString& name) {
	plugXMLHandler plug_handler;
	return !(plug_handler.getPackageNames().contains(name));
}

#ifndef VKONTAKTEACCOUNT_H
#define VKONTAKTEACCOUNT_H
#include <qutim/account.h>
#include "vkontakte_global.h"

struct VAccountPrivate;
class VContact;
class LIBVKONTAKTE_EXPORT VAccount : public Account
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(VAccount)
public:
	VAccount(const QString& uid);
	virtual VContact* getContact(const QString& uid, bool create = false);
	virtual ChatUnit* getUnit(const QString& unitId, bool create = false) {return getUnit(unitId,create);};
	virtual ~VAccount();
public slots:
	void updateSettings();
protected:
	QString password();
private:
	QScopedPointer<VAccountPrivate> d_ptr;
};

#endif // VKONTAKTEACCOUNT_H

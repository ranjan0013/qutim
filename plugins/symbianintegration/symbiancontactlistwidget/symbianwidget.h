/****************************************************************************
 *  symbianwidget.h
 *
 *  (c) LLC INRUSCOM, Russia
 *
*****************************************************************************/

#ifndef SYMBIANWIDGET_H
#define SYMBIANWIDGET_H
#include <QWidget>
#include <qutim/simplecontactlist/abstractcontactlist.h>
#include <qutim/simplecontactlist/simplecontactlistview.h>
#include <qutim/status.h>

namespace qutim_sdk_0_3
{
class Account;
class Contact;
}

class QPushButton;
class QLineEdit;
class QAction;
namespace Core {
namespace SimpleContactList {

class SymbianWidget  : public QWidget, public AbstractContactListWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::SimpleContactList::AbstractContactListWidget)
	Q_CLASSINFO("Uses", "ContactDelegate")
	Q_CLASSINFO("Uses", "ContactModel")
	Q_CLASSINFO("Service", "ContactListWidget")
public:
    SymbianWidget();
	virtual void addButton(ActionGenerator *generator);
	virtual void removeButton(ActionGenerator *generator);
protected:
	QAction *createGlobalStatusAction(Status::Type type);
	bool event(QEvent *event);
private slots:
	void init();
	void onAccountCreated(qutim_sdk_0_3::Account *account);
	void onAccountStatusChanged(const qutim_sdk_0_3::Status &status);
	void onAccountDestroyed(QObject *obj);
	void onStatusChanged();
	void showStatusDialog();
	void changeStatusTextAccepted();
private:
	TreeView *m_view;
	AbstractContactModel *m_model;
	QAction *m_statusBtn;
	QAction *m_actionsBtn;
	QLineEdit *m_searchBar;
	QHash<Account *, QAction *> m_actions;
	QAction *m_status_action;
	QList<QAction *> m_statusActions;
};

} // namespace SimpleContactList
} // namespace Core

#endif // SYMBIANWIDGET_H


#ifndef __KDEVPART_FILESELECTOR_H__
#define __KDEVPART_FILESELECTOR_H__

#include <qguardedptr.h>
#include <kdialogbase.h>
#include <kdevpart.h>
#include <kfileitem.h>


class FileSelectorWidget;

class FileSelectorPart : public KDevPart
{
  Q_OBJECT

public:

  FileSelectorPart(KDevApi *api, QObject *parent=0, const char *name=0);
  ~FileSelectorPart();

public slots:

//  void showTip();
//  void showOnStart();
//  void refresh();
	void fileSelected(const KFileItem *file);

 private slots:
   void slotProjectOpened();
private:
    QGuardedPtr<FileSelectorWidget> m_filetree;


};


#endif

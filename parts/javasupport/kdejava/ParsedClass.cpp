/***************************************************************************
                            ParsedClass.cpp -  description
                             -------------------
    begin                : Wed Dec 19 15:43:05 2001
    copyright            : (C) 2000-2001 Lost Highway Ltd.
    email                : Richard_Dale@tipitina.demon.co.uk
    generated by         : duke@tipitina on Wed Dec 19 15:43:05 2001, using kalyptus 0.91.
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
***************************************************************************/

#define _BOOL_H_
#include <parsedclass.h>
#include <qstring.h>
#include <qcstring.h>

#include <qtjava/QtSupport.h>
#include <kdejava/KDESupport.h>
#include <kdejava/ParsedClass.h>

class ParsedClassJBridge : public ParsedClass
{
public:
	ParsedClassJBridge() : ParsedClass() {}
	~ParsedClassJBridge() {QtSupport::qtKeyDeleted(this);}
};


JNIEXPORT void JNICALL
Java_org_kde_koala_ParsedClass_newParsedClass(JNIEnv *env, jobject obj)
{
	if (QtSupport::getQt(env, obj) == 0) {
		QtSupport::setQt(env, obj, new ParsedClassJBridge());
		QtSupport::setObjectForQtKey(env, obj, QtSupport::getQt(env, obj));
	}
	return;
}

JNIEXPORT void JNICALL
Java_org_kde_koala_ParsedClass_finalize(JNIEnv *env, jobject obj)
{
	if (QtSupport::allocatedInJavaWorld(env, obj)) {
		delete (ParsedClass*)QtSupport::getQt(env, obj);
		QtSupport::setQt(env, obj, 0);
	}
	return;
}

JNIEXPORT void JNICALL
Java_org_kde_koala_ParsedClass_removeWithReferences(JNIEnv *env, jobject obj, jstring aFile)
{
static QString * _qstring_aFile = 0;
	((ParsedClassJBridge*) QtSupport::getQt(env, obj))->removeWithReferences((QString&) * (QString *) QtSupport::toQString(env, aFile, &_qstring_aFile));
	return;
}

JNIEXPORT void JNICALL
Java_org_kde_koala_ParsedClass_removeMethod(JNIEnv *env, jobject obj, jobject aMethod)
{
	((ParsedClassJBridge*) QtSupport::getQt(env, obj))->removeMethod((ParsedMethod*) QtSupport::getQt(env, aMethod));
	return;
}

JNIEXPORT void JNICALL
Java_org_kde_koala_ParsedClass_clearDeclaration(JNIEnv *env, jobject obj)
{
	((ParsedClassJBridge*) QtSupport::getQt(env, obj))->clearDeclaration();
	return;
}

JNIEXPORT void JNICALL
Java_org_kde_koala_ParsedClass_addParent(JNIEnv *env, jobject obj, jobject aParent)
{
	((ParsedClassJBridge*) QtSupport::getQt(env, obj))->addParent((ParsedParent*) QtSupport::getQt(env, aParent));
	return;
}

JNIEXPORT void JNICALL
Java_org_kde_koala_ParsedClass_addFriend(JNIEnv *env, jobject obj, jstring aName)
{
static QString * _qstring_aName = 0;
	((ParsedClassJBridge*) QtSupport::getQt(env, obj))->addFriend((QString&) * (QString *) QtSupport::toQString(env, aName, &_qstring_aName));
	return;
}

JNIEXPORT void JNICALL
Java_org_kde_koala_ParsedClass_addSignal(JNIEnv *env, jobject obj, jobject aMethod)
{
	((ParsedClassJBridge*) QtSupport::getQt(env, obj))->addSignal((ParsedMethod*) QtSupport::getQt(env, aMethod));
	return;
}

JNIEXPORT void JNICALL
Java_org_kde_koala_ParsedClass_addSlot(JNIEnv *env, jobject obj, jobject aMethod)
{
	((ParsedClassJBridge*) QtSupport::getQt(env, obj))->addSlot((ParsedMethod*) QtSupport::getQt(env, aMethod));
	return;
}

JNIEXPORT jobject JNICALL
Java_org_kde_koala_ParsedClass_friends(JNIEnv *env, jobject obj)
{
	QStringList  _qlist;
	_qlist = ((ParsedClassJBridge*) QtSupport::getQt(env, obj))->friends();
	return (jobject) QtSupport::arrayWithQStringList(env, (QStringList *) &_qlist);
}

JNIEXPORT void JNICALL
Java_org_kde_koala_ParsedClass_setIsSubClass(JNIEnv *env, jobject obj, jboolean aState)
{
	((ParsedClassJBridge*) QtSupport::getQt(env, obj))->setIsSubClass((bool) aState);
	return;
}

JNIEXPORT jboolean JNICALL
Java_org_kde_koala_ParsedClass_isSubClass(JNIEnv *env, jobject obj)
{
	return (jboolean) ((ParsedClassJBridge*) QtSupport::getQt(env, obj))->isSubClass();
}

JNIEXPORT jobject JNICALL
Java_org_kde_koala_ParsedClass_getMethod(JNIEnv *env, jobject obj, jobject aMethod)
{
	return (jobject) QtSupport::objectForQtKey(env, (void *)((ParsedClassJBridge*) QtSupport::getQt(env, obj))->getMethod((ParsedMethod*) QtSupport::getQt(env, aMethod)), "org.kde.koala.ParsedMethod");
}

JNIEXPORT jobject JNICALL
Java_org_kde_koala_ParsedClass_getSignalByNameAndArg(JNIEnv *env, jobject obj, jstring aName)
{
static QString * _qstring_aName = 0;
	return (jobject) QtSupport::objectForQtKey(env, (void *)((ParsedClassJBridge*) QtSupport::getQt(env, obj))->getSignalByNameAndArg((QString&) * (QString *) QtSupport::toQString(env, aName, &_qstring_aName)), "org.kde.koala.ParsedMethod");
}

JNIEXPORT jobject JNICALL
Java_org_kde_koala_ParsedClass_getSlotByNameAndArg(JNIEnv *env, jobject obj, jstring aName)
{
static QString * _qstring_aName = 0;
	return (jobject) QtSupport::objectForQtKey(env, (void *)((ParsedClassJBridge*) QtSupport::getQt(env, obj))->getSlotByNameAndArg((QString&) * (QString *) QtSupport::toQString(env, aName, &_qstring_aName)), "org.kde.koala.ParsedMethod");
}

//JNIEXPORT  JNICALL
//Java_org_kde_koala_ParsedClass_getSortedSignalList(JNIEnv *env, jobject obj)
//{
//	return () ((ParsedClassJBridge*) QtSupport::getQt(env, obj))->getSortedSignalList();
//}

//JNIEXPORT  JNICALL
//Java_org_kde_koala_ParsedClass_getSortedSlotList(JNIEnv *env, jobject obj)
//{
//	return () ((ParsedClassJBridge*) QtSupport::getQt(env, obj))->getSortedSlotList();
//}

//JNIEXPORT  JNICALL
//Java_org_kde_koala_ParsedClass_getVirtualMethodList(JNIEnv *env, jobject obj)
//{
//	return () ((ParsedClassJBridge*) QtSupport::getQt(env, obj))->getVirtualMethodList();
//}

JNIEXPORT jboolean JNICALL
Java_org_kde_koala_ParsedClass_hasParent(JNIEnv *env, jobject obj, jstring aName)
{
static QString * _qstring_aName = 0;
	return (jboolean) ((ParsedClassJBridge*) QtSupport::getQt(env, obj))->hasParent((QString&) * (QString *) QtSupport::toQString(env, aName, &_qstring_aName));
}

JNIEXPORT jboolean JNICALL
Java_org_kde_koala_ParsedClass_hasVirtual(JNIEnv *env, jobject obj)
{
	return (jboolean) ((ParsedClassJBridge*) QtSupport::getQt(env, obj))->hasVirtual();
}

JNIEXPORT void JNICALL
Java_org_kde_koala_ParsedClass_out(JNIEnv *env, jobject obj)
{
	((ParsedClassJBridge*) QtSupport::getQt(env, obj))->out();
	return;
}


/***************************************************************************
                            Core.cpp -  description
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
#include <kdevcore.h>
#include <qstring.h>
#include <qcstring.h>

#include <qtjava/QtSupport.h>
#include <kdejava/KDESupport.h>
#include <kdejava/Core.h>
#include <kdejava/KDevJavaSlot.h>

JNIEXPORT void JNICALL
Java_org_kde_koala_Core_finalize(JNIEnv *env, jobject obj)
{
	if (QtSupport::allocatedInJavaWorld(env, obj) && ((KDevCore*)QtSupport::getQt(env, obj))->parent() == (QObject *) 0) {
		delete (KDevCore*)QtSupport::getQt(env, obj);
		QtSupport::setQt(env, obj, 0);
	}
	return;
}

JNIEXPORT void JNICALL
Java_org_kde_koala_Core_embedWidget(JNIEnv *env, jobject obj, jobject arg1, jint arg2, jstring arg3)
{
static QString * _qstring_arg3 = 0;
	((KDevCore*) QtSupport::getQt(env, obj))->embedWidget((QWidget*) QtSupport::getQt(env, arg1), (KDevCore::Role) arg2, (QString&) * (QString *) QtSupport::toQString(env, arg3, &_qstring_arg3));
	return;
}

JNIEXPORT void JNICALL
Java_org_kde_koala_Core_raiseWidget(JNIEnv *env, jobject obj, jobject arg1)
{
	((KDevCore*) QtSupport::getQt(env, obj))->raiseWidget((QWidget*) QtSupport::getQt(env, arg1));
	return;
}

JNIEXPORT void JNICALL
Java_org_kde_koala_Core_fillContextMenu(JNIEnv *env, jobject obj, jobject popup, jobject context)
{
	((KDevCore*) QtSupport::getQt(env, obj))->fillContextMenu((QPopupMenu*) QtSupport::getQt(env, popup), (Context*) QtSupport::getQt(env, context));
	return;
}

JNIEXPORT void JNICALL
Java_org_kde_koala_Core_openProject(JNIEnv *env, jobject obj, jstring projectFileName)
{
static QString * _qstring_projectFileName = 0;
	((KDevCore*) QtSupport::getQt(env, obj))->openProject((QString&) * (QString *) QtSupport::toQString(env, projectFileName, &_qstring_projectFileName));
	return;
}

JNIEXPORT void JNICALL
Java_org_kde_koala_Core_gotoFile(JNIEnv *env, jobject obj, jobject url)
{
	((KDevCore*) QtSupport::getQt(env, obj))->gotoFile((KURL&) *(KURL *) QtSupport::getQt(env, url));
	return;
}

JNIEXPORT void JNICALL
Java_org_kde_koala_Core_gotoDocumentationFile__Lorg_kde_koala_KURL_2I(JNIEnv *env, jobject obj, jobject url, jint embed)
{
	((KDevCore*) QtSupport::getQt(env, obj))->gotoDocumentationFile((KURL&) *(KURL *) QtSupport::getQt(env, url), (KDevCore::Embedding) embed);
	return;
}

JNIEXPORT void JNICALL
Java_org_kde_koala_Core_gotoDocumentationFile__Lorg_kde_koala_KURL_2(JNIEnv *env, jobject obj, jobject url)
{
	((KDevCore*) QtSupport::getQt(env, obj))->gotoDocumentationFile((KURL&) *(KURL *) QtSupport::getQt(env, url), KDevCore::Replace);
	return;
}

JNIEXPORT void JNICALL
Java_org_kde_koala_Core_gotoSourceFile__Lorg_kde_koala_KURL_2II(JNIEnv *env, jobject obj, jobject url, jint lineNum, jint embed)
{
	((KDevCore*) QtSupport::getQt(env, obj))->gotoSourceFile((KURL&) *(KURL *) QtSupport::getQt(env, url), (int) lineNum, (KDevCore::Embedding) embed);
	return;
}

JNIEXPORT void JNICALL
Java_org_kde_koala_Core_gotoSourceFile__Lorg_kde_koala_KURL_2(JNIEnv *env, jobject obj, jobject url)
{
	((KDevCore*) QtSupport::getQt(env, obj))->gotoSourceFile((KURL&) *(KURL *) QtSupport::getQt(env, url), 0, KDevCore::Replace);
	return;
}

JNIEXPORT void JNICALL
Java_org_kde_koala_Core_gotoExecutionPoint__Ljava_lang_String_2I(JNIEnv *env, jobject obj, jstring fileName, jint lineNum)
{
static QString * _qstring_fileName = 0;
	((KDevCore*) QtSupport::getQt(env, obj))->gotoExecutionPoint((QString&) * (QString *) QtSupport::toQString(env, fileName, &_qstring_fileName), (int) lineNum);
	return;
}

JNIEXPORT void JNICALL
Java_org_kde_koala_Core_gotoExecutionPoint__Ljava_lang_String_2(JNIEnv *env, jobject obj, jstring fileName)
{
static QString * _qstring_fileName = 0;
	((KDevCore*) QtSupport::getQt(env, obj))->gotoExecutionPoint((QString&) * (QString *) QtSupport::toQString(env, fileName, &_qstring_fileName), 0);
	return;
}

JNIEXPORT void JNICALL
Java_org_kde_koala_Core_saveAllFiles(JNIEnv *env, jobject obj)
{
	((KDevCore*) QtSupport::getQt(env, obj))->saveAllFiles();
	return;
}

JNIEXPORT void JNICALL
Java_org_kde_koala_Core_revertAllFiles(JNIEnv *env, jobject obj)
{
	((KDevCore*) QtSupport::getQt(env, obj))->revertAllFiles();
	return;
}

JNIEXPORT void JNICALL
Java_org_kde_koala_Core_setBreakpoint(JNIEnv *env, jobject obj, jstring fileName, jint lineNum, jint identifier, jboolean enabled, jboolean pending)
{
static QString * _qstring_fileName = 0;
	((KDevCore*) QtSupport::getQt(env, obj))->setBreakpoint((QString&) * (QString *) QtSupport::toQString(env, fileName, &_qstring_fileName), (int) lineNum, (int) identifier, (bool) enabled, (bool) pending);
	return;
}

JNIEXPORT void JNICALL
Java_org_kde_koala_Core_running(JNIEnv *env, jobject obj, jobject which, jboolean runs)
{
	((KDevCore*) QtSupport::getQt(env, obj))->running((KDevPart*) QtSupport::getQt(env, which), (bool) runs);
	return;
}

JNIEXPORT void JNICALL
Java_org_kde_koala_Core_message(JNIEnv *env, jobject obj, jstring str)
{
static QString * _qstring_str = 0;
	((KDevCore*) QtSupport::getQt(env, obj))->message((QString&) * (QString *) QtSupport::toQString(env, str, &_qstring_str));
	return;
}

JNIEXPORT jobject JNICALL
Java_org_kde_koala_Core_partManager(JNIEnv *env, jobject obj)
{
	return (jobject) QtSupport::objectForQtKey(env, (void *)((KDevCore*) QtSupport::getQt(env, obj))->partManager(), "org.kde.koala.PartManager");
}

JNIEXPORT jobject JNICALL
Java_org_kde_koala_Core_editor(JNIEnv *env, jobject obj)
{
	return (jobject) QtSupport::objectForQtKey(env, (void *)((KDevCore*) QtSupport::getQt(env, obj))->editor(), "org.kde.koala.Editor");
}

JNIEXPORT jobject JNICALL
Java_org_kde_koala_Core_statusBar(JNIEnv *env, jobject obj)
{
	return (jobject) QtSupport::objectForQtKey(env, (void *)((KDevCore*) QtSupport::getQt(env, obj))->statusBar(), "org.kde.qt.QStatusBar");
}

JNIEXPORT void JNICALL
Java_org_kde_koala_Core_setJavaSlotFactory(JNIEnv * env, jclass cls)
{
	JavaSlot::setJavaSlotFactory(new KDevJavaSlotFactory());
	return;
}



/***************************************************************************
                            EditorInterface.cpp -  description
                             -------------------
    begin                : Fri Dec 21 13:45:30 2001
    copyright            : (C) 2000-2001 Lost Highway Ltd.
    email                : Richard_Dale@tipitina.demon.co.uk
    generated by         : duke@tipitina on Fri Dec 21 13:45:30 2001, using kalyptus 0.91.
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
#include <keditor/interface.h>
#include <qstring.h>
#include <qcstring.h>

#include <qtjava/QtSupport.h>
#include <kdejava/KDESupport.h>
#include <kdejava/EditorInterface.h>

JNIEXPORT jobject JNICALL
Java_org_kde_koala_EditorInterface_actionCollection(JNIEnv *env, jobject obj)
{
	return (jobject) QtSupport::objectForQtKey(env, (void *)((KEditor::EditorInterface*) QtSupport::getQt(env, obj))->actionCollection(), "org.kde.koala.KActionCollection");
}

JNIEXPORT jobject JNICALL
Java_org_kde_koala_EditorInterface_editor(JNIEnv *env, jobject obj)
{
	return (jobject) QtSupport::objectForQtKey(env, (void *)((KEditor::EditorInterface*) QtSupport::getQt(env, obj))->editor(), "org.kde.koala.Editor");
}

//JNIEXPORT  JNICALL
//Java_org_kde_koala_EditorInterface_dcopInterface(JNIEnv *env, jobject obj)
//{
//	return () ((KEditor::EditorInterface*) QtSupport::getQt(env, obj))->dcopInterface();
//}


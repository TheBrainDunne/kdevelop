/***************************************************************************
                            KDevPart.java -  description
                             -------------------
    begin                : Wed Dec 19 12:26:58 2001
    copyright            : (C) 2000-2001 Lost Highway Ltd.
    email                : Richard_Dale@tipitina.demon.co.uk
    generated by         : duke@tipitina on Wed Dec 19 12:26:58 2001, using kalyptus 0.91.
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
***************************************************************************/

package org.kde.koala;

import org.kde.qt.*;
import java.util.*;
import java.lang.Error;

/**

 This is the base class for all components like doc tree view, LFV, RFV
 and so on.
 
 @short 
 This is the base class for all components like doc tree view, LFV, RFV
 and so on.
*/
public class KDevPart extends Part  {
	protected KDevPart(Class dummy){super((Class) null);}

	private native void newKDevPart( KDevApi api, QObject parent, String name);

/** 
 Constructs a component. The first parameters (of type KDevApi)
 should not be just by any part. It's just internally used.
     
*/
	public KDevPart( KDevApi api, QObject parent, String name) {
		super((Class) null);
		newKDevPart( api, parent, name);
	}
	private native void newKDevPart( KDevApi api);
	public KDevPart( KDevApi api) {
		super((Class) null);
		newKDevPart( api);
	}

/** 
 Destructs a component.
     
*/
	protected native void finalize() throws InternalError;

/** 
 Gives a reference to the application core
     
*/
	public native KDevCore core();

/** 
 Gives a reference to the current project component
     
*/
	public native KDevProject project();

/** 
 Gives a reference to the version control component
     
*/
	public native KDevVersionControl versionControl();

/** 
 Gives a reference to the language support component
     
*/
	public native KDevLanguageSupport languageSupport();

/** 
 Gives a reference to the make frontend component
     
*/
	public native KDevMakeFrontend makeFrontend();

/** 
 Gives a reference to the application frontend component
     
*/
	public native KDevAppFrontend appFrontend();

/** 
 Gives a reference to the class store
     
*/
	public native ClassStore classStore();

/** 
 Gives a reference to the DOM tree that represents
 the project file.
     
*/
	public native QDomDocument projectDom();
}

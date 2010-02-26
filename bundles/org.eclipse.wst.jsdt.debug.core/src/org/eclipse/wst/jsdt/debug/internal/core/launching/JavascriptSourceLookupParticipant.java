/*******************************************************************************
 * Copyright (c) 2010 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.eclipse.wst.jsdt.debug.internal.core.launching;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;
import org.eclipse.debug.core.sourcelookup.AbstractSourceLookupParticipant;
import org.eclipse.wst.jsdt.debug.core.model.IJavaScriptStackFrame;
import org.eclipse.wst.jsdt.debug.internal.core.JavaScriptDebugPlugin;

/**
 * Default source lookup participant
 * 
 * @since 1.0
 */
public class JavascriptSourceLookupParticipant extends AbstractSourceLookupParticipant {

	static final Object[] NO_SOURCE = new Object[0];
	static final Object[] FILE = new Object[1];

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.eclipse.debug.core.sourcelookup.ISourceLookupParticipant#getSourceName(java.lang.Object)
	 */
	public String getSourceName(Object object) throws CoreException {
		if (object instanceof IJavaScriptStackFrame) {
			return ((IJavaScriptStackFrame) object).getSourceName();
		}
		return null;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.eclipse.debug.core.sourcelookup.AbstractSourceLookupParticipant#findSourceElements(java.lang.Object)
	 */
	public Object[] findSourceElements(Object object) throws CoreException {
		if (object instanceof IJavaScriptStackFrame) {
			IJavaScriptStackFrame jframe = (IJavaScriptStackFrame) object;
			String path = jframe.getSourcePath();
			if (path != null) {
				// TODO not sure if we should do a search for the member if the URI path is a miss
				IFile file = (IFile) ResourcesPlugin.getWorkspace().getRoot().findMember(new Path(path), false);
				if (file != null) {
					return new IFile[] { file };
				}
				return showExternalSource(jframe.getSource(), path);
			}
		}
		return NO_SOURCE;
	}

	/**
	 * Shows the source in an external editor
	 * 
	 * @param source
	 * @param path
	 * @return the collection of files to show in external editors
	 */
	Object[] showExternalSource(String source, String path) {
		try {
			File tempdir = new File(System.getProperty("java.io.tmpdir")); //$NON-NLS-1$
			File file = new File(tempdir, path);
			file.deleteOnExit();
			FileWriter writer = new FileWriter(file);
			writer.write(source);
			writer.close();
			FILE[0] = file;
			return FILE;

		} catch (IOException e) {
			JavaScriptDebugPlugin.log(e);
			return NO_SOURCE;
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.eclipse.debug.core.sourcelookup.AbstractSourceLookupParticipant#isFindDuplicates()
	 */
	public boolean isFindDuplicates() {
		return true;
	}
}

/*******************************************************************************
 * Copyright (c) 2009 IBM Corporation and others All rights reserved. This
 * program and the accompanying materials are made available under the terms of
 * the Eclipse Public License v1.0 which accompanies this distribution, and is
 * available at http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors: IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.eclipse.wst.jsdt.debug.rhino.debugger;

import java.net.URI;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.eclipse.wst.jsdt.debug.rhino.transport.Constants;
import org.eclipse.wst.jsdt.debug.rhino.transport.JSONConstants;
import org.mozilla.javascript.debug.DebuggableScript;

/**
 * Rhino script implementation
 * 
 * @since 1.0
 */
public class ScriptSource {

	/**
	 * The id of the script
	 */
	private Long scriptId;
	/**
	 * Any special source properties to consider
	 */
	private final Map properties;
	/**
	 * The backing source String that was compiled to give us this proxy
	 */
	private final String source;
	/**
	 * If this {@link ScriptSource} was generated
	 */
	private final boolean generated;
	/**
	 * The ordered list of function names
	 */
	private ArrayList functionNames = null;
	/**
	 * The ordered list of line numbers
	 */
	private HashSet lineNumbers = null;
	/**
	 * The current collection of breakpoints
	 */
	private ArrayList breakpoints = new ArrayList();
	/**
	 * The location of the script
	 */
	private URI uri = null;
	/**
	 * The array of cached {@link FunctionSource}s
	 */
	private FunctionSource[] functionSources = null;
	/**
	 * Mapping of the {@link DebuggableScript} backing the FunctionSource to the FunctionSource handle
	 */
	private HashMap/*<DebuggableScript, FunctionSource>*/ scriptToFunction = new HashMap();
	
	/**
	 * Constructor
	 * 
	 * @param scriptId
	 * @param debuggableScript
	 * @param source
	 */
	public ScriptSource(DebuggableScript script, String source, URI uri, boolean generated, Map properties) {
		this.uri = uri;
		this.source = source;
		this.properties = properties;
		this.generated = generated;

		int[] rootlines = script.getLineNumbers();		
		DebuggableScript[] functions = collectFunctions(script);
		int flength = functions.length;
		lineNumbers = new HashSet(flength+rootlines.length);
		//dump in the line #'s from the root script
		for (int i = 0; i < rootlines.length; i++) {
			this.lineNumbers.add(new Integer(rootlines[i]+1));
		}
		//dump in the line numbers from the expanded set of functions
		if(flength > 0) {
			functionSources = new FunctionSource[flength];
			functionNames = new ArrayList(flength);
			
			int start = 0, end = 0;;
			for (int i = 0; i < functions.length; i++) {
				int[] lines = functions[i].getLineNumbers();
				if(lines != null && lines.length > 0) {
					start = lines[0] + 1;
					for (int j = 0; j < lines.length; j++) {
						int currentLine = lines[j] + 1;
						if (currentLine < start) {
							start = currentLine;
						}
						else if(currentLine > end) {
							end = currentLine;
						}
						lineNumbers.add(new Integer(currentLine));
					}
				}
				String name = functions[i].getFunctionName();
				name = (name == null ? Constants.EMPTY_STRING : name);
				functionSources[i] = new FunctionSource(this, name, computeFunctionSource(0, 0, source), start);
				functionNames.add(name);
				scriptToFunction.put(functions[i], functionSources[i]);
				start = 0;
			}
		}
	}

	/**
	 * Collects all of the {@link DebuggableScript} objects for the functions
	 * @param root the root script
	 * @return the collected array of functions - {@link DebuggableScript} - objects or an empty array
	 */
	DebuggableScript[] collectFunctions(DebuggableScript root) {
		ArrayList functions = new ArrayList();
		collectFunctions(root, functions);
		int size = functions.size();
		if(size < 1) {
			return RhinoDebugger.NO_SCRIPTS;
		}
		DebuggableScript[] funcs = new DebuggableScript[size];
		functions.toArray(funcs);
		return funcs;
	}
	
	/**
	 * Recursively collects function {@link DebuggableScript}s
	 * 
	 * @param root
	 * @param collector
	 */
	void collectFunctions(DebuggableScript root, List collector) {
		if(root.isFunction()) {
			collector.add(root);
		}
		for (int i = 0; i < root.getFunctionCount(); i++) {
			collectFunctions(root.getFunction(i), collector);
		}
	}
	
	/**
	 * Computes the functions' source from the given compiled buffer
	 * 
	 * @param start
	 * @param end
	 * @param source
	 * @return the string for the source or <code>null</code> if it could not be computed
	 */
	String computeFunctionSource(int start, int end, String source) {
		if(start > -1 && end <= source.length()) {
			return source.substring(start, end);
		}
		return null;
	}
	
	/**
	 * Sets the id for the script, <code>null</code> will throw and {@link IllegalArgumentException}
	 * @param id
	 * @throws IllegalArgumentException if <code>null</code> is specified as the new id
	 */
	public void setId(Long id) throws IllegalArgumentException {
		if(id == null) {
			throw new IllegalArgumentException();
		}
		this.scriptId = id;
	}
	
	/* (non-Javadoc)
	 * @see java.lang.Object#equals(java.lang.Object)
	 */
	public boolean equals(Object obj) {
		if(obj instanceof ScriptSource) {
			return this.uri.getPath().equals(((ScriptSource)obj).uri.getPath());
		}
		return false;
	}
	
	/* (non-Javadoc)
	 * @see java.lang.Object#hashCode()
	 */
	public int hashCode() {
		return this.uri.getPath().hashCode();
	}

	/**
	 * @return a new JSON map
	 */
	public Object toJSON() {
		HashMap result = new HashMap();
		result.put(JSONConstants.SCRIPT_ID, this.scriptId);
		result.put(JSONConstants.LOCATION, this.uri.getPath());
		result.put(JSONConstants.PROPERTIES, properties);
		result.put(JSONConstants.SOURCE, source);
		result.put(JSONConstants.GENERATED, Boolean.valueOf(generated));
		result.put(JSONConstants.LINES, (lineNumbers == null ? Collections.EMPTY_SET : lineNumbers));
		result.put(JSONConstants.FUNCTIONS, (functionNames == null ? Collections.EMPTY_LIST : functionNames));
		return result;
	}

	/**
	 * @return the id for this script object
	 */
	public Long getId() {
		return scriptId;
	}

	/**
	 * Returns the complete collection of breakpoints that match the given attributes or an empty collection, never <code>null</code>
	 * 
	 * @param functionName
	 * @param lineNumber
	 * @param frame
	 * @return all breakpoints that match the given attributes or an empty collection
	 */
	public synchronized Collection getBreakpoints(String functionName, Integer lineNumber, StackFrame frame) {
		ArrayList result = new ArrayList();
		for (Iterator iterator = breakpoints.iterator(); iterator.hasNext();) {
			Breakpoint breakpoint = (Breakpoint) iterator.next();
			if (breakpoint.matches(functionName, lineNumber, frame)) {
				result.add(breakpoint.getId());
			}
		}
		return result;
	}

	/**
	 * Returns if the line number or the function is valid wrt this script
	 * 
	 * @param lineNumber
	 * @param functionName
	 * @return true if the line number or function name is valid wrt this script
	 */
	public boolean isValid(Integer lineNumber, String functionName) {
		synchronized (lineNumbers) {
			if (lineNumber != null) {
				return lineNumbers.contains(lineNumber);
			}
		}
		synchronized (functionNames) {
			if (functionName != null) {
				return functionNames.contains(functionName);
			}
		}
		return functionName == null;
	}

	/**
	 * Copies items from the given object to this one
	 * @param script
	 */
	public void clone(ScriptSource script) {
		this.scriptId = script.scriptId;
		this.breakpoints = script.breakpoints;
	}
	
	/**
	 * Adds a breakpoint to this script
	 * 
	 * @param breakpoint
	 */
	public synchronized void addBreakpoint(Breakpoint breakpoint) {
		synchronized (breakpoints) {
			breakpoints.add(breakpoint);
		}
	}

	/**
	 * Removes a breakpoint from this script
	 * 
	 * @param breakpoint
	 */
	public synchronized void removeBreakpoint(Breakpoint breakpoint) {
		synchronized (breakpoints) {
			breakpoints.remove(breakpoint);
		}
	}

	/**
	 * @return the string location of this script
	 */
	public String getLocation() {
		return this.uri.getPath();
	}
	
	/**
	 * Returns the {@link FunctionSource} at the given position iff the given index
	 * is within the bounds of the sources array.
	 * 
	 * @param index
	 * @return the {@link FunctionSource} at the given index or <code>null</code>
	 */
	public FunctionSource functionAt(int index) {
		synchronized (functionSources) {
			if(functionSources != null && (index < functionSources.length && index > -1)) {
				return functionSources[index];
			}
		}
		return null;
	}
	
	/**
	 * Returns the {@link FunctionSource} for the given {@link DebuggableScript}
	 * 
	 * @param script
	 * @return the {@link FunctionSource} for the given {@link DebuggableScript} or <code>null</code>
	 */
	public FunctionSource getFunction(DebuggableScript script) {
		synchronized (scriptToFunction) {
			return (FunctionSource) scriptToFunction.get(script);
		}
	}
	
	/* (non-Javadoc)
	 * @see java.lang.Object#toString()
	 */
	public String toString() {
		StringBuffer buffer = new StringBuffer();
		buffer.append("ScriptSource: [id - ").append(scriptId).append("] [uri - ").append(uri.toString()).append("]"); //$NON-NLS-1$ //$NON-NLS-2$ //$NON-NLS-3$
		buffer.append("[generated - ").append(generated).append("]\n"); //$NON-NLS-1$ //$NON-NLS-2$
		if(lineNumbers != null) {
			buffer.append("\tline numbers: {"); //$NON-NLS-1$
			for (Iterator iter = lineNumbers.iterator(); iter.hasNext();) {
				buffer.append(iter.next());
				if(iter.hasNext()) {
					buffer.append(", "); //$NON-NLS-1$
				}
			}
			buffer.append("}\n"); //$NON-NLS-1$
		}
		if(functionNames != null) {
			buffer.append("\tfunction names: {"); //$NON-NLS-1$
			for (int i = 0; i < functionNames.size(); i++) {
				buffer.append(functionNames.get(i));
				if(i < functionNames.size()-1) {
					buffer.append(", "); //$NON-NLS-1$
				}
			}
			buffer.append("}\n"); //$NON-NLS-1$
		}
		if(functionSources != null) {
			buffer.append("\tfunction sources:\n"); //$NON-NLS-1$
			for (int i = 0; i < functionSources.length; i++) {
				buffer.append(functionSources[i]).append("\n"); //$NON-NLS-1$
			}
		}
		return buffer.toString();
	}
}

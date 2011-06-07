/*******************************************************************************
 * Copyright (c) 2010, 2011 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.eclipse.wst.jsdt.debug.internal.crossfire.jsdi;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.eclipse.core.resources.IMarkerDelta;
import org.eclipse.debug.core.DebugPlugin;
import org.eclipse.debug.core.IBreakpointListener;
import org.eclipse.debug.core.IBreakpointManager;
import org.eclipse.debug.core.model.IBreakpoint;
import org.eclipse.wst.jsdt.debug.core.breakpoints.IJavaScriptLineBreakpoint;
import org.eclipse.wst.jsdt.debug.core.jsdi.BooleanValue;
import org.eclipse.wst.jsdt.debug.core.jsdi.NullValue;
import org.eclipse.wst.jsdt.debug.core.jsdi.NumberValue;
import org.eclipse.wst.jsdt.debug.core.jsdi.StringValue;
import org.eclipse.wst.jsdt.debug.core.jsdi.UndefinedValue;
import org.eclipse.wst.jsdt.debug.core.jsdi.VirtualMachine;
import org.eclipse.wst.jsdt.debug.core.jsdi.event.EventQueue;
import org.eclipse.wst.jsdt.debug.core.jsdi.request.EventRequestManager;
import org.eclipse.wst.jsdt.debug.core.model.JavaScriptDebugModel;
import org.eclipse.wst.jsdt.debug.internal.crossfire.Constants;
import org.eclipse.wst.jsdt.debug.internal.crossfire.CrossFirePlugin;
import org.eclipse.wst.jsdt.debug.internal.crossfire.Tracing;
import org.eclipse.wst.jsdt.debug.internal.crossfire.event.CFEventQueue;
import org.eclipse.wst.jsdt.debug.internal.crossfire.transport.Attributes;
import org.eclipse.wst.jsdt.debug.internal.crossfire.transport.CFEventPacket;
import org.eclipse.wst.jsdt.debug.internal.crossfire.transport.CFRequestPacket;
import org.eclipse.wst.jsdt.debug.internal.crossfire.transport.CFResponsePacket;
import org.eclipse.wst.jsdt.debug.internal.crossfire.transport.Commands;
import org.eclipse.wst.jsdt.debug.internal.crossfire.transport.JSON;
import org.eclipse.wst.jsdt.debug.transport.DebugSession;
import org.eclipse.wst.jsdt.debug.transport.exception.DisconnectedException;
import org.eclipse.wst.jsdt.debug.transport.exception.TimeoutException;

/**
 * Default CrossFire implementation of {@link VirtualMachine}
 * 
 * @since 1.0
 */
public class CFVirtualMachine extends CFMirror implements VirtualMachine, IBreakpointListener {

	private final NullValue nullvalue = new CFNullValue(this);
	private final UndefinedValue undefinedvalue = new CFUndefinedValue(this);
	
	private final DebugSession session;
	private final CFEventRequestManager ermanager = new CFEventRequestManager(this);
	private final CFEventQueue queue = new CFEventQueue(this, ermanager);
	private boolean disconnected = false;
	
	private Map threads = null;
	private Map scripts = null;
	private Map breakpointHandles = new HashMap();
	
	/**
	 * Constructor
	 * 
	 * @param session
	 */
	public CFVirtualMachine(DebugSession session) {
		super();
		this.session = session;
		IBreakpointManager bpManager = DebugPlugin.getDefault().getBreakpointManager();
		bpManager.addBreakpointListener(this);
		initializeBreakpoints();
	}
	
	/**
	 * Collects all of the breakpoints 
	 */
	void initializeBreakpoints() {
		CFRequestPacket request = new CFRequestPacket(Commands.GET_BREAKPOINTS, null);
		CFResponsePacket response = sendRequest(request);
		if(response.isSuccess()) {
			List list = (List) response.getBody().get(Attributes.BREAKPOINTS);
			Map bp = null;
			for (Iterator i = list.iterator(); i.hasNext();) {
				bp = (Map) i.next();
				try {
					//TODO this works for now because we only handle breakpoints with url / line locations
					//in the future we need to create based on location / type
					Number id = (Number) bp.get(Attributes.HANDLE);
					Map loc = (Map) bp.get(Attributes.LOCATION);
					this.breakpointHandles.put(id, 
							new RemoteBreakpoint(
									this, 
									id, 
									(String)loc.get(Attributes.URL), 
									((Number)loc.get(Attributes.LINE)).intValue(), 
									((Boolean)bp.get(Attributes.ENABLED)).booleanValue(), 
									(String)bp.get(Attributes.CONDITION), 
									(String)bp.get(Attributes.TYPE)));
				}
				catch(Exception e) {}
			}
		}
		else if(TRACE) {
			Tracing.writeString("VM [failed getbreakpoints request]: "+JSON.serialize(request)); //$NON-NLS-1$
		}
	}
	
	/**
	 * Locates the breakpoint for the handle given in the map and updates its attributes
	 * 
	 * @param json the JSON map, cannot be <code>null</code>
	 */
	public void updateBreakpoint(Map json) {
		if(json != null) {
			Number id = (Number) json.get(Attributes.HANDLE);
			if(id != null) {
				RemoteBreakpoint bp = (RemoteBreakpoint) breakpointHandles.get(id);
				if(bp != null) {
					bp.setEnabled(RemoteBreakpoint.getEnabled(json));
					bp.setCondition(RemoteBreakpoint.getCondition(json));
				}
			}
		}
	}
	
	/**
	 * Adds or removes the breakpoint from the cache based on the <code>isset</code> attribute
	 * 
	 * @param json the JSON map, cannot be <code>null</code>
	 */
	public void toggleBreakpoint(Map json) {
		if(json != null) {
			Boolean isset = (Boolean)json.get(Attributes.SET);
			if(isset != null && isset.booleanValue()) {
				updateBreakpoint(json);
			}
			else {
				Number id = (Number) json.get(Attributes.HANDLE);
				breakpointHandles.remove(id);
			}
		}
	}
	
	/**
	 * @return the 'readiness' of the VM - i.e. is it in a state to process requests, etc
	 */
	boolean ready() {
		return !disconnected;
	}
	
	/**
	 * Sends an <code>createcontext</code> request for the given URL and returns the status of the request.
	 * 
	 * @param url the URL to open / update in the remote target, <code>null</code> is not accepted
	 * @return <code>true</code> if the request was successful, <code>false</code> otherwise
	 */
	boolean createContext(String url) {
		if(url != null && ready()) {
			CFRequestPacket request = new CFRequestPacket(Commands.CREATE_CONTEXT, null);
			request.getArguments().put(Attributes.URL, url);
			CFResponsePacket response = sendRequest(request);
			if(response.isSuccess()) {
				return true;
			}
			else if(TRACE) {
				Tracing.writeString("VM [failed createcontext request]: "+JSON.serialize(request)); //$NON-NLS-1$
			}
		}
		return false;
	}
	
	/**
	 * Sends the frame request
	 * @param contextid
	 * @param index
	 * @param includescopes
	 * @return
	 */
	CFStackFrame getFrame(String contextid, int index, boolean includescopes) {
		if(index > -1) {
			CFThreadReference thread = findThread(contextid);
			if(thread != null) {
				CFRequestPacket request = new CFRequestPacket(Commands.FRAME, thread.id());
				request.setArgument(Attributes.INDEX, new Integer(index));
				request.setArgument(Attributes.INCLUDE_SCOPES, new Boolean(includescopes));
				CFResponsePacket response = sendRequest(request);
				if(response.isSuccess()) {
					return new CFStackFrame(this, thread, response.getBody());
				}
				else if(TRACE) {
					Tracing.writeString("VM [failed frame request]: "+JSON.serialize(request)); //$NON-NLS-1$
				}
			}
		}
		return null;
		
	}
	
	/**
	 * Sends a request to enable the tool with the given name in the remote Crossfire server
	 * 
	 * @param tools the array of tool names to enable, <code>null</code> is not allowed
	 * @return <code>true</code> if the server reports the tool became enabled, <code>false</code> otherwise
	 */
	boolean enableTools(String[] tools) {
		if(tools != null && tools.length > 0 && ready()) {
			CFRequestPacket request = new CFRequestPacket(Commands.ENABLE_TOOLS, null);
			request.getArguments().put(Attributes.TOOLS, Arrays.asList(tools));
			CFResponsePacket response = sendRequest(request);
			if(response.isSuccess()) {
				//TODO handle the tool being enabled
				return true;
			}
			else if(TRACE) {
				Tracing.writeString("VM [failed enabletool request]: "+JSON.serialize(request)); //$NON-NLS-1$
			}
		}
		return false;
	}
	
	/**
	 * Sends a request to disable the tool with the given name in the remote Crossfire server
	 * 
	 * @param tools the array of tool names to disable, <code>null</code> is not allowed
	 * @return <code>true</code> if the server reports the tool became disabled, <code>false</code> otherwise
	 */
	boolean disableTools(String[] tools) {
		if(tools != null && tools.length > 0 && ready()) {
			CFRequestPacket request = new CFRequestPacket(Commands.DISABLE_TOOLS, null);
			request.getArguments().put(Attributes.TOOLS, Arrays.asList(tools));
			CFResponsePacket response = sendRequest(request);
			if(response.isSuccess()) {
				//TODO handle the tool being enabled
				return true;
			}
			else if(TRACE) {
				Tracing.writeString("VM [failed disabletool request]: "+JSON.serialize(request)); //$NON-NLS-1$
			}
		}
		return false;
	}
	
	/**
	 * Returns the complete listing of tools from Crossfire regardless of their enabled state.
	 * 
	 * @return the listing of tools or an empty list, never <code>null</code>
	 */
	List allTools() {
		if(ready()) {
			CFRequestPacket request = new CFRequestPacket(Commands.GET_TOOLS, null);
			CFResponsePacket response = sendRequest(request);
			if(response.isSuccess()) {
				//TODO do we want to make these first-class objects in our model so we can track state, etc for tools?
				List tools = (List) response.getBody().get(Attributes.TOOLS);
				if(tools != null) {
					return tools;
				}
			}
			else if(TRACE) {
				Tracing.writeString("VM [failed alltools request]: "+JSON.serialize(request)); //$NON-NLS-1$
			}
		}
		return Collections.EMPTY_LIST;
	}
	
	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.core.jsdi.VirtualMachine#resume()
	 */
	public void resume() {
		if(ready()) {
			if(threads != null) {
				Entry entry = null;
				for (Iterator iter = threads.entrySet().iterator(); iter.hasNext();) {
					entry = (Entry) iter.next();
					CFThreadReference thread = (CFThreadReference) entry.getValue();
					if(thread.isSuspended()) {
						CFRequestPacket request = new CFRequestPacket(Commands.CONTINUE, thread.id());
						CFResponsePacket response = sendRequest(request);
						if(response.isSuccess()) {
							if(thread.isSuspended()) {
								thread.markSuspended(false);
							}
						}
						else if(TRACE) {
							Tracing.writeString("VM [failed continue request][context: "+thread.id()+"]: "+JSON.serialize(request)); //$NON-NLS-1$ //$NON-NLS-2$
						}
					}
				}
			}
		}
	}

	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.core.jsdi.VirtualMachine#suspend()
	 */
	public void suspend() {
		if(ready()) {
			if(threads != null) {
				Entry entry = null;
				for (Iterator iter = threads.entrySet().iterator(); iter.hasNext();) {
					entry = (Entry) iter.next();
					CFThreadReference thread = (CFThreadReference) entry.getValue();
					if(thread.isRunning()) {
						CFRequestPacket request = new CFRequestPacket(Commands.SUSPEND, thread.id());
						CFResponsePacket response = sendRequest(request);
						if(response.isSuccess()) {
							if(!thread.isSuspended()) {
								thread.markSuspended(true);
							}
						}
						else if(TRACE) {
							Tracing.writeString("VM [failed suspend request]: "+JSON.serialize(request)); //$NON-NLS-1$
						}
					}
				}
			}
		}
	}

	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.core.jsdi.VirtualMachine#terminate()
	 */
	public void terminate() {
		if(ready()) {
			disconnectVM();
		}
	}

	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.core.jsdi.VirtualMachine#name()
	 */
	public String name() {
		return Messages.vm_name;
	}

	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.core.jsdi.VirtualMachine#description()
	 */
	public String description() {
		return Messages.crossfire_vm;
	}

	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.core.jsdi.VirtualMachine#version()
	 */
	public synchronized String version() {
		if(ready()) {
			CFRequestPacket request = new CFRequestPacket(Commands.VERSION, null);
			CFResponsePacket response = sendRequest(request);
			if(response.isSuccess()) {
				Map json = response.getBody();
				return (String) json.get(Commands.VERSION);
			}
			if(TRACE) {
				Tracing.writeString("VM [failed version request]: "+JSON.serialize(request)); //$NON-NLS-1$
			}
		}
		return Constants.UNKNOWN;
	}

	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.core.jsdi.VirtualMachine#allThreads()
	 */
	public synchronized List allThreads() {
		if(threads == null) {
			threads = new HashMap();
			CFRequestPacket request = new CFRequestPacket(Commands.LISTCONTEXTS, null);
			CFResponsePacket response = sendRequest(request);
			if(response.isSuccess()) {
				List contexts = (List) response.getBody().get(Attributes.CONTEXTS);
				for (Iterator iter = contexts.iterator(); iter.hasNext();) {
					Map json = (Map) iter.next();
					CFThreadReference thread = new CFThreadReference(this, json);
					threads.put(thread.id(), thread);
				}
			}
			else if(TRACE) {
				Tracing.writeString("VM [failed allthreads request]: "+JSON.serialize(request)); //$NON-NLS-1$
			}
		}
		return new ArrayList(threads.values());
	}

	/**
	 * Adds a thread to the listing
	 * 
	 * @param id
	 * @param url
	 * @return the new thread
	 */
	public CFThreadReference addThread(String id, String url) {
		if(threads == null) {
			allThreads();
		}
		CFThreadReference thread  = new CFThreadReference(this, id, url);
		threads.put(thread.id(), thread);
		return thread;
	}
	
	/**
	 * Removes the thread with the given id
	 * 
	 * @param id the id of the thread to remove
	 */
	public void removeThread(String id) {
		if(threads != null) {
			Object obj = threads.remove(id);
			if(TRACE && obj == null) {
				Tracing.writeString("VM [failed to remove thread]: "+id); //$NON-NLS-1$
			}
		}
	}
	
	/**
	 * Returns the thread with the given id or <code>null</code>
	 * 
	 * @param id
	 * @return the thread or <code>null</code>
	 */
	public synchronized CFThreadReference findThread(String id) {
		if(threads == null) {
			allThreads();
		}
		CFThreadReference thread = (CFThreadReference) threads.get(id);
		if(TRACE && thread == null) {
			Tracing.writeString("VM [failed to find thread]: "+id); //$NON-NLS-1$
		}
		return thread;
	}
	
	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.core.jsdi.VirtualMachine#allScripts()
	 */
	public synchronized List allScripts() {
		if(scripts == null) {
			scripts = new HashMap();
			List threads = allThreads();
			for (Iterator iter = threads.iterator(); iter.hasNext();) {
				CFThreadReference thread = (CFThreadReference) iter.next();
				CFRequestPacket request = new CFRequestPacket(Commands.SCRIPTS, thread.id());
				request.setArgument(Attributes.INCLUDE_SOURCE, Boolean.FALSE);
				CFResponsePacket response = sendRequest(request);
				if(response.isSuccess()) {
					List scriptz = (List) response.getBody().get(Commands.SCRIPTS);
					for (Iterator iter2 = scriptz.iterator(); iter2.hasNext();) {
						Map smap = (Map) iter2.next();
						if(smap != null) {
							CFScriptReference script = new CFScriptReference(this, thread.id(), smap); 
							scripts.put(script.id(), script);
						}
					}
				}
				else if(TRACE) {
					Tracing.writeString("VM [failed scripts request]: "+JSON.serialize(request)); //$NON-NLS-1$
				}
			}
			if(scripts.size() < 1) {
				scripts = null;
				return Collections.EMPTY_LIST;
			}
		}
		return new ArrayList(scripts.values());
	}

	/**
	 * Returns the script with the given id or <code>null</code>
	 * 
	 * @param id
	 * @return the thread or <code>null</code>
	 */
	public synchronized CFScriptReference findScript(String id) {
		if(scripts == null) {
			allScripts();
		}
		CFScriptReference script = null;
		if(scripts != null) {
			//the scripts collection can be null after a call to allScripts()
			//when the remote target had no scripts loaded. In this case
			//we do not keep the initialized collection so that any successive 
			//calls the this method or allScripts will cause the remote target 
			//to be asked for all of its scripts
			script = (CFScriptReference) scripts.get(id);
		}
		//if we find we have a script id that is not cached, we should try a lookup + add in the vm
		if(script == null) {
			if(TRACE) {
				Tracing.writeString("VM [failed to find script]: "+id); //$NON-NLS-1$
			}
		}
		return script;
	}
	
	/**
	 * Adds the given script to the listing
	 * 
	 * @param context_id
	 * @param json
	 * 
	 * @return the new script
	 */
	public CFScriptReference addScript(String context_id, Map json) {
		if(scripts == null) {
			allScripts();
		}
		CFScriptReference script = new CFScriptReference(this, context_id, json);
		scripts.put(script.id(), script);
		return script;
	}
	
	/**
	 * Removes all {@link CFScriptReference}s from the cache when the associated context is destroyed
	 * 
	 * @param contextid
	 */
	public void removeScriptsForContext(String contextid) {
		if(scripts != null) {
			Entry e = null;
			for(Iterator i = scripts.entrySet().iterator(); i.hasNext();) {
				e = (Entry) i.next();
				if(contextid.equals(((CFScriptReference)e.getValue()).context())) {
					i.remove();
				}
			}
		}
	}
	
	/**
	 * Removes the script with the given id form the listing
	 * 
	 * @param id the script to remove
	 */
	public void removeScript(String id) {
		if(scripts != null) {
			Object obj = scripts.remove(id);
			if(TRACE && obj == null) {
				Tracing.writeString("VM [failed to remove script]: "+id); //$NON-NLS-1$
			}
		}
	}
	
	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.core.jsdi.VirtualMachine#dispose()
	 */
	public synchronized void dispose() {
		try {
			if(TRACE) {
				Tracing.writeString("VM [disposing]"); //$NON-NLS-1$
			}
			queue.dispose();
			ermanager.dispose();
		}
		finally {
			//fall-back in case the VM has been disposed but not disconnected
			disconnectVM();
		}
	}

	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.core.jsdi.VirtualMachine#mirrorOfUndefined()
	 */
	public UndefinedValue mirrorOfUndefined() {
		return undefinedvalue;
	}

	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.core.jsdi.VirtualMachine#mirrorOfNull()
	 */
	public NullValue mirrorOfNull() {
		return nullvalue;
	}

	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.core.jsdi.VirtualMachine#mirrorOf(boolean)
	 */
	public BooleanValue mirrorOf(boolean bool) {
		return new CFBooleanValue(this, bool);
	}

	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.core.jsdi.VirtualMachine#mirrorOf(java.lang.Number)
	 */
	public NumberValue mirrorOf(Number number) {
		return new CFNumberValue(this, number);
	}

	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.core.jsdi.VirtualMachine#mirrorOf(java.lang.String)
	 */
	public StringValue mirrorOf(String string) {
		return new CFStringValue(this, string);
	}

	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.core.jsdi.VirtualMachine#eventRequestManager()
	 */
	public synchronized EventRequestManager eventRequestManager() {
		return ermanager;
	}

	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.core.jsdi.VirtualMachine#eventQueue()
	 */
	public synchronized EventQueue eventQueue() {
		return queue;
	}
	
	/**
	 * Receives an {@link CFEventPacket} from the underlying {@link DebugSession}, 
	 * waiting for the {@link VirtualMachine#DEFAULT_TIMEOUT}.
	 * 
	 * @return the next {@link CFEventPacket} never <code>null</code>
	 * @throws TimeoutException
	 * @throws DisconnectedException
	 */
	public CFEventPacket receiveEvent() throws TimeoutException, DisconnectedException {
		return (CFEventPacket) session.receive(CFEventPacket.EVENT, DEFAULT_TIMEOUT);
	}

	/**
	 * Receives an {@link CFEventPacket} from the underlying {@link DebugSession}, 
	 * waiting for the {@link VirtualMachine#DEFAULT_TIMEOUT}.
	 * @param timeout
	 * @return the next {@link CFEventPacket} never <code>null</code>
	 * @throws TimeoutException
	 * @throws DisconnectedException
	 */
	public CFEventPacket receiveEvent(int timeout) throws TimeoutException, DisconnectedException {
		return (CFEventPacket) session.receive(CFEventPacket.EVENT, timeout);
	}
	
	/**
	 * Sends a request to the underlying {@link DebugSession}, waiting
	 * for the {@link VirtualMachine#DEFAULT_TIMEOUT}.
	 * 
	 * @param request
	 * @return the {@link CFResponsePacket} for the request
	 */
	public CFResponsePacket sendRequest(CFRequestPacket request) {
		try {
			session.send(request);
			return (CFResponsePacket) session.receiveResponse(request.getSequence(), 3000);
		}
		catch(DisconnectedException de) {
			disconnectVM();
			handleException(de.getMessage(), (de.getCause() == null ? de : de.getCause()));
		}
		catch(TimeoutException te) {
			CrossFirePlugin.log(te);
		}
		return CFResponsePacket.FAILED;
	}
	
	/**
	 * disconnects the VM
	 */
	public synchronized void disconnectVM() {
		if (disconnected) {
			// no-op it is already disconnected
			if(TRACE) {
				Tracing.writeString("VM [already disconnected]"); //$NON-NLS-1$
			}
			return;
		}
		if(TRACE) {
			Tracing.writeString("VM [disconnecting]"); //$NON-NLS-1$
		}
		try {
			if(threads != null) {
				threads.clear();
			}
			if(scripts != null) {
				scripts.clear();
			}
			this.queue.dispose();
			this.ermanager.dispose();
			this.session.dispose();
		} finally {
			disconnected = true;
			DebugPlugin.getDefault().getBreakpointManager().removeBreakpointListener(this);
		}
	}

	/* (non-Javadoc)
	 * @see org.eclipse.debug.core.IBreakpointListener#breakpointAdded(org.eclipse.debug.core.model.IBreakpoint)
	 */
	public void breakpointAdded(IBreakpoint breakpoint) {
		if (JavaScriptDebugModel.MODEL_ID.equals(breakpoint.getModelIdentifier())) {
			if (breakpoint instanceof IJavaScriptLineBreakpoint) {
				//TODO check handle map send request as needed
			}
	 	}
	}

	/* (non-Javadoc)
	 * @see org.eclipse.debug.core.IBreakpointListener#breakpointRemoved(org.eclipse.debug.core.model.IBreakpoint, org.eclipse.core.resources.IMarkerDelta)
	 */
	public void breakpointRemoved(IBreakpoint breakpoint, IMarkerDelta delta) {
		if (JavaScriptDebugModel.MODEL_ID.equals(breakpoint.getModelIdentifier())) {
			if (breakpoint instanceof IJavaScriptLineBreakpoint) {
				//TODO check handle map send request as needed
			}
		}
	}

	/* (non-Javadoc)
	 * @see org.eclipse.debug.core.IBreakpointListener#breakpointChanged(org.eclipse.debug.core.model.IBreakpoint, org.eclipse.core.resources.IMarkerDelta)
	 */
	public void breakpointChanged(IBreakpoint breakpoint, IMarkerDelta delta) {
		if (JavaScriptDebugModel.MODEL_ID.equals(breakpoint.getModelIdentifier())) {
			if (breakpoint instanceof IJavaScriptLineBreakpoint) {
				//TODO check handle map send request as needed
			}
		}
	}
}

/*******************************************************************************
 * Copyright (c) 2010 IBM Corporation and others All rights reserved. This
 * program and the accompanying materials are made available under the terms of
 * the Eclipse Public License v1.0 which accompanies this distribution, and is
 * available at http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors: IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.eclipse.wst.jsdt.debug.internal.core.jsdi.connect;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.eclipse.wst.jsdt.debug.core.jsdi.connect.Packet;
import org.eclipse.wst.jsdt.debug.core.jsdi.json.JSONConstants;

/**
 * Default request implementation using JSON
 * 
 * @since 1.0
 */
public class Request extends Packet {

	private final String command;
	private final Map arguments = Collections.synchronizedMap(new HashMap());

	/**
	 * Constructor
	 * 
	 * @param command the command, <code>null</code> is not accepted
	 */
	public Request(String command) {
		super(JSONConstants.REQUEST);
		Assert.isNotNull(command);
		this.command = command.intern();
	}

	/**
	 * Constructor
	 * 
	 * @param json map of JSON attributes, <code>null</code> is not accepted
	 */
	public Request(Map json) {
		super(json);
		Assert.isNotNull(json);
		String packetCommand = (String) json.get(JSONConstants.COMMAND);
		this.command = packetCommand.intern();
		Map packetArguments = (Map) json.get(JSONConstants.ARGUMENTS);
		arguments.putAll(packetArguments);
	}

	/**
	 * Returns the command that this {@link Request} will was created for.<br>
	 * <br>
	 * This method cannot return <code>null</code>
	 * 
	 * @return the underlying command, never <code>null</code>
	 */
	public String getCommand() {
		return command;
	}

	/**
	 * Returns the complete collection of JSON arguments in the {@link Request}.<br>
	 * <br>
	 * This method cannot return <code>null</code>, an empty map will be returned
	 * if there are no properties.
	 * 
	 * @return the arguments or an empty map never <code>null</code>
	 */
	public Map getArguments() {
		return arguments;
	}

	/**
	 * Sets the given argument in the JSON map.
	 * 
	 * @param key the key for the attribute, <code>null</code> is not accepted
	 * @param argument the value for the argument, <code>null</code> is not accepted
	 */
	public void setArgument(String key, Object argument) {
		Assert.isNotNull(key);
		Assert.isNotNull(argument);
		arguments.put(key, argument);
	}

	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.internal.core.jsdi.connect.Packet#toJSON()
	 */
	public Map toJSON() {
		Map json = super.toJSON();
		json.put(JSONConstants.COMMAND, command);
		json.put(JSONConstants.ARGUMENTS, arguments);
		return json;
	}
}

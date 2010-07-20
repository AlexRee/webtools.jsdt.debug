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
package org.eclipse.wst.jsdt.debug.internal.chrome.transport;

import java.util.Collections;
import java.util.Map;

import org.eclipse.wst.jsdt.debug.transport.packet.Event;

/**
 * Event packet
 * 
 * @since 1.0
 */
public class EventPacketImpl extends PacketImpl implements Event {

	/**
	 * The type of this packet
	 */
	public static final String EVENT = "event"; //$NON-NLS-1$
	
	private String event = null;
	private Map body = null;
	
	/**
	 * Constructor
	 * @param event
	 */
	public EventPacketImpl(String event) {
		super(EVENT);
		if(event == null) {
			throw new IllegalArgumentException(Messages.cannot_create_packet_with_no_event);
		}
		this.event = event;
	}

	/**
	 * Constructor
	 * 
	 * @param json
	 */
	public EventPacketImpl(Map json) {
		super(json);
		this.event = (String) json.get(EVENT);
		if(event == null) {
			throw new IllegalArgumentException(Messages.no_event_found_in_json);
		}
	}
	
	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.transport.packet.Event#getEvent()
	 */
	public String getEvent() {
		return event;
	}

	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.transport.packet.Event#getBody()
	 */
	public Map getBody() {
		if(body == null) {
			return Collections.EMPTY_MAP;
		}
		return body;
	}

	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.internal.chrome.transport.PacketImpl#toJSON()
	 */
	public Map toJSON() {
		Map json = super.toJSON();
		json.put(Attributes.COMMAND, event);
		if(body != null) {
			json.put(Attributes.BODY, body);
		}
		return json;
	}
	
	/* (non-Javadoc)
	 * @see java.lang.Object#toString()
	 */
	public String toString() {
		StringBuffer buffer = new StringBuffer();
		Object json = toJSON();
		buffer.append("EventPacketImpl: "); //$NON-NLS-1$
		JSON.writeValue(json, buffer);
		return buffer.toString();
	}
}

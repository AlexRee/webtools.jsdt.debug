/*******************************************************************************
 * Copyright (c) 2010 IBM Corporation and others All rights reserved. This
 * program and the accompanying materials are made available under the terms of
 * the Eclipse Public License v1.0 which accompanies this distribution, and is
 * available at http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors: IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.eclipse.wst.jsdt.debug.internal.crossfire.transport;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.EOFException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.Reader;
import java.io.Writer;
import java.net.Socket;
import java.util.Map;

import org.eclipse.wst.jsdt.debug.internal.crossfire.Constants;
import org.eclipse.wst.jsdt.debug.internal.crossfire.CrossFirePlugin;
import org.eclipse.wst.jsdt.debug.internal.crossfire.Tracing;

/**
 * A specialized {@link Connection} that communicates using {@link Socket}s
 * 
 * @since 1.0
 */
public class SocketConnection implements Connection {

	private Writer writer;
	private Reader reader;
	private Socket socket;

	/**
	 * Constructor
	 * 
	 * @param socket the underlying {@link Socket}, <code>null</code> is not accepted
	 * 
	 * @throws IOException
	 */
	public SocketConnection(Socket socket) throws IOException {
		if(socket == null) {
			throw new IllegalArgumentException("You cannot create a new SocketConnection on a null Socket"); //$NON-NLS-1$
		}
		this.socket = socket;
		writer = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream(), Constants.UTF_8));
		reader = new BufferedReader(new InputStreamReader(socket.getInputStream(), Constants.UTF_8));
	}

	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.internal.core.jsdi.connect.Connection#isOpen()
	 */
	public boolean isOpen() {
		return !socket.isClosed();
	}

	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.internal.core.jsdi.connect.Connection#close()
	 */
	public void close() throws IOException {
		socket.close();
	}
	
	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.internal.core.jsdi.connect.Connection#writePacket(org.eclipse.wst.jsdt.debug.internal.core.jsdi.connect.Packet)
	 */
	public void writePacket(Packet packet) throws IOException {
		String serialized = JSON.serialize(packet);
		if(Packet.TRACE) {
			Tracing.writeString("WRITE PACKET: "+serialized); //$NON-NLS-1$
		}
		writer.write(serialized);
		writer.flush();
	}

	/**
	 * Writes the standard handshake packet to connect
	 * 
	 * @param packet
	 * @throws IOException
	 */
	public void writeHandShake() throws IOException {
		if(Packet.TRACE) {
			Tracing.writeString("WRITE HANDSHAKE: "+HandShake.getHandshake()); //$NON-NLS-1$
		}
		writer.write(HandShake.getHandshake());
		writer.flush();
		waitForReadyRead();
	}
	
	/**
	 * Method to wait for the socket reader to become ready after the handshake
	 * 
	 * @throws IOException
	 */
	void waitForReadyRead() throws IOException {
		long timeout = System.currentTimeMillis() + 1000;
		boolean timedout = System.currentTimeMillis() > timeout;
		while(!reader.ready() && !timedout) {
			try {
				Thread.sleep(100);
				timedout = System.currentTimeMillis() > timeout;
			} catch (InterruptedException e) {
				CrossFirePlugin.log(e);
			}
		}
		if(timedout) {
			if(Packet.TRACE) {
				Tracing.writeString("HANDSHAKE: Timed out waiting for ready read from handshake"); //$NON-NLS-1$
			}
			//throw new IOException("Waiting for the socket to become available after receiving handshake timed out."); //$NON-NLS-1$
		}
	}
	
	/**
	 * Reads the {@link HandShake} packet from the the stream
	 * 
	 * @return the {@link HandShake}, never <code>null</code>
	 * @throws IOException
	 */
	public Packet readHandShake() throws IOException {
		StringBuffer buffer = new StringBuffer();
		//read the header first
		int c = 0;
		boolean r = false;
		while((c = reader.read()) > -1) {
			buffer.append((char)c);
			if(r) {
				if(c == '\n') {
					break;
				}
			}
			r = c == '\r';
		}
		if(buffer.toString().equals(HandShake.getHandshake())) {
			HandShake ack = new HandShake();
			if(Packet.TRACE) {
				Tracing.writeString("ACK HANDSHAKE: "+ack); //$NON-NLS-1$
			}
			return ack;
		}
		while(reader.ready()) {
			c = reader.read();
			if(Packet.TRACE) {
				Tracing.writeString("Reading extra post-amble from socket: "+(char)c); //$NON-NLS-1$
			}
		}
		
		throw new IOException("Did not get correct CrossFire handshake"); //$NON-NLS-1$
	}
	
	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.internal.core.jsdi.connect.Connection#readPacket()
	 */
	public Packet readPacket() throws IOException {
		StringBuffer buffer = new StringBuffer();
		int c = -1;
		boolean len = false;
		while((c = reader.read()) > -1) {
			if(c == '\r') {
				break;
			}
			if(len) {
				buffer.append((char)c);
				continue;
			}
			len = c == ':';
		}
		int length = 0;
		try {
			length = Integer.parseInt(buffer.toString());
		} catch (NumberFormatException e) {
			throw new IOException("Failed to parse content length: " + buffer.toString()); //$NON-NLS-1$
		}
		c = reader.read();
		if(c != '\n') {
			throw new IOException("Failed to parse content length: " + buffer.toString() + "next char was not '\n'" + (char)c); //$NON-NLS-1$ //$NON-NLS-2$
		}
		char[] message = new char[length];
		int n = 0;
		int off = 0;
		while (n < length) {
			int count = reader.read(message, off + n, length - n);
			if (count < 0) {
				throw new EOFException();
			}
			n += count;
		}
		if(Packet.TRACE) {
			Tracing.writeString("READ PACKET: [length - "+length+"]"+new String(message)); //$NON-NLS-1$ //$NON-NLS-2$
		}
		Map json = (Map) JSON.read(new String(message));
		String type = Packet.getType(json);
		if (Event.EVENT.equals(type)) {
			return new Event(json);
		}
		if (Request.REQUEST.equals(type)) {
			return new Request(json);
		}
		if (Response.RESPONSE.equals(type)) {
			return new Response(json);
		}
		throw new IOException("Unknown packet type: " + type); //$NON-NLS-1$
	}
}

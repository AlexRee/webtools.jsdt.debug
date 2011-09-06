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
package org.eclipse.wst.jsdt.debug.internal.crossfire.jsdi;

import java.util.Map;

import org.eclipse.wst.jsdt.debug.core.jsdi.StackFrame;
import org.eclipse.wst.jsdt.debug.core.jsdi.Value;
import org.eclipse.wst.jsdt.debug.core.jsdi.Variable;
import org.eclipse.wst.jsdt.debug.internal.crossfire.transport.Attributes;

/**
 * Default implementation of a {@link Variable} for Crossfire
 * 
 * @since 1.0
 */
public class CFVariable extends CFProperty implements Variable {

	/**
	 * Constructor
	 * 
	 * @param vm
	 * @param frame
	 * @param name
	 * @param ref
	 * @param values
	 */
	public CFVariable(CFVirtualMachine vm, CFStackFrame frame, String name, Number ref, Map values) {
		super(vm, frame, name, ref);
		Value value = null;
		if(values != null) {
			String kind = (String) values.get(Attributes.TYPE);
			//if we have a primitive type create it value now
			if(kind != null) {
				if(kind.equals(Attributes.STRING)) {
					value = new CFStringValue(vm, (String) values.get(Attributes.VALUE));
				}
				else if(kind.equals(Attributes.NUMBER)) {
					Object o = values.get(Attributes.VALUE);
					if(o instanceof Number) {
						value = new CFNumberValue(vm, (Number)o);
					}
					else if(o instanceof String) {
						value = new CFNumberValue(vm, (String)o);
					}
				}
				else if(kind.equals(Attributes.BOOLEAN)) {
					value = new CFBooleanValue(vm, ((Boolean)values.get(Attributes.VALUE)).booleanValue());
				}
				if(Attributes.THIS.equals(name)) {
					//special object that has no lookup so we have to pre-populate the properties
					value = new CFObjectReference(crossfire(), frame, values);
				}
				if(CFUndefinedValue.UNDEFINED.equals(kind)) {
					value = crossfire().mirrorOfUndefined();
				}
			}
		}
		else {
			value = crossfire().mirrorOfNull();
		}
		if(value != null) {
			setValue(value);
		}
	}

	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.core.jsdi.Variable#isArgument()
	 */
	public boolean isArgument() {
		return false;
	}

	/* (non-Javadoc)
	 * @see org.eclipse.wst.jsdt.debug.core.jsdi.Variable#isVisible(org.eclipse.wst.jsdt.debug.core.jsdi.StackFrame)
	 */
	public boolean isVisible(StackFrame frame) {
		if(frame instanceof CFStackFrame) {
			return ((CFStackFrame)frame).isVisible(this);
		}
		return false;
	}
}
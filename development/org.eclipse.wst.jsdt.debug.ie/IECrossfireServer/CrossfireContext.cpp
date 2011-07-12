/*******************************************************************************
 * Copyright (c) 2011 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/


#include "StdAfx.h"
#include "CrossfireContext.h"

#include "JSONParser.h"

/* command: backtrace */
const wchar_t* CrossfireContext::COMMAND_BACKTRACE = L"backtrace";
const wchar_t* CrossfireContext::KEY_FRAMES = L"frames";
const wchar_t* CrossfireContext::KEY_FROMFRAME = L"fromFrame";
const wchar_t* CrossfireContext::KEY_TOFRAME = L"toFrame";
const wchar_t* CrossfireContext::KEY_TOTALFRAMES = L"totalFrames";

/* command: continue */
const wchar_t* CrossfireContext::COMMAND_CONTINUE = L"continue";

/* command: evaluate */
const wchar_t* CrossfireContext::COMMAND_EVALUATE = L"evaluate";
const wchar_t* CrossfireContext::KEY_EXPRESSION = L"expression";
const wchar_t* CrossfireContext::KEY_RESULT = L"result";

/* command: frame */
const wchar_t* CrossfireContext::COMMAND_FRAME = L"frame";
const wchar_t* CrossfireContext::KEY_FRAME = L"frame";
const wchar_t* CrossfireContext::KEY_INDEX = L"index";

/* command: inspect */
const wchar_t* CrossfireContext::COMMAND_INSPECT = L"inspect";

/* command: lookup */
const wchar_t* CrossfireContext::COMMAND_LOOKUP = L"lookup";

/* command: scope */
const wchar_t* CrossfireContext::COMMAND_SCOPE = L"scope";
const wchar_t* CrossfireContext::KEY_SCOPE = L"scope";
const wchar_t* CrossfireContext::KEY_SCOPEINDEX = L"scopeIndex";

/* command: scopes */
const wchar_t* CrossfireContext::COMMAND_SCOPES = L"scopes";
const wchar_t* CrossfireContext::KEY_FROMSCOPE = L"fromScope";
const wchar_t* CrossfireContext::KEY_SCOPES = L"scopes";
const wchar_t* CrossfireContext::KEY_TOSCOPE = L"toScope";
const wchar_t* CrossfireContext::KEY_TOTALSCOPECOUNT = L"totalScopeCount";

/* command: script */
const wchar_t* CrossfireContext::COMMAND_SCRIPT = L"script";
const wchar_t* CrossfireContext::KEY_SCRIPT = L"script";

/* command: scripts */
const wchar_t* CrossfireContext::COMMAND_SCRIPTS = L"scripts";
const wchar_t* CrossfireContext::KEY_SCRIPTS = L"scripts";

/* command: suspend */
const wchar_t* CrossfireContext::COMMAND_SUSPEND = L"suspend";
const wchar_t* CrossfireContext::KEY_STEPACTION = L"stepAction";
const wchar_t* CrossfireContext::VALUE_IN = L"in";
const wchar_t* CrossfireContext::VALUE_NEXT = L"next";
const wchar_t* CrossfireContext::VALUE_OUT = L"out";

/* event: onBreak */
const wchar_t* CrossfireContext::EVENT_ONBREAK = L"onBreak";
const wchar_t* CrossfireContext::KEY_CAUSE = L"cause";
const wchar_t* CrossfireContext::KEY_MESSAGE = L"message";
const wchar_t* CrossfireContext::KEY_TITLE = L"title";

/* event: onScript */
const wchar_t* CrossfireContext::EVENT_ONSCRIPT = L"onScript";

/* event: onToggleBreakpoint */
const wchar_t* CrossfireContext::EVENT_ONTOGGLEBREAKPOINT = L"onToggleBreakpoint";
const wchar_t* CrossfireContext::KEY_SET = L"set";

/* shared */
const wchar_t* CrossfireContext::KEY_BREAKPOINT = L"breakpoint";
const wchar_t* CrossfireContext::KEY_CONTEXTID = L"contextId";
const wchar_t* CrossfireContext::KEY_FRAMEINDEX = L"frameIndex";
const wchar_t* CrossfireContext::KEY_HANDLE = L"handle";
const wchar_t* CrossfireContext::KEY_LOCATION = L"location";
const wchar_t* CrossfireContext::KEY_INCLUDESCOPES = L"includeScopes";
const wchar_t* CrossfireContext::KEY_INCLUDESOURCE = L"includeSource";
const wchar_t* CrossfireContext::KEY_LINE = L"line";
const wchar_t* CrossfireContext::KEY_TYPE = L"type";
const wchar_t* CrossfireContext::KEY_URL = L"url";

/* breakpoint objects */
const wchar_t* CrossfireContext::BPTYPE_LINE = L"line";

/* frame objects */
const wchar_t* CrossfireContext::KEY_FUNCTIONNAME = L"functionName";

/* object objects */
const wchar_t* CrossfireContext::JSVALUE_BOOLEAN = L"Boolean";
const wchar_t* CrossfireContext::JSVALUE_FUNCTION = L"\"function\"";
const wchar_t* CrossfireContext::JSVALUE_NUMBER = L"Number";
const wchar_t* CrossfireContext::JSVALUE_NULL = L"Null";
const wchar_t* CrossfireContext::JSVALUE_STRING = L"String";
const wchar_t* CrossfireContext::JSVALUE_TRUE = L"true";
const wchar_t* CrossfireContext::JSVALUE_UNDEFINED = L"Undefined";
const wchar_t* CrossfireContext::KEY_LOCALS = L"locals";
const wchar_t* CrossfireContext::KEY_THIS = L"this";
const wchar_t* CrossfireContext::KEY_VALUE = L"value";
const wchar_t* CrossfireContext::VALUE_BOOLEAN = L"boolean";
const wchar_t* CrossfireContext::VALUE_FUNCTION = L"function";
const wchar_t* CrossfireContext::VALUE_NUMBER = L"number";
const wchar_t* CrossfireContext::VALUE_OBJECT = L"object";
const wchar_t* CrossfireContext::VALUE_STRING = L"string";
const wchar_t* CrossfireContext::VALUE_UNDEFINED = L"undefined";

/* script objects */
const wchar_t* CrossfireContext::KEY_COLUMNOFFSET = L"columnOffset";
const wchar_t* CrossfireContext::KEY_LINECOUNT = L"lineCount";
const wchar_t* CrossfireContext::KEY_LINEOFFSET = L"lineOffset";
const wchar_t* CrossfireContext::KEY_SOURCE = L"source";
const wchar_t* CrossfireContext::KEY_SOURCELENGTH = L"sourceLength";
const wchar_t* CrossfireContext::VALUE_EVALCODE = L"eval code";
const wchar_t* CrossfireContext::VALUE_EVALLEVEL = L"eval-level";
const wchar_t* CrossfireContext::VALUE_TOPLEVEL = L"top-level";

/* constants */
const wchar_t* CrossfireContext::ID_PREAMBLE = L"xfIE::";

CrossfireContext::CrossfireContext(DWORD processId, wchar_t* url, CrossfireServer* server) {
	static int s_counter = 0;
	m_processId = processId;
	std::wstringstream stream;
	stream << ID_PREAMBLE;
	stream << m_processId;
	stream << "-";
	stream << s_counter++;
	m_name = _wcsdup((wchar_t*)stream.str().c_str());

	CComObject<IEDebugger>* result = NULL;
	HRESULT hr = CComObject<IEDebugger>::CreateInstance(&result);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext(): CoCreateInstance(IIEDebugger) failed", hr);
		return;
	}
	m_asyncEvals = new std::vector<JSEvalCallback*>;
	m_debugger = result;
	m_debugger->AddRef(); /* CComObject::CreateInstance gives initial ref count of 0 */
	m_breakpoints = new std::map<unsigned int, CrossfireBreakpoint*>;
	m_cpcApplicationNodeEvents = 0;
	m_server = server;
	m_debugApplicationThread = NULL;
	m_debuggerHooked = false;
	m_lastInitializedScriptNode = NULL;
	m_nextObjectHandle = 1;
	m_objects = new std::map<unsigned int, JSObject*>;
	m_pendingScriptLoads = new std::map<IDebugApplicationNode*, PendingScriptLoad*>;
	m_running = true;
	m_scriptNodes = new std::map<std::wstring, IDebugApplicationNode*>;
	m_url = _wcsdup(url);
	hookDebugger();
}

CrossfireContext::~CrossfireContext() {
	if (m_asyncEvals) {
		std::vector<JSEvalCallback*>::iterator iterator = m_asyncEvals->begin();
		while (iterator != m_asyncEvals->end()) {
			delete *iterator;
			iterator++;
		}
		delete m_asyncEvals;
	}

	if (m_breakpoints) {
		std::map<unsigned int, CrossfireBreakpoint*>::iterator iterator = m_breakpoints->begin();
		while (iterator != m_breakpoints->end()) {
			delete iterator->second;
			iterator++;
		}
		delete m_breakpoints;
	}

	if (m_lastInitializedScriptNode) {
		m_lastInitializedScriptNode->Release();
	}

	if (m_pendingScriptLoads) {
		std::map<IDebugApplicationNode*, PendingScriptLoad*>::iterator iterator = m_pendingScriptLoads->begin();
		while (iterator != m_pendingScriptLoads->end()) {
			iterator->first->Release();
			iterator->second->detach();
			iterator->second->Release();
			iterator++;
		}
		delete m_pendingScriptLoads;
	}

	if (m_scriptNodes) {
		std::map<std::wstring, IDebugApplicationNode*>::iterator iterator = m_scriptNodes->begin();
		while (iterator != m_scriptNodes->end()) {
			iterator->second->Release();
			iterator++;
		}
		delete m_scriptNodes;
	}

	if (m_cpcApplicationNodeEvents) {
		CComPtr<IRemoteDebugApplication> debugApplication = NULL;
		if (!getDebugApplication(&debugApplication)) {
			Logger::error("~CrossfireContext(): cannot Unadvise() the root node");
		} else {
			CComPtr<IDebugApplicationNode> rootNode = NULL;
			HRESULT hr = debugApplication->GetRootNode(&rootNode);
			if (FAILED(hr)) {
				Logger::error("~CrossfireContext(): GetRootNode() failed", hr);
			} else {
				CComPtr<IConnectionPointContainer> connectionPointContainer = NULL;
				hr = rootNode->QueryInterface(IID_IConnectionPointContainer, (void**)&connectionPointContainer);
				if (FAILED(hr)) {
					Logger::error("~CrossfireContext(): QI(IConnectionPointContainer) failed", hr);
				} else {
					CComPtr<IConnectionPoint> connectionPoint = NULL;
					hr = connectionPointContainer->FindConnectionPoint(IID_IDebugApplicationNodeEvents, &connectionPoint);
					if (FAILED(hr)) {
						Logger::error("~CrossfireContext(): FindConnectionPoint() failed", hr);
					} else {
						hr = connectionPoint->Unadvise(m_cpcApplicationNodeEvents);
						if (FAILED(hr)) {
							Logger::error("~CrossfireContext(): Unadvise() failed", hr);
						}
					}
				}
			}
		}
	}

	if (m_name) {
		free(m_name);
	}
	if (m_url) {
		free(m_url);
	}
	if (m_objects) {
		clearObjects();
		delete m_objects;
	}
	if (m_debuggerHooked) {
		unhookDebugger();
	}
	if (m_debugger) {
		m_debugger->Release();
	}
	if (m_debugApplicationThread) {
		m_debugApplicationThread->Release();
	}
}

/* IBreakpointTarget */

bool CrossfireContext::breakpointAttributeChanged(unsigned int handle, wchar_t* name, Value* value) {
	std::map<unsigned int, CrossfireBreakpoint*>::iterator iterator = m_breakpoints->find(handle);
	if (iterator == m_breakpoints->end()) {
		Logger::error("CrossfireContext.breakpointAttributeChanged(): unknown breakpoint handle", handle);
		return false;
	}
	CrossfireBreakpoint* breakpoint = iterator->second;

	if (wcscmp(name, CrossfireLineBreakpoint::ATTRIBUTE_ENABLED) == 0) {
		return setBreakpointEnabled((CrossfireLineBreakpoint*)breakpoint, value->getBooleanValue());
	}

	/* receiver does not do anything for any other breakpoint attributes, so just answer success */
	return true;
}

bool CrossfireContext::setBreakpointEnabled(CrossfireBreakpoint* breakpoint, bool enabled) {
	CrossfireLineBreakpoint* lineBp = (CrossfireLineBreakpoint*)breakpoint;
	IDebugApplicationNode* node = getScriptNode((wchar_t*)lineBp->getUrl()->c_str());
	if (!node) {
		Logger::error("CrossfireContext.setBreakpointEnabled(): unknown target url");
		return false;
	}

	CComPtr<IDebugDocument> document = NULL;
	HRESULT hr = node->GetDocument(&document);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.setBreakpointEnabled(): GetDocument() failed", hr);
		return false;
	}

	CComPtr<IDebugDocumentText> documentText = NULL;
	hr = document->QueryInterface(IID_IDebugDocumentText, (void**)&documentText);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.setBreakpointEnabled(): QI(IDebugDocumentText) failed", hr);
		return false;
	}

	ULONG characterPosition = 0;
	hr = documentText->GetPositionOfLine(lineBp->getLine() - 1, &characterPosition);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.setBreakpointEnabled(): GetPositionOfLine() failed [1]", hr);
		return false;
	}

	CComPtr<IDebugDocumentContext> documentContext = NULL;
	hr = documentText->GetContextOfPosition(characterPosition, 1, &documentContext);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.setBreakpointEnabled(): GetContextOfPosition() failed", hr);
		return false;
	}

	CComPtr<IEnumDebugCodeContexts> codeContexts = NULL;
	hr = documentContext->EnumCodeContexts(&codeContexts);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.setBreakpointEnabled(): EnumCodeContexts() failed", hr);
		return false;
	}

	ULONG fetched = 0;
	IDebugCodeContext* codeContext = NULL;
	hr = codeContexts->Next(1, &codeContext, &fetched);
	if (FAILED(hr) || fetched == 0) {
		Logger::error("CrossfireContext.setBreakpointEnabled(): Next() failed", hr);
		return false;
	}

	hr = codeContext->SetBreakPoint(enabled ? BREAKPOINT_ENABLED : BREAKPOINT_DISABLED);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.setBreakpointEnabled(): SetBreakPoint() failed", hr);
		return false;
	}

	lineBp->setEnabled(enabled);
	return true;
}

bool CrossfireContext::deleteBreakpoint(unsigned int handle) {
	std::map<unsigned int, CrossfireBreakpoint*>::iterator iterator = m_breakpoints->find(handle);
	if (iterator == m_breakpoints->end()) {
		Logger::error("CrossfireContext.deleteBreakpoint(): unknown breakpoint handle", handle);
		return false;
	}
	CrossfireLineBreakpoint* breakpoint = (CrossfireLineBreakpoint*)iterator->second;

	IDebugApplicationNode* node = getScriptNode((wchar_t*)breakpoint->getUrl()->c_str());
	if (!node) {
		Logger::error("CrossfireContext.deleteBreakpoint(): unknown target url");
		return false;
	}

	CComPtr<IDebugDocument> document = NULL;
	HRESULT hr = node->GetDocument(&document);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.deleteBreakpoint(): GetDocument() failed", hr);
		return false;
	}

	CComPtr<IDebugDocumentText> documentText = NULL;
	hr = document->QueryInterface(IID_IDebugDocumentText,(void**)&documentText);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.deleteBreakpoint(): QI(IDebugDocumentText) failed", hr);
		return false;
	}

	ULONG characterPosition = 0;
	hr = documentText->GetPositionOfLine(breakpoint->getLine() - 1, &characterPosition);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.deleteBreakpoint(): GetPositionOfLine() failed [1]", hr);
		return false;
	}

	CComPtr<IDebugDocumentContext> documentContext = NULL;
	hr = documentText->GetContextOfPosition(characterPosition, 1, &documentContext);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.deleteBreakpoint(): GetContextOfPosition() failed", hr);
		return false;
	}

	CComPtr<IEnumDebugCodeContexts> codeContexts = NULL;
	hr = documentContext->EnumCodeContexts(&codeContexts);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.deleteBreakpoint(): EnumCodeContexts() failed", hr);
		return false;
	}

	ULONG fetched = 0;
	IDebugCodeContext* codeContext = NULL;
	hr = codeContexts->Next(1, &codeContext, &fetched);
	if (FAILED(hr) || fetched == 0) {
		Logger::error("CrossfireContext.deleteBreakpoint(): Next() failed", hr);
		return false;
	}

	hr = codeContext->SetBreakPoint(BREAKPOINT_DELETED);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.deleteBreakpoint(): SetBreakPoint() failed", hr);
		return false;
	}

	CrossfireEvent toggleEvent;
	toggleEvent.setName(EVENT_ONTOGGLEBREAKPOINT);
	Value data;
	data.addObjectValue(KEY_SET, &Value(false));
	Value* value_breakpoint = NULL;
	breakpoint->toValueObject(&value_breakpoint);
	data.addObjectValue(KEY_BREAKPOINT, value_breakpoint);
	delete value_breakpoint;
	toggleEvent.setData(&data);
	sendEvent(&toggleEvent);

	delete iterator->second;
	m_breakpoints->erase(iterator);
	return true;
}

CrossfireBreakpoint* CrossfireContext::getBreakpoint(unsigned int handle) {
	std::map<unsigned int, CrossfireBreakpoint*>::iterator iterator = m_breakpoints->find(handle);
	if (iterator == m_breakpoints->end()) {
		Logger::error("CrossfireContext.getBreakpoint(): unknown breakpoint handle", handle);
		return false;
	}
	return iterator->second;
}

bool CrossfireContext::getBreakpoints(CrossfireBreakpoint*** ___values) {
	size_t size = m_breakpoints->size();
	CrossfireBreakpoint** breakpoints = new CrossfireBreakpoint*[size + 1];

	std::map<unsigned int, CrossfireBreakpoint*>::iterator it = m_breakpoints->begin();
	int index = 0;
	while (it != m_breakpoints->end()) {
		it->second->clone(&breakpoints[index++]);
		it++;
	}
	breakpoints[index] = NULL;

	*___values = breakpoints;
	return true;
}

bool CrossfireContext::setBreakpoint(CrossfireBreakpoint *breakpoint, bool isRetry) {
	// TODO uncomment the following once Refreshes cause new contexts to be created

//	unsigned int handle = breakpoint->getHandle();
//	std::map<unsigned int, CrossfireBreakpoint*>::iterator iterator = m_breakpoints->find(handle);
//	if (iterator != m_breakpoints->end()) {
//		/* this breakpoint is already set on this context */
//		return true;
//	}

	CrossfireLineBreakpoint* lineBp = (CrossfireLineBreakpoint*)breakpoint;
	IDebugApplicationNode* node = NULL;
	if (m_currentScriptNode) {
		node = m_currentScriptNode;
	} else {
		node = getScriptNode((wchar_t*)lineBp->getUrl()->c_str());
		if (!node) {
			Logger::error("CrossfireContext.setLineBreakpoint(): unknown target url");
			return false;
		}
	}

	CComPtr<IDebugDocument> document = NULL;
	HRESULT hr = node->GetDocument(&document);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.setLineBreakpoint(): GetDocument() failed", hr);
		return false;
	}

	CComPtr<IDebugDocumentText> documentText = NULL;
	hr = document->QueryInterface(IID_IDebugDocumentText, (void**)&documentText);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.setLineBreakpoint(): QI(IDebugDocumentText) failed", hr);
		return false;
	}

	ULONG characterPosition = 0;
	hr = documentText->GetPositionOfLine(lineBp->getLine() - 1, &characterPosition);
	if (FAILED(hr)) {
//		/*
//		 * In this context E_INVALIDARG failures are often caused by the target document
//		 * not being adequately loaded yet.  If this is the first attempt to set this
//		 * breakpoint then create a pending breakpoint that will attempt to hook itself
//		 * later if it is detected that the document load may have completed.
//		 */
//		if (!isRetry && hr == E_INVALIDARG) {
//			CComObject<PendingBreakpoint>* pendingBreakpoint = NULL;
//			HRESULT hr = CComObject<PendingBreakpoint>::CreateInstance(&pendingBreakpoint);
//			if (FAILED(hr)) {
//				Logger::error("CrossfireContext.setLineBreakpoint(): CreateInstance(CLSID_PendingBreakpoint) failed [1]", hr);
//				return false;
//			}
//
//			if (pendingBreakpoint->init(breakpoint, document, this)) {
//				pendingBreakpoint->AddRef(); /* CComObject::CreateInstance gives initial ref count of 0 */
//				m_pendingBreakpoints->push_back(pendingBreakpoint);
//			}
//		} else {
			Logger::error("CrossfireContext.setLineBreakpoint(): GetPositionOfLine() failed [1]", hr);
//		}
		return false;
	}

	CComPtr<IDebugDocumentContext> documentContext = NULL;
	hr = documentText->GetContextOfPosition(characterPosition, 1, &documentContext);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.setLineBreakpoint(): GetContextOfPosition() failed", hr);
		return false;
	}

	CComPtr<IEnumDebugCodeContexts> codeContexts = NULL;
	hr = documentContext->EnumCodeContexts(&codeContexts);
	if (FAILED(hr)) {
//		/*
//		 * In this context E_INVALIDARG failures are often caused by the target document
//		 * not being adequately loaded yet.  If this is the first attempt to set this
//		 * breakpoint then create a pending breakpoint that will attempt to hook itself
//		 * later if it is detected that the document load may have completed.
//		 */
//		if (!isRetry && hr == E_INVALIDARG) {
//			CComObject<PendingBreakpoint>* pendingBreakpoint = NULL;
//			HRESULT hr = CComObject<PendingBreakpoint>::CreateInstance(&pendingBreakpoint);
//			if (FAILED(hr)) {
//				Logger::error("CrossfireContext.setLineBreakpoint(): CreateInstance(CLSID_PendingBreakpoint) failed [2]", hr);
//				return false;
//			}
//
//			if (pendingBreakpoint->init(breakpoint, document, this)) {
//				pendingBreakpoint->AddRef(); /* CComObject::CreateInstance gives initial ref count of 0 */
//				m_pendingBreakpoints->push_back(pendingBreakpoint);
//			}
//		} else {
			Logger::error("CrossfireContext.setLineBreakpoint(): EnumCodeContexts() failed", hr);
//		}
		return false;
	}

	ULONG fetched = 0;
	CComPtr<IDebugCodeContext> codeContext = NULL;
	hr = codeContexts->Next(1, &codeContext, &fetched);
	if (FAILED(hr) || fetched == 0) {
		Logger::error("CrossfireContext.setLineBreakpoint(): Next() failed", hr);
		return false;
	}

	hr = codeContext->SetBreakPoint(lineBp->isEnabled() ? BREAKPOINT_ENABLED : BREAKPOINT_DISABLED);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.setLineBreakpoint(): SetBreakPoint() failed", hr);
		return false;
	}

	/* Determine the line number the breakpoint was set on (it may not match the requested line number) */

	CComPtr<IDebugDocumentContext> bpDocumentContext = NULL;
	hr = codeContext->GetDocumentContext(&bpDocumentContext);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.setLineBreakpoint(): GetDocumentContext() failed", hr);
		return false;
	}

	characterPosition = 0;
	ULONG numChars = 0;
	hr = documentText->GetPositionOfContext(bpDocumentContext, &characterPosition, &numChars);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.setLineBreakpoint(): GetPositionOfContext() failed", hr);
		return false;
	}

	ULONG bpLineNumber = 0;
	ULONG offset = 0;
	hr = documentText->GetLineOfPosition(characterPosition, &bpLineNumber, &offset);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.setLineBreakpoint(): GetLineOfPosition() failed", hr);
		return false;
	}

	CrossfireLineBreakpoint* copy = NULL;
	breakpoint->clone((CrossfireBreakpoint**)&copy);
	copy->setLine(bpLineNumber + 1);
	copy->setContextId(&std::wstring(m_name));
	m_breakpoints->insert(std::pair<unsigned int, CrossfireBreakpoint*>(breakpoint->getHandle(), copy));

	CrossfireEvent toggleEvent;
	toggleEvent.setName(EVENT_ONTOGGLEBREAKPOINT);
	Value data;
	data.addObjectValue(KEY_SET, &Value(true));
	Value* value_breakpoint = NULL;
	copy->toValueObject(&value_breakpoint);
	data.addObjectValue(KEY_BREAKPOINT, value_breakpoint);
	delete value_breakpoint;
	toggleEvent.setData(&data);
	sendEvent(&toggleEvent);
	return true;
}

void CrossfireContext::clearObjects() {
	std::map<unsigned int, JSObject*>::iterator iterator = m_objects->begin();
	while (iterator != m_objects->end()) {
		JSObject* jsObject = iterator->second;
		jsObject->debugProperty->Release();
		jsObject->stackFrame->Release();
		delete jsObject;
		iterator++;
	}
	m_objects->clear();
}

/* IJSEvalHandler methods */

void CrossfireContext::evalComplete(IDebugProperty* value, void* data) {
	/*
	 * Currently only breakpoint conditions are evaluated asynchronously, so it's assumed
	 * that data is a breakpoint.  If more asynchronous evaluations are introduced in the
	 * future then a new class implementing IJSEvalHandler should be created for handling
	 * breakpoint condition evals.
	 */

	bool resume = false;
	DebugPropertyInfo propertyInfo;
	HRESULT hr = value->GetPropertyInfo(PROP_INFO_TYPE | PROP_INFO_VALUE, 10, &propertyInfo);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.evalComplete(): GetPropertyInfo() failed", hr);
		resume = true;
	} else {
		BSTR type = propertyInfo.m_bstrType;
		if (wcscmp(type, JSVALUE_BOOLEAN) != 0) {
			resume = true;
		} else {
			BSTR stringValue = propertyInfo.m_bstrValue;
			if (wcscmp(stringValue, JSVALUE_TRUE) != 0) {
				resume = true;
			}
		}
	}

	if (resume && resumeFromBreak()) {
		return;
	}

	CrossfireLineBreakpoint* breakpoint = (CrossfireLineBreakpoint*)data;
	CrossfireEvent onBreakEvent;
	onBreakEvent.setName(EVENT_ONBREAK);
	Value value_data;
	Value location;
	location.addObjectValue(KEY_LINE, &Value((double)breakpoint->getLine()));
	location.addObjectValue(KEY_URL, &Value(breakpoint->getUrl()->c_str()));
	value_data.addObjectValue(KEY_LOCATION, &location);
	Value cause;
	cause.addObjectValue(KEY_TITLE, &Value(L"breakpoint"));
	value_data.addObjectValue(KEY_CAUSE, &cause);
	onBreakEvent.setData(&value_data);
	sendEvent(&onBreakEvent);
}

/* CrossfireContext */

void CrossfireContext::breakpointHit(IRemoteDebugApplicationThread *pDebugAppThread, BREAKREASON br, IActiveScriptErrorDebug *pScriptErrorDebug) {
	m_running = false;

	CComPtr<IEnumDebugStackFrames> stackFrames = NULL;
	HRESULT hr = pDebugAppThread->EnumStackFrames(&stackFrames);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.breakpointHit(): EnumStackFrames() failed", hr);
		return;
	}

	DebugStackFrameDescriptor stackFrameDescriptor;
	ULONG numFetched = 0;
	hr = stackFrames->Next(1,&stackFrameDescriptor,&numFetched);
	if (FAILED(hr) || numFetched != 1) {
		Logger::error("CrossfireContext.breakpointHit(): EnumStackFrames->Next() failed", hr);
		return;
	}

	IDebugStackFrame* frame = stackFrameDescriptor.pdsf;
	CComPtr<IDebugCodeContext> codeContext = NULL;
	hr = frame->GetCodeContext(&codeContext);
	// TODO This fails if the current position is not in a user document (eg.- following
	// a return).  Not sure what to do here (send an event with no url/line?  Or no event?)
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.breakpointHit(): GetCodeContext() failed", hr);
		return;
	}

	CComPtr<IDebugDocumentContext> documentContext = NULL;
	hr = codeContext->GetDocumentContext(&documentContext);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.breakpointHit(): GetDocumentContext() failed", hr);
		return;
	}

	CComPtr<IDebugDocument> document = NULL;
	hr = documentContext->GetDocument(&document);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.breakpointHit(): GetDocument() failed", hr);
		return;
	}

	CComPtr<IDebugDocumentText> documentText = NULL;
	hr = document->QueryInterface(IID_IDebugDocumentText, (void**)&documentText);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.breakpointHit(): QueryInterface() failed", hr);
		return;
	}

	ULONG position, numChars;
	hr = documentText->GetPositionOfContext(documentContext, &position, &numChars);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.breakpointHit(): GetPositionOfContext() failed", hr);
		return;
	}

	ULONG lineNumber, column;
	hr = documentText->GetLineOfPosition(position, &lineNumber, &column);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.breakpointHit(): GetLineOfContext() failed", hr);
		return;
	}
	lineNumber++;

	CComBSTR bstrUrl;
	hr = document->GetName(DOCUMENTNAMETYPE_TITLE, &bstrUrl);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.breakpointHit(): GetName() failed", hr);
		return;
	}

	/*
	 * If the cause of the break is a breakpoint then locate the breakpoint and
	 * determine whether the onBreak event should be sent (eg.- does the breakpoint
	 * have a hit count to respect, a condition to evaluate, etc.).
	 */
	if (br == BREAKREASON_BREAKPOINT) {
		CrossfireBreakpoint* breakpoint = NULL;
		std::map<unsigned int, CrossfireBreakpoint*>::iterator iterator = m_breakpoints->begin();
		while (iterator != m_breakpoints->end()) {
			CrossfireBreakpoint* current = iterator->second;
			if (current->getType() == CrossfireLineBreakpoint::BPTYPE_LINE) {
				CrossfireLineBreakpoint* lineBp = (CrossfireLineBreakpoint*)current;
				if (lineBp->getLine() == lineNumber && wcscmp(lineBp->getUrl()->c_str(), bstrUrl) == 0) {
					lineBp->breakpointHit();
					if (!lineBp->matchesHitCount() && resumeFromBreak()) {
						return;
					}
					const std::wstring* conditionString = lineBp->getCondition();
					if (conditionString) {
						wchar_t* condition = (wchar_t*)conditionString->c_str();
						if (evaluateAsync(frame, condition, DEBUG_TEXT_RETURNVALUE | DEBUG_TEXT_NOSIDEEFFECTS, this, lineBp)) {
							return;
						}
					}
					break;
				}
			}
			iterator++;
		}
	}

	CrossfireEvent onBreakEvent;
	onBreakEvent.setName(EVENT_ONBREAK);
	Value data;
	Value location;
	location.addObjectValue(KEY_LINE, &Value((double)lineNumber));
	location.addObjectValue(KEY_URL, &Value(bstrUrl));
	data.addObjectValue(KEY_LOCATION, &location);
	Value cause;
	switch (br) {
		case BREAKREASON_ERROR: {
			cause.addObjectValue(KEY_TITLE, &Value(L"error"));
			EXCEPINFO excepInfo;
			HRESULT hr = pScriptErrorDebug->GetExceptionInfo(&excepInfo);
			if (FAILED(hr)) {
				Logger::error("IEDebugger::onHandleBreakPoint(): GetExceptionInfo() failed", hr);
			} else {
				if (excepInfo.bstrDescription) {
					cause.addObjectValue(KEY_MESSAGE, &Value(excepInfo.bstrDescription));
				}
			}
			break;
		}
		case BREAKREASON_DEBUGGER_HALT: {
			cause.addObjectValue(KEY_TITLE, &Value(L"suspend"));
			break;
		}
		case BREAKREASON_STEP: {
			cause.addObjectValue(KEY_TITLE, &Value(L"step"));
			break;
		}
		case BREAKREASON_BREAKPOINT: {
			cause.addObjectValue(KEY_TITLE, &Value(L"breakpoint"));
			break;
		}
		default: {
			cause.addObjectValue(KEY_TITLE, &Value(L"suspend"));
			break;
		}
	}
	data.addObjectValue(KEY_CAUSE, &cause);
	onBreakEvent.setData(&data);
	sendEvent(&onBreakEvent);
	return;
}

bool CrossfireContext::createValueForFrame(IDebugStackFrame* stackFrame, unsigned int frameIndex, bool includeScopes, Value** _value) {
	*_value = NULL;

	CComBSTR description = NULL;
	HRESULT hr = stackFrame->GetDescriptionString(true, &description);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.createValueForFrame(): GetDescriptionString() failed", hr);
		return false;
	}

	ULONG lineNumber = 0, column;
	CComBSTR scriptId = NULL;
	Value* locals = NULL;

	CComPtr<IDebugCodeContext> codeContext = NULL;
	hr = stackFrame->GetCodeContext(&codeContext);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.createValueForFrame(): GetCodeContext() failed", hr);
		return false;
	}

	CComPtr<IDebugDocumentContext> documentContext = NULL;
	hr = codeContext->GetDocumentContext(&documentContext);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.createValueForFrame(): GetDocumentContext() failed", hr);
		return false;
	}

	CComPtr<IDebugDocument> document = NULL;
	hr = documentContext->GetDocument(&document);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.createValueForFrame(): GetDocument() failed", hr);
		return false;
	}

	CComPtr<IDebugDocumentText> documentText = NULL;
	hr = document->QueryInterface(IID_IDebugDocumentText, (void**)&documentText);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.createValueForFrame(): QI(IDebugDocumentText) failed", hr);
		return false;
	}

	ULONG position, numChars;
	hr = documentText->GetPositionOfContext(documentContext, &position, &numChars);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.createValueForFrame(): GetPositionOfContext() failed", hr);
		return false;
	}

	hr = documentText->GetLineOfPosition(position, &lineNumber, &column);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.createValueForFrame(): GetLineOfPosition() failed", hr);
		return false;
	}

	/* get the locals */

	CComPtr<IDebugProperty> debugProperty = NULL;
	hr = stackFrame->GetDebugProperty(&debugProperty);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.createValueForFrame(): GetDebugProperty() failed", hr);
		return false;
	}

	// TODO this frameObject is currently created temporarily, but to
	// get the full benefit of handle reuse it should be stored and
	// reused for cases where the frame command is invoked on the same
	// frame multiple times
	JSObject* newObject = new JSObject();
	newObject->debugProperty = debugProperty;
	//newObject->debugProperty->AddRef();
	newObject->stackFrame = stackFrame;
	//newObject->stackFrame->AddRef();
	if (!createValueForObject(newObject, true, &locals)) {
		delete newObject;
		return false;
	}
	delete newObject;

	/* get "this" */

	Value* value_this = NULL;
	CComPtr<IDebugProperty> debugProperty2 = NULL;
	if (evaluate(stackFrame, L"this", DEBUG_TEXT_RETURNVALUE, &debugProperty2)) {
		// TODO this thisObject is currently created temporarily, but to
		// get the full benefit of handle reuse it should be stored and
		// reused for cases where the frame command is invoked on the same
		// frame multiple times
		JSObject* newObject = new JSObject();
		newObject->debugProperty = debugProperty2;
		//newObject->debugProperty->AddRef();
		newObject->stackFrame = stackFrame;
		//newObject->stackFrame->AddRef();
		createValueForObject(newObject, true, &value_this); /* proceed even if fails */
		delete newObject;
	}

	if (value_this) {
		locals->addObjectValue(KEY_THIS, value_this);
		delete value_this;
	} else {
		/* create an empty "this" value */
		Value value_thisChildren;
		value_thisChildren.setType(TYPE_OBJECT);
		Value value_this2;
		value_this2.addObjectValue(KEY_TYPE, &Value(VALUE_OBJECT));
		value_this2.addObjectValue(KEY_VALUE, &value_thisChildren);
		locals->addObjectValue(KEY_THIS, &value_this2);
	}

	hr = document->GetName(DOCUMENTNAMETYPE_TITLE, &scriptId);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.createValueForFrame(): GetName() failed", hr);
		return false;
	}

	if (!locals) {
		locals = new Value();
		locals->setType(TYPE_OBJECT);
	}

	Value* result = new Value();
	result->addObjectValue(KEY_FUNCTIONNAME, &Value(description));
	result->addObjectValue(KEY_INDEX, &Value((double)frameIndex));
	result->addObjectValue(KEY_LINE, &Value((double)lineNumber + 1));
	result->addObjectValue(KEY_LOCALS, locals);
	result->addObjectValue(KEY_URL, &Value(scriptId));
	// TODO includeScopes
	delete locals;

	*_value = result;
	return true;
}

bool CrossfireContext::createValueForObject(JSObject* object, bool resolveChildObjects, Value** _value) {
	*_value = NULL;

	DebugPropertyInfo propertyInfo;
	IDebugProperty* debugProperty = object->debugProperty;
	HRESULT hr = debugProperty->GetPropertyInfo(PROP_INFO_NAME | PROP_INFO_TYPE | PROP_INFO_VALUE | PROP_INFO_ATTRIBUTES | PROP_INFO_DEBUGPROP, 10, &propertyInfo);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.createValueForObject(): GetPropertyInfo() failed", hr);
		return false;
	}

	BSTR type = propertyInfo.m_bstrType;
	Value result;
	if (wcscmp(type, JSVALUE_NULL) == 0) {
		result.setType(TYPE_NULL);
	} else if (wcscmp(type, JSVALUE_UNDEFINED) == 0) {
		result.setValue(VALUE_UNDEFINED);
	} else {
		BSTR stringValue = propertyInfo.m_bstrValue;
		if (wcscmp(type, JSVALUE_NUMBER) == 0) {
			wchar_t* endPtr = 0;
			double value = wcstod(stringValue, &endPtr);
			result.addObjectValue(KEY_TYPE, &Value(VALUE_NUMBER));
			result.addObjectValue(KEY_VALUE, &Value(value));
		} else if (wcscmp(type, JSVALUE_BOOLEAN) == 0) {
			result.addObjectValue(KEY_TYPE, &Value(VALUE_BOOLEAN));
			if (wcscmp(stringValue, JSVALUE_TRUE) == 0) {
				result.addObjectValue(KEY_VALUE, &Value(true));
			} else {
				result.addObjectValue(KEY_VALUE, &Value(false));
			}
		} else if (wcscmp(type, JSVALUE_STRING) == 0) {
			std::wstring string(stringValue);
			string = string.substr(1, string.length() - 2);
			result.addObjectValue(KEY_TYPE, &Value(VALUE_STRING));
			result.addObjectValue(KEY_VALUE, &Value(&string));
		} else if ((propertyInfo.m_dwAttrib & OBJECT_ATTRIB_VALUE_IS_INVALID) != 0) {
			// TODO error object, should fail?
		} else if ((propertyInfo.m_dwAttrib & OBJECT_ATTRIB_VALUE_IS_OBJECT) == 0) {
			/* object is a function */
			result.addObjectValue(KEY_TYPE, &Value(VALUE_FUNCTION));
			if (resolveChildObjects) {
				// TODO
				Value value_empty;
				value_empty.setType(TYPE_OBJECT);
				result.addObjectValue(KEY_VALUE, &value_empty);
			}
		} else {
			if (!resolveChildObjects) {
				result.addObjectValue(KEY_TYPE, &Value(VALUE_OBJECT));
			} else {
				CComPtr<IEnumDebugPropertyInfo> enumPropertyInfo = NULL;
				HRESULT hr = debugProperty->EnumMembers(
					PROP_INFO_NAME | PROP_INFO_TYPE | PROP_INFO_VALUE | PROP_INFO_ATTRIBUTES | PROP_INFO_DEBUGPROP,
					10,
					IID_IDebugPropertyEnumType_LocalsPlusArgs,
					&enumPropertyInfo);
				if (FAILED(hr)) {
					Logger::error("CrossfireContext.createValueForObject(): EnumMembers() failed", hr);
					return false;
				}

				Value children;
				children.setType(TYPE_OBJECT);
				ULONG fetched;
				do {
					DebugPropertyInfo propertyInfo;
					hr = enumPropertyInfo->Next(1, &propertyInfo, &fetched);
					if (SUCCEEDED(hr) && fetched) {
						Value* value_child = NULL;
						IDebugStackFrame* stackFrame = object->stackFrame;
						JSObject childObject;
						childObject.debugProperty = propertyInfo.m_pDebugProp;
						childObject.stackFrame = stackFrame;
						if (createValueForObject(&childObject, false, &value_child)) {
							if (value_child->getType() == TYPE_OBJECT) {
								Value* value_type = value_child->getObjectValue(KEY_TYPE);
								const wchar_t* type = value_type->getStringValue()->c_str();
								if (wcscmp(type, VALUE_OBJECT) == 0 || (wcscmp(type, VALUE_FUNCTION) == 0)) {
									std::map<std::wstring, unsigned int> objects = object->children;
									std::map<std::wstring, unsigned int>::iterator iterator = objects.find(propertyInfo.m_bstrName);
									Value value_handle;
									if (iterator != objects.end()) {
										value_handle.setValue((double)iterator->second);
									} else {
										value_handle.setValue((double)m_nextObjectHandle);
										JSObject* newObject = new JSObject();
										newObject->debugProperty = propertyInfo.m_pDebugProp;
										newObject->debugProperty->AddRef();
										newObject->isObject = wcscmp(type, VALUE_OBJECT) == 0;
										newObject->stackFrame = stackFrame;
										newObject->stackFrame->AddRef();
										m_objects->insert(std::pair<unsigned int, JSObject*>(m_nextObjectHandle++, newObject));
									}
									value_child->addObjectValue(KEY_HANDLE, &value_handle);
								}
							}
							children.addObjectValue(propertyInfo.m_bstrName, value_child);
							delete value_child;
						}
					}
				} while (fetched);
				result.addObjectValue(KEY_TYPE, &Value(VALUE_OBJECT));
				result.addObjectValue(KEY_VALUE, &children);
			}
		}
	}

	Value* copy = NULL;
	result.clone(&copy);
	*_value = copy;
	return true;
}

bool CrossfireContext::createValueForScript(IDebugApplicationNode* node, bool includeSource, bool failIfEmpty, Value** _value) {
	*_value = NULL;

	wchar_t* id = getScriptId(node);
	if (!id) {
		Logger::error("CrossfireContext.createValueForScript(): unknown script");
		return false;
	}

	CComPtr<IDebugDocument> document = NULL;
	HRESULT hr = node->GetDocument(&document);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.createValueForScript(): GetDocument() failed", hr);
		return false;
	}

	CComPtr<IDebugDocumentText> documentText = NULL;
	hr = document->QueryInterface(IID_IDebugDocumentText, (void**)&documentText);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.createValueForScript(): QI(IDebugDocumentText) failed", hr);
		return false;
	}

	ULONG numLines = 0;
	ULONG numChars = 0;
	hr = documentText->GetSize(&numLines, &numChars);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.createValueForScript(): GetSize() failed", hr);
		return false;
	}
	if (failIfEmpty && numChars == 0) {
		return false;
	}

	Value* result = new Value();
	result->addObjectValue(/*KEY_ID*/ L"url", &Value(id)); // TODO
	result->addObjectValue(KEY_LINEOFFSET, &Value((double)0)); // TODO right?
	result->addObjectValue(KEY_COLUMNOFFSET, &Value((double)0));
	result->addObjectValue(KEY_SOURCELENGTH, &Value((double)numChars));
	result->addObjectValue(KEY_LINECOUNT, &Value((double)numLines));
	if (wcsstr(id, VALUE_EVALCODE)) {
		result->addObjectValue(KEY_TYPE, &Value(VALUE_EVALLEVEL)); // TODO right?
	} else {
		result->addObjectValue(KEY_TYPE, &Value(VALUE_TOPLEVEL)); // TODO right?
	}

	if (includeSource) {
		wchar_t* sourceChars = new wchar_t[numChars + 1];
		ULONG charsRead = 0;
		hr = documentText->GetText(0, sourceChars, NULL, &charsRead, numChars);
		if (FAILED(hr)) {
			Logger::error("CrossfireContext.createValueForScript(): GetText()[2] failed", hr);
			return false;
		}
		sourceChars[numChars] = NULL;
		result->addObjectValue(KEY_SOURCE, &Value(sourceChars));
		delete[] sourceChars;
	}

	*_value = result;
	return true;
}

bool CrossfireContext::evaluate(IDebugStackFrame* stackFrame, wchar_t* expression, int flags, IDebugProperty** _result) {
	*_result = NULL;

	CComPtr<IDebugExpressionContext> expressionContext = NULL;
	HRESULT hr = stackFrame->QueryInterface(IID_IDebugExpressionContext, (void**)&expressionContext);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.evaluate(): QI(IDebugExpressionContext) failed", hr);
		return false;
	}

	CComPtr<IDebugExpression> parsedExpression = NULL;
	hr = expressionContext->ParseLanguageText(
		expression,
		10,
		L"",
		flags,
		&parsedExpression);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.evaluate(): ParseLanguageText() failed", hr);
		return false;
	}

	hr = parsedExpression->Start(NULL);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.evaluate(): Start() failed", hr);
		return false;
	}

	int ms = 0;
	while (ms < 2000) {
		if (parsedExpression->QueryIsComplete() == S_OK) {
			break;
		}
		ms += 10;
		::Sleep(10);
	}
	if (2000 <= ms) {
		Logger::error("CrossfireContext.evaluate(): Evaluation took too long");
		return false;
	}

	HRESULT evalResult;
	hr = parsedExpression->GetResultAsDebugProperty(&evalResult, _result);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.evaluate(): GetResultAsDebugProperty() failed", hr);
		return false;
	}
	if (FAILED(evalResult)) {
		Logger::error("CrossfireContext.evaluate(): evaluation of GetResultAsDebugProperty() failed", evalResult);
		return false;
	}

	return true;
}

bool CrossfireContext::evaluateAsync(IDebugStackFrame* stackFrame, wchar_t* expression, int flags, IJSEvalHandler* handler, void* data) {
	CComPtr<IDebugExpressionContext> expressionContext = NULL;
	HRESULT hr = stackFrame->QueryInterface(IID_IDebugExpressionContext, (void**)&expressionContext);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.evaluateAsync(): QI(IDebugExpressionContext) failed", hr);
		return false;
	}

	CComPtr<IDebugExpression> parsedExpression = NULL;
	hr = expressionContext->ParseLanguageText(
		expression,
		10,
		L"",
		flags,
		&parsedExpression);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.evaluateAsync(): ParseLanguageText() failed", hr);
		return false;
	}

	CComObject<JSEvalCallback>* listener = NULL;
	hr = CComObject<JSEvalCallback>::CreateInstance(&listener);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.evaluateAsync(): CreateInstance() failed", hr);
		return false;
	}

	listener->AddRef(); /* CComObject::CreateInstance gives initial ref count of 0 */
	m_asyncEvals->push_back(listener);
	listener->start(parsedExpression, handler, data);
	return true;
}

bool CrossfireContext::getDebugApplication(IRemoteDebugApplication** _value) {
	*_value = NULL;

	CComPtr<IRemoteDebugApplicationThread> thread = NULL;
	if (!getDebugApplicationThread(&thread)) {
		return false;
	}

	HRESULT hr = thread->GetApplication(_value);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.getDebugApplication: GetApplication() failed", hr);
		return false;
	}

	return true;
}

bool CrossfireContext::getDebugApplicationThread(IRemoteDebugApplicationThread** _value) {
	*_value = NULL;

	if (m_debugApplicationThread) {
		m_debugApplicationThread->AddRef();
		*_value = m_debugApplicationThread;
		return true;
	}

    CComPtr<IDebugProgramProvider2> pPDM;
    HRESULT hr = pPDM.CoCreateInstance(__uuidof(MsProgramProvider), NULL, CLSCTX_INPROC_SERVER);
    if (FAILED(hr)) {
        Logger::error("CrossfireContext.getDebugApplicationThread(): CoCreateInstance() failed", hr);
        return false;
    }

    AD_PROCESS_ID processId;
	processId.ProcessIdType = AD_PROCESS_ID_SYSTEM;
	processId.ProcessId.dwProcessId = m_processId;
	CONST_GUID_ARRAY filter;
	filter.dwCount = 0;
	filter.Members = NULL;
	PROVIDER_PROCESS_DATA processData;
    hr = pPDM->GetProviderProcessData(PFLAG_GET_PROGRAM_NODES | PFLAG_DEBUGGEE | PFLAG_ATTACHED_TO_DEBUGGEE, NULL, processId, filter, &processData);
	if (FAILED(hr)) {
        Logger::error("CrossfireContext.getDebugApplicationThread(): GetProviderProcessData() failed", hr);
        return false;
	}
	if (processData.ProgramNodes.dwCount != 1) {
        Logger::error("CrossfireContext.getDebugApplicationThread(): GetProviderProcessData() returned program nodes count != 1", processData.ProgramNodes.dwCount);
        return false;
	}

	IDebugProgramNode2* node = processData.ProgramNodes.Members[0];
	CComPtr<IDebugProviderProgramNode2> providerProgramNode;
	hr = node->QueryInterface(__uuidof(IDebugProviderProgramNode2), (void**)&providerProgramNode);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.getDebugApplicationThread(): QI(IDebugProviderProgramNode2) failed", hr);
        return false;
	}

	CComPtr<IRemoteDebugApplication> debugApplication;
	hr = providerProgramNode->UnmarshalDebuggeeInterface(IID_IRemoteDebugApplication, (void**)&debugApplication);
	if (FAILED(hr)) {
        Logger::error("CrossfireContext.getDebugApplicationThread(): UnmarshalDebuggeeInterface() failed", hr);
        return false;
	}

	CComPtr<IDebugApplicationNode> rootNode = NULL;
	hr = debugApplication->GetRootNode(&rootNode);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.getDebugApplicationThread(): GetRootNode() failed", hr);
	} else {
		CComPtr<IConnectionPointContainer> connectionPointContainer = NULL;
		hr = rootNode->QueryInterface(IID_IConnectionPointContainer, (void**)&connectionPointContainer);
		if (FAILED(hr)) {
			Logger::error("CrossfireContext.getDebugApplicationThread(): QI(IConnectionPointContainer) failed", hr);
		} else {
			CComPtr<IConnectionPoint> connectionPoint = NULL;
			hr = connectionPointContainer->FindConnectionPoint(IID_IDebugApplicationNodeEvents, &connectionPoint);
			if (FAILED(hr)) {
				Logger::error("CrossfireContext.getDebugApplicationThread(): FindConnectionPoint() failed", hr);
			} else {
				hr = connectionPoint->Advise(m_debugger, &m_cpcApplicationNodeEvents);
				if (FAILED(hr)) {
					Logger::error("CrossfireContext.getDebugApplicationThread(): Advise() failed", hr);
				}
			}
		}
	}

	CComPtr<IEnumRemoteDebugApplicationThreads> debugApplicationThreads;
	hr = debugApplication->EnumThreads(&debugApplicationThreads);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.getDebugApplicationThread(): EnumThreads() failed", hr);
		return false;
	}

	ULONG fetchedThreads = 0;
	hr = debugApplicationThreads->Next(1, &m_debugApplicationThread, &fetchedThreads);
	if (FAILED(hr) || fetchedThreads == 0) {
		Logger::error("CrossfireContext.getDebugApplicationThread(): Next() failed", hr);
		return false;
	}

	m_debugApplicationThread->AddRef();
	*_value = m_debugApplicationThread;
	return true;
}

IDebugApplicationNode* CrossfireContext::getLastInitializedScriptNode() {
	return m_lastInitializedScriptNode;
}

wchar_t* CrossfireContext::getName() {
	return m_name;
}

DWORD CrossfireContext::getProcessId() {
	return m_processId;
}

wchar_t* CrossfireContext::getScriptId(IDebugApplicationNode* node) {
	std::map<std::wstring, IDebugApplicationNode*>::iterator iterator = m_scriptNodes->begin();
	while (iterator != m_scriptNodes->end()) {
		if (iterator->second == node) {
			return (wchar_t*)iterator->first.c_str();
		}
		iterator++;
	}
	return NULL;
}

IDebugApplicationNode* CrossfireContext::getScriptNode(wchar_t* name) {
	std::map<std::wstring, IDebugApplicationNode*>::iterator iterator = m_scriptNodes->find(name);
	if (iterator == m_scriptNodes->end()) {
		return NULL;
	}

	return iterator->second;
}

wchar_t* CrossfireContext::getUrl() {
	return m_url;
}

bool CrossfireContext::hookDebugger() {
	if (m_debuggerHooked) {
		return true;
	}

	CComPtr<IRemoteDebugApplication> application = NULL;
	if (!getDebugApplication(&application)) {
		return false;
	}

	CComPtr<IApplicationDebugger> currentDebugger = NULL;
	HRESULT hr = application->GetDebugger(&currentDebugger);
	if (SUCCEEDED(hr) && currentDebugger) {
		Logger::log("CrossfireContext.hookDebugger(): the application already has a debugger");
		return false;
	}

	IEDebugger* ieDebugger = static_cast<IEDebugger*>(m_debugger);
	ieDebugger->setContext(this);
	hr = application->ConnectDebugger(ieDebugger);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.hookDebugger(): ConnectDebugger() failed", hr);
		ieDebugger->setContext(NULL);
		return false;
	}

	Logger::log("CrossfireContext.hookDebugger(): debugger successfully hooked");
	m_debuggerHooked = true;
	return true;
}

void CrossfireContext::installBreakpoints(std::vector<Value*>* breakpoints) {
	std::vector<Value*>::iterator iterator = breakpoints->begin();
	while (iterator != breakpoints->end()) {
		Value* current = *iterator;
		std::wstring* type = current->getObjectValue(KEY_TYPE)->getStringValue();
		if (type->compare(std::wstring(BPTYPE_LINE)) == 0) {
			Value* value_location = current->getObjectValue(KEY_LOCATION);
			Value* value_url = value_location->getObjectValue(KEY_LINE);
			if (value_url && value_url->getType() == TYPE_STRING) {

			}
		}
		iterator++;
	}
}

bool CrossfireContext::performRequest(CrossfireRequest* request) {
	if (!m_debuggerHooked) {
		hookDebugger();
	}
	wchar_t* command = request->getName();
	Value* arguments = request->getArguments();
	Value* responseBody = NULL;
	wchar_t* message = NULL;
	int code = CODE_OK;

	if (wcscmp(command, COMMAND_BACKTRACE) == 0) {
		code = commandBacktrace(arguments, &responseBody, &message);
	} else if (wcscmp(command, COMMAND_CONTINUE) == 0) {
		code = commandContinue(arguments, &responseBody, &message);
	} else if (wcscmp(command, COMMAND_EVALUATE) == 0) {
		code = commandEvaluate(arguments, &responseBody, &message);
	} else if (wcscmp(command, COMMAND_FRAME) == 0) {
		code = commandFrame(arguments, &responseBody, &message);
	} else if (wcscmp(command, COMMAND_LOOKUP) == 0) {
		code = commandLookup(arguments, &responseBody, &message);
	} else if (wcscmp(command, COMMAND_SCRIPT) == 0) {
		code = commandScript(arguments, &responseBody, &message);
	} else if (wcscmp(command, COMMAND_SCRIPTS) == 0) {
		code = commandScripts(arguments, &responseBody, &message);
	} else if (wcscmp(command, COMMAND_SCOPE) == 0) {
		code = commandScope(arguments, &responseBody, &message);
	} else if (wcscmp(command, COMMAND_SCOPES) == 0) {
		code = commandScopes(arguments, &responseBody, &message);
	} else if (wcscmp(command, COMMAND_SUSPEND) == 0) {
		code = commandSuspend(arguments, &responseBody, &message);
	} else if (wcscmp(command, COMMAND_INSPECT) == 0) {
		code = CODE_NOTIMPLEMENTED;
	} else {
		return false;	/* command not handled */
	}

	CrossfireResponse response;
	response.setContextId(&std::wstring(m_name));
	response.setName(command);
	response.setRequestSeq(request->getSeq());
	response.setRunning(m_running);
	response.setCode(code);
	response.setMessage(message);
	if (message) {
		free(message);
	}
	if (code == CODE_OK) {
		response.setBody(responseBody);
	} else {
		Value emptyBody;
		emptyBody.setType(TYPE_OBJECT);
		response.setBody(&emptyBody);
	}
	if (responseBody) {
		delete responseBody;
	}
	m_server->sendResponse(&response);
	return true;
}

bool CrossfireContext::registerScript(IDebugApplicationNode* applicationNode) {
	CComBSTR value = NULL;
	wchar_t* id = NULL;
	HRESULT hr = applicationNode->GetName(DOCUMENTNAMETYPE_URL, &value);
	if (SUCCEEDED(hr)) {
		id = value.m_str;
	} else {
		/*
		 * Failure to get the URL indicates that the node represents something like a JScript
		 * block or eval code.  For these cases get the node's title instead and append an
		 * index which will uniquely distinguish it from other nodes of a similar kind.
		 */
		hr = applicationNode->GetName(DOCUMENTNAMETYPE_TITLE, &value);
		if (FAILED(hr)) {
			Logger::error("CrossfireContext.registerScript(): GetName() failed", hr);
			return false;
		} else {
			id = value.m_str;
		}
	}

	/*
	 * There could be another script with the same id already registered for id's that
	 * are not urls.  In this case append a qualifier to the id to make it unique.
	 */
	int qualifierIndex = 1;
	std::wstring key(id);
	applicationNode->AddRef();
	while (true) {
		if (m_scriptNodes->insert(std::pair<std::wstring, IDebugApplicationNode*>(key, applicationNode)).second) {
			/* script was successfully inserted */
			break;
		}
		key.assign(id);
		key += wchar_t('/');
		wchar_t qualifierString[4];
		_ltow_s(qualifierIndex++, qualifierString, 4, 10); /* trailing linebreak */
		key += qualifierString;
	}
	
	return true;
}

bool CrossfireContext::resumeFromBreak() {
	CComPtr<IRemoteDebugApplicationThread> thread = NULL;
	HRESULT hr = getDebugApplicationThread(&thread);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.resumeFromBreak(): getDebugApplicationThread() failed", hr);
		return false;
	}
	CComPtr<IRemoteDebugApplication> application = NULL;
	hr = thread->GetApplication(&application);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.resumeFromBreak(): GetApplication() failed", hr);
		return false;
	}
	hr = application->ResumeFromBreakPoint(thread, BREAKRESUMEACTION_CONTINUE, ERRORRESUMEACTION_SkipErrorStatement);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.resumeFromBreak(): ResumeFromBreakPoint() failed", hr);
		return false;
	}
	return true;
}

bool CrossfireContext::scriptInitialized(IDebugApplicationNode *applicationNode) {
	if (m_lastInitializedScriptNode) {
		m_lastInitializedScriptNode->Release();
	}
	m_lastInitializedScriptNode = applicationNode;
	m_lastInitializedScriptNode->AddRef();

	registerScript(applicationNode);

	if (!scriptLoaded(applicationNode)) {
		/*
		 * The script's content has not been loaded yet, so create a listener
		 * object that will invoke #scriptLoaded() when this happens
		 */
		CComObject<PendingScriptLoad>* pendingScriptLoad = NULL;
		HRESULT hr = CComObject<PendingScriptLoad>::CreateInstance(&pendingScriptLoad);
		if (FAILED(hr)) {
			Logger::error("CrossfireContext.scriptInitialized(): CreateInstance(CLSID_PendingScriptLoad) failed [1]", hr);
			return false;
		}

		if (pendingScriptLoad->attach(applicationNode, this)) {
			applicationNode->AddRef();
			pendingScriptLoad->AddRef(); /* CComObject::CreateInstance gives initial ref count of 0 */
			m_pendingScriptLoads->insert(std::pair<IDebugApplicationNode*, PendingScriptLoad*>(applicationNode, pendingScriptLoad));
		}
	}

	return true;
}

bool CrossfireContext::scriptLoaded(IDebugApplicationNode *applicationNode) {
	CrossfireBPManager* bpManager = m_server->getBreakpointManager();

	wchar_t* id = getScriptId(applicationNode);
	if (!id) {
		Logger::error("CrossfireContext.scriptLoaded(): unknown script");
		return false;
	}

	/*
	 * Incoming IBreakpointTarget method invocations should always be for this
	 * application node, so store it temporarily so that's it's easily accessible,
	 * rather than repeatedly looking it up for each IBreakpointTarget invocation.
	 */
	m_currentScriptNode = applicationNode;
	bpManager->setBreakpointsForScript(&std::wstring(id), this);
	m_currentScriptNode = NULL;

	Value* script = NULL;
	if (!createValueForScript(applicationNode, false, true, &script)) {
		/* the script's content has probably not loaded yet */
		return false;
	}

	/* Ensure that there is not a PendingScriptLoad remaining for this node */
	std::map<IDebugApplicationNode*, PendingScriptLoad*>::iterator iterator = m_pendingScriptLoads->find(applicationNode);
	if (iterator != m_pendingScriptLoads->end()) {
		iterator->first->Release();
		iterator->second->Release();
		m_pendingScriptLoads->erase(iterator);
	}

	CrossfireEvent onScriptEvent;
	onScriptEvent.setName(EVENT_ONSCRIPT);
	Value data;
	data.addObjectValue(KEY_SCRIPT, script);
	delete script;
	onScriptEvent.setData(&data);
	sendEvent(&onScriptEvent);
	return true;
}

void CrossfireContext::sendEvent(CrossfireEvent* eventObj) {
	eventObj->setContextId(&std::wstring(m_name));
	m_server->sendEvent(eventObj);
}

bool CrossfireContext::unhookDebugger() {
	if (!m_debuggerHooked) {
		return true;
	}

	CComPtr<IRemoteDebugApplication> application = NULL;
	if (!getDebugApplication(&application)) {
		return false;
	}
	HRESULT hr = application->DisconnectDebugger();
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.unhookDebugger(): DisconnectDebugger() failed", hr);
	}

	static_cast<IEDebugger*>(m_debugger)->setContext(NULL);
	return SUCCEEDED(hr);
}

/* commands */

int CrossfireContext::commandBacktrace(Value* arguments, Value** _responseBody, wchar_t** _message) {
	unsigned int fromFrame = 0;
	Value* value_fromFrame = arguments->getObjectValue(KEY_FROMFRAME);
	if (value_fromFrame) {
		if (value_fromFrame->getType() != TYPE_NUMBER || (unsigned int)value_fromFrame->getNumberValue() < 0) {
			*_message = _wcsdup(L"'backtrace' command has an invalid 'fromFrame' value");
			return CODE_INVALIDPACKET;
		}
		fromFrame = (unsigned int)value_fromFrame->getNumberValue();
	}

	unsigned int toFrame = 99;
	Value* value_toFrame = arguments->getObjectValue(KEY_TOFRAME);
	if (value_toFrame) {
		if (value_toFrame->getType() != TYPE_NUMBER || (unsigned int)value_toFrame->getNumberValue() < 0) {
			*_message = _wcsdup(L"'backtrace' command has an invalid 'toFrame' value");
			return CODE_INVALIDPACKET;
		}
		toFrame = (unsigned int)value_toFrame->getNumberValue();
		if (toFrame < fromFrame) {
			*_message = _wcsdup(L"'backtrace' command has 'toFrame' value < 'fromFrame' value");
			return CODE_INVALIDPACKET;
		}
	}

	bool includeScopes = true;
	Value* value_includeScopes = arguments->getObjectValue(KEY_INCLUDESCOPES);
	if (value_includeScopes) {
		if (value_includeScopes->getType() != TYPE_BOOLEAN) {
			*_message = _wcsdup(L"'backtrace' command has an invalid 'includeScopes' value");
			return CODE_INVALIDPACKET;
		}
		includeScopes = value_includeScopes->getBooleanValue();
	}

	CComPtr<IRemoteDebugApplicationThread> applicationThread = NULL;
	if (!getDebugApplicationThread(&applicationThread)) {
		return CODE_FAILURE;
	}

	CComPtr<IEnumDebugStackFrames> stackFrames = NULL;
	HRESULT hr = applicationThread->EnumStackFrames(&stackFrames);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.commandBacktrace(): EnumStackFrames() failed", hr);
		return CODE_FAILURE;
	}

	if (fromFrame > 0) {
		HRESULT hr = stackFrames->Skip(fromFrame);
		if (FAILED(hr)) {
			Logger::error("CrossfireContext.commandBacktrace(): Skip() failed", hr);
			return CODE_FAILURE;
		}
	}

	/* count the available frames, and adjust toFrame accordingly if needed */

	Value framesArray;
	framesArray.setType(TYPE_ARRAY);
	unsigned int index = 0;
	for (index = fromFrame; index <= toFrame; index++) {
		ULONG fetched = 0;
		DebugStackFrameDescriptor stackFrameDescriptor;
		hr = stackFrames->Next(1, &stackFrameDescriptor, &fetched);
		if (FAILED(hr) || fetched == 0) {
			/*
			 * Have enumerated past the last stack frame, which is expected
			 * to happen if the toFrame value is not specified or is too large.
			 */
			break;
		}

		Value* frame = NULL;
		if (!createValueForFrame(stackFrameDescriptor.pdsf, index, includeScopes, &frame)) {
			/* Attempting to access a frame without available information, break */
			break;
		}

		framesArray.addArrayValue(frame);
		delete frame;
	}

	/* index now points to one frame beyond the last frame to be returned */

	int totalFrameCount = index;

	/*
	 * If the frame with index toFrame was reached successfully then continue on to
	 * count any remaining frames in order to determine totalFrameCount.
	 */
	if (toFrame < index) {
		while (true) {
			ULONG fetched = 0;
			DebugStackFrameDescriptor stackFrameDescriptor;
			hr = stackFrames->Next(1, &stackFrameDescriptor, &fetched);
			if (FAILED(hr) || fetched == 0) {
				break;
			}
			totalFrameCount++;
		}
	}

	Value* result = new Value();
	result->addObjectValue(KEY_FRAMES, &framesArray);
	result->addObjectValue(KEY_FROMFRAME, &Value((double)fromFrame));
	result->addObjectValue(KEY_TOFRAME, &Value((double)index - 1));
	result->addObjectValue(KEY_TOTALFRAMES, &Value((double)totalFrameCount));
	*_responseBody = result;
	return CODE_OK;
}

int CrossfireContext::commandContinue(Value* arguments, Value** _responseBody, wchar_t** _message) {
	BREAKRESUMEACTION action;
	Value* value_action = NULL;
	if (arguments) {
		value_action = arguments->getObjectValue(KEY_STEPACTION);
	}
	if (!value_action) {
		action = BREAKRESUMEACTION_CONTINUE;
	} else {
		if (value_action->getType() != TYPE_STRING) {
			*_message = _wcsdup(L"'continue' command has invalid 'stepaction' value");
			return CODE_INVALIDPACKET;
		}
		std::wstring* actionString = value_action->getStringValue();
		if (actionString->compare(VALUE_IN) == 0) {
			action = BREAKRESUMEACTION_STEP_INTO;
		} else if (actionString->compare(VALUE_NEXT) == 0) {
			action = BREAKRESUMEACTION_STEP_OVER;
		} else if (actionString->compare(VALUE_OUT) == 0) {
			action = BREAKRESUMEACTION_STEP_OUT;
		} else {
			*_message = _wcsdup(L"'continue' command has invalid 'stepaction' value");
			return CODE_INVALIDPACKET;
		}
	}

	CComPtr<IRemoteDebugApplicationThread> applicationThread = NULL;
	if (!getDebugApplicationThread(&applicationThread)) {
		return CODE_FAILURE;
	}
	CComPtr<IRemoteDebugApplication> application = NULL;
	HRESULT hr = applicationThread->GetApplication(&application);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.commandContinue(): GetApplication() failed", hr);
		return CODE_FAILURE;
	}

	hr = application->ResumeFromBreakPoint(applicationThread, action, ERRORRESUMEACTION_SkipErrorStatement);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.commandContinue(): ResumeFromBreakPoint() failed", hr);
		return CODE_FAILURE;
	}
	
	clearObjects();
	*_responseBody = new Value();
	(*_responseBody)->setType(TYPE_OBJECT);
	m_running = true;
	return CODE_OK;
}

int CrossfireContext::commandEvaluate(Value* arguments, Value** _responseBody, wchar_t** _message) {
	unsigned int frame = 0;
	Value* value_frame = arguments->getObjectValue(KEY_FRAMEINDEX);
	if (value_frame) {
		if (value_frame->getType() != TYPE_NUMBER || (unsigned int)value_frame->getNumberValue() < 0) {
			*_message = _wcsdup(L"'evaluate' command has invalid 'frame' value");
			return CODE_INVALIDPACKET;
		}
		frame = (unsigned int)value_frame->getNumberValue();
	}

	Value* value_expression = arguments->getObjectValue(KEY_EXPRESSION);
	if (!value_expression || value_expression->getType() != TYPE_STRING) {
		*_message = _wcsdup(L"'evaluate' command does not have a valid 'expression' value");
		return CODE_INVALIDPACKET;
	}

	CComPtr<IRemoteDebugApplicationThread> applicationThread = NULL;
	if (!getDebugApplicationThread(&applicationThread)) {
		return CODE_FAILURE;
	}

	CComPtr<IEnumDebugStackFrames> stackFrames = NULL;
	HRESULT hr = applicationThread->EnumStackFrames(&stackFrames);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.commandEvaluate(): EnumStackFrames() failed", hr);
		return CODE_FAILURE;
	}

	if (frame > 0) {
		hr = stackFrames->Skip(frame);
		if (FAILED(hr)) {
			Logger::error("CrossfireContext.commandEvaluate(): Skip() failed", hr);
			return CODE_FAILURE;
		}
	}

	ULONG fetched = 0;
	DebugStackFrameDescriptor stackFrameDescriptor;
	hr = stackFrames->Next(1, &stackFrameDescriptor, &fetched);
	if (FAILED(hr) || fetched == 0) {
		Logger::error("CrossfireContext.commandEvaluate(): Next() failed", hr);
		return CODE_FAILURE;
	}

	IDebugStackFrame* stackFrame = stackFrameDescriptor.pdsf;
	CComPtr<IDebugProperty> debugProperty = NULL;
	if (!evaluate(
		stackFrame,
		(wchar_t *)value_expression->getStringValue()->c_str(),
		DEBUG_TEXT_ISEXPRESSION | DEBUG_TEXT_RETURNVALUE | DEBUG_TEXT_ALLOWBREAKPOINTS | DEBUG_TEXT_ALLOWERRORREPORT,
		&debugProperty)) {
			return CODE_FAILURE;
	}

	JSObject newObject;
	newObject.debugProperty = debugProperty;
	newObject.stackFrame = stackFrame;
	Value* value_result = NULL;
	if (!createValueForObject(&newObject, true, &value_result)) {
		return CODE_FAILURE;
	}

	Value* result = new Value();
	result->addObjectValue(KEY_RESULT, value_result);
	delete value_result;
	*_responseBody = result;
	return CODE_OK;
}

int CrossfireContext::commandFrame(Value* arguments, Value** _responseBody, wchar_t** _message) {
	bool includeScopes = true;
	Value* value_includeScopes = arguments->getObjectValue(KEY_INCLUDESCOPES);
	if (value_includeScopes) {
		if (value_includeScopes->getType() != TYPE_BOOLEAN) {
			*_message = _wcsdup(L"'frame' command has an invalid 'includeScopes' value");
			return CODE_INVALIDPACKET;
		}
		includeScopes = value_includeScopes->getBooleanValue();
	}

	unsigned int index = 0;
	Value* value_index = arguments->getObjectValue(KEY_INDEX);
	if (value_index) {
		if (value_index->getType() != TYPE_NUMBER || (unsigned int)value_index->getNumberValue() < 0) {
			*_message = _wcsdup(L"'frame' command has an invalid 'index' value");
			return CODE_INVALIDPACKET;
		}
		index = (unsigned int)value_index->getNumberValue();
	}

	CComPtr<IRemoteDebugApplicationThread> applicationThread = NULL;
	if (!getDebugApplicationThread(&applicationThread)) {
		return false;
	}

	CComPtr<IEnumDebugStackFrames> stackFrames = NULL;
	HRESULT hr = applicationThread->EnumStackFrames(&stackFrames);
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.commandFrame(): EnumStackFrames() failed", hr);
		return CODE_FAILURE;
	}

	if (index > 0) {
		hr = stackFrames->Skip(index);
		if (FAILED(hr)) {
			Logger::error("CrossfireContext.commandFrame(): Skip() failed", hr);
			return CODE_FAILURE;
		}
	}

	ULONG fetched = 0;
	DebugStackFrameDescriptor stackFrameDescriptor;
	hr = stackFrames->Next(1, &stackFrameDescriptor, &fetched);
	if (FAILED(hr) || fetched == 0) {
		Logger::error("CrossfireContext.commandFrame(): Next() failed", hr);
		return CODE_FAILURE;
	}

	Value* frame = NULL;
	if (!createValueForFrame(stackFrameDescriptor.pdsf, index, includeScopes, &frame)) {
		return CODE_FAILURE;
	}

	Value* result = new Value();
	result->addObjectValue(KEY_FRAME, frame);
	*_responseBody = result;
	delete frame;
	return CODE_OK;
}

int CrossfireContext::commandLookup(Value* arguments, Value** _responseBody, wchar_t** _message) {
	Value* value_handle = arguments->getObjectValue(KEY_HANDLE);
	if (!value_handle || value_handle->getType() != TYPE_NUMBER || (unsigned int)value_handle->getNumberValue() < 1) {
		*_message = _wcsdup(L"'lookup' command does not have a valid 'handle' value");
		return CODE_INVALIDPACKET;
	}

	bool includeSource = false;
	Value* value_includeSource = arguments->getObjectValue(KEY_INCLUDESOURCE);
	if (value_includeSource) {
		if (value_includeSource->getType() != TYPE_BOOLEAN) {
			*_message = _wcsdup(L"'lookup' command has an invalid 'includeSource' value");
			return CODE_INVALIDPACKET;
		}
		includeSource = value_includeSource->getBooleanValue();
	}

	unsigned int handle = (unsigned int)value_handle->getNumberValue();

	std::map<unsigned int, JSObject*>::iterator iterator = m_objects->find(handle);
	if (iterator == m_objects->end()) {
		*_message = _wcsdup(L"'lookup' handle value is not a known object handle");
		return CODE_INVALIDPACKET;
	}
	JSObject* object = iterator->second;

	Value* value_object = NULL;
	if (!createValueForObject(object, true, &value_object)) {
		return CODE_FAILURE;
	}
	
	if (includeSource && !object->isObject) {
		// TODO
//		GUID iid;
//		IIDFromString(OLESTR("{94E1E004-0672-423d-AD62-78783DEF1E76}"), &iid);
//		CComPtr<IDebugProperty3> documentContext = NULL;
//		HRESULT hr2 = object->debugProperty->QueryInterface(iid, (LPVOID*)&documentContext);
//		if (SUCCEEDED(hr2)) {
//			ULONG len = 0;
//			hr2 = documentContext->GetStringCharLength(&len);
//			if (SUCCEEDED(hr2)) {
//				Logger::error("maybe?");
//			}
//		}
//		VARIANT info;
//		object->debugProperty->GetExtendedInfo(1, &iid, &info); 
//		if (info.vt == VT_UNKNOWN) {
//			CComPtr<IDebugDocumentContext2> documentContext = NULL;
//			IIDFromString(OLESTR("{931516ad-b600-419c-88fc-dcf5183b5fa9}"), &iid);
//			HRESULT hr = info.punkVal->QueryInterface(/*IID_IDebugDocumentContext2*/iid, (LPVOID*)&documentContext);
//			if (FAILED(hr)) {
//				Logger::error("CrossfireContext.commandLookup(): QI(IDebugDocumentContext2) failed", hr);
//			} else {
//				TEXT_POSITION start, end;
//				hr = documentContext->GetSourceRange(&start, &end);
//				if (FAILED(hr)) {
//					Logger::error("CrossfireContext.commandLookup(): GetSourceRange() failed", hr);
//				}
//			}
//		}
	}

	*_responseBody = value_object;
	return CODE_OK;
}

int CrossfireContext::commandScope(Value* arguments, Value** _responseBody, wchar_t** _message) {
	// TODO
	return CODE_NOTIMPLEMENTED;
}

int CrossfireContext::commandScopes(Value* arguments, Value** _responseBody, wchar_t** _message) {
	// TODO
	return CODE_NOTIMPLEMENTED;
}

int CrossfireContext::commandScript(Value* arguments, Value** _responseBody, wchar_t** _message) {
	Value* value_id = arguments->getObjectValue(/*KEY_ID*/ L"url"); // TODO
	if (!value_id || value_id->getType() != TYPE_STRING) {
		*_message = _wcsdup(L"'script' command does not have a valid 'id' value");
		return CODE_INVALIDPACKET;
	}
	std::wstring* id = value_id->getStringValue();

	bool includeSource = false;
	Value* value_includeSource = arguments->getObjectValue(KEY_INCLUDESOURCE);
	if (value_includeSource) {
		if (value_includeSource->getType() != TYPE_BOOLEAN) {
			*_message = _wcsdup(L"'script' command has an invalid 'includeSource' value");
			return CODE_INVALIDPACKET;
		}
		includeSource = value_includeSource->getBooleanValue();
	}

	IDebugApplicationNode* node = getScriptNode((wchar_t*)id->c_str());
	if (!node) {
		*_message = _wcsdup(L"'script' command specifies an unknown script id");
		return CODE_INVALIDPACKET;
	}

	Value* script = NULL;
	if (!createValueForScript(node, includeSource, false, &script)) {
		return CODE_FAILURE;
	}

	Value* result = new Value();
	result->addObjectValue(KEY_SCRIPT, script);
	*_responseBody = result;
	delete script;
	return CODE_OK;
}

int CrossfireContext::commandScripts(Value* arguments, Value** _responseBody, wchar_t** _message) {
	bool includeSource = false;
	Value* value_includeSource = arguments->getObjectValue(KEY_INCLUDESOURCE);
	if (value_includeSource) {
		if (value_includeSource->getType() != TYPE_BOOLEAN) {
			*_message = _wcsdup(L"'scripts' command has an invalid 'includeSource' value");
			return CODE_INVALIDPACKET;
		}
		includeSource = value_includeSource->getBooleanValue();
	}

	Value scriptsArray;
	scriptsArray.setType(TYPE_ARRAY);
	std::map<std::wstring, IDebugApplicationNode*>::iterator iterator = m_scriptNodes->begin();
	while (iterator != m_scriptNodes->end()) {
		Value* value = NULL;
		if (createValueForScript(iterator->second, includeSource, false, &value)) {
			scriptsArray.addArrayValue(value);
			delete value;
		}
		iterator++;
	}
	Value* result = new Value();
	result->addObjectValue(KEY_SCRIPTS, &scriptsArray);
	*_responseBody = result;
	return CODE_OK;
}

int CrossfireContext::commandSuspend(Value* arguments, Value** _responseBody, wchar_t** _message) {
	CComPtr<IRemoteDebugApplication> application = NULL;
	if (!getDebugApplication(&application)) {
		return CODE_FAILURE;
	}

	HRESULT hr = application->CauseBreak();
	if (FAILED(hr)) {
		Logger::error("CrossfireContext.commandSuspend(): CauseBreak() failed", hr);
		return CODE_FAILURE;
	}

	*_responseBody = new Value();
	(*_responseBody)->setType(TYPE_OBJECT);
	return CODE_OK;
}

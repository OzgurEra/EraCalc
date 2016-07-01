#include "EraCalc.h"
#include <cstdio>
#include <cstring>
#define CURCHAR (*mCursorPos)
#define IS_EOF (*mCursorPos == '\0')
#define NEXT() {++mCursorPos;}

#ifndef M_PHI
#define M_PHI 1.61803398875
#endif

EraCalc::EraCalc() {
	mConstantsRoot = nullptr;
	RegisterConstant("pi", 2, M_PI);
	RegisterConstant("e", 1, M_E);
	RegisterConstant("phi", 3, M_PHI);
}
	
EraCalc::Status EraCalc::Evaluate(const char* in, EraCalc::Real* out) {
	mStackTop = 0;
	mErrorMsg[0] = '\0';
	mHasError = false;
	mCursorPos = (char*)in;
	if(IS_EOF) {
		RaiseError("empty input");
		return Failed;
	}
	
	NextToken();
	while(GetToken() != Tk_EOF && (mHasError == false)) { 
		ParseExpression();
	}
	if(mHasError == false) {
		*out = mStack[0];
		return Success;
	} 
	return Failed;
}

const char* EraCalc::GetErrorMessage() const {
	return mErrorMsg;
}
	
void EraCalc::RaiseError(const char* s, ...) {
	va_list vl;
	va_start(vl, s);
	vsnprintf(mErrorMsg, 64, s, vl);
	va_end(vl);
	mHasError = true;
}
	
void EraCalc::SetToken(Token tok){
	mCurrentToken = tok;
}

void EraCalc::NextToken() {
	while (!IS_EOF) {
		switch (CURCHAR) {
		case '\n':
		case ' ':
		case '\t':
		case '\r':
			NEXT();
			continue;
		case '(':
			NEXT();
			SetToken(Tk_ParenthesisOpen);
			return;
		case ')':
			NEXT();
			SetToken(Tk_ParenthesisClose);
			return;
		case '*':
			NEXT();
			SetToken(Tk_Multiply);
			return;
		case '/':
			NEXT();
			SetToken(Tk_Divide);
			return;
		case '%':
			NEXT();
			SetToken(Tk_Modulus);
			return;
		case '+':
			NEXT();
			SetToken(Tk_Plus);
			return;
		case '-':
			NEXT();
			SetToken(Tk_Minus);
			return;
		case '^':
			NEXT();
			SetToken(Tk_Power);
			return;
		default:
			if (isdigit(CURCHAR)) {
				if(ParseNumber()) {
					SetToken(Tk_Number);
				}	
				return;	
			} else if (isalpha(CURCHAR)) {
				if(ParseConstant()){
					SetToken(Tk_Number);
				} 
				return;
			} 
			
			SetToken(Tk_Unknown);
			RaiseError("unknown token");
			return;
		}
	}
	SetToken(Tk_EOF);
	return;
}

bool EraCalc::ParseNumber() {
	char tmp[64];
	int cursor = 0;
	while((isdigit(CURCHAR) || (CURCHAR == '.')) && !IS_EOF) {
		tmp[cursor++] = CURCHAR;
		assert(cursor < 64);
		NEXT();
	}
	tmp[cursor] = '\0';
	mNumber = strtod(tmp, NULL);
	return true;
}

bool EraCalc::ParseConstant() {
	char tmp[32];
	int cursor = 0;
	while(isalpha(CURCHAR) && !IS_EOF) {
		tmp[cursor++] = CURCHAR;
		assert(cursor < 32);
		NEXT();
	}
	tmp[cursor] = '\0';
	
	if(MatchConstant(tmp, &mNumber)){
		return true;
	} else {
		RaiseError("unknown constant '%s'", tmp);
		return false;
	}
}

bool EraCalc::MatchConstant(const char* name, Real* value) {
	Constant* cnst = mConstantsRoot;
	while(cnst) {
		if(strcmp(cnst->name, name) == 0) {
			*value = cnst->value;
			return true;
		}
		cnst = cnst->next;
	}
	return false;
}

void EraCalc::RegisterConstant(const char* name, int namelen, Real value) {
	if (namelen == 0)
		namelen = strlen(name);
		
	if(mConstantsRoot == nullptr) {
		mConstantsRoot = new Constant;
		mConstantsRoot->next = nullptr;
	} else {
		Constant* last = mConstantsRoot;
		mConstantsRoot = new Constant;
		mConstantsRoot->next = last;
	}
	namelen += 1;
	mConstantsRoot->name = new char[namelen];
	memcpy(mConstantsRoot->name, name, namelen);
	mConstantsRoot->value = value;
}

EraCalc::Real EraCalc::GetNumber() const {
	return mNumber;
}

EraCalc::Token EraCalc::GetToken() const {
	return mCurrentToken;
}

char EraCalc::GetOperatorByToken(Token tok) {
	switch(tok){
	case Tk_Plus:
		return '+';
	case Tk_Minus:
		return '-';
	case Tk_Multiply:
		return '*';
	case Tk_Divide:
		return '/';
	case Tk_Modulus:
		return '%';
	case Tk_Power:
		return '^';
	default: 
		RaiseError("unknown operator");
		return '\0';
	}
}

void EraCalc::ParseExpression(){
		PlusExpr();
}

void EraCalc::PlusExpr() {
	MultExpr();
	while (true) {
		switch (GetToken()) {
		case Tk_Plus:
		case Tk_Minus:
			BinaryOp(GetOperatorByToken(GetToken()), &EraCalc::MultExpr);
			continue;
		default:
			return;
		}
	}
}

void EraCalc::MultExpr() {
	PowExpr();
	while (true) {
		switch (GetToken()) {
		case Tk_Multiply:
		case Tk_Divide:
		case Tk_Modulus:
			BinaryOp(GetOperatorByToken(GetToken()), &EraCalc::PowExpr);
			continue;
		default:
			return;
		}
	}
}

void EraCalc::PowExpr() {
	TermExpr();
	while (true) {
		switch (GetToken()) {
		case Tk_Power:
			BinaryOp(GetOperatorByToken(GetToken()), &EraCalc::TermExpr);
			continue;
		default:
			return;
		}
	}
}

void EraCalc::TermExpr() {
	switch(GetToken()) {
	case Tk_Number:
		PushTerm(GetNumber()); 
		NextToken();
		break;
	case Tk_Minus:
		NextToken();
		if(GetToken() == Tk_Number){
			PushTerm(-(GetNumber())); 
		} else {
			TermExpr();
			PushTerm(-(PopTerm()));
		}
		NextToken(); 
		break;
	case Tk_ParenthesisOpen: 
		NextToken(); 
		ParseExpression(); 
		if(GetToken() != Tk_ParenthesisClose){
			RaiseError("expected ')'");
		}
		NextToken();
		break;
	default:
		break;
	}
}

void EraCalc::BinaryOp(char op, void (EraCalc::*next)(void)) {
	NextToken();
	(this->*next)();
	EraCalc::Real r = PopTerm();
	EraCalc::Real l = PopTerm();
	switch(op){
	case '+':
		PushTerm(l + r);
		break;
	case '-':
		PushTerm(l - r);
		break;
	case '*':
		PushTerm(l * r);
		break;
	case '/':
		if(r == 0) {
			RaiseError("division by zero");
			return;
		}
		PushTerm(l / r);
		break;
	case '%':
		PushTerm(fmod(l, r));
		break;
	case '^':
		PushTerm(pow(l, r));
		break;
	}
}

void EraCalc::PushTerm(EraCalc::Real num) {
	assert(mStackTop+1 < STACK_SIZE);
	mStack[mStackTop++] = num;
}

EraCalc::Real EraCalc::PopTerm() {
	if(mStackTop == 0) {
		RaiseError("syntax error");
		return 1;
	}
	return mStack[--mStackTop];
}

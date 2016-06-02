#pragma once
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cctype>

#define CURCHAR (*mCursorPos)
#define IS_EOF (*mCursorPos == '\0')
#define NEXT() {++mCursorPos;}
#define SKIP(n) {mCursorPos+=n;}

class EraCalc {
public:
	typedef double Real;
	
	EraCalc(){
	}
	
	Real Evaluate(const char* in) {
		mStackTop = 0;
		mCursorPos = (char*)in;
		NextToken();
		while(GetToken() != Tk_EOF) {
			ParseExpression();
		}
		return mStack[0];
	}
private:
	enum Token {
		Tk_Unknown = 0,
		Tk_Divide,
		Tk_Dot,
		Tk_Number,
		Tk_Minus,
		Tk_Modulus,
		Tk_Multiply,
		Tk_ParenthesisClose,
		Tk_ParenthesisOpen,
		Tk_Plus,
		Tk_Power,
		Tk_EOF,
		Tk__MAXTOKENS__
	};
	
	void SetToken(Token tok){
		mCurrentToken = tok;
	}

	void NextToken() {
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
					ParseNumber();
					SetToken(Tk_Number);
				}else{
					SetToken(Tk_Unknown);
				}
				return;
			}
		}
		SetToken(Tk_EOF);
		return;
	}
	
	void ParseNumber() {
		char tmp[64];
		char* cursor = tmp;
		while((isdigit(CURCHAR) || (CURCHAR == '.')) && !IS_EOF) {
			*cursor = CURCHAR;
			++cursor;
			NEXT();
		}
		*cursor = '\0';
		mNumber = strtod(tmp, NULL);
	}
	
	Real GetNumber() const {
		return mNumber;
	}
	
	Token GetToken() const {
		return mCurrentToken;
	}
	
	char GetOperatorByToken(Token tok) {
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
			assert(0 && "Unknown operator Token");
			return '\0';
		}
	}
	
	void ParseExpression(){
			PlusExpr();
	}
	
	void PlusExpr() {
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

	void MultExpr() {
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

	void PowExpr() {
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
	
	void TermExpr() {
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
				assert(0 && "Expected ')'");
			}
			NextToken();
			break;
		default:
			break;
		}
	}
	
	void BinaryOp(char op, void (EraCalc::*next)(void)) {
		NextToken();
		(this->*next)();
		Real r = PopTerm();
		Real l = PopTerm();
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
				assert(0 && "Division by zero");
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
	
	void PushTerm(Real num) {
		assert(mStackTop < 63);
		mStack[mStackTop++] = num;
	}
	
	Real PopTerm() {
		return mStack[--mStackTop];
	}
	
	Real mStack[64];
	int mStackTop;
	char* mCursorPos;
	Real mNumber;
	Token mCurrentToken;
};

#undef CURCHAR
#undef IS_EOF
#undef NEXT
#undef SKIP


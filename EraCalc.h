#pragma once
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cctype>

#define STACK_SIZE 32
class EraCalc {
public:
	typedef double Real;
	
	enum Status {
		Success,
		Failed
	};
	
	EraCalc() {
	}
	
	Status Evaluate(const char* in, Real* out);
	
	const char* GetErrorMessage() const;
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
	
	void RaiseError(const char* msg);
	void SetToken(Token tok);
	void NextToken();
	Token GetToken() const;
	Real GetNumber() const;
	
	bool ParseNumber();
	bool ParseConstant();
	
	char GetOperatorByToken(Token tok);
	
	void ParseExpression();
	void PlusExpr();
	void MultExpr();
	void PowExpr();
	void TermExpr();
	
	void BinaryOp(char op, void (EraCalc::*next)(void));
	
	void PushTerm(Real num);
	Real PopTerm();
	
	Real mStack[STACK_SIZE];
	char mErrorMsg[64];
	Real mNumber;
	int mStackTop;
	Token mCurrentToken;
	bool mHasError;
	char* mCursorPos;
};


#pragma once

#include "CompilerDataTypes.h"
#include <unordered_map>

namespace MarC
{
	class Tokenizer
	{		
	public:
		static void tokenize(const std::string& code, TokenList& tokensOut);
	public:
		struct Error
		{
			enum class ErrCode
			{} errCode;
			
		};
	private:
		enum class State
		{
			StoreToken,
			FindTokenStart,
			TokenizeIdentifier,
			CheckIfOperatorOrComment,
			TokenizeOperator,
			TokenizeSingleLineComment,
			TokenizeMultiLineComment,
			CheckEndOfMultiLineComment,
			TokenizeLiteralNumber,
			TokenizeLiteralChar,
			TokenizeLiteralString,
			CheckEndOfString,
			TokenizeError,
		};
	};
}

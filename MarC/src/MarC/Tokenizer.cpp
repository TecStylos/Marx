#include "mcpch.h"
#include "Tokenizer.h"

namespace MarC
{
	void Tokenizer::tokenize(const std::string& code, TokenList& tokensOut)
	{
		Token token;
		State state = State::FindTokenStart;

		uint32_t offset = 0;
		while (offset <= code.size() && state != State::TokenizeError)
		{
			char c = code[offset++];

			bool breakMainSwitch = false;

			switch (state)
			{
			case State::FindTokenStart:
				if (c == ' ' || c == '\t' || c == '\n') break;
				if (strchr("()[]{};:,.", c))
				{
					token.name = TokenName::Separator;
					token.value = c;
					state = State::StoreToken;
					break;
				}
				if (('a' <= c && c <= 'z') ||
					('A' <= c && c <= 'Z') ||
					'_' == c)
				{
					token.name = TokenName::Identifier;
					token.value.push_back(c);
					state = State::TokenizeIdentifier;
					break;
				}
				if ('/' == c)
				{
					token.value.push_back(c);
					state = State::CheckIfOperatorOrComment;
					break;
				}
				if (strchr("=!<>+-*%^&|~", c))
				{
					token.name = TokenName::Operator;
					token.value.push_back(c);
					state = State::TokenizeOperator;
					break;
				}
				if ('0' <= c && c <= '9')
				{
					token.name = TokenName::LiteralNumber;
					token.value.push_back(c);
					state = State::TokenizeLiteralNumber;
					break;
				}
				if (c == '\'')
				{
					token.name = TokenName::LiteralChar;
					token.value.push_back(c);
					state = State::TokenizeLiteralChar;
					break;
				}
				if (c == '"')
				{
					token.name = TokenName::LiteralString;
					token.value.push_back(c);
					state = State::TokenizeLiteralString;
					break;
				}

				state = State::TokenizeError;
				break;
			case State::TokenizeIdentifier:
				if (('a' <= c && c <= 'z') || 
					('A' <= c && c <= 'Z') || 
					('0' <= c && c <= '9') ||
					'_' == c)
				{
					token.value.push_back(c);
					break;
				}

				--offset;
				state = State::StoreToken;
				break;
			case State::CheckIfOperatorOrComment:
				if ('/' == c)
				{
					token.name = TokenName::Comment;
					token.value.push_back(c);
					state = State::TokenizeSingleLineComment;
					break;
				}
				if ('*' == c)
				{
					token.name = TokenName::Comment;
					token.value.push_back(c);
					state = State::TokenizeMultiLineComment;
					break;
				}
				--offset;
				token.name = TokenName::Operator;
				state = State::TokenizeOperator;
				break;
			case State::TokenizeOperator:
				switch (token.value.size())
				{
				case 1:
					if (
						('=' == c && strchr("=!>+-*/%^&|", token.value[0])) ||
						('+' == c && '+' == token.value[0]) ||
						('-' == c && '-' == token.value[0]) ||
						('&' == c && '&' == token.value[0]) ||
						('|' == c && '|' == token.value[0]))
					{
						token.value.push_back(c);
						state = State::StoreToken;
						break;
					}
					if (
						('<' == c && '<' == token.value[0]) ||
						('=' == c && '<' == token.value[0]) ||
						('>' == c && '>' == token.value[0]))
					{
						token.value.push_back(c);
						break;
					}
					
					state = State::StoreToken;
					--offset;
					break;
				case 2:
					if (('=' == c && '<' == token.value[1]) ||
						('=' == c && '>' == token.value[1]) ||
						('>' == c && '=' == token.value[1]))
					{
						token.value.push_back(c);
						state = State::StoreToken;
						break;
					}
					--offset;
					state = State::StoreToken;
					break;
				}
				break;
			case State::TokenizeSingleLineComment:
				if ('\n' == c)
					state = State::StoreToken;
				else
					token.value.push_back(c);
				break;
			case State::TokenizeMultiLineComment:
				if ('*' == c)
					state = State::CheckEndOfMultiLineComment;
				token.value.push_back(c);
				break;
			case State::CheckEndOfMultiLineComment:
				if ('/' == c)
					state = State::StoreToken;
				else
					state = State::TokenizeMultiLineComment;
				token.value.push_back(c);
				break;
			case State::TokenizeLiteralNumber:
				if (('0' <= c && c <= '9'))
				{
					token.value.push_back(c);
					break;
				}
				if ('.' == c)
				{
					if (token.value.find(c) == std::string::npos)
					{
						token.value.push_back(c);
						break;
					}
				}

				--offset;
				state = State::StoreToken;
				break;
			case State::TokenizeLiteralChar:
				if ('\'' == c)
				{
					if (token.value.size() == 1)
					{
						state = State::TokenizeError;
						break;
					}
					token.value.push_back(c);
					state = State::StoreToken;
					break;
				}
				if (token.value.size() == 1)
				{
					token.value.push_back(c);
					break;
				}

				state = State::TokenizeError;
				break;
			case State::TokenizeLiteralString:
				token.value.push_back(c);
				if ('"' == c)
					state = State::CheckEndOfString;
				break;
			case State::CheckEndOfString:
				if ('"' == c)
				{
					state = State::TokenizeLiteralString;
					break;
				}
				--offset;
				state = State::StoreToken;
				break;
			}

			if (state == State::StoreToken)
			{
				state = State::FindTokenStart;
				tokensOut.push_back(token);
				token.name = TokenName::Unknown;
				token.value.clear();
			}
		}
	}
}

## Lexer

The function of Lexer is to identify lexeme and handle type A error

1. Lexer uses regular expression to identify lexemes. Once a lexeme is identified, lexer will construct and return a `Node` object which contains inforamtion of the lexeme.
2. Lexer also uses regular expression to recognize erroneous patterns, e.g, illiegal identifiers, hexidecimal number with leading zeros, etc. Once an incorrect pattern is recognized, lexer will print the error message.


#include "syntax.tab.c"

int main(int argc, char **argv) {
    if (argc <= 1) {
        fprintf(PARSER_error_OUTPUT, "no input path");
        return 1;
    } else if (argc > 2) {
        fprintf(PARSER_error_OUTPUT, "too much input path");
        return 1;
    } else {
        FILE *f = fopen(argv[1], "r");
        if (!f) {
            fprintf(PARSER_error_OUTPUT, "error of path %s", argv[1]);
            return 1;
        }
        yyrestart(f);
        // yydebug = 1;
        yyparse();
    }
    if (isError == 0) {
        traverse("", root);
    }
    return 0;
}
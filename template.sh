escapes='s/\\/\\\\/g;s/"/\\"/g'
putscmd='s/^/puts("/;s/$/");++nr;/'
yycmd='/YY/{s/\(.*\)\YY\(.*\)/fputs("\1",stdout);fputs(YY,stdout);puts("\2");++nr;/p;n}'
sedcmd="$escapes;$putscmd"

echo '#include <stdio.h>'
echo '#include "template.h"'

echo 'void pre_decl(void) {'
sed -n -e "$sedcmd" -e '/EMIT_DECLARATIONS/q; p' pacc.tmpl
echo '}'

echo 'void pre_engine(char *YY) {'
sed -n -e "$escapes;$yycmd;$putscmd" -e '1,/EMIT_DECLARATIONS/d; /EMIT_ENGINE/q; p' pacc.tmpl
echo '}'

echo 'void post_engine(char *YY) {'
sed -n -e "$escapes;$yycmd;$putscmd" -e '1,/EMIT_ENGINE/d; p' pacc.tmpl
echo '}'

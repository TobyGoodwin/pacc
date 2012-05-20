esc='s/\\/\\\\/g;s/"/\\"/g'
yy='/YY/s/\(.*\)\YY\(.*\)/fputs("\1",stdout);fputs(YY,stdout);puts("\2");++nr;/'
puts='/YY/!s/^/puts("/;/YY/!s/$/");++nr;/'
cmd="$esc;$yy;$puts"

echo '#include <stdio.h>'
echo '#include "template.h"'

echo 'void pre_decl(void) {'
sed -e "/EMIT_DECLARATIONS/,\$d;$cmd" pacc.tmpl
echo '}'

echo 'void pre_engine(char *YY) {'
sed -e "1,/EMIT_DECLARATIONS/d;/EMIT_ENGINE/,\$d;$cmd" pacc.tmpl
echo '}'

echo 'void post_engine(char *YY) {'
sed -e "1,/EMIT_ENGINE/d;$cmd" pacc.tmpl
echo '}'

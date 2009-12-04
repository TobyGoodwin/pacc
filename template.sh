sedcmd='s/\\/\\\\/g;s/"/\\"/g;s/^/puts("/;s/$/");/'

echo '#include <stdio.h>'

echo '#include "template.h"'

echo 'void pre_decl(void) {'
sed -n -e "$sedcmd" -e '/EMIT_DECLARATIONS/q; p' pacc.tmpl
echo '}'

echo 'void pre_engine(void) {'
sed -n -e "$sedcmd" -e '1,/EMIT_DECLARATIONS/d; /EMIT_ENGINE/q; p' pacc.tmpl
echo '}'

echo 'void post_engine(void) {'
sed -n -e "$sedcmd" -e '1,/EMIT_ENGINE/d; p' pacc.tmpl
echo '}'

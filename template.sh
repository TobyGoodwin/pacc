esc='s/\\/\\\\/g;s/"/\\"/g'
puts='s/^/puts("/;s/$/");++nr;/'
cmd="$esc;$puts"

echo '#include <stdio.h>'
echo '#include "template.h"'

echo 'void pre_decl(void) {'
sed -e "/EMIT_DECLARATIONS/,\$d;$cmd" pacc.tmpl
echo '}'

echo 'void pre_engine(void) {'
sed -e "1,/EMIT_DECLARATIONS/d;/EMIT_ENGINE/,\$d;$cmd" pacc.tmpl
echo '}'

echo 'void post_engine(void) {'
sed -e "1,/EMIT_ENGINE/d;$cmd" pacc.tmpl
echo '}'

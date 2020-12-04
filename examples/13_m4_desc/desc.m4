divert(-1)

define(`M4_FATAL', `divert(0)errprint(`FATAL: $1
')m4exit(1)')

define(`M4_CHECK_STRING',
       `ifelse(regexp(`$1',`^"[^"]*"$'), -1,
               `M4_FATAL(`$1' should be string)')')

define(`M4_CHECK_INT',
       `ifelse(regexp(`$1',`^[-0-9]+$'), -1,
               `M4_FATAL(`$1' should be int)')')

define(`M4_CHECK_DOUBLE',
       `ifelse(regexp(`$1',`^[-0-9]+\.[0-9]+$'), -1,
               `M4_FATAL(`$1' should be double)')')

define(`M4_CHECK_RANGE',
       `ifelse(regexp(`$1',`^\[[-0-9]+, [-0-9]+\]$'),
               -1, `M4_FATAL(`$1' should be range)')')

define(`M4_CHECK_SIZE',
       `ifelse(regexp(`$1',`^\[[0-9]+\]$')regexp(`$1',`^\[[0-9]+, [0-9]+\]$'),
               -1-1, `M4_FATAL(`$1' should be size)')')

define(`M4_RANGE',
       `ifelse(eval($1 > $2), 1, `M4_FATAL(`Invalid range')',
               $#, 2,
``[$1, $2]'', `M4_FATAL(`Invalid args for $0')')')

define(`M4_SIZE',
       `ifelse($#, 1,
``[$1]'', $#, 2,
``[$1, $2]'', `M4_FATAL(`Invalid args for $0')')')

# M4_STRUCT_DESC(ALIAS, SIZE, UI_MODULE)
define(`M4_STRUCT_DESC',
       `ifelse($#, 3,
               `divert(-1)
               M4_CHECK_STRING(`$1')
               M4_CHECK_SIZE(`$2')
               M4_CHECK_STRING(`$3')
               divert(0)'dnl
`// @desc: alias=$1, type="struct_list", size="$2", ui_module=$3',
               `M4_FATAL(`Invalid args for $0')')')

# M4_NUMBER_DESC(ALIAS, TYPE, SIZE, RANGE, DEFAULT)
define(`M4_NUMBER_DESC',
       `ifelse($#, 5,
               `divert(-1)
               M4_CHECK_STRING(`$1')
               M4_CHECK_STRING(`$2')
               M4_CHECK_SIZE(`$3')
               M4_CHECK_RANGE(`$4')
               M4_CHECK_INT(`$5')
               divert(0)'dnl
`// @desc: alias=$1, type=$2, size="$3", range="$4", default="$5"',
               `M4_FATAL(`Invalid args for $0')')')

# M4_STRING_DESC(ALIAS, SIZE, RANGE, DEFAULT)
define(`M4_STRING_DESC',
       `ifelse($#, 4,
               `divert(-1)
               M4_CHECK_STRING(`$1')
               M4_CHECK_SIZE(`$2')
               M4_CHECK_RANGE(`$3')
               M4_CHECK_STRING(`$4')
               divert(0)'dnl
`// @desc: alias=$1, type="string", size="$2", range="$3", default=$4',
               `M4_FATAL(`Invalid args for $0')')')

# M4_ENUM_DESC(ALIAS, ENUM, DEFAULT)
define(`M4_ENUM_DESC',
       `ifelse($#, 3,
               `divert(-1)
               M4_CHECK_STRING(`$1')
               M4_CHECK_STRING(`$2')
               M4_CHECK_STRING(`$3')
               divert(0)'dnl
`// @desc: type="enum", alias=$1, enum_def=$2, default=$3',
               `M4_FATAL(`Invalid args for $0')')')

divert(0)dnl

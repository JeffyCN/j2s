divert(-1)

define(`BEGIN_HEADER',
`divert(0)'dnl
`#ifndef $1'
`#define $1'
`divert(1)'dnl
`#ifdef ENABLE_DESC_STRUCT
// Desc structs
'
`divert(2)'dnl
`// Desc struct instances
'
`divert(3)'dnl
`// Register desc struct instances

static int num_info = 256; // Max num
static struct struct_info {
	const char *name;
	void *ptr;
} info[] = {'
`divert(0)'dnl
)

define(`END_HEADER',
`divert(4)'dnl
`	{ NULL, NULL },
};

#endif // ENABLE_DESC_STRUCT
#endif // $1'dnl
)

define(`BEGIN_STRUCT',
`divert(0)'dnl
`typedef struct {'dnl
`divert(1)'dnl
`typedef struct {'
`divert(2)'dnl
`static $1_desc_t $1_desc = {'
`divert(3)'dnl
`	{ .name = "$1_desc_t", .ptr = &$1_desc },'
`divert(0)'dnl
)

define(`MEMBER',
`divert(0)'dnl
`define(`MEMBER_NAME', $3)'dnl
`   $1;undefine(`MEMBER_NAME')'dnl
`divert(1)'dnl
`        $2 $3;'
`divert(2)'dnl
`       .$3 = $4,'
`divert(0)'dnl
)

define(`END_STRUCT',
`divert(0)'dnl
`} $1;'dnl
`divert(1)'dnl
`} $1_desc_t;'

`divert(2)'dnl
`};'

`divert(0)'dnl
)

divert(0)dnl

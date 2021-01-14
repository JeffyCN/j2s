#!/bin/bash -e

shopt -s expand_aliases

REGEXP_NON_ZERO_INT="[1-9]+([0-9]+)?"
REGEXP_POSITIVE_INT="^$REGEXP_NON_ZERO_INT$"
REGEXP_UNSIGNED_INT="^(0|$REGEXP_NON_ZERO_INT)$"
REGEXP_SIGNED_INT="^(0|-?$REGEXP_NON_ZERO_INT)$"
REGEXP_NUMBER="^-?((0|($REGEXP_NON_ZERO_INT))(\.[0-9]+)?)$"

check_regexp() {
	REGEXP=$1
	shift

	for arg in $@; do
		echo $arg |grep -Eq "$REGEXP" || exit 1
	done
}

alias check_positive_int='check_regexp "$REGEXP_POSITIVE_INT"'
alias check_unsigned_int='check_regexp "$REGEXP_UNSIGNED_INT"'
alias check_signed_int='check_regexp "$REGEXP_SIGNED_INT"'
alias check_number='check_regexp "$REGEXP_NUMBER"'

handle_size() {
	check_positive_int $@

	case $# in
		1)
			echo -n \`\`\"[1, $1]\"\'\'
			;;
		2)
			echo -n \`\`\"[$1, $2]\"\'\'
			;;
		*)
			exit -1
	esac
}

handle_range() {
	[ $# -eq 2 ] || exit 1

	check_number $@

	# Max should >= min
	echo "$2 >= $1" |bc |grep -wq 1 || exit 1

	echo -n \`\`\"[$1, $2]\"\'\'
}

handle_number() {
	[ $# -eq 5 -o $# -eq 6 ] || exit 1

	ALIAS=$1
	TYPE=$2
	SIZE=$3
	RANGE=$4
	DEFAULT=$5
	PRECISION=0

	case $TYPE in
		u8|u16|u32|u64)
			check_unsigned_int $DEFAULT
			;;
		s8|s16|s32|s64)
			check_signed_int $DEFAULT
			;;
		f32|f64)
			check_number $DEFAULT
			PRECISION=${6:-unknown}
			check_positive_int $PRECISION
			;;
		*)
			exit 1
	esac

	echo -n "@desc: alias=\"$ALIAS\", type=\"$TYPE\", size=\"$SIZE\", range=\"$RANGE\", default=\"[$DEFAULT]\", fmt=\"[$PRECISION]\""
}

handle_string() {
	[ $# -eq 4 ] || exit 1

	echo -n @desc: alias=\"$1\", type=\"string\", size=\"$2\", range=\"$3\", default=\"$4\"
}

handle_bool() {
	[ $# -eq 2 ] || exit 1

	ALIAS=$1
	DEFAULT=$2

	case $DEFAULT in
		true) DEFAULT=1;;
		false) DEFAULT=0;;
		0|1) ;;
		*) exit 1
	esac

	echo -n @desc: alias=\"$ALIAS\", type=\"bool\", default=\"[$DEFAULT]\"
}

handle_enum() {
	[ $# -eq 3 ] || exit 1

	echo -n @desc: alias=\"$1\", type=\"enum\", enum=\"$2\", default=\"$3\"
}

handle_struct() {
	[ $# -eq 3 -o $# -eq 2 ] || exit 1

	echo -n @desc: alias=\"$1\", type=\"struct\", size=\"$2\", ui_module=\"${3:-normal_ui_style}\"
}

MACRO=$1
shift
case $MACRO in
	M4_SIZE)
		handle_size "$@"
		;;
	M4_RANGE)
		handle_range "$@"
		;;
	M4_NUMBER_DESC)
		handle_number "$@"
		;;
	M4_STRING_DESC)
		handle_string "$@"
		;;
	M4_BOOL_DESC)
		handle_bool "$@"
		;;
	M4_ENUM_DESC)
		handle_enum "$@"
		;;
	M4_STRUCT_DESC)
		handle_struct "$@"
		;;
	*)
		exit 1
		;;
esac

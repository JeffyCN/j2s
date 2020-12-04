typedef struct {
	const char *student_name; // @desc: no-empty max-size=64
} student_t;

typedef struct {
	const char *group_name; // @desc: test
	student_t students[2]; // @desc: 1 boy + 1 girl
} group_t;

typedef struct {
	const char *class_name;
	/* @desc: alias=teams desc="Student groups" */
	group_t groups[2];
} class_t;

typedef struct {
	const char *school_name;
	class_t classes[2];
} school_t;

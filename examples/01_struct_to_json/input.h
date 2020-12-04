typedef struct {
	const char *name;
} student_t;

typedef struct {
	const char *name;
	student_t students[2];
} group_t;

typedef struct {
	const char *name;
	group_t groups[2];
} class_t;

typedef struct {
	const char *name;
	class_t classes[2];
} school_t;
